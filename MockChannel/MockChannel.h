#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H

///////////////////////////////////////////////////////////////////////
// MockChannel.h - Build as DLL for C++\CLI client to use            //
//				   native code channel                               //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
This package implements a client:
* which sends HTTP style messages and files to a server 
* which recieves messages and stores files.

Public Interface:
=================
*	std::string getMessage() - receives messages from client to display on GUI
*	void postMessage()	- send HTTP style message request to client
*	void start()		- starts the client

Build Process:
==============
Required Files:
*   MsgClient.h, MsgClient.cpp
*   HttpMessage.h, HttpMessage.cpp
*	WPF.h, WPF.cpp, Cpp11-BlockingQueue.h

Build commands:
===============
- devenv project4.sln

Maintenance History:
====================
ver 1.0 : 03 May 16
- first release

*/

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif
#include <string>

struct IMockChannel
{
public:
	virtual std::string getMessage() = 0;
	virtual void postMessage(const std::string& msg, const std::string& body) = 0;
	virtual void start() = 0;
};

extern "C" {
  struct ObjectFactory
  {
    DLL_DECL IMockChannel* createMockChannel(std::string ip, int port);
  };
}

#endif