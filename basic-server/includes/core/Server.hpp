#pragma once

#include <httpparser/request.h>
#include <sys/socket.h>

#include "../controller/UserController.hpp"
#include "./Router.hpp"

#define BACKLOG 10

using namespace httpparser;

class Server {
  Router m_router;

  static void  configureSignalHandling();
  static void *getClientIPAddress(sockaddr *socketAddress);

  static int setupSocket();

  /**
   * Reads the complete HTTP request from the specified client socket.
   *
   * This method uses the recv system call to read data from the given socket
   * until the full HTTP request is received.
   *
   * @param clientSocket The file descriptor for the client socket connection.
   */
  static std::string receiveHttpRequest(int clientSocket);

  /**
   * Takes the raw request stream and parses it into a Request struct.
   * @param rawRequest The raw HTTP request string from the client.
   * @return The request parsed into a struct.
   */
  static Request parseHttpRequest(const std::string &rawRequest);

  void processClientRequest(int clientSocket) const;

public:
  Server() = default;
  void run() const;

  static constexpr std::string PORT = "4210";
};