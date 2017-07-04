///////////////////////////////////////////////////////////////////////
// MsgClient.cpp -  Implements client and send files to server		 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

#include "MsgClient.h"

//----< post message from WPF application >---------------

void MsgClient::postHMessage(const HttpMessage& msg)
{
	sendingQueue.enQ(msg);
}

/* ----< Starts Clients Sender and Receiver >---- */

void MsgClient::start()
{
	startSenderThread();
	startReceiverThread();
}

/* ----< here take request from blocking queue and process that request. >---- */

void MsgClient::startSender()
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (true)
		{
			HttpMessage request = sendingQueue.deQ();
			std::string command = request.findValue("COMMAND");
			std::string body = request.bodyString();
			processRequest(si, command, body);
		}
	}
	catch (std::exception& ex)
	{
		Show::write("  Exception caught:");
		Show::write(std::string("\n  ") + ex.what() + "\n\n");
	}
}

/* ----< start receiver and initialize socket on particular port. >---- */

void MsgClient::startReceiver()
{
	try
	{
		SocketSystem ss;
		SocketListener sl(9080, Socket::IP6);
		sl.start(clientReceiver);
		Show::write("press key to exit: ");
		std::cout.flush();
		std::cin.get();
	}
	catch (std::exception& ex)
	{
		Show::write("  Exception caught:");
		Show::write(std::string("\n  ") + ex.what() + "\n\n");
	}
}

/* ----< start sender thread. >---- */

void MsgClient::startSenderThread()
{
	std::thread clientThread(&MsgClient::startSender, this);
	clientThread.detach();
}

/* ----< start receiver thread >---- */

void MsgClient::startReceiverThread()
{
	std::thread serverThread(&MsgClient::startReceiver, this);
	serverThread.detach();
}

/* ----< Connecting to Server >---- */

bool MsgClient::Connect(SocketConnecter& si, std::string ipAddress, int port)
{
	while (!si.connect(ipAddress, port))
	{
		Show::write("\n client waiting to connect");
		::Sleep(100);
	}
	Show::write("\n Client connected to server" + ipAddress + ":" + Utilities::Converter<int>::toString(port));
	return true;
}

/* ----< Process Request recieved from client input >---- */

void MsgClient::processRequest(SocketConnecter& si, std::string request, std::string body)
{
	try
	{
		bool connected = Connect(si, "localhost", 8080);
		if (connected && request == "CHECKIN")
		{
			size_t pos = 0;
			std::string bod = body;
			std::vector<std::string> files;
			size_t pos1 = bod.find('|');
			std::string dependency = bod.substr(pos1 + 1, bod.size());
			bod.erase(pos1, bod.size());
			while ((pos = bod.find(',')) != std::string::npos)
			{
				files.push_back(bod.substr(0, pos));
				bod.erase(0, pos + 1);
			}
			for (size_t i = 0; i < files.size(); ++i)
			{
				Show::write("\n\n  sending file " + files[i]);
				sendFile(files[i], dependency, si);
			}
			// shut down server's client handler
			HttpMessage hMsg;
			hMsg = makeMessage(1, "quit", "toAddr:localhost:8080");
			sendMessage(hMsg, si);
			//Show::write("\n  All done folks");
		}
		else if (connected && request == "GETFILES")
		{
			HttpMessage messg = makeMessage(1, "", "localhost:8080");
			messg.addAttribute(HttpMessage::Attribute("COMMAND", "GETFILES"));
			sendMessage(messg, si);
		}
		else if (connected && request == "EXTRACT")
		{
			extractRequest(si, request, body);
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		Show::write("\n  " + std::string(exc.what()) + "\n\n");
	}
}

/*--- Sending Message to Server on recieving Extract Command ---*/
void MsgClient::extractRequest(SocketConnecter& si, std::string request, std::string body)
{
	std::string bod = body;
	size_t pos = 0;
	std::vector<std::string> exFiles;
	HttpMessage messg;
	size_t posDepend = bod.find('|');
	bool checked = false;
	if (posDepend != std::string::npos)
	{
		std::string dependency = bod.substr(posDepend + 1, bod.size());
		if (dependency == "DEPENDENCY")
			checked = true;
		bod.erase(posDepend + 1, bod.size());
	}
	while ((pos = bod.find(',')) != std::string::npos)
	{
		exFiles.push_back(bod.substr(0, pos));
		bod.erase(0, pos + 1);
	}
	for (size_t i = 0; i < exFiles.size(); ++i)
	{
		Show::write("\n\n  sending file request " + exFiles[i] + "for download");
		messg = makeMessage(1, "", "localhost:8080");
		messg.addAttribute(HttpMessage::Attribute("file", exFiles[i]));
		messg.addAttribute(HttpMessage::Attribute("CHECKED", Utilities::Converter<bool>::toString(checked)));
		messg.addAttribute(HttpMessage::Attribute("COMMAND", "EXTRACT"));
		sendMessage(messg, si);
	}
}

/* ----< WPF application will call this method to get message from receving queue >---- */

std::string MsgClient::getMessage()
{
	HttpMessage result = rcvQueue.deQ();

	std::string files = result.findValue("getList");
	return files;
}

/* ----< Defines processing to frame messages >---- */

HttpMessage ClientReceiver::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;
	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}
	if (msg.attributes()[0].first == "POST")
	{
		// is this a file message?
		std::string command = msg.findValue("COMMAND");
		if (command == "GETFILES")
		{
			return msg;
		}
		std::string filename = msg.findValue("file");
		if (filename != "")
		{
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;

			readFile(filename, contentSize, socket);
		}
		if (filename != "")
		{
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}
		else
		{
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}

/*--- POST Message to Server Reciever ---*/

HttpMessage ClientReceiver::messagePost(HttpMessage&msg, Socket& socket)
{
	return msg;
}

//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool ClientReceiver::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "../ExtractedFiles/" + filename ;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}

/* ----< receiver functionality is defined by this function >---- */

void ClientReceiver::operator()(Socket socket)
{
	//std::cout << "\n\n in Client operator() \n\n";
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

//----< factory for creating messages >------------------------------
/*
 * This function only creates one type of message for this demo.
 * - To do that the first argument is 1, e.g., index for the type of message to create.
 * - The body may be an empty string.
 * - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
 *   expects the receiver EndPoint for the toAddr attribute.
 */
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
  HttpMessage msg;
  HttpMessage::Attribute attrib;
  EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
                                           // given to its constructor.
  switch (n)
  {
  case 1:
    msg.clear();
    msg.addAttribute(HttpMessage::attribute("POST", "Message"));
    msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
    msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
    msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
    msg.addBody(body);
    if (body.size() > 0)
    {
      attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
      msg.addAttribute(attrib);
    }
    break;
  default:
    msg.clear();
    msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
  }
  return msg;
}

/* ----< send message using socket >---- */

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
  std::string msgString = msg.toString();
  std::cout << "\n\nClient sending Message: \n";
  std::cout << std::string(24, '=');
  std::cout << "\n\n"+ msgString + "\n\n";
  socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}

//----< send file using socket >-------------------------------------
/*
 * - Sends a message to tell receiver a file is coming.
 * - Then sends a stream of bytes until the entire file
 *   has been sent.
 * - Sends in binary mode which works for either text or binary.
 */
bool MsgClient::sendFile(const std::string& filename,const std::string& depend, Socket& socket)
{
  // assumes that socket is connected

  //std::string fqname = "../TestFiles/" + filename;
  FileSystem::FileInfo fi(filename);
  size_t fileSize = fi.size();
  std::string sizeString = Converter<size_t>::toString(fileSize);
  FileSystem::File file(filename);
  file.open(FileSystem::File::in, FileSystem::File::binary);
  if (!file.isGood())
    return false;
  
  HttpMessage msg = makeMessage(1, "", "localhost::8080");
  msg.addAttribute(HttpMessage::Attribute("COMMAND", "CHECKIN"));
  msg.addAttribute(HttpMessage::Attribute("DEPENDENCY", depend));
  msg.addAttribute(HttpMessage::Attribute("file", filename));
  msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
  sendMessage(msg, socket);
  const size_t BlockSize = 2048;
  Socket::byte buffer[BlockSize];
  while (true)
  {
    FileSystem::Block blk = file.getBlock(BlockSize);
    if (blk.size() == 0)
      break;
    for (size_t i = 0; i < blk.size(); ++i)
      buffer[i] = blk[i];
    socket.send(blk.size(), buffer);
    if (!file.isGood())
      break;
  }
  file.close();
  return true;
}

/* ----< Test Stub >---- */

int main()
{
  ::SetConsoleTitle(L"Clients Running on Threads");

  Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

}