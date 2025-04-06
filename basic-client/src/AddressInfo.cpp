#include "../includes/AddressInfo.hpp"
#include <stdexcept>

AddressInfo::AddressInfo(const addrinfo &hints, const std::string &url, const std::string &port) {
  int status = getaddrinfo(url.c_str(), port.c_str(), &hints, &m_result);

  if (status != 0) {
    throw std::runtime_error("getaddrinfo() failed: " + std::string(gai_strerror(status)));
  }
}

AddressInfo::~AddressInfo() {
  if (m_result != nullptr) {
    freeaddrinfo(m_result);
  }
}

addrinfo *AddressInfo::getResult() const {
  return m_result;
}
