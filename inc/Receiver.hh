#pragma once

#include <string>
#include <map>
#include <list>

#include "UdpSocket.hh"

class Receiver {
public:
  Receiver(std::string pseudo);
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

  std::string parseMessage(std::string const& message);

  std::string join(MessageData const& md);
  std::string talk(MessageData const& md);
  std::string leave(MessageData const& md);
  std::string who(MessageData const& md);
  std::string quit(MessageData const& md);
  std::string ping(MessageData const& md);
  typedef std::string (Receiver::*build_)(MessageData const&);
  std::map<std::string, build_> buildFuncs_;

  void user(MessageData& md, std::string const& value);
  void command(MessageData& md, std::string const& value);
  void message(MessageData& md, std::string const& value);
  typedef void (Receiver::*parse_)(MessageData&, std::string const&);
  std::map<std::string, parse_> parseFuncs_;

  std::list<std::string> connected_;
  std::string pseudo_;
  UdpSocket* listenSocket_;
  bool keep_;
};
