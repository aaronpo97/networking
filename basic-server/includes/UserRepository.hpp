#pragma once

#include "./User.hpp"
#include <memory>
#include <vector>

class UserRepository
{
    std::vector<std::shared_ptr<User>> m_users = {
        std::make_shared<User>(1, "Daniel", "Anderson", "danderson@email.com"),
        std::make_shared<User>(2, "Rachel", "Lee", "rachlee@example.com"),
        std::make_shared<User>(3, "Hadid", "El Tyeb", "heltyeb@example.com")
    };

    size_t m_next_id = 4;

public:
    UserRepository() = default;

    std::shared_ptr<User>
    create(const std::string& first_name, const std::string& last_name, const std::string& email);
    [[nodiscard]] std::shared_ptr<User> getById(size_t id) const;
    [[nodiscard]] std::vector<std::shared_ptr<User>> getAll() const;

    void update(size_t id,
                const std::string& first_name,
                const std::string& last_name,
                const std::string& email);

    void remove(size_t id);
    void clear();
};
