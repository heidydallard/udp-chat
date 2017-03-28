#pragma once

#include <map>
#include <thread>
#include "UdpSocket.hh"

class ChatApplication;

class ChatApplication {
public:
  ChatApplication();
  ~ChatApplication();

  void run(std::string const& ip_address);
  bool isFinished();

  std::string getJoin();
  std::string buildMessage(std::string const& message);
  std::string parseMessage(std::string const& message);
  
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
  void user(MessageData& md, std::string const& value);
  void command(MessageData& md, std::string const& value);
  void message(MessageData& md, std::string const& value);
  typedef void (ChatApplication::*parse_)(MessageData&, std::string const&);
  std::map<std::string, parse_> parseFuncs_;

  std::string join(MessageData const& md);
  std::string talk(MessageData const& md);
  std::string leave(MessageData const& md);
  typedef std::string (ChatApplication::*build_)(MessageData const&);
  std::map<std::string, build_> buildFuncs_;

  std::string leave();
  typedef std::string (ChatApplication::*command_)();
  std::map<std::string, command_> commandFuncs_;
  
  std::string pseudo_;
  bool end_;
  
  class Receiver {
  public:
    Receiver(ChatApplication* ca);
    ~Receiver();

    void operator()();
  private:
    UdpSocket* s_;
    ChatApplication* ca_;
  };

  class Sender {
  public:
    Sender(ChatApplication* ca);
    ~Sender();

    void operator()(std::string const& ip_address);
  private:
    UdpSocket* s_;
    ChatApplication* ca_;
  };
};
