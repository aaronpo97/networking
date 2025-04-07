#include "../includes/HTTPClient.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

std::unique_ptr<Socket> HTTPClient::createSocketForAddress(const addrinfo *addr) {
  return std::make_unique<Socket>(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
}

std::unique_ptr<AddressInfo> HTTPClient::resolveDNS(const std::string &url,
                                                    const std::string &port) {
  addrinfo hints{};
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  return std::make_unique<AddressInfo>(hints, url, port);
}

void HTTPClient::connectToHost(int fd, sockaddr *addr, socklen_t addrlen, const std::string &url) {
  if (connect(fd, addr, addrlen) == -1) {
    throw std::runtime_error("connect() failed");
  }

  std::cout << "Connected to " << url << std::endl;

  const std::string request = "GET / HTTP/1.1\r\n"
                              "Host: " +
                              url +
                              "\r\n"
                              "Accept: */*\r\n"
                              "Connection: close\r\n\r\n";

  if (send(fd, request.c_str(), request.size(), 0) == -1) {
    throw std::runtime_error("send() failed");
  }

  char    buffer[4096];
  ssize_t bytesReceived;
  while ((bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
    buffer[bytesReceived] = '\0';
    std::cout << buffer;
  }

  if (bytesReceived == -1) {
    throw std::runtime_error("recv() failed");
  }

  std::cout << std::endl;
}

void HTTPClient::get(const std::string &url) {
  const std::string PORT        = "80";
  const auto        addressInfo = resolveDNS(url, PORT);
  addrinfo         *result      = addressInfo->getResult();

  std::runtime_error lastError("No valid addresses found");

  /*
   * Traverse the linked list and attempt to connect to an address.
   *
   * When one is successful, the function returns. Otherwise, it
   * continues to the next address. If all addresses fail, an error
   * message is printed and an exception is thrown.
   */
  while (result != nullptr) {
    try {
      const std::shared_ptr<Socket> &socket = createSocketForAddress(result);
      connectToHost(socket->getFileDescriptor(), result->ai_addr, result->ai_addrlen, url);
      return;
    } catch (const std::runtime_error &e) {
      lastError = e;
      result    = result->ai_next;
    }
  }

  std::cerr << "Failed to connect to any address for " << url << ": " << lastError.what()
            << std::endl;
  throw lastError;
}