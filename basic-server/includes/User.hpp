#pragma once
#include <string>

class User
{
    size_t m_id;
    std::string m_first_name;
    std::string m_last_name;
    std::string m_email;

public:
    User(const size_t id,
         const std::string& first_name,
         const std::string& last_name,
         const std::string& email) :
        m_id(id), m_first_name(first_name), m_last_name(last_name), m_email(email)
    {
    }

    [[nodiscard]] size_t getId() const
    {
        return m_id;
    }

    [[nodiscard]] const std::string& getFirstName() const
    {
        return m_first_name;
    }

    [[nodiscard]] const std::string& getLastName() const
    {
        return m_last_name;
    }

    [[nodiscard]] const std::string& getEmail() const
    {
        return m_email;
    }

    void setFirstName(const std::string& first_name)
    {
        m_first_name = first_name;
    }

    void setLastName(const std::string& last_name)
    {
        m_last_name = last_name;
    }

    void setEmail(const std::string& email)
    {
        m_email = email;
    }

    void setId(const size_t id)
    {
        m_id = id;
    }
};
