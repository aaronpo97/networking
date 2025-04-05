#pragma once

#include <netdb.h>
#include <string>

/**
 * @class AddressInfo
 * @brief A wrapper around the POSIX getaddrinfo() function to simplify
 *        resolving network addresses.
 */
class AddressInfo
{
    addrinfo *m_result = nullptr;

public:
    /**
     * @brief Constructs an AddressInfo object by resolving a URL and port
     *        into a linked list of addrinfo structures.
     *
     * @param hints A reference to an addrinfo struct used to provide hints
     *              about the type of socket the caller supports (e.g. AF_INET).
     * @param url   The hostname or IP address to resolve.
     * @param port  The service name or port number as a string.
     *
     * @throws std::runtime_error if getaddrinfo() fails.
     */
    AddressInfo(const addrinfo &hints, const std::string &url,
                const std::string &port);

    /**
     * @brief Destructor that frees the memory allocated by getaddrinfo().
     */
    ~AddressInfo();

    /**
     * @brief Returns a pointer to the result of getaddrinfo().
     *
     * @return A pointer to the linked list of addrinfo structures.
     */
    [[nodiscard]] addrinfo *getResult() const;
};
