#include "../includes/core/Router.hpp"
#include "../includes/controller/UserController.hpp"
#include <httpparser/request.h>
#include <httpparser/response.h>
#include <iostream>
#include <memory>
#include <sys/socket.h>

using namespace httpparser;

Router::Router() :
    m_userController(std::make_unique<UserController>(
        std::make_unique<UserService>(std::make_unique<UserRepository>()))) {}

void Router::route(const int clientFD, const Request &request) const {
  const std::string &method = request.method;
  const std::string &uri    = request.uri;

  // route /users
  if (request.method == "GET" && uri == "/users") {
    m_userController->get(clientFD);
    return;
  }
  // route /users/{id}
  if (request.method == "GET" && uri.find("/users/") == 0) {
    const std::string id = uri.substr(7);
    m_userController->getById(clientFD, std::stoul(id));
    return;
  }

  nlohmann::json responseJSON;
  responseJSON["error"]     = "Sorry we couldn't find what you were looking for.";
  const std::string content = responseJSON.dump(4);

  // catch-all route
  auto response         = Response();
  response.statusCode   = 404;
  response.versionMajor = 1;
  response.versionMinor = 1;
  response.content      = std::vector<char>(content.begin(), content.end());
  response.headers.push_back({"Content-Type", "application/json"});
  response.headers.push_back({"Content-Length", std::to_string(content.size())});

  const std::string &raw = response.serialize();
  // Send the response to the client
  const ssize_t bytesSent = send(clientFD, raw.c_str(), raw.size(), 0);
  if (bytesSent == -1) {
    std::cerr << "Error sending response: " << strerror(errno) << std::endl;
  }
}