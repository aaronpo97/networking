#pragma once
#include "./UserService.hpp"
#include <memory>

class UserController {
    std::unique_ptr<UserService> m_userService;

    public:
      UserController(const UserService &service);

      void get() {
        std::vector<std::shared_ptr<User>> users = m_userService->getAll();
        // @todo
      }
};