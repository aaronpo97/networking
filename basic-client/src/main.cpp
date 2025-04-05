#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class AddressInfo
{
    addrinfo *m_result = nullptr;

public:
    AddressInfo(const addrinfo &hints, const std::string &url,
                const std::string &port)
    {
        int status = getaddrinfo(url.c_str(), port.c_str(), &hints, &m_result);

        if (status != 0)
        {
            throw std::runtime_error("getaddrinfo() failed");
        }
    };

    ~AddressInfo()
    {
        if (m_result != nullptr)
        {
            freeaddrinfo(m_result);
        }
    }

    addrinfo *getResult() const { return m_result; }
};

class Socket
{
    int m_fileDescriptor;

    /**
     * @brief Initializes the socket using the socket() system call.
     * @throws std::runtime_error if socket() fails.
     */
    void initializeSocket()
    {
        m_fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (m_fileDescriptor == -1)
        {
            throw std::runtime_error("socket() failed");
        }
    }

public:
    /**
     * @brief Constructs a Socket object and initializes the socket.
     * @param domain The communication domain (e.g., AF_INET).
     * @param type The socket type (e.g., SOCK_STREAM).
     * @param protocol The protocol to be used (e.g., IPPROTO_TCP).
     * @throws std::runtime_error if socket() fails.
     */
    explicit Socket(int domain, int type, int protocol) { initializeSocket(); }

    /**
     * @brief Destructor that closes the socket if it is open.
     * @note Used to prevent resource leaks.
     */
    ~Socket()
    {
        if (m_fileDescriptor != -1)
        {
            close(m_fileDescriptor);
        }
    }
    Socket(const Socket &) = delete;            // Disable copy constructor
    Socket &operator=(const Socket &) = delete; // Disable copy assignment

    /**
     * @returns the file descriptor for the socket.
     * @note This is used to perform operations on the socket.
     */
    int getFileDescriptor() const { return m_fileDescriptor; }
};

void connectToHost(const std::unique_ptr<Socket> &socket, sockaddr *addr,
                   socklen_t addrlen, const std::string &url)
{
    // Create the socket
    const int fileDescriptor = socket->getFileDescriptor();

    // Connect to the host
    if (connect(fileDescriptor, addr, addrlen) == -1)
    {
        throw std::runtime_error("connect() failed");
    }

    std::cout << "Connected to " << url << std::endl;

    // Send the HTTP GET request
    const std::string request = "GET / HTTP/1.1\r\n"
                                "Host: " +
                                url + "\r\n" + "Connection: close\r\n\r\n";

    if (send(fileDescriptor, request.c_str(), request.size(), 0) == -1)
    {
        throw std::runtime_error("send() failed");
    }

    // Receive the response
    char buffer[4096];
    ssize_t bytesReceived;
    while ((bytesReceived =
                recv(fileDescriptor, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0'; // Null-terminate the received data
        std::cout << buffer;
    }
    if (bytesReceived == -1)
    {
        throw std::runtime_error("recv() failed");
    }
    std::cout << std::endl;
}

int main()
{
    const std::string url = "www.google.com";
    const std::string port = "80";

    auto hints = addrinfo();
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    auto addressInfo = std::make_unique<AddressInfo>(hints, url, port);

    const addrinfo *servInfo = addressInfo->getResult();
    auto socket = std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
    if (socket->getFileDescriptor() == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    connectToHost(socket, servInfo->ai_addr, servInfo->ai_addrlen, url);

    return 0;
}
