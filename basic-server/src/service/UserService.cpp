#include "../includes/service/UserService.hpp"

UserService::UserService(std::unique_ptr<UserRepository> &&repository) :
    m_repository(std::move(repository)) {}

User UserService::create(const std::string &first_name,
                         const std::string &last_name,
                         const std::string &email) const {
  return m_repository->create(first_name, last_name, email);
}

std::optional<User> UserService::getById(const size_t id) const {
  return m_repository->getById(id);
}

std::vector<User> UserService::getAll() const {
  return m_repository->getAll();
}

void UserService::update(const size_t       id,
                         const std::string &first_name,
                         const std::string &last_name,
                         const std::string &email) const {
  return m_repository->update(id, first_name, last_name, email);
}

void UserService::remove(const size_t id) const {
  return m_repository->remove(id);
}