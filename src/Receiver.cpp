#include <iostream>
#include <sstream>

#include <cstring>

#include "Receiver.hh"

Receiver::Receiver(std::string pseudo, UdpSocket* broadcast)
{
  parseFuncs_["user"] = &Receiver::user;
  parseFuncs_["command"] = &Receiver::command;
  parseFuncs_["message"] = &Receiver::message;
  parseFuncs_["channel"] = &Receiver::channel;

  buildFuncs_["JOIN"] = &Receiver::join;
  buildFuncs_["TALK"] = &Receiver::talk;
  buildFuncs_["LEAVE"] = &Receiver::leave;
  buildFuncs_["WHO"] = &Receiver::who;
  buildFuncs_["QUIT"] = &Receiver::quit;
  buildFuncs_["PING"] = &Receiver::ping;
  buildFuncs_["REQUEST-PRIVATE"] = &Receiver::requestPrivate;
  buildFuncs_["PRIVATE-TALK"] = &Receiver::privateTalk;
  buildFuncs_["CHANNEL-CHANGE"] = &Receiver::changeChannel;

  pseudo_ = pseudo;
  broadcast_ = broadcast;
  keep_ = true;
  listenSocket_ = NULL;
  channel_ = "general";
}

Receiver::~Receiver()
{
  if (listenSocket_)
    delete listenSocket_;
}

void Receiver::operator()()
{
  listenSocket_ = new UdpSocket("12000");
  struct sockaddr_in* addr = new struct sockaddr_in;

  while (keep_) {
    const char* message = listenSocket_->recv(4096, addr);
    std::string toPrint = parseMessage(message, addr);

    if (toPrint.size() > 0)
      std::cout << toPrint << std::endl;

    delete [] message;
  }
  std::cout << "Bye now!" << std::endl;
}

std::string Receiver::parseMessage(std::string const& message, struct sockaddr_in* addr)
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
    return (this->*buildFuncs_[md.type])(md, addr);
  }
  return "";
}

void Receiver::user(Receiver::MessageData& md, std::string const& value)
{
  if (md.user == "")
    md.user = value;
}

void Receiver::command(Receiver::MessageData& md, std::string const& value)
{
  if (md.type == "") {
    md.type = value;
  }
}

void Receiver::message(MessageData& md, std::string const& value)
{
  if (md.message == "") {
    md.message = value;
  }
}

void Receiver::channel(MessageData& md, std::string const& value)
{
  if (md.channel == "") {
    md.channel = value;
  }
}

std::string Receiver::join(MessageData const& md, struct sockaddr_in* addr)
{
  std::string message = "user:" + pseudo_ + "\ncommand:PING\n\n";
  User* u = new User;

  u->pseudo = md.user;
  u->ip_address = inet_ntoa(addr->sin_addr);
  u->socket = new UdpSocket(u->ip_address, "12000");
  
  broadcast_->send(message.c_str(), message.size());
  connected_.push_back(u);
  return md.user + " joined!";
}

std::string Receiver::talk(MessageData const& md, struct sockaddr_in* addr)
{
  if (md.channel == channel_)
    return "[" + md.user + " #" + md.channel +"]: " + md.message;
  return "";
}

std::string Receiver::leave(MessageData const& md, struct sockaddr_in* addr)
{
  User* ret = NULL;
  for (auto u : connected_) {
    if (u->pseudo == md.user) {
      ret = u;
      break;
    }
  }
  if (ret != NULL) {
    connected_.remove(ret);
  }
  return md.user + " left!";
}

std::string Receiver::who(MessageData const& md, struct sockaddr_in* addr)
{
  std::string message = "Connected users : [";
  bool isFirst = true;

  for (auto user : connected_) {
    if (isFirst)
      isFirst = false;
    else
      message.append(", ");
    message.append("'" + user->pseudo + "'");
  }
  message.append("]");
  return message;
}

std::string Receiver::quit(MessageData const& md, struct sockaddr_in* addr)
{
  keep_ = false;
  return "";
}

std::string Receiver::ping(MessageData const& md, struct sockaddr_in* addr)
{
  for (auto u : connected_) {
    if (u->pseudo == md.user)
      return "";
  }
  User* user = new User;

  user->pseudo = md.user;
  user->ip_address = inet_ntoa(addr->sin_addr);
  user->socket = new UdpSocket(user->ip_address, "12000");

  connected_.push_back(user);

  return "";
}

std::string Receiver::privateTalk(MessageData const& md, struct sockaddr_in* addr)
{
  return "[" + md.user + "] (PRIVATE): " + md.message;
}

std::string Receiver::requestPrivate(MessageData const& md, struct sockaddr_in* addr)
{
  for (auto u : connected_) {
    if (u->pseudo == md.user) {
      std::string message = "user:" + pseudo_ + "\ncommand:PRIVATE-TALK\nmessage:" + md.message + "\n";

      u->socket->send(message.c_str(), message.size());
    }
  }
  return "";
}

std::string Receiver::changeChannel(MessageData const& md, struct sockaddr_in* addr)
{
  channel_ = md.channel;
  return "Switched to channel " + channel_;
}
