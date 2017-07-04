///////////////////////////////////////////////////////////////////////
// Server.cpp -  Server acts as a Remote repository					 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

#include "Server.h"

/* ----< Builds XML for the file and stores the metadata >---- */

void Receiver::buildXML(std::string path, std::string file_, std::string depend)
{
	using sPtr = std::shared_ptr < AbstractXmlElement >;
	sPtr mtdata = makeTaggedElement("metadata");
	sPtr name1 = makeTaggedElement("name");
	mtdata->addChild(name1);
	name1->addChild(makeTextElement(file_));
	sPtr deps = makeTaggedElement("deps");
	mtdata->addChild(deps);
	size_t pos = 0;
	while (pos != std::string::npos)
	{
		pos = depend.find(',');
		if (pos != std::string::npos)
		{
			sPtr dep1 = makeTaggedElement("dep");
			deps->addChild(dep1);
			dep1->addChild(makeTextElement(depend.substr(0, pos)));
			depend.erase(0, pos + 1);
		}
	}
	sPtr author = makeTaggedElement("author");
	mtdata->addChild(author);
	author->addChild(makeTextElement("Client1"));
	sPtr closed = makeTaggedElement("closed");
	mtdata->addChild(closed);
	closed->addChild(makeTextElement("true"));
	sPtr docE1 = makeDocElement(mtdata);
	if (FileSystem::Directory::exists(path))
	{
		FileSystem::File mtdata(path + "/metadata.xml");
		mtdata.open(FileSystem::File::out, FileSystem::File::binary);
		std::string mtdataString = docE1->toString();
		const size_t siz = mtdataString.size();
		FileSystem::Block blk;
		FileSystem::Byte tab2[1024];
		strncpy(tab2, mtdataString.c_str(), sizeof(tab2));
		tab2[sizeof(tab2) - 1] = 0;

		for (size_t i = 0; i < siz; i++)
			blk.push_back(tab2[i]);

		mtdata.putBlock(blk);
		mtdata.close();
	}
	std::cout << "\n MetaData for " + file_ + "\n" << docE1->toString();
	std::cout << "\n\n";
}

/* ----< Defines processing to frame messages >---- */

HttpMessage Receiver::readMessage(Socket& socket)
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
	std::cout << "\n\nServer received : \n"+ std::string(19, '=');
	std::cout<< "\n\n" +msg.toString() + "\n";
	if (msg.attributes()[0].first == "POST")
	{
		std::string command = msg.findValue("COMMAND");
		std::string strfiles;

		if (command == "CHECKIN")
		{
			return constructMsg(socket, msg);
		}
		else if (command == "GETFILES")
		{
			strfiles = getFiles();
			msg.removeAttribute("toAddr");
			msg.removeAttribute("fromAddr");
			msg.addAttribute(HttpMessage::Attribute("toAddr", "9080"));
			msg.addAttribute(HttpMessage::Attribute("fromAddr", "9081"));
			msg.addAttribute(HttpMessage::Attribute("getList", strfiles));
		}
		else if (command == "EXTRACT")
			return msg;
	}
	return msg;
}

/* ----< Parses the Message and stores the file >---- */

HttpMessage Receiver::constructMsg(Socket& socket, HttpMessage message)
{
	std::string filename = message.findValue("file");
	std::string dependency = message.findValue("DEPENDENCY");
	if (filename != "")
	{
		size_t contentSize;
		std::string sizeString = message.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		else
			return message;

		readFile(filename, contentSize, dependency, socket);
	}
	if (filename != "")
	{
		message.removeAttribute("content-length");
		std::string bodyString = "<file>" + filename + "</file>";
		std::string sizeString = Converter<size_t>::toString(bodyString.size());
		message.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		message.addBody(bodyString);
	}
	else
	{
		size_t numBytes = 0;
		size_t pos = message.findAttribute("content-length");
		if (pos < message.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(message.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			message.addBody(msgBody);
			delete[] buffer;
		}
	}
}

//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool Receiver::readFile(const std::string& filename, size_t fileSize, const std::string& depend, Socket& socket)
{
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	std::string year = Converter<int>::toString(now->tm_year + 1900);
	std::string mon = Converter<int>::toString(now->tm_mon + 1);
	std::string day = Converter<int>::toString(now->tm_mday);
	std::string hour = Converter<int>::toString(now->tm_hour);
	std::string fname = Utilities::StringHelper::trimFileName(filename);
	std::string dir = serverpath_ + fname + "_" + year + "_" + mon + "_" + day + "_" + hour + "_" + Converter<int>::toString(now->tm_min) + "_" + Converter<int>::toString(now->tm_sec);
	FileSystem::Directory::create(dir);

	std::string fqname = dir + "/" + fname;
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
	buildXML(dir, filename, depend);
	return true;
}

/* ----< Get Files from the server using file manager >---- */

std::string Receiver::getFiles()
{
	std::string stringFiles = ",";
	DataStore ds;
	FileMgr fm(serverpath_, ds);
	fm.search();
	std::vector<std::string> vecFiles = fm.getFiles();
	if (vecFiles.size()>0)
		stringFiles.clear();
	for each (auto str in vecFiles)
	{
		stringFiles.append(str);
		stringFiles.append(",");
	}
	return stringFiles;
}

/*----< receiver functionality is defined by this function >---- */

void Receiver::operator()(Socket socket)
{
	while (true)
	{
		//std::cout << "\n\n in server operator()";
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

/* ----< Make HTTP Style Message >---- */

HttpMessage MsgServer::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
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

//----< send message using socket >----------------------------------

void MsgServer::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	//std::cout << "\n\nServer received : \n" + msgString + "\n";
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgServer::sendFile(const std::string& filename, Socket& socket)
{
	FileSystem::FileInfo fi(filename);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(filename);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::9080");
	msg.addAttribute(HttpMessage::Attribute("file", Utilities::StringHelper::trimFileName(filename)));
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

/* ----< Returns dependencies for the given metadata filepath >---- */

std::string MsgServer::getDependency(std::string filepath)
{
	XmlParser parser(filepath);
	parser.verbose();
	XmlDocument* pDoc = parser.buildDocument();
	return parser.returnDependency();
}

/* ----< Processing Message Commands for Server >---- */

void MsgServer::processRequest(SocketConnecter& si, HttpMessage& request)
{
	while (!si.connect("localhost", 9080))
	{
		Show::write("\n server waiting to connect");
		::Sleep(100);
	}
	std::string command = request.findValue("COMMAND");
	std::string dependency = "";
	if (command == "EXTRACT")
	{
		std::cout << "\n EXTRACT command received..\n";
		std::string file = request.findValue("file");
		std::string checked = request.findValue("CHECKED");
		std::vector<std::string> vecDepend;
		if (checked == "1")
		{
			std::vector<std::string> temp = recursiveDependency(file);
			vecDepend.insert(vecDepend.end(), temp.begin(), temp.end());
		}
		for (std::string str : vecDepend)
			sendFile(str, si);
		sendFile(file, si);
		return;
	}
	sendMessage(request, si);
}


/* ----< Get layered Dependencies >---- */

std::vector<std::string> MsgServer::recursiveDependency(std::string file)
{
	const size_t last = file.find_last_of("\\/");
	std::string metadata;
	std::vector<std::string> vecDepend;
	if (std::string::npos != last)
	{
		metadata = file.substr(0, last + 1) + "\\metadata.xml";
	}
	std::string dependency = getDependency(metadata);
	//std::cout << "\n\n Dependency from XML Parser: " + dependency + "\n\n";

	if (dependency != "")
	{
		size_t pos = 0;
		do
		{
			pos = dependency.find(',');
			std::string depFile = dependency.substr(0, pos);
			int excess = depFile.find_last_of('\n');
			if (excess > 0)
			{
				depFile.erase(excess, depFile.size());
				vecDepend.push_back(depFile);
				dependency.erase(0, pos + 1);
			}
		} while (pos != std::string::npos);
		std::vector<std::string> temp1;
		for (std::string str : vecDepend)
		{
			std::vector<std::string> temp = recursiveDependency(str);
			for (std::string tmp : temp)
			{
				if (std::find(vecDepend.begin(), vecDepend.end(), tmp) != vecDepend.end())
					continue;
				else
					temp1.push_back(tmp);
			}

		}
		vecDepend.insert(vecDepend.end(), temp1.begin(), temp1.end());
	}
	return vecDepend;
}

/* ----< Starts Sender >---- */

void MsgServer::startSender()
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (true)
		{
			auto request = queue.deQ();
			//std::cout << "\ndequeued item: " + request.toString() + "\n";
			processRequest(si, request);
		}
	}
	catch (std::exception& ex)
	{
		Show::write("  Exception caught:");
		Show::write(std::string("\n  ") + ex.what() + "\n\n");
	}
}

/* ----< Starts Receiver >---- */

void MsgServer::startReceiver()
{
	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);

		sl.start(serverReceiver);

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

void MsgServer::startSenderThread()
{
	std::thread receiverThread(&MsgServer::startSender, this);
	receiverThread.detach();
}

void MsgServer::startReceiverThread()
{
	std::thread receiverThread(&MsgServer::startReceiver, this);
	receiverThread.detach();
}

//----< test stub >--------------------------------------------------

int main()
{
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");

	BlockingQueue<HttpMessage> msgQ;

	try
	{
		MsgServer server_(msgQ);
		server_.startReceiverThread();
		server_.startSenderThread();

		while (true)
		{
			//HttpMessage msg = msgQ.deQ();
			//Show::write("\n\n  server recvd message contents:\n" + msg.bodyString());
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}