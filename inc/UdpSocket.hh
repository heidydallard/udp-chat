#pragma once

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class UdpSocket {
public:
  UdpSocket(std::string const& port);
  UdpSocket(std::string const& ip_address, std::string const& port);
  ~UdpSocket();
  
  ssize_t send(const char* message, int len);
  const char* recv(int maxSize);
private:
  int socket_;
  struct sockaddr* sockaddr_;
  socklen_t socklen_;
};
