#include "../includes/controller/UserController.hpp"
#include <iostream>

UserController::UserController(std::unique_ptr<UserService> &&service) :
  m_userService(std::move(service)) {
}

std::string UserController::get() const {
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

  const std::string json     = jsonResponse.dump(4);
  const std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Content-Length: " +
                               std::to_string(json.size()) + "\r\n\r\n" + json;

  return response;
}

void UserController::getById(size_t id) {
  auto user = m_userService->getById(id);
  if (!user) {
    std::cout << "User not found" << std::endl;
    return;
  }

  nlohmann::json jsonResponse;
  jsonResponse["id"]         = user->getId();
  jsonResponse["first_name"] = user->getFirstName();
  jsonResponse["last_name"]  = user->getLastName();
  jsonResponse["email"]      = user->getEmail();
  std::cout << jsonResponse.dump(4) << std::endl;
}