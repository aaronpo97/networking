#pragma once

#include <httpparser/request.h>
#include <string>
#include <sys/socket.h>

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

  static inline void         configureSigAction();
  static httpparser::Request parseRequest(int clientFD);
};
