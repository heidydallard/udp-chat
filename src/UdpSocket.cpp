#include <stdexcept>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "UdpSocket.hh"

UdpSocket::UdpSocket(std::string const& port)
{
  struct addrinfo *res = nullptr, *it;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, port.c_str(), &hints, &res) != 0) {
    throw(std::runtime_error("getaddrinfo failed"));
  }

  for (it = res; it != NULL; it = it->ai_next) {
    socket_ = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
    if (socket_ == -1) {
      throw(std::runtime_error("could not create socket"));
    }

    struct sockaddr_in *ipv = (struct sockaddr_in *)it->ai_addr;
    if (bind(socket_, it->ai_addr, it->ai_addrlen) == 0) {
      sockaddr_ = it->ai_addr;
      socklen_ = it->ai_addrlen;
      break;
    }
    close(socket_);
  }

  if (it == NULL)
    throw(std::runtime_error("could not find appropriate socket"));
}

UdpSocket::UdpSocket(std::string const& ip_address, std::string const& port)
{
  struct addrinfo *res = nullptr, *it;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if (getaddrinfo(ip_address.c_str(), port.c_str(), &hints, &res) != 0) {
    throw(std::runtime_error("getaddrinfo failed"));
  }

  for (it = res; it != NULL; it = it->ai_next) {
    socket_ = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
    if (socket_ == -1) {
      throw(std::runtime_error("could not create socket"));
    }

    sockaddr_ = it->ai_addr;
    socklen_ = it->ai_addrlen;
    break;
  }
  if (it == NULL)
    throw(std::runtime_error("nothing found"));

  int enable = 1;
  if (setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int)) != 0) {
    throw(std::runtime_error("could not use broadcast"));
  }
}

UdpSocket::~UdpSocket()
{
  close(socket_);
}

ssize_t UdpSocket::send(const char* message, int len)
{
  return sendto(socket_, message, len, 0, sockaddr_, socklen_);
}

const char* UdpSocket::recv(int maxSize)
{
  char* buffer = new char[maxSize];
  struct sockaddr_in other;
  socklen_t socklen = 0;
  ssize_t received = recvfrom(socket_, buffer, maxSize, 0,
			      (struct sockaddr*)&other, &socklen);

  if (received == -1) {
    delete [] buffer;
    return NULL;
  }
  buffer[received] = 0;
  return buffer;
}
