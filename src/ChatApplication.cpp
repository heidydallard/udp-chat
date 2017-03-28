#include <iostream>
#include <sstream>
#include <string.h>

#include "ChatApplication.hh"

ChatApplication::ChatApplication()
{
}

ChatApplication::~ChatApplication()
{
}

void ChatApplication::run(std::string const& ip_address)
{
  std::cout << "Enter you name : ";
  std::getline(std::cin, pseudo_);
  std::thread* receiver = new std::thread(Receiver(this));
  std::thread* sender = new std::thread(Sender(this), ip_address);

  receiver->join();
  sender->join();
}

std::string ChatApplication::buildMessage(std::string const& message)
{
  return "username:" + pseudo_ + "\nmessage:" + message + "\n\n";
}

std::string ChatApplication::parseMessage(std::string const& message)
{
  std::stringstream ss;
  std::string usernameLine;
  std::string messageLine;

  ss << message;
  getline(ss, usernameLine);
  getline(ss, messageLine);

  return "[" + usernameLine.substr(usernameLine.find(':') + 1) + "]: "
    + messageLine.substr(messageLine.find(':') + 1);
}

ChatApplication::Receiver::Receiver(ChatApplication* ca)
{
  ca_ = ca;
}

ChatApplication::Receiver::~Receiver()
{
}

void ChatApplication::Receiver::operator()()
{
  s_ = new UdpSocket("12000");
  while (1) {
    const char* message = s_->recv(4096);

    std::cout << ca_->parseMessage(message) << std::endl;

    delete [] message;
  }
}

ChatApplication::Sender::Sender(ChatApplication* ca)
{
  ca_ = ca;
}

ChatApplication::Sender::~Sender()
{
}

void ChatApplication::Sender::operator()(std::string const& ip_address)
{
  s_ = new UdpSocket(ip_address, "12000");
  std::string message;

  while (1) {
    std::getline(std::cin, message);
    std::string toSend = ca_->buildMessage(message);
    
    s_->send(toSend.c_str(), toSend.size());
  }
}
