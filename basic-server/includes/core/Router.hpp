#pragma once
#include "./controller/UserController.hpp"
#include <httpparser/request.h>
using namespace httpparser;

class Router {
  std::unique_ptr<UserController> m_userController;

public:
  Router();

  void route(int clientFD, const Request &request) const;
};