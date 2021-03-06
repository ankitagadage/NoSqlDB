/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Ankita Gadage, E687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Project2/RepositoryCore/RepositoryCore.h"
#include <chrono>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;
//get file message handler
Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}
//get directory message handler
Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}
//function to show message

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}
//function to handler get connection request
std::function<Msg(Msg)> estConnection = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("estConnection");
  reply.attribute("content","Connected to Server 8080...");
  return reply;
};

//function to handler get connection request
std::function<Msg(Msg)> getAuthor = [](Msg msg) {
	Msg reply ;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getAuthor");
	int currentVersionState = -1;
	currentVersionState = Version::getLatestVersion(RepoCoreProvider::getRepoCore()->getDbCore(), msg.value("key"), currentVersionState);

	if (currentVersionState == 0)
	{
		reply.attribute("isValid","true");
		return reply;


	}
	else
	{
		std::string newKey = msg.value("key") + "."+std::to_string(currentVersionState);
		std::string name = RepoCoreProvider::getRepoCore()->getDbCore()[newKey].name();
		bool i = RepoCoreProvider::getRepoCore()->getDbCore()[newKey].name().compare(msg.value("name"));
		if (name.compare(msg.value("name")) != 0)
		{
			reply.attribute("isValid", "false");
			return reply;


		}
	}
	reply.attribute("isValid", "true");

	return reply;
};

//function to handler get file request
std::function<Msg(Msg)> getFile = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFile");
	std::string path = msg.value("path");
	size_t pos = path.find_last_of("/");
	std::string fileName = path.substr(pos + 1, path.length());
	std::string dest = "../CppCommWithFileXfer/SendFiles/"+ fileName;
	FileSystem::File::copy(path,dest);
	reply.attribute("file", fileName);
	reply.attribute("isChildRequest",msg.value("isChildRequest"));
	std::string key = regex_replace(path.erase(3, 0), std::regex("/"), "::");
	if(msg.value("isChildRequest").compare("false")==0)
	{
		int childCount = RepoCoreProvider::getRepoCore()->getDbCore()[key].children().size();
		reply.attribute("childCount", std::to_string(childCount));

		if (childCount > 0)
		{
			for (int i = 0; i < childCount; i++)
			{
				reply.attribute("child" + std::to_string(i), RepoCoreProvider::getRepoCore()->getDbCore()[key].payLoad().value());

			}

		}
	}

	return reply;
};

//function to handler echo message

std::function<Msg(Msg)> echo = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	return reply;
};

//function to handler get parent files message

std::function<Msg(Msg)> getIndependentFiles = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	std::vector<std::string> finalList;
	std::string msgString;
	for (unsigned int i = 0; i < RepoCoreProvider::getRepoCore()->getDbCore().keys().size(); i++)
	{
		bool flag = false;
		std::string key = RepoCoreProvider::getRepoCore()->getDbCore().keys()[i];

		for (unsigned int j = 0; j < RepoCoreProvider::getRepoCore()->getDbCore().keys().size(); j++)
		{
			std::string key1 = RepoCoreProvider::getRepoCore()->getDbCore().keys()[j];

			if (RepoCoreProvider::getRepoCore()->getDbCore()[key1].containsChildKey(key)==true)
			{
				flag = true;
				break;
			}
		}
		if (flag == false)
		{
			finalList.push_back(key); ;
		}


	}
	if (finalList.size() > 0)
	{
		msgString = finalList[0];
		for (unsigned int i = 1; i < finalList.size(); i++)
		{
			msgString = msgString + "#" + finalList[i];
		}
		reply.attribute("idplist", msgString);

	}
	else
	{
		reply.attribute("idplist", "NotPresent");

	}


	return reply;
};


//function to handler checkin request from client

std::function<Msg(Msg)> checkin = [](Msg msg) {
	Msg reply ;
	FileSystem::File::copy("../CppCommWithFileXfer/ClientSaveFiles/"+msg.value("file"),msg.value("path") + "/" + msg.value("file"));
	
	return reply;
};


//splitting string as per the delimiter
std::vector<std::string> splitStringWithdelimiter(const std::string& str, const char& ch) {
	std::string next;
	std::vector<std::string> result;

	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (*it == ch) {
			if (!next.empty()) {
				result.push_back(next);
				next.clear();
			}
		}
		else {
			next += *it;
		}
	}
	if (!next.empty())
		result.push_back(next);
	return result;
}


std::function<Msg(Msg)> BrowseReq = [](Msg msg) {
	Msg reply;
	std::vector<std::string> vec_category, vec_filename, vec_version, vec_children, vec_keys;
	if (msg.containsKey("category"))
		vec_category = splitStringWithdelimiter(msg.value("category"), ';');
	if (msg.containsKey("filename"))
		vec_filename = splitStringWithdelimiter(msg.value("filename"), ';');
	if (msg.containsKey("dependency"))
		vec_children = splitStringWithdelimiter(msg.value("dependency"), ';');
	if (msg.containsKey("version"))
		vec_version = splitStringWithdelimiter(msg.value("version"), ';');

	vec_keys = RepoCoreProvider::getRepoCore()->BrowseOnMutipleInput(vec_category, vec_filename, vec_version, vec_children);
	int i = 1;
	if (vec_keys.size() > 0)
	{
		std::string content;
		for (auto x : vec_keys)
		{
			reply.attribute("file" + std::to_string(i), x);
			i++;
			content = content + x + ";";
		}
		reply.attribute("content", "Server -> Filtered Files are " + content);
	}
	else
	{
		reply.attribute("content", "Server -> Sorry No Results - Change Filter Criteria ");
		reply.attribute("NoFile", "NoFile");
	}
	reply.attribute("tabname", "browse");
	reply.to(msg.from());
	reply.from(msg.to()); reply.command("BrowseReply");

	return reply;

};


//function to handler get metadata request from client
std::function<Msg(Msg)> getMetadata = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getMetadata");
	std::string key = regex_replace(msg.value("path").erase(0, 11), std::regex("/"), "::");
	DbElement<PayLoad> dbElem = RepoCoreProvider::getRepoCore()->getDbCore()[key];
	reply.attribute("Name", msg.value("Name"));

	reply.attribute("DateTime", dbElem.dateTime());
	reply.attribute("Description", dbElem.descrip());
	if (dbElem.children().size() > 0)
	{
		std::string children = dbElem.children()[0];

		for (unsigned int i = 1; i < dbElem.children().size(); i++)
		{

			children = children + dbElem.children()[i];
		}
		reply.attribute("Dependency", children);


	}
	else
	{
		reply.attribute("Dependency", "None");

	}

	return reply;
};
//funtion to send  checkin complete response to client
std::function<Msg(Msg)> checkinDone = [](Msg msg) {
	
	Msg reply;
	std::cout << "Checkin Response" << std::endl;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("CheckinResp");
	RepoCoreProvider::getRepoCore()->displayDb();
	NoSqlDb::DbElement<PayLoad> dbElem;

	dbElem.name(msg.value("author"));
	if (msg.value("checkinStatus").compare("open") == 0)
		dbElem.payLoad().setCheckinStatus(open);
	else
		dbElem.payLoad().setCheckinStatus(closed);

	dbElem.descrip(msg.value("description"));
	dbElem.dateTime(DateTime().now());
	dbElem.payLoad().value("../CppCommWithFileXfer/ClientSaveFiles/" + msg.value("fileName"));
	std::string s = msg.value("category");
	std::vector<std::string> result;
	std::istringstream iss(s);
	for (std::string s; iss >> s; )
		dbElem.payLoad().categories().push_back(s);
	dbElem.payLoad().setNamespace(msg.value("namespace"));
	int childCount = stoi(msg.value("childCount"));
	for (int i = 0; i < childCount; i++)
	{
		std::string childName = (msg.value("child" + std::to_string(i))).erase(0, 11);
		dbElem.children().push_back(regex_replace(childName, std::regex("/"), "::"));
	}
	RepoCoreProvider::getRepoCore()->checkinFile(dbElem);
	RepoCoreProvider::getRepoCore()->displayDb();
	reply.attribute("fileName", msg.value("fileName"));
	return reply;
};
//funtion to send  checkin complete response to client
std::function<Msg(Msg)> getChildren = [](Msg msg) {

	Msg reply;
	std::cout << "children Response" << std::endl;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getChildResp");
	std::string msgPath = msg.value("path");
	std::string childFilePath = msgPath.erase(0, 11);
	std::string key = regex_replace(childFilePath, std::regex("/"), "::");
	std::string Parentkey = msg.value("PFileKey") ;
	int currentVersionState = -1;
	currentVersionState = Version::getLatestVersion(RepoCoreProvider::getRepoCore()->getDbCore(), Parentkey, currentVersionState);

	if (currentVersionState == 0)
		Parentkey = Parentkey + ".1";
	
	else
	{
		Parentkey = Parentkey + "."+ std::to_string(currentVersionState);
		if (RepoCoreProvider::getRepoCore()->getDbCore()[Parentkey].payLoad().getCheckinStatus() == open)
		{
			DbCore<PayLoad> obj = RepoCoreProvider::getRepoCore()->getDbCore();
			DbElement<PayLoad> elem = RepoCoreProvider::getRepoCore()->getDbCore()[key];
			int childCount = RepoCoreProvider::getRepoCore()->getDbCore()[key].children().size();
			if (childCount > 0)
			{
				for (int i = 0; i<childCount; i++)
				{
					std::string childName = RepoCoreProvider::getRepoCore()->getDbCore()[key].children()[i];
					int length = childName.length();
					if (childName.compare(Parentkey) == 0)
					{
						reply.attribute("DependencyStatus", "Present");
						return reply;
					}
				}

			}
		}
		

	}
	if (key.compare(Parentkey) == 0)
	{
		reply.attribute("DependencyStatus", "Present");
		return reply;

	}
	reply.attribute("DependencyStatus", "NotPresent");
	return reply;

};

// function which is used to handle get mutliple files request from client
std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};
//function to send directory list to client
std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};
// main start of the project
int main()
{
  std::cout << "\n  Starting Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";

  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  RepoCoreProvider::getRepoCore()->readXml("../Project2/DBInput.xml");
  RepoCoreProvider::getRepoCore()->displayDb();

  server.addMsgProc("echo", echo);
  server.addMsgProc("estConnection", estConnection);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getFile", getFile);
  server.addMsgProc("checkin", checkin);
  server.addMsgProc("getChildren", getChildren);
  server.addMsgProc("BrowseReq", BrowseReq);
  server.addMsgProc("getIndependentFile", getIndependentFiles);



  server.addMsgProc("checkinDone",checkinDone);
  server.addMsgProc("getMetadata", getMetadata);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("getAuthor", getAuthor);

  server.processMessages();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "\n  press enter to exit";
  std::cin.get();
  server.stop();
  return 0;
}

