# udp-chat
A simple C++ UDP broadcast chat

## Table of contents
* [Introduction](#introduction)
* [Build](#build)
* [Usage](#usage)

### Introduction
udp-chat is a simple UDP chat application I have developed during my studies for a class on Data Communication and Computer Networks at Concordia University.
The project has been developed in C++ using Linux socket library and pthread library.

### Build
In order to build the project, you have to install CMake on your Linux machine.
The build steps are then normal CMake steps:
```ssh
$ mkdir build
$ cd build
$ cmake [OPTIONS] ..
$ make
```

### Usage
Once you have compiled the project, you will find an executable named **chat**, to run it do as follow:
```ssh
$ ./chat broadcast_address
```
**broadcast_address** is the network broadcast address on which you want to connect to.

Here is a running example of the application with all the possible commands:
```ssh
$ ./chat broadcast_address
Enter you name : Heidy
Heidy joined!
Hey anyone here ?
[Heidy #general]: Hey anyone here ?
Sal joined!
Hey Sal
[Heidy #general]: Hey Sal
[Sal #general]: Hi dude
/who
Connected users : ['Heidy', 'Sal']
Sal@172.17.0.4 joined!
[Sal@172.17.0.4 #general]: Hey guys
/private Sal@172.17.0.4       
Private message to Sal@172.17.0.4 : Dude come on channel game
[Sal@172.17.0.4] (PRIVATE): Coming
/channel game
Switched to channel game
[Sal@172.17.0.4 #game]: Are you here ?
Yeah !
[Heidy #game]: Yeah !
...
/leave
Bye now!```
