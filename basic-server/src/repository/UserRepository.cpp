#include "../includes/repository/UserRepository.hpp"

User UserRepository::create(const std::string &first_name,
                            const std::string &last_name,
                            const std::string &email) {

  const auto user = User(m_next_id, first_name, last_name, email);
  m_next_id += 1;
  m_users.push_back(user);

  return user;
}

std::optional<User> UserRepository::getById(const size_t id) const {
  for (const auto &user : m_users) {
    if (user.getId() == id) {
      return user;
    }
  }

  return std::nullopt;
}

std::vector<User> UserRepository::getAll() const {
  return m_users;
}

void UserRepository::update(const size_t       id,
                            const std::string &first_name,
                            const std::string &last_name,
                            const std::string &email) {
  for (auto &user : m_users) {
    if (user.getId() != id) {
      continue;
    }
    user.setFirstName(first_name);
    user.setLastName(last_name);
    user.setEmail(email);
    break;
  }
}

void UserRepository::remove(const size_t id) {
  std::erase_if(m_users, [id](const User &user) { return user.getId() == id; });
}

void UserRepository::clear() {
  m_users.clear();
  m_next_id = 1;
}