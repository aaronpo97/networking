#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../includes/Socket.hpp"

void Socket::initializeSocket()
{
    m_fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fileDescriptor == -1)
    {
        throw std::runtime_error("socket() failed");
    }
}

Socket::Socket(int domain, int type, int protocol) { initializeSocket(); };

Socket::~Socket()
{
    if (m_fileDescriptor != -1)
    {
        close(m_fileDescriptor);
    }
};

int Socket::getFileDescriptor() const { return m_fileDescriptor; }
