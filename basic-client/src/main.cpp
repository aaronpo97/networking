#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include "../includes/Socket.hpp"
#include "../includes/AddressInfo.hpp"


void connectToHost(const std::unique_ptr<Socket>& socket, sockaddr* addr,
                   socklen_t addrlen, const std::string& url)
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
    const std::string url = "www.facebook.com";
    const std::string port = "80";

    auto hints = addrinfo{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    const auto addressInfo = std::make_unique<AddressInfo>(hints, url, port);

    const addrinfo* servInfo = addressInfo->getResult();
    const auto socket = std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
    connectToHost(socket, servInfo->ai_addr, servInfo->ai_addrlen, url);

    return 0;
}
