#pragma once
#include "./controller/UserController.hpp"
#include <httpparser/request.h>
using namespace httpparser;

// A struct with a method and path attribute for representing valid paths on the server.
struct RouteKey {
  std::string method;
  std::string path;

  bool operator==(const RouteKey &other) const {
    return method == other.method && path == other.path;
  }
};

using RouteHandler = std::function<void(int, const Request &)>;

// A custom hash so that the RouteKey struct can be used as a
struct RouteKeyHash {
  std::size_t operator()(const RouteKey &k) const {
    return std::hash<std::string>()(k.method) ^ std::hash<std::string>()(k.path);
  }
};

class Router {
  std::unique_ptr<UserController>                          m_userController;
  std::unordered_map<RouteKey, RouteHandler, RouteKeyHash> m_routes;

  /**
   * Send a 404 Not Found response to the client.
   *
   * @param clientFD The file descriptor of the client socket.
   */
  static void sendNotFoundResponse(int clientFD) ;

public:
  Router();

  /**
   * Route the request to the appropriate controller based on the HTTP method and URI.
   *
   * @param clientFD The file descriptor of the client socket.
   * @param request The HTTP request to route.
   */
  void route(int clientFD, const Request &request) const;
};