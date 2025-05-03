#include "../includes/core/Server.hpp"
#include "../includes/controller/UserController.hpp"
#include "../includes/service/UserService.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <httpparser/httprequestparser.h>
#include <httpparser/request.h>

using namespace httpparser;
using json = nlohmann::json;

void Server::run() const {
  const int listenFD = setupSocket();
  configureSignalHandling();

  std::cout << "Server: waiting for connections on port " << PORT << "...\n";

  sockaddr_storage clientAddr{};
  socklen_t        sin_size;

  while (true) {
    // main accept() loop
    sin_size           = sizeof clientAddr;
    const int clientFD = accept(listenFD, reinterpret_cast<sockaddr *>(&clientAddr), &sin_size);
    if (clientFD == -1) {
      perror("accept");
      continue;
    }

    char s[INET6_ADDRSTRLEN];
    inet_ntop(clientAddr.ss_family, getClientIPAddress(reinterpret_cast<sockaddr *>(&clientAddr)),
              s,
              sizeof s);

    if (fork() == 0) {
      // this is the child process
      close(listenFD); // child doesn't need the listener
      processClientRequest(clientFD);
      close(clientFD);
      exit(0);
    }

    close(clientFD); // parent doesn't need this
  }
}

int Server::setupSocket() {
  addrinfo hints{}, *servinfo;
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE; // use my IP

  const int yes = 1;
  const int rv  = getaddrinfo(nullptr, PORT.c_str(), &hints, &servinfo);
  if (rv != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
    exit(1);
  } // end if

  int       sockFD = 0;
  addrinfo *p;

  for (p = servinfo; p != nullptr; p = p->ai_next) {
    /**
     * 1. Ask the operating system for a socket.
     *
     * This uses the socket() system call (UNIX) to create a socket.
     */
    sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockFD == -1) {
      continue;
    }

    /**
     * 2. Set socket options.
     *
     * This allows us to reuse the port even if it is in the TIME_WAIT state.
     */
    if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      close(sockFD);
      continue;
    }

    /**
     * 3. Bind the socket to the address and port.
     *
     * This uses the bind() system call (UNIX) to bind the socket to the address and port.
     * The address is specified in the addrinfo structure returned by getaddrinfo().
     */
    if (bind(sockFD, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockFD);
      continue;
    }

    break; // success
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (!p) {
    std::cerr << "server: failed to bind\n";
    exit(1);
  }

  /**
   * 4. Listen for incoming connections.
   *
   * This uses the listen() system call (UNIX) to listen for incoming connections on the socket.
   *
   * The backlog parameter specifies the maximum number of pending connections that can be queued
   * up before the server starts rejecting new connections.
   */
  if (listen(sockFD, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  return sockFD;
}

void Server::handleSigChild(int) {

}

// get sockaddr, IPv4 or IPv6:
void *Server::getClientIPAddress(sockaddr *socketAddress) {
  return (socketAddress->sa_family == AF_INET)
           ? reinterpret_cast<void *>(&(reinterpret_cast<sockaddr_in *>(socketAddress)->sin_addr))
           : reinterpret_cast<void *>(&(reinterpret_cast<sockaddr_in6 *>(socketAddress)->
             sin6_addr));
}


void handleSignalChild(int) {
  const int saved_errno = errno;
  while (waitpid(-1, nullptr, WNOHANG) > 0) {
  }
  errno = saved_errno;
}

void Server::configureSignalHandling() {
  /**
   * This sets up the signal handler for SIGCHLD to clean up terminated child processes.
   */
  struct sigaction sa{};

  /**
   * This function reaps all dead child processes to prevent zombie processes.
   *
   * It is set as the handler for SIGCHLD using sigaction().
   */
  sa.sa_handler = handleSignalChild;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
    perror("sigaction");
    exit(1);
  }
}

std::string Server::receiveHttpRequest(const int clientSocket) {
  std::string requestString;
  while (true) {
    char          buffer[1024];
    const ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
      break;
    }

    buffer[bytes] = '\0'; // null terminate the buffer
    requestString.append(buffer);

    // Check for end of headers
    size_t headersEnd = requestString.find("\r\n\r\n");
    if (headersEnd != std::string::npos) {
      break; // @todo keep reading until the end of the body using the Content-Length
    }
  }
  return requestString;
}


Request Server::parseHttpRequest(const std::string &rawRequest) {
  Request           request;
  HttpRequestParser parser;

  const char *begin = rawRequest.data();
  const char *end   = rawRequest.data() + rawRequest.size();

  parser.parse(request, begin, end);
  return request;
}

void Server::processClientRequest(const int clientSocket) const {
  std::setvbuf(stdout, nullptr, _IONBF, 0); // Disable buffering for stdout

  /*
   * Handle an incoming HTTP request from a client.
   * This function generates a JSON response containing all users from the service.
   */

  // this gets the raw HTTP string using rec
  const std::string &requestString = receiveHttpRequest(clientSocket);
  //  Parse the HTTP request
  const Request &request = parseHttpRequest(requestString);

  /*
   * Use the router object to send a response to the client based on the appropriate route.
   */
  m_router.route(clientSocket, request);
}