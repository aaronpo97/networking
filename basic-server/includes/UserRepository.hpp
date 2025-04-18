#pragma once

#include <memory>
#include "./User.hpp"
#include <vector>

class UserRepository
{
    std::vector<std::shared_ptr<User>> m_users;
    size_t m_next_id = 1;

public:
    UserRepository() = default;

    std::shared_ptr<User>
    create(const std::string& first_name, const std::string& last_name, const std::string& email);
    [[nodiscard]] std::shared_ptr<User> getById(size_t id) const;
    [[nodiscard]] std::vector<std::shared_ptr<User>> getAll() const;

    void update(size_t id, const std::string&
                first_name,
                const std::string&
                last_name,
                const std::string&
                email);

    void remove(size_t id);
    void clear();
};
