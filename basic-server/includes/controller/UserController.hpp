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
  std::string get() const;

  /* /users/{id} */
  void getById(size_t id);
};
