#pragma once

#include <httpparser/response.h>
#include <string>
#include <vector>

class ResponseBuilder {
  httpparser::Response m_response;

public:
  ResponseBuilder();

  ResponseBuilder     &setStatusCode(int code);
  ResponseBuilder     &setStatusText(const std::string &text);
  ResponseBuilder     &setKeepAlive(bool keepAlive);
  ResponseBuilder     &setVersion(int major, int minor);
  ResponseBuilder     &setContent(const std::string &content);
  ResponseBuilder     &addHeader(const std::string &key, const std::string &value);
  ResponseBuilder     &removeHeader(const std::string &key);
  ResponseBuilder      reset();
  httpparser::Response build();
};
