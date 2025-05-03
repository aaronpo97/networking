#pragma once
#include "./controller/UserController.hpp"
#include <httpparser/request.h>
using namespace httpparser;

class Router {
  std::unique_ptr<UserController> m_userController;

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