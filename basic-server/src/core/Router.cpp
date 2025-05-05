#include "../includes/core/Router.hpp"
#include "../includes/controller/UserController.hpp"
#include "../includes/core/ResponseBuilder.hpp"
#include <httpparser/request.h>
#include <httpparser/response.h>
#include <iostream>
#include <memory>
#include <sys/socket.h>

using namespace httpparser;

Router::Router() :
    m_userController(std::make_unique<UserController>(
        std::make_unique<UserService>(std::make_unique<UserRepository>()))) {

  m_routes[{"GET", "/users"}] = [this](const int fd, const Request &req) {
    m_userController->get(fd);
  };

  m_routes[{"GET", "/teapot"}] = [this](const int fd, const Request &req) {
    nlohmann::json jsonResponse;
    jsonResponse["error"]   = "I'm a teapot";
    jsonResponse["message"] = "The server is in fact a teapot.";
    jsonResponse["status"]  = 418;

    // Serialize the JSON response
    const std::string &jsonString = jsonResponse.dump();
    // create a response object
    ResponseBuilder responseBuilder = ResponseBuilder();

    const Response &response = responseBuilder.setStatusCode(418)
                                   .setStatusText("I'm a teapot")
                                   .setKeepAlive(false)
                                   .setVersion(1, 1)
                                   .setContent(jsonString)
                                   .addHeader("Content-Type", "application/json")
                                   .addHeader("Content-Length", std::to_string(jsonString.size()))
                                   .build();

    const std::string &raw = response.serialize();

    std::cout << raw << "\n";
    ssize_t bytesSent = send(fd, raw.c_str(), raw.size(), 0);
    if (bytesSent == -1) {
      std::cerr << "Error sending response: " << strerror(errno) << "\n";
    }
  };
}

void Router::sendNotFoundResponse(const int clientFD) {
  nlohmann::json jsonResponse;
  jsonResponse["error"]   = "Not Found";
  jsonResponse["message"] = "The requested resource was not found on the server.";
  jsonResponse["status"]  = 404;

  // Serialize the JSON response
  const std::string &jsonString = jsonResponse.dump();

  // Create a response builder object and use it to build the response
  ResponseBuilder responseBuilder = ResponseBuilder();
  const Response &response        = responseBuilder.setStatusCode(404)
                                 .setStatusText("Not Found")
                                 .setKeepAlive(false)
                                 .setVersion(1, 1)
                                 .setContent(jsonString)
                                 .addHeader("Content-Type", "application/json")
                                 .addHeader("Content-Length", std::to_string(jsonString.size()))
                                 .build();

  const std::string &raw = response.serialize();
  std::cout << raw << "\n";

  std::cout << "Sending 404 response to client: " << clientFD << "\n";
  ssize_t bytesSent = send(clientFD, raw.c_str(), raw.size(), 0);
  if (bytesSent == -1) {
    std::cerr << "Error sending response: " << strerror(errno) << "\n";
  }
}

void Router::route(const int clientFD, const Request &request) const {
  auto routeIterator = m_routes.find({request.method, request.uri});
  if (routeIterator != m_routes.end()) {
    const auto &[routeKey, handler] = *routeIterator;

    // Call the handler function with the client file descriptor and request
    handler(clientFD, request);
  } else {

    std::cout << "No route found for " << request.method << " " << request.uri << "\n";
    // If no route is found, send a 404 response
    sendNotFoundResponse(clientFD);
  }
}
