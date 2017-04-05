#pragma once

#include <string>
#include <map>

#include "UdpSocket.hh"

class Sender {
public:
  Sender(std::string pseudo, UdpSocket* broadcast);
  ~Sender();

  void operator()();
private:
  std::string buildMessage(std::string const& message);

  void leave();
  void who();
  typedef void (Sender::*command_)();
  std::map<std::string, command_> commandFuncs_;

  UdpSocket* broadcast_;
  UdpSocket* local_;

  std::string pseudo_;
  bool keep_;
};
