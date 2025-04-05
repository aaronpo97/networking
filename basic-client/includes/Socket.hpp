#pragma once

/**
 * @brief  This class is an RAII wrapper for a BSD Socket. Manages the socket automatically.
 */
class Socket
{
    int m_fileDescriptor{};
    /**
     * @brief Initializes the socket using the socket() system call.
     * @throws std::runtime_error if socket() fails.
     */
    void initializeSocket();

public:
    /**
     * @brief Constructs a Socket object and initializes the socket.
     * @param domain The communication domain (e.g., AF_INET).
     * @param type The socket type (e.g., SOCK_STREAM).
     * @param protocol The protocol to be used (e.g., IPPROTO_TCP).
     * @throws std::runtime_error if socket() fails.
     */
    explicit Socket(int domain, int type, int protocol);

    /**
     * @brief Destructor that closes the socket if it is open.
     * @note Used to prevent resource leaks.
     */
    ~Socket();

    /**
     * Disable the copy constructor.
     */
    Socket(const Socket&) = delete;

    /**
     * Disable the copy assignment operator.
     */
    Socket& operator=(const Socket&) = delete;

    /**
     * @returns the file descriptor for the socket.
     * @note This is used to perform operations on the socket.
     */
    [[nodiscard]] int getFileDescriptor() const;


};
