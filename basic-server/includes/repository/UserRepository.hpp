#pragma once

#include "./IRepository.hpp"
#include "./model/User.hpp"
#include <memory>
#include <optional>
#include <vector>

class UserRepository : public IRepository<User> {
  std::vector<User> m_users = {{1, "Daniel", "Anderson", "danderson@email.com"},
                               {2, "Rachel", "Lee", "rachlee@example.com"},
                               {3, "Hadid", "El Tyeb", "heltyeb@example.com"}};

  size_t m_next_id = 4;

public:
  UserRepository() = default;

  User
  create(const std::string &first_name, const std::string &last_name, const std::string &email);

  std::optional<User> getById(size_t id) const;
  std::vector<User>   getAll() const;
  void                remove(size_t id);

  void update(size_t             id,
              const std::string &first_name,
              const std::string &last_name,
              const std::string &email);

  void clear();
};