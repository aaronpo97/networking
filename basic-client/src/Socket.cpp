#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../includes/Socket.hpp"

void Socket::initializeSocket(const int domain, const int type, const int protocol) {
  m_fileDescriptor = socket(domain, type, protocol);
  if (m_fileDescriptor == -1) {
    throw std::runtime_error("socket() failed");
  }
}

Socket::Socket(int domain, int type, int protocol) {
  initializeSocket(domain, type, protocol);
};

Socket::~Socket() {
  if (m_fileDescriptor != -1) {
    close(m_fileDescriptor);
  }
};

int Socket::getFileDescriptor() const {
  return m_fileDescriptor;
}
