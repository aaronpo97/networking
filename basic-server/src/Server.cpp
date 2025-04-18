// Server.cpp
#include "../includes/Server.hpp"
#include <arpa/inet.h>
#include <sys/wait.h>

using json = nlohmann::json;

void Server::run() {
  int listenFD = setupSocket();
  configureSigAction();

  std::cout << "Server: waiting for connections...\n";

  sockaddr_storage clientAddr{};
  socklen_t        sin_size;

  while (true) {
    // main accept() loop
    sin_size     = sizeof clientAddr;
    int clientFD = accept(listenFD, reinterpret_cast<sockaddr *>(&clientAddr), &sin_size);
    if (clientFD == -1) {
      perror("accept");
      continue;
    }

    char s[INET6_ADDRSTRLEN];
    inet_ntop(clientAddr.ss_family, getInAddr(reinterpret_cast<sockaddr *>(&clientAddr)), s,
              sizeof s);

    if (fork() == 0) {
      // this is the child process
      close(listenFD); // child doesn't need the listener
      handleRequest(clientFD);
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
  const int rv  = getaddrinfo(nullptr, PORT, &hints, &servinfo);
  if (rv != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
    exit(1);
  }

  int       sockFD = 0;
  addrinfo *p;
  for (p = servinfo; p != nullptr; p = p->ai_next) {
    /**
     * 1. Ask the operating system for a socket.
     *
     * This uses the socket() system call (UNIX) to create a socket.
     */
    sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockFD == -1)
      continue;

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

void Server::handleSigChld(int) {
  /**
   * This function reaps all dead child processes to prevent zombie processes.
   *
   * It is set as the handler for SIGCHLD using sigaction().
   */
  const int saved_errno = errno;
  while (waitpid(-1, nullptr, WNOHANG) > 0);
  errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *Server::getInAddr(sockaddr *sa) {
  return (sa->sa_family == AF_INET)
           ? reinterpret_cast<void *>(&(reinterpret_cast<sockaddr_in *>(sa)->sin_addr))
           : reinterpret_cast<void *>(&(reinterpret_cast<sockaddr_in6 *>(sa)->sin6_addr));
}

void Server::configureSigAction() {
  /**
   * This sets up the signal handler for SIGCHLD to clean up terminated child processes.
   */
  struct sigaction sa{};
  sa.sa_handler = handleSigChld;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
    perror("sigaction");
    exit(1);
  }
}

void Server::handleRequest(const int clientFD) {
  /**
   * Handle an incoming HTTP request from a client.
   * This function generates a JSON response containing all users from the service.
   */
  UserService                        service = UserService(UserRepository());
  std::vector<std::shared_ptr<User>> users   = service.getAll();

  json usersJson = json::array();
  for (const auto &user : users) {
    if (user) {
      usersJson.push_back({
          {"id", user->getId()},
          {"first_name", user->getFirstName()},
          {"last_name", user->getLastName()},
          {"email", user->getEmail()}
      });
    }
  }

  const std::string responseBody = usersJson.dump();
  const std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Content-Length: " + std::to_string(responseBody.size()) +
                                   "\r\nConnection: close\r\n\r\n" +
                                   responseBody;

  // recv(): Read incoming data from the client
  char          buffer[1024]{};
  const ssize_t received = recv(clientFD, buffer, sizeof(buffer) - 1, 0);
  if (received != -1) {
    buffer[received] = '\0'; // Null-terminate the received data
    std::cout << "Received request:\n" << buffer;

    // Extract HTTP method and path from the request
    std::string       requestLine(buffer);
    const std::string method = requestLine.substr(0, requestLine.find(' '));
    const std::string path   = requestLine.substr(requestLine.find(' ') + 1,
                                                  requestLine.find(' ', requestLine.find(' ') + 1) -
                                                  requestLine.find(' ') - 1);

    std::cout << "HTTP Path: " << path << "\n";
    std::cout << "HTTP Method: " << method << "\n";
  }

  // send(): Send the JSON HTTP response back to the client
  if (send(clientFD, httpResponse.c_str(), httpResponse.size(), 0) == -1) {
    perror("send");
  }
}