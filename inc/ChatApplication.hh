#pragma once

#include <thread>
#include "UdpSocket.hh"

class ChatApplication;

class ChatApplication {
public:
  ChatApplication();
  ~ChatApplication();

  void run(std::string const& ip_address);
  std::string buildMessage(std::string const& message);
  std::string parseMessage(std::string const& message);
  
private:
  std::string pseudo_;
  
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
