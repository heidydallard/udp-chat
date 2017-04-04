#include <iostream>
#include <thread>

#include <unistd.h>
#include <fcntl.h>

#include "Receiver.hh"
#include "Sender.hh"

int main(int ac, char** av)
{
  if (ac != 2) {
    std::cerr << "usage: chat ip_address" << std::endl;
    return 1;
  }
  std::string pseudo;
  std::cout << "Enter you name : ";
  std::getline(std::cin, pseudo);

  std::thread* receiver = new std::thread(Receiver(pseudo));
  std::thread* sender = new std::thread(Sender(pseudo), av[1]);

  receiver->join();
  sender->join();
  return 0;
}
