#pragma once

#include <string>
#include <map>
#include <list>

#include "UdpSocket.hh"

class Receiver {
public:
  Receiver(std::string pseudo, UdpSocket* broadcast);
  ~Receiver();

  void operator()();
private:
  struct MessageData {
    MessageData() {
      user = "";
      message = "";
      type = "";
    }
    std::string user;
    std::string message;
    std::string type;
  };

  struct User {
    std::string pseudo;
    std::string ip_address;
    struct sockaddr_in* addr;
  };

  std::string parseMessage(std::string const& message, struct sockaddr_in* addr);

  std::string join(MessageData const& md, struct sockaddr_in* addr);
  std::string talk(MessageData const& md, struct sockaddr_in* addr);
  std::string leave(MessageData const& md, struct sockaddr_in* addr);
  std::string who(MessageData const& md, struct sockaddr_in* addr);
  std::string quit(MessageData const& md, struct sockaddr_in* addr);
  std::string ping(MessageData const& md, struct sockaddr_in* addr);
  typedef std::string (Receiver::*build_)(MessageData const&, struct sockaddr_in* addr);
  std::map<std::string, build_> buildFuncs_;

  void user(MessageData& md, std::string const& value);
  void command(MessageData& md, std::string const& value);
  void message(MessageData& md, std::string const& value);
  typedef void (Receiver::*parse_)(MessageData&, std::string const&);
  std::map<std::string, parse_> parseFuncs_;

  std::list<User*> connected_;
  std::string pseudo_;
  UdpSocket* listenSocket_;
  UdpSocket* broadcast_;
  bool keep_;
};