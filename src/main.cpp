#include <iostream>

#include <unistd.h>
#include <fcntl.h>

#include "ChatApplication.hh"

int main()
{
  ChatApplication* ca = new ChatApplication;

  ca->run("localhost");
}
