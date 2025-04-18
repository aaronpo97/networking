#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <csignal>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <vector>

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(int s) {
  (void)s; // quiet unused variable warning

  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, nullptr, WNOHANG) > 0);

  errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(reinterpret_cast<sockaddr_in *>(sa)->sin_addr);
  }

  return &(reinterpret_cast<sockaddr_in6 *>(sa)->sin6_addr);
}



// main
void handleRequest(int clientSocketFileDescriptor) {
  const std::string message      = "{\"message\": \"Hello, world!\"}";
  const std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Content-Length: " +
                                   std::to_string(message.size()) +
                                   "\r\n"
                                   "Connection: close\r\n"
                                   "\r\n" +
                                   message;

  // recv(): Read incoming data from the client
  char buffer[1024]{};
  int  bytesReceived = recv(clientSocketFileDescriptor, buffer, sizeof(buffer) - 1, 0);
  if (bytesReceived != -1) {
    buffer[bytesReceived] = '\0'; // Null-terminate the received data
    std::cout << "Received request:\n" << buffer;

    // Extract HTTP method (e.g., GET, POST) from the request
    const std::string requestLine(buffer);
    const std::string method = requestLine.substr(0, requestLine.find(' '));
    const std::string path   = requestLine.substr(requestLine.find(' ') + 1,
                                                requestLine.find(' ', requestLine.find(' ') + 1) -
                                                requestLine.find(' ') - 1);

    std::cout << "HTTP Path: " << path << std::endl;
    std::cout << "HTTP Method: " << method << std::endl;
  } else {
    perror("recv");
  }

  ssize_t sendRet = send(clientSocketFileDescriptor, httpResponse.c_str(), httpResponse.size(), 0);

  if (sendRet == -1) {
    perror("send");
  }
}

int main() {
  /**
   *    File descriptors:
   *    - listen on sock_fd and accept new connections on new_fd
   */
  int listeningSocketFileDescriptor;
  int clientSocketFileDescriptor;

  // A datatype used to store the IP address (v4 and v6)
  sockaddr_storage clientAddress{}; // connector's address information
  socklen_t        sin_size;
  struct sigaction sa{};
  const int        yes = 1;

  addrinfo hints{};
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE; // use my IP

  addrinfo *serverAddressList;
  int       getaddrinfo_result = getaddrinfo(nullptr, PORT, &hints, &serverAddressList);

  if (getaddrinfo_result != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_result));
    return 1;
  }

  // loop through all the results and bind to the first we can
  addrinfo *result = serverAddressList;
  while (result != nullptr) {
    /**
     * 1. Ask the operating system for a socket.
     *
     * This uses the socket() system call (UNIX) to create a socket.
     */
    listeningSocketFileDescriptor =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    // -1 indicates that the socket() system call failed, move on to the next address
    if (listeningSocketFileDescriptor == -1) {
      std::cout << "Failed to create socket.\n";
      result = result->ai_next;
      continue;
    }

    int setsockoptResult =
        setsockopt(listeningSocketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (setsockoptResult == -1) {
      close(listeningSocketFileDescriptor);
      std::cout << "Failed to set socket options.\n";
      result = result->ai_next; // Move to the next address
      continue;
    }

    /**
     * 2. Bind the socket to the address and port.
     *
     * This uses the bind() system call (UNIX) to bind the socket to the address and port.
     * The address is specified in the addrinfo structure returned by getaddrinfo().
     */
    int bindResult = bind(listeningSocketFileDescriptor, result->ai_addr, result->ai_addrlen);
    if (bindResult == -1) {
      close(listeningSocketFileDescriptor); // close the socket that failed to bind
      std::cout << "Failed to bind socket.\n";
      result = result->ai_next; // Move to the next address
      continue;
    }

    break; // success, can exit loop
  }

  freeaddrinfo(serverAddressList); // all done with this structure

  /**
   * This means that we could not create a socket or bind it to an address and port.
   * Fatal error, exit program
   */
  if (result == nullptr) {
    std::cerr << stderr << "server: failed to bind\n";
    exit(1);
  }

  /**
   * 3. Listen for incoming connections.
   *
   * This uses the listen() system call (UNIX) to listen for incoming connections on the socket.
   *
   * The backlog parameter specifies the maximum number of pending connections that can be queued
   * up before the server starts rejecting new connections.
   */
  if (listen(listeningSocketFileDescriptor, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; //
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
    perror("sigaction");
    exit(1);
  }

  std::cout << "Server: waiting for connections...\n";

  std::string s;
  while (1) {

    // main accept() loop
    sin_size                   = sizeof clientAddress;
    clientSocketFileDescriptor = accept(listeningSocketFileDescriptor,
                                        reinterpret_cast<sockaddr *>(&clientAddress), &sin_size);
    if (clientSocketFileDescriptor == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(clientAddress.ss_family, get_in_addr(reinterpret_cast<sockaddr *>(&clientAddress)),
              s.data(), sizeof s);

    if (!fork()) {
      // this is the child process
      close(listeningSocketFileDescriptor); // child doesn't need the listener
      handleRequest(clientSocketFileDescriptor);
      close(clientSocketFileDescriptor);
      exit(0);
    }
    close(clientSocketFileDescriptor); // parent doesn't need this
  }

  return 0;
}