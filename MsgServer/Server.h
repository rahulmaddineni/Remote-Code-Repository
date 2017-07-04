#ifndef MSGSERVER_H
#define MSGSERVER_H

///////////////////////////////////////////////////////////////////////
// Server.h -  Server acts as a Remote repository					 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
* This module implements a server that receives HTTP style messages and
* files from clients and simply displays the messages and stores files.

Public Interface:
=================
*	void startSenderThread()                 - starts server sender i.e socketconnector to send messages to client
*	void startReceiverThread()               - starts server receiver i.e socketlistener to receive messages from client
*	void processRequest()	                 - processes the HTTP messages received by client
*	BlockingQueue<HttpMessage>& getServerQ() - return the Blocking queue reference
*   void operator()(Socket socket)           - operator overloading

Build Process:
==============
Required Files:
*   MsgClient.h, MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h, DataStore.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp, FileMgr.h
*   Utilities.h, Utilities.cpp
*	XmlElement.h, XmlElement.cpp
*	XmlParser.h, XmlParser.cpp

Build commands:
===============
- devenv project4.sln

Maintenance History:
====================
ver 1.0 : 03 May 16
- first release

*/

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/FileMgr.h"
#include "../FileSystem/DataStore.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "XmlElement.h"
#include "../XmlParser/XmlParser.h"
#include <string>
#include <iostream>
#include <ctime>

using Show = StaticLogger<1>;
using namespace Utilities;
using namespace XmlProcessing;


////////////////////////////////////////////////////////////////////////
// Receiver class
///////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.

class Receiver
{
public:
	Receiver(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
private:
	bool connectionClosed_;
	std::string serverpath_ = "../ServerFiles/";
	HttpMessage readMessage(Socket& socket);
	std::string getFiles();
	HttpMessage constructMsg(Socket& socket, HttpMessage message);
	bool readFile(const std::string& filename, size_t fileSize, const std::string& depend, Socket& socket);
	void buildXML(std::string path, std::string file, std::string depend);
	BlockingQueue<HttpMessage>& msgQ_;
};

////////////////////////////////////////////////////////////////////////
// MsgServer class
///////////////////////////////////////////////////////////////////////
// - instances of this class are passed to SocketConnector
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.

class MsgServer
{
public:
	using EndPoint = std::string;
	MsgServer(BlockingQueue<HttpMessage>& q) : queue(q), serverReceiver(queue) {}
	void startSenderThread();
	void startReceiverThread();
	void processRequest(SocketConnecter& si, HttpMessage& request);
	BlockingQueue<HttpMessage>& getServerQ() { return queue; }
private:
	BlockingQueue<HttpMessage> queue;
	
	void startSender();
	void startReceiver();
	std::vector<std::string> recursiveDependency(std::string file);
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
	std::string getDependency(std::string filepath);
	Receiver serverReceiver;
};

#endif