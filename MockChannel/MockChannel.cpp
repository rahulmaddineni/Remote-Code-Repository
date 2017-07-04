///////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Build as DLL for C++\CLI client to use          //
//				     native code channel                             //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "MockChannel.h"
#include <string>
#include <iostream>
#include "../MsgClient/MsgClient.h"

class MockChannel : public IMockChannel
{
public:
	MockChannel(std::string ip, int port);
	void start();
	void postMessage(const std::string& msg, const std::string& body);
	std::string getMessage();
private:
	MsgClient client_;
	bool stop_ = false;
};

//----< Mockchannel Constructor >-------------------------------

MockChannel::MockChannel(std::string ip, int port) : client_(ip,port){}

//----< Starts the client >-------

void MockChannel::start()
{
  std::cout << "\n  MockChannel starting up";
  client_.start();
}

//----< Sends commands from WPF >----

void MockChannel::postMessage(const std::string& msg, const std::string& body) 
{ 
	HttpMessage messge;
	messge.addAttribute(HttpMessage::attribute("COMMAND", msg));	
	messge.addBody(body);
	client_.postHMessage(messge);
}

// ----< Recieves Files from the server >----

std::string MockChannel::getMessage()
{
	return client_.getMessage(); 
}

IMockChannel* ObjectFactory::createMockChannel(std::string ip, int port)
{ 
  return new MockChannel(ip,port); 
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  IMockChannel* pMockChannel = objFact.createMockChannel(ip,port);
  pMockChannel->start();
  pMockChannel->postMessage("GETFILES", "");
  std::string msg = pMockChannel->getMessage();
  std::cout << "\n  received files from server = \"" << msg << "\"";
}
#endif
