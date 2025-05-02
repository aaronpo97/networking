#pragma once

#include "../controller/UserController.hpp"
#include "../repository/UserRepository.hpp"
#include "../service/UserService.hpp"

#include <httpparser/httprequestparser.h>
#include <httpparser/httpresponseparser.h>
#include <httpparser/request.h>
#include <httpparser/response.h>

#include <iostream>
#include <memory>
#include <sys/socket.h>
#include "./Router.hpp"

#define BACKLOG 10

using namespace httpparser;

class Server {

  Router       m_router;
  static void  handleSigChld(int s);
  static void *getInAddr(sockaddr *sa);
  void         handleRequest(int clientSocketFileDescriptor);
  static int   setupSocket();

  static inline void         configureSigAction();
  static Request parseRequest(int clientFD);

public:
  Server() = default;
  void run();

  static constexpr std::string PORT = "4210";
};
