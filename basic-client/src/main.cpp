#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include "../includes/AddressInfo.hpp"
#include "../includes/Socket.hpp"

class HTTPClient {

  /**
   * Create a socket for a given instance of addrinfo.get
   *
   * @param addr The addrinfo instance to create a socket for
   * @return A unique pointer to the created socket
   * @throws std::runtime_error if socket creation fails
   */
  static std::unique_ptr<Socket> createSocketForAddress(const addrinfo *addr) {
    return std::make_unique<Socket>(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  }

  /**
   * Resolve the DNS for a given URL and port
   *
   * @param url The URL to resolve
   * @param port The port to resolve
   * @return A unique pointer to the AddressInfo instance
   * @throws std::runtime_error if DNS resolution fails
   */
  static std::unique_ptr<AddressInfo> resolveDNS(const std::string &url, const std::string &port) {
    addrinfo hints{};
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    return std::make_unique<AddressInfo>(hints, url, port);
  }

  /**
   * Connect to a host using the given socket file descriptor and address
   *
   * @param fd The socket file descriptor
   * @param addr The address to connect to
   * @param addrlen The length of the address
   * @param url The URL to connect to
   * @throws std::runtime_error if connection fails
   */
  static void connectToHost(int fd, sockaddr *addr, socklen_t addrlen, const std::string &url) {
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

public:
  /**
   * Perform a GET request to the specified URL
   *
   * @param url The URL to perform the GET request on
   * @throws std::runtime_error if any error occurs during the process
   */
  static void get(const std::string &url) {
    const std::string PORT        = "80";
    const auto        addressInfo = resolveDNS(url, PORT);
    addrinfo         *result      = addressInfo->getResult();

    std::runtime_error lastError("Something went wrong.");
    /*
     * Each addrinfo is a linked list, so we can iterate through each result to make a connection if
     * the previous one fails.
     */

    while (result != nullptr) {
      try {
        auto socket = createSocketForAddress(result);
        connectToHost(socket->getFileDescriptor(), result->ai_addr, result->ai_addrlen, url);
        return; // Successfully connected and sent request - exit the loop
      } catch (const std::runtime_error &e) {
        lastError = e;
        result    = result->ai_next; // Move to the next address in the list
      }
    }
    // If we reach here, all attempts to connect failed
    std::cerr << "Failed to connect to any address for " << url << ": " << lastError.what()
              << std::endl;
    throw lastError;
  }
};

int main(int argc, char *argv[]) {
  {
    try {
      if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return 1;
      }

      std::string url = argv[1];
      // Remove "http://" or "https://" if present
      if (url.find("http://") == 0) {
        url.erase(0, 7);
      } else if (url.find("https://") == 0) {
        url.erase(0, 8);
      }
      // Remove trailing slash if present
      if (url.back() == '/') {
        url.pop_back();
      }
      std::cout << "Performing GET request to: " << url << std::endl;

      HTTPClient::get(url);
      std::cout << "Request completed successfully." << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
    }
    return 0;
  }
}
