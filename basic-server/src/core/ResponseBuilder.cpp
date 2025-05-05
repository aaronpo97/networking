#include "../includes/core/ResponseBuilder.hpp"
#include <algorithm>

using namespace httpparser;

ResponseBuilder::ResponseBuilder() = default;

ResponseBuilder &ResponseBuilder::setStatusCode(int code) {
  m_response.statusCode = code;
  return *this;
}

ResponseBuilder &ResponseBuilder::setStatusText(const std::string &text) {
  m_response.status = text;
  return *this;
}

ResponseBuilder &ResponseBuilder::setKeepAlive(bool keepAlive) {
  m_response.keepAlive = keepAlive;
  return *this;
}

ResponseBuilder &ResponseBuilder::setVersion(int major, int minor) {
  m_response.versionMajor = major;
  m_response.versionMinor = minor;
  return *this;
}

ResponseBuilder &ResponseBuilder::setContent(const std::string &content) {
  m_response.content = std::vector<char>(content.begin(), content.end());
  return *this;
}

ResponseBuilder &ResponseBuilder::addHeader(const std::string &key, const std::string &value) {
  m_response.headers.push_back({key, value});
  return *this;
}

ResponseBuilder &ResponseBuilder::removeHeader(const std::string &key) {
  auto it = std::remove_if(m_response.headers.begin(), m_response.headers.end(),
                           [&key](const auto &header) { return header.name == key; });
  m_response.headers.erase(it, m_response.headers.end());
  return *this;
}

ResponseBuilder ResponseBuilder::reset() {
  m_response = Response();
  return *this;
}

Response ResponseBuilder::build() {
  Response response = m_response;
  reset();
  return response;
}
