#pragma once
#include "../repository/UserRepository.hpp"

class UserService {
  std::unique_ptr<UserRepository> m_repository;

public:
  explicit UserService(std::unique_ptr<UserRepository> &&repository);

  [[nodiscard]] User create(const std::string &first_name,
                            const std::string &last_name,
                            const std::string &email) const;

  [[nodiscard]] std::optional<User> getById(size_t id) const;

  [[nodiscard]] std::vector<User> getAll() const;

  void update(size_t             id,
              const std::string &first_name,
              const std::string &last_name,
              const std::string &email) const;

  void remove(size_t id) const;
};