#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include "../includes/Socket.hpp"
#include "../includes/AddressInfo.hpp"

class HTTPClient
{
    static std::unique_ptr<Socket> createSocket(const int domain, const int type, const int protocol)
    {
        return std::make_unique<Socket>(domain, type, protocol);
    }

    static std::unique_ptr<AddressInfo> resolveDNS(const std::string& url, const std::string& port)
    {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        return std::make_unique<AddressInfo>(hints, url, port);
    }

    static void connectToHost(int fd,sockaddr* addr, socklen_t addrlen, const std::string& url)
    {
        if (connect(fd, addr, addrlen) == -1)
        {
            throw std::runtime_error("connect() failed");
        }

        std::cout << "Connected to " << url << std::endl;

       const std::string request =
            "GET / HTTP/1.1\r\n"
            "Host: " + url + "\r\n"
            "Accept: */*\r\n"
            "Connection: close\r\n\r\n";

        if (send(fd, request.c_str(), request.size(), 0) == -1)
        {
            throw std::runtime_error("send() failed");
        }

        char buffer[4096];
        ssize_t bytesReceived;
        while ((bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0)
        {
            buffer[bytesReceived] = '\0';
            std::cout << buffer;
        }

        if (bytesReceived == -1)
        {
            throw std::runtime_error("recv() failed");
        }

        std::cout << std::endl;
    }

public:
    static void get(const std::string& url, const std::string& port = "80")
    {
        const auto addressInfo = resolveDNS(url, port)->getResult();
        const auto socket = createSocket(AF_INET, SOCK_STREAM,0);

        connectToHost(socket->getFileDescriptor(),addressInfo->ai_addr,
                      addressInfo->ai_addrlen,
                      url);
    }
};

int main()
{
    const std::string url = "httpbin.org";
    const std::string port = "80";

    HTTPClient::get(url, port);

    return 0;
}
