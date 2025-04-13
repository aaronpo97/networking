/*
** server.c -- a stream socket server demo
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(int s) {
  (void)s; // quiet unused variable warning

  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(reinterpret_cast<sockaddr_in *>(sa)->sin_addr);
  }

  return &(reinterpret_cast<sockaddr_in6 *>(sa)->sin6_addr);
}

int main() {
  /**
   *    File descriptors:
   *    - listen on sock_fd and accept new connections on new_fd
   */
  int listeningSocketFileDescriptor;
  int clientSocketFileDescriptor;



  // A datatype used to store the IP address (v4 and v6)
  sockaddr_storage clientAddress{}; // connector's address information
  socklen_t        sin_size;
  struct sigaction sa{};
  int              yes = 1;
  char             s[INET6_ADDRSTRLEN];

  addrinfo hints{};
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE; // use my IP


  addrinfo *serverAddressList;
  int getaddrinfo_result = getaddrinfo(nullptr, PORT, &hints, &serverAddressList);

  if (getaddrinfo_result != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_result));
    return 1;
  }

  // loop through all the results and bind to the first we can
  addrinfo *p;
  for (p = serverAddressList; p != nullptr; p = p->ai_next) {
    if ((listeningSocketFileDescriptor =
         socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(listeningSocketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(listeningSocketFileDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
      close(listeningSocketFileDescriptor);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(serverAddressList); // all done with this structure

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(listeningSocketFileDescriptor, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while (1) {
    // main accept() loop
    sin_size                   = sizeof clientAddress;
    clientSocketFileDescriptor = accept(listeningSocketFileDescriptor,
                                        reinterpret_cast<sockaddr *>(&clientAddress), &sin_size);
    if (clientSocketFileDescriptor == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(clientAddress.ss_family, get_in_addr(reinterpret_cast<sockaddr *>(&clientAddress)), s,
              sizeof s);
    printf("server: got connection from %s\n", s);

    if (!fork()) {
      // this is the child process
      close(listeningSocketFileDescriptor); // child doesn't need the listener
      if (send(clientSocketFileDescriptor, "Hello, world!", 13, 0) == -1)
        perror("send");
      close(clientSocketFileDescriptor);
      exit(0);
    }
    close(clientSocketFileDescriptor); // parent doesn't need this
  }

  return 0;
}