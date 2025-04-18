#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <UserRepository.hpp>
#include <UserService.hpp>

#define PORT "3490"
#define BACKLOG 10

class Server {
public:
  void run();

private:
  static void handleSigChld(int s);
  static void *getInAddr(sockaddr *sa);
  static void handleRequest(int clientSocketFileDescriptor);
  int setupSocket();

  static inline void configureSigAction();
};
