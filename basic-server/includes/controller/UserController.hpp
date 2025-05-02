#pragma once
#include "../service/UserService.hpp"
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class UserController {
  std::unique_ptr<UserService> m_userService;

public:
  explicit UserController(std::unique_ptr<UserService> &&service);

  /* /users */
  void get(int fileDescriptor) const;

  /* /users/{id} */
  void getById(int fileDescriptor, size_t id) const;
};
