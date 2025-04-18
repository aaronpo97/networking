
#include "../includes/UserRepository.hpp"

std::shared_ptr<User> UserRepository::create(const std::string &first_name,
                                             const std::string &last_name,
                                             const std::string &email) {
  const auto user = std::make_shared<User>(m_next_id, first_name, last_name, email);
  m_next_id += 1;
  m_users.push_back(user);

  return user;
}

std::shared_ptr<User> UserRepository::getById(const size_t id) const {
  for (const auto &user : m_users) {
    if (user->getId() == id) {
      return user;
    }
  }
  return nullptr;
}

std::vector<std::shared_ptr<User>> UserRepository::getAll() const {
  return m_users;
}

void UserRepository::update(const size_t       id,
                            const std::string &first_name,
                            const std::string &last_name,
                            const std::string &email) {
  for (const auto &user : m_users) {
    if (user->getId() != id) {
      continue;
    }
    user->setFirstName(first_name);
    user->setLastName(last_name);
    user->setEmail(email);
    break;
  }
}

void UserRepository::remove(const size_t id) {
  std::erase_if(m_users, [id](const std::shared_ptr<User> &user) { return user->getId() == id; });
}

void UserRepository::clear() {
  m_users.clear();
  m_next_id = 1;
}