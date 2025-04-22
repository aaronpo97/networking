#pragma once

#include <UserRepository.hpp>
#include <UserService.hpp>
#include <csignal>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BACKLOG 10

class Server {
public:
  void run();

  static constexpr std::string PORT = "3490";

private:
  static void  handleSigChld(int s);
  static void *getInAddr(sockaddr *sa);
  static void  handleRequest(int clientSocketFileDescriptor);
  static int   setupSocket();

  static inline void configureSigAction();
  static std::string parseRequest(const int clientFD);
};
