#include <iostream>
#include <sstream>

#include "Receiver.hh"

Receiver::Receiver(std::string pseudo)
{
  parseFuncs_["user"] = &Receiver::user;
  parseFuncs_["command"] = &Receiver::command;
  parseFuncs_["message"] = &Receiver::message;

  buildFuncs_["JOIN"] = &Receiver::join;
  buildFuncs_["TALK"] = &Receiver::talk;
  buildFuncs_["LEAVE"] = &Receiver::leave;
  buildFuncs_["WHO"] = &Receiver::who;
  buildFuncs_["QUIT"] = &Receiver::quit;

  pseudo_ = pseudo;
  keep_ = true;
}

Receiver::~Receiver()
{
  delete listenSocket_;
}

void Receiver::operator()()
{
  listenSocket_ = new UdpSocket("12000");

  while (keep_) {
    const char* message = listenSocket_->recv(4096);
    std::string toPrint = parseMessage(message);

    if (toPrint.size() > 0)
      std::cout << toPrint << std::endl;

    delete [] message;
  }
  std::cout << "Bye now!" << std::endl;
}

std::string Receiver::parseMessage(std::string const& message)
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

std::string Receiver::join(MessageData const& md)
{
  std::string message = "user:" + pseudo_ + "\ncommand:PING\n\n";

  listenSocket_->send(message.c_str(), message.size());
  connected_.push_back(md.user);
  return md.user + " joined!";
}

std::string Receiver::talk(MessageData const& md)
{
  return "[" + md.user + "]: " + md.message;
}

std::string Receiver::leave(MessageData const& md)
{
  connected_.remove(md.user);
  return md.user + " left!";
}

std::string Receiver::who(MessageData const& md)
{
  std::string message = "Connected users : [";
  bool isFirst = true;

  for (auto user : connected_) {
    if (isFirst)
      isFirst = false;
    else
      message.append(", ");
    message.append("'" + user + "'");
  }
  message.append("]");
  return message;
}

std::string Receiver::quit(MessageData const& md)
{
  keep_ = false;
  return "";
}

std::string Receiver::ping(MessageData const& md)
{
  connected_.push_back(md.user());
  return "";
}
