#pragma once

#include "./model/User.hpp"
#include <memory>
#include <optional>
#include <vector>

class UserRepository {
  std::vector<User> m_users = {{1, "Daniel", "Anderson", "danderson@email.com"},
                               {2, "Rachel", "Lee", "rachlee@example.com"},
                               {3, "Hadid", "El Tyeb", "heltyeb@example.com"}};

  size_t m_next_id = 4;

public:
  UserRepository() = default;

  User
  create(const std::string &first_name, const std::string &last_name, const std::string &email);

  [[nodiscard]] std::optional<User> getById(size_t id) const;
  [[nodiscard]] std::vector<User>   getAll() const;

  void update(size_t             id,
              const std::string &first_name,
              const std::string &last_name,
              const std::string &email);

  void remove(size_t id);
  void clear();
};