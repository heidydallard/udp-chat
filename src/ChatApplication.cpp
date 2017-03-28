#include <iostream>
#include <sstream>
#include <string.h>

#include "ChatApplication.hh"

ChatApplication::ChatApplication()
{
  parseFuncs_["user"] = &ChatApplication::user;
  parseFuncs_["command"] = &ChatApplication::command;
  parseFuncs_["message"] = &ChatApplication::message;

  buildFuncs_["JOIN"] = &ChatApplication::join;
  buildFuncs_["TALK"] = &ChatApplication::talk;
  buildFuncs_["LEAVE"] = &ChatApplication::leave;

  commandFuncs_["/leave"] = &ChatApplication::leave;

  end_ = false;
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

bool ChatApplication::isFinished()
{
  return end_;
}

std::string ChatApplication::getJoin()
{
  return "user:" + pseudo_ + "\ncommand:JOIN\n\n";
}

std::string ChatApplication::buildMessage(std::string const& message)
{
  if (commandFuncs_.find(message) != commandFuncs_.end()) {
    return (this->*commandFuncs_[message])();
  }
  return "user:" + pseudo_ + "\ncommand:TALK\nmessage:" + message + "\n\n";
}

std::string ChatApplication::parseMessage(std::string const& message)
{
  std::stringstream ss;
  std::string line;
  std::string type;
  std::string value;
  MessageData md;

  ss << message;
  while (getline(ss, line)) {
    size_t idx = line.find(':');
    if (idx != std::string::npos) {
      type = line.substr(0, idx);
      value = line.substr(idx + 1);

      if (parseFuncs_.find(type) != parseFuncs_.end()) {
	(this->*parseFuncs_[type])(md, value);
      }
    }
  }
  if (buildFuncs_.find(md.type) != buildFuncs_.end()) {
    return (this->*buildFuncs_[md.type])(md);
  }
  return "";
}

void ChatApplication::user(ChatApplication::MessageData& md, std::string const& value)
{
  if (md.user == "")
    md.user = value;
}

void ChatApplication::command(ChatApplication::MessageData& md, std::string const& value)
{
  if (md.type == "") {
    md.type = value;
  }
}

void ChatApplication::message(MessageData& md, std::string const& value)
{
  if (md.message == "") {
    md.message = value;
  }
}

std::string ChatApplication::join(MessageData const& md)
{
  return md.user + " joined!";
}

std::string ChatApplication::talk(MessageData const& md)
{
  return "[" + md.user + "]: " + md.message;
}

std::string ChatApplication::leave(MessageData const& md)
{
  return md.user + " left!";
}

std::string ChatApplication::leave()
{
  end_ = true;
  return "user:" + pseudo_ + "\ncommand:LEAVE\n\n";
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

  while (!ca_->isFinished()) {
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
  std::string joinMessage = ca_->getJoin();

  s_->send(joinMessage.c_str(), joinMessage.size());
  while (!ca_->isFinished()) {
    std::getline(std::cin, message);
    std::string toSend = ca_->buildMessage(message);
    
    s_->send(toSend.c_str(), toSend.size());
  }
}
