#pragma once
#include "./UserRepository.hpp"

class UserService {
  std::unique_ptr<UserRepository> m_repository;

public:
  explicit UserService(const UserRepository &repository);

  [[nodiscard]] std::shared_ptr<User> create(const std::string &first_name,
                                             const std::string &last_name,
                                             const std::string &email) const;

  [[nodiscard]] std::shared_ptr<User> getById(size_t id) const;

  [[nodiscard]] std::vector<std::shared_ptr<User>> getAll() const;

  void update(size_t             id,
              const std::string &first_name,
              const std::string &last_name,
              const std::string &email) const;

  void remove(size_t id) const;
};
