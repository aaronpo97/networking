#pragma once
#include "./UserService.hpp"
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class UserController {
  std::unique_ptr<UserService> m_userService;

public:
  UserController(std::unique_ptr<UserService> service);

  /* /users */
  void get();

  /* /users/{id} */
  void getById(size_t id);
};
