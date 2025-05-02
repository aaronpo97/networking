#include "../includes/controller/UserController.hpp"
#include <httpparser/response.h>
#include <iostream>
#include <sys/socket.h>

#include <nlohmann/json.hpp>

UserController::UserController(std::unique_ptr<UserService> &&service) :
    m_userService(std::move(service)) {}

void UserController::get(const int fileDescriptor) const {
  const auto &users = m_userService->getAll();
  json        jsonResponse;
  json        userJson;

  for (const auto &user : users) {
    userJson["id"]         = user.getId();
    userJson["first_name"] = user.getFirstName();
    userJson["last_name"]  = user.getLastName();
    userJson["email"]      = user.getEmail();
    jsonResponse.push_back(userJson);
  }

  const std::string json = jsonResponse.dump(4);

  auto response = httpparser::Response();

  response.statusCode   = 200;
  response.versionMajor = 1;
  response.versionMinor = 1;

  response.content = std::vector(json.begin(), json.end());

  response.headers.push_back({"Content-Type", "application/json"});
  response.headers.push_back({"Content-Length", std::to_string(json.size())});

  const std::string &raw = response.serialize();

  // Send the response to the client
  const ssize_t bytesSent = send(fileDescriptor, raw.c_str(), raw.size(), 0);
  if (bytesSent == -1) {
    std::cerr << "Error sending response: " << strerror(errno) << std::endl;
  }
};

void UserController::getById(const int fileDescriptor, const size_t id) const {
  const std::optional<User> &user = m_userService->getById(id);

  nlohmann::json responseJSON;
  if (user) {
    responseJSON["id"]         = user->getId();
    responseJSON["first_name"] = user->getFirstName();
    responseJSON["last_name"]  = user->getLastName();
    responseJSON["email"]      = user->getEmail();
  } else {
    responseJSON["error"] = "Sorry we couldn't find what you were looking for.";
  }

  const std::string rawJSON  = responseJSON.dump(4);
  auto              response = httpparser::Response();

  response.statusCode   = user ? 200 : 404;
  response.versionMajor = 1;
  response.versionMinor = 1;
  response.content      = std::vector(rawJSON.begin(), rawJSON.end());

  response.headers.push_back({"Content-Type", "application/json"});
  response.headers.push_back({"Content-Length", std::to_string(rawJSON.size())});

  const std::string &raw       = response.serialize();
  const ssize_t      bytesSent = send(fileDescriptor, raw.c_str(), raw.size(), 0);

  if (bytesSent == -1) {
    std::cerr << "Error sending response: " << strerror(errno) << std::endl;
  }
}