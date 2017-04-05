#include <iostream>

#include "Sender.hh"

Sender::Sender(std::string pseudo, UdpSocket* broadcast)
{
  commandFuncs_["/leave"] = &Sender::leave;
  commandFuncs_["/who"] = &Sender::who;

  pseudo_ = pseudo;
  broadcast_ = broadcast;
  keep_ = true;
  local_ = NULL;
}

Sender::~Sender()
{
  if (local_)
    delete local_;
}

void Sender::operator()()
{
  local_ = new UdpSocket("127.0.0.1", "12000");
  std::string message;
  std::string joinMessage = "user:" + pseudo_ + "\ncommand:JOIN\n\n";

  broadcast_->send(joinMessage.c_str(), joinMessage.size());
  while (keep_) {
    std::getline(std::cin, message);
    
    buildMessage(message);
  }
}

std::string Sender::buildMessage(std::string const& message)
{
  if (commandFuncs_.find(message) != commandFuncs_.end()) {
    (this->*commandFuncs_[message])();
  }
  else {
    std::string m = "user:" + pseudo_ + "\ncommand:TALK\nmessage:" + message + "\n\n";

    broadcast_->send(m.c_str(), m.size());
  }
}

void Sender::leave()
{
  std::string message = "user:" + pseudo_ + "\ncommand:LEAVE\n\n";
  std::string quitMessage = "command:QUIT\n\n";
  
  local_->send(quitMessage.c_str(), quitMessage.size());
  broadcast_->send(message.c_str(), message.size());
  keep_ = false;
}

void Sender::who()
{
  std::string message = "command:WHO\n\n";
  
  local_->send(message.c_str(), message.size());
}
