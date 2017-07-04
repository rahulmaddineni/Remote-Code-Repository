#ifndef MSGCLIENT_H
#define MSGCLIENT_H

///////////////////////////////////////////////////////////////////////
// MsgClient.h -  Implements client and send files to server		 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

/*
Module operations:
==================
* This module implements a client that sends HTTP style messages and
* files to a server and recieves them

Public Interface:
=================
*	void start() - starts sender and receiver threads upon request
*	void startSender() - starts client sender i.e socketconnector to send messages to server
*	bool Connect() - connects to the server upon request
*	void processRequest() - processes the request 
*	void MsgClient::postHMessage() -
*	void startReceiver() - starts client receiver i.e socketlistener to receive messages from server
*	void startSenderThread() -
*	void startReceiverThread() -
*	std::string MsgClient::getMessage()-
*	BlockingQueue<HttpMessage> getQ() -

Build Process:
==============
Required Files:
*   Server.h, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp

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
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;


class ClientReceiver
{
public:
	ClientReceiver(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
	HttpMessage messagePost(HttpMessage&msg, Socket& socket);
};

class MsgClient
{
public:
	using EndPoint = std::string;
	MsgClient(std::string ip, int port):ip_(ip),port_(port),clientReceiver(rcvQueue) {}
	void start();
	void startSender();
	bool Connect(SocketConnecter& si, std::string ipAddress, int port);
	void processRequest(SocketConnecter& si, std::string msg, std::string body);
	void MsgClient::postHMessage(const HttpMessage& msg);
	void startReceiver();
	void startSenderThread();
	void startReceiverThread();
	std::string MsgClient::getMessage();
	BlockingQueue<HttpMessage> getQ() { return sendingQueue; }
private:
	EndPoint endpoint;
	std::string ip_;
	int port_;
	BlockingQueue<HttpMessage> sendingQueue;
	BlockingQueue<HttpMessage> rcvQueue;
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, const std::string& dep, Socket& socket);
	ClientReceiver clientReceiver;
	void extractRequest(SocketConnecter& si, std::string request, std::string body);
};

#endif