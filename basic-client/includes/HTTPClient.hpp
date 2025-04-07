#pragma once

#include "./Socket.hpp"
#include "./AddressInfo.hpp"
#include <memory>

class HTTPClient {

  /**
   * Create a socket for a given instance of addrinfo.get
   *
   * @param addr The addrinfo instance to create a socket for
   * @return A unique pointer to the created socket
   * @throws std::runtime_error if socket creation fails
   */
  static std::unique_ptr<Socket> createSocketForAddress(const addrinfo *addr);

  /**
   * Resolve the DNS for a given URL and port
   *
   * @param url The URL to resolve
   * @param port The port to resolve
   * @return A unique pointer to the AddressInfo instance
   * @throws std::runtime_error if DNS resolution fails
   */
  static std::unique_ptr<AddressInfo> resolveDNS(const std::string &url, const std::string &port);

  /**
   * Connect to a host using the given socket file descriptor and address
   *
   * @param fd The socket file descriptor
   * @param addr The address to connect to
   * @param addrlen The length of the address
   * @param url The URL to connect to
   * @throws std::runtime_error if connection fails
   */
  static void connectToHost(int fd, sockaddr *addr, socklen_t addrlen, const std::string &url);

public:

  /**
   * Perform a GET request to the specified URL
   *
   * @param url The URL to perform the GET request on
   * @throws std::runtime_error if any error occurs during the process
   */
  static void get(const std::string &url);

};
