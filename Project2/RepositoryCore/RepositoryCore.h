#pragma once
////////////////////////////////////////////////////////////////////////////
// RepositoryCore.h - Has design of Repository Core which provides checkin checkout and versioning  //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                  //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018          //
///////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- RepositiryCore : Template Class ,  used to checkIn , CheckOut and Version,brwosing a File
void checkinFiles(std::vector<DbElement<T>> dbElements);
void readXml(std::string path);
void createDirectory(const std::string& path, std::string& remainder, std::size_t offset);
inline std::vector<std::string> checkOut(const std::string& chekoutKey, int ver=0);
std::vector<std::string> getFilesToClosed(std::vector<std::string>& filesToClose,std::map<std::string, std::vector<std::string>>& depMap, std::string keyToSearch);
DbCore<T> BrowseFiles(std::vector<std::string> & categoryName);
std::vector<std::string> getDependencyOfFiles(std::vector<std::string> keys);
void readDependencyfromXML(std::string path);
void displayDb();
void checkinFile(DbElement<T> &dbElem);
int dependencyhandler(const std::string & childKey, DbElement<T> db, const std::string& tempKey);
void closeDependencies(DbCore<T> &db, std::vector<std::string>& keys);
std::string getRepoDest(const std::string& source, const std::string & namespace_);
void checkInMultipleFiles(std::vector<DbElement<T>>& dbElemList);
void BrowseFiles(const std::string &browseKey, int version=0);
void persistDb(std::string path);
inline void saveMaptoXML(std::string path);
size_t directoryExists(const std::string& path);
~RepositoryCore();

* Required Files:
* ---------------
* Executive.h, CheckIn.cpp
* CheckOut.h, RepoXmlUtil.h
* Browse.h, Version.h

*
*
* Maintenance History:
* --------------------
* ver 1.0 : 2nd Mar 2018

*/
#include <string>
#include <algorithm>
#include <regex>
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"
#include "../NoSQLDB/XmlDocument/XmlDocument/XmlDocument.h"
#include "../NoSQLDB/XmlDocument/XmlParser/XmlParser.h"
#include "../Checkout/Checkout.h"
#include "../Version/Version.h"
#include "../FileMgr/FileMgr.h"
#include "../Version/Version.h"
#include "../Checkin/Checkin.h"
#include "../RepoXmlUtil/RepoXmlUtil.h"
#include "../Browse/Browse.h"

using namespace NoSqlDb;

//provide the interface to checkin checkout nad version
template <typename T>
class RepositoryCore
{
public:
	RepositoryCore(DbCore<T>& db) :db_(db) {};
	RepositoryCore() {};
	DbCore<T>& getDbCore();
	static std::string ROOT_PATH;
	static std::string CHECKOUT_PATH;
	void checkinFiles(std::vector<DbElement<T>> dbElements);
	void readXml(std::string path);
	DbCore<T> BrowseOnCategory(std::vector<std::string>& category_name, int flag = 0);
	DbCore<T> BrowseOnFileName(std::vector<std::string> & filename, DbCore<T>& db);
	DbCore<T> BrowseOnVersion(std::vector<std::string> & version, DbCore<T>& db);
	DbCore<T> BrowseOnChildren(std::vector<std::string> & children, DbCore<T>& db);

	std::vector<std::string> BrowseOnMutipleInput(std::vector<std::string> & category_name, std::vector<std::string> & filename, std::vector<std::string> & version, std::vector<std::string> & children);
	void createDirectory(const std::string& path, std::string& remainder, std::size_t offset);
	inline std::vector<std::string> checkOut(const std::string& chekoutKey, int ver=0);
	std::vector<std::string> getFilesToClosed(std::vector<std::string>& filesToClose,std::map<std::string, std::vector<std::string>>& depMap, std::string keyToSearch);
	DbCore<T> BrowseFiles(std::vector<std::string> & categoryName);
	std::vector<std::string> getDependencyOfFiles(std::vector<std::string> keys);
	void readDependencyfromXML(std::string path);
	void displayDb();
	void checkinFile(DbElement<T> &dbElem);
	int dependencyhandler(const std::string & childKey, DbElement<T> db, const std::string& tempKey);
	void closeDependencies(DbCore<T> &db, std::vector<std::string>& keys);
	std::string getRepoDest(const std::string& source, const std::string & namespace_);
	void checkInMultipleFiles(std::vector<DbElement<T>>& dbElemList);
	void BrowseFiles(const std::string &browseKey, int version=0);
	void persistDb(std::string path);
	inline void saveMaptoXML(std::string path);
	size_t directoryExists(const std::string& path);
	int  isFilePresent(std::string key);

	~RepositoryCore();
private:
	DbCore<T> db_;
	Checkin<T> ck_;
	Checkout ckot_;
	RepoXmlUtil<T> xp_;
	Browse<T> br_;
	std::map<std::string, std::vector<std::string>> dependencyRelationMap;

};
//static initilizaton
template<typename T>
std::string RepositoryCore<T>::ROOT_PATH = "../Storage";
template<typename T>
std::string RepositoryCore<T>::CHECKOUT_PATH = "../stage";
template<typename T>
//This function creates the destination path
inline std::string RepositoryCore<T>::getRepoDest(const std::string& source, const std::string & namespace_)
{
	size_t start_slash = source.rfind("/");
	std::string dest = this->ROOT_PATH + "/" + namespace_ + "/" + source.substr(start_slash + 1, source.size());
	return dest;
}


//This function finds the series of parent a particular key has.
template<typename T>
inline std::vector<std::string> RepositoryCore<T>::getDependencyOfFiles(std::vector<std::string> keys)
{
	for (size_t i = 0; i < keys.size(); i++)
	{
		for (auto y : db_[keys[i]].children())
		{
			auto temp_it = find(keys.begin(), keys.end(), y);
			if (temp_it == keys.end())
			{
				keys.push_back(y);
			}
		}

	}
	return keys;
}


//This fucntions check in the all DBelemnts present in a vector.
template<typename T>
inline void RepositoryCore<T>::checkInMultipleFiles(std::vector<DbElement<T>>& dbElemList)
{
	for (auto x : dbElemList)
	{
		checkinFile(x); 
	}
}

//This function will iterate over dependency map and check whick key is value free.
template<typename T>
inline std::vector<std::string> RepositoryCore<T>::getFilesToClosed(std::vector<std::string>& filesToClose, std::map<std::string, std::vector<std::string>>& depMap, std::string keyToSearch)
{
	int iSize = depMap.size();
	
	for (auto itr = depMap.begin(); itr != depMap.end(); ++itr)
	{
		iSize--;
		std::vector<std::string> value = depMap[itr->first];
		int length = value.size();
		int count = 0;
		for (auto i : value)
		{
			if (i == keyToSearch) 
			{
				length--;
				depMap[itr->first].erase(depMap[itr->first].begin() + count);
			}
			count++;
		}
		if (length == 0) 
		{
			filesToClose.push_back(itr->first);
		}
	}
	for (auto x : filesToClose)
		depMap.erase(x);

	return filesToClose;
}


template<typename T>
//This functions to close the dependent files
inline void RepositoryCore<T>::closeDependencies(DbCore<T> &db, std::vector<std::string>& keys)
{
	if (keys.size() > 0)
	{
		int count = 0;
		for (auto x : keys)
		{
			int currentVersionState = -1;
			int current_version = Version::getLatestVersion(db, x, currentVersionState);
			std::string last_Version = x; 
			T t = db[last_Version].payLoad();
			std::string destination = getRepoDest(t.value(), t.getNamespace());
			ck_.copyFile(t.value(), destination + "." + std::to_string(current_version), t.getNamespace(), ROOT_PATH);
			db[last_Version].payLoad().setCheckinStatus(closed);

			keys.erase(keys.begin() + count);
			count++;
			keys = getFilesToClosed(keys, this->dependencyRelationMap, x);
			closeDependencies(db,keys);
			if (keys.size() == 0)
				break;
		}
	}
}



//this function call the checkout class opertion to perform checkout
template<typename T>
inline std::vector<std::string> RepositoryCore<T>::checkOut(const std::string& chekoutKey, int ver)
{
	int specVersion;
	std::vector<std::string> dependencyList;
	int latestverion = -1;
	specVersion = Version::getLatestVersion(db_, chekoutKey, latestverion);
	if (ver != 0) 
	{
		if (ver > specVersion)
			return dependencyList;
		specVersion = ver+1;
	}
	dependencyList.push_back(chekoutKey + "." + std::to_string(specVersion));
	ckot_.copyFileToStage(getDependencyOfFiles(dependencyList), ROOT_PATH, CHECKOUT_PATH);
	return dependencyList;
}

//adding dp elemet to the database
template<typename T>
inline void RepositoryCore<T>::checkinFile(DbElement<T> &dbElem)
{
	T t = dbElem.payLoad();
	std::string source = t.value();std::string namespace_ = t.getNamespace();std::string destination = getRepoDest(source, namespace_);
	std::string keyRegEx = namespace_ + "::" + source.substr(source.rfind("/") + 1, source.size());
	int versionNumber = Version::getVersion(db_, source, namespace_, dbElem);int lastVersionState = -1;int lVer = Version::getLatestVersion(db_, keyRegEx, lastVersionState); 
	if (lastVersionState == 0) 
		versionNumber = versionNumber + 1;
	if (/*versionNumber != 1*/ lVer !=0) {
		std::string tempKey = namespace_ + "::" + source.substr(source.rfind("/") + 1, source.size()) + "." + std::to_string(versionNumber);
		if ((dbElem.payLoad().getCheckinStatus() == 0 && lastVersionState == 0) || (t.getCheckinStatus() == 0 && lastVersionState == 1)) { //Conditions where user sttaus of file is closed against server status of open and close.
			std::string childKey = namespace_ + "::" + source.substr(source.rfind("/") + 1, source.size()) + "." + std::to_string(versionNumber);
			std::vector<std::string> vec;
			int isDependencyOpen = 0;
			if (dbElem.children().size() >= 1)  isDependencyOpen = dependencyhandler(childKey, dbElem, tempKey); 
			if (isDependencyOpen == 0) {
				if (lastVersionState = 1) { 
					db_[tempKey].name() = dbElem.name(); db_[tempKey].descrip() = dbElem.descrip(); db_[tempKey].dateTime() = dbElem.dateTime();
					db_[tempKey].children() = dbElem.children();db_[tempKey].payLoad() = dbElem.payLoad();
				}
				else
					db_[tempKey] = dbElem;
				ck_.copyFile(source, destination + "." + std::to_string(versionNumber), namespace_, ROOT_PATH);
			}
			if (isDependencyOpen == 0) { 
				std::vector<std::string> filesToClose;
				getFilesToClosed(filesToClose,dependencyRelationMap, namespace_ + "::" + source.substr(source.rfind("/") + 1, source.size()) + "." + std::to_string(versionNumber));closeDependencies(db_, filesToClose);
			}
		}
		else if (t.getCheckinStatus() == 1 && lastVersionState == 0) { 
			db_[tempKey] = dbElem;ck_.copyFile(source, destination + "." + std::to_string(versionNumber), namespace_, ROOT_PATH);
		}
		else if (t.getCheckinStatus() == 1 && lastVersionState == 1) {
			db_[tempKey].name() = dbElem.name();db_[tempKey].descrip() = dbElem.descrip();db_[tempKey].dateTime() = dbElem.dateTime();
			db_[tempKey].children() = dbElem.children();db_[tempKey].payLoad() = dbElem.payLoad();
			ck_.copyFile(source, destination + "." + std::to_string(versionNumber), namespace_, ROOT_PATH);
		}
	}
	else { 
		std::string tempKey = namespace_ + "::" + source.substr(source.rfind("/") + 1, source.size()) + "." + std::to_string(versionNumber);dbElem.payLoad().value(destination);
		db_[tempKey] = dbElem;ck_.copyFile(source, destination + "." + std::to_string(versionNumber), namespace_, ROOT_PATH);
	}
}

//dependency handler to find all the dependencies of the file
template<typename T>
inline int RepositoryCore<T>::dependencyhandler(const std::string & childKey, DbElement<T> db, const std::string& tempKey)
{
	int isDependencyOpen = 0;
	for (auto x : db.children())
	{
		int latestVersionState, version;
		version = Version::getLatestVersion(db_, x, latestVersionState);
		if (latestVersionState == 1)
		{
			dependencyRelationMap[childKey].push_back(x); 
			isDependencyOpen = 1;
		}
	}

	if (isDependencyOpen == 1) 
	{
		db_[tempKey] = db;
		db_[tempKey].payLoad().setCheckinStatus(closing); 
	}

	return isDependencyOpen;
}


template<typename T>
inline DbCore<T>& RepositoryCore<T>::getDbCore() {
	return db_;
}

template<typename T>
inline void RepositoryCore<T>::readDependencyfromXML(std::string path)
{
	xp_.readDependencyMapfromXML(dependencyRelationMap, path);
}

//This functions shows the CodeRepo DB.
template<typename T>
inline void RepositoryCore<T>::displayDb()
{
	showDb(db_); //Printing DB .
}
template<typename T>
inline void RepositoryCore<T>::createDirectory(const std::string& path, std::string& remainder, std::size_t offset) {
	std::string val = path;
	while (remainder != "") {
		std::size_t remainOffset = remainder.find_first_of("/") + 1;
		val = val.substr(0, offset + 1).append(remainder.substr(0, remainOffset));        
		FileSystem::Directory::create(val);
		remainder = remainder.substr(remainOffset);
		offset += remainOffset;
	}
}

//checking if directory exists
template<typename T>
inline size_t RepositoryCore<T>::directoryExists(const std::string& path) {
	bool result = FileSystem::Directory::exists(path);
	std::size_t found = path.size();

	if (!result) {
		std::size_t found = path.find_last_of("/");
		return directoryExists(path.substr(0, found));
	}
	else {
		return found;
	}

}

//check in multiple files
template<typename T>
inline void RepositoryCore<T>::checkinFiles(std::vector<DbElement<T>> dbElements)
{
	for (size_t i = 0; i < dbElements.size(); i++) {
		size_t indexofFileNameStart = dbElements[i].payLoad().value().find_last_of("/");
		std::string fileName = dbElements[i].payLoad().value().substr(indexofFileNameStart + 1, dbElements[i].payLoad().value().size());
		std::string key = dbElements[i].payLoad().getNamespace() + fileName;
		std::string dest = "../root" + regex_replace(dbElements[i].payLoad().getNamespace(), std::regex("::"), "/");


		int fileVersion = ck_.isFilePresent(key);
		if (fileVersion > 0)
		{
			ck_.acceptFile(dbElements[i], dest, fileVersion);
		}
		else
		{
			bool result = FileSystem::File::exists(dest);
			std::size_t dirUpto = 0;
			if (!result) {
				std::size_t found = dest.find_last_of("/");
				dirUpto = directoryExists(dest.substr(0, found));
				std::string remainder = dest.substr(dirUpto + 1, dest.substr(dirUpto + 1).find_last_of("/") + 1);	 // get ride of the file name
				createDirectory(dest, remainder, dirUpto);

			}

			ck_.addFile(dbElements[i], dest);
		}
	}
	
}

//----< This function is  used to read XML >--------------------

template<typename T>

inline void RepositoryCore<T>::readXml(std::string path)
{
	xp_.readDbFromXml(db_, path);
}


//----< This function is  used to save DB to XML >--------------------

template<typename T>
inline void RepositoryCore<T>::persistDb(std::string path)
{
	xp_.saveDbToXml(this->db_, path);
}

//----< This function is  used to save the dependency map to XML >--------------------

template<typename T>
inline void RepositoryCore<T>::saveMaptoXML(std::string path)
{
	xp_.saveDependencyMaptoXML(this->dependencyRelationMap, path);
}

//----< Browsing funtionality >--------------------

template<typename T>
inline DbCore<T> RepositoryCore<T>::BrowseFiles(std::vector<std::string> & categoryName)
{
	DbCore<T> tempDb;
	if (categoryName.size() > 0){
		for (auto x : categoryName){
			std::vector<std::string> keys;
			keys = br_.browseOnCategory(db_, x); 
			if (keys.size() > 0){
				for (auto i : keys){
					tempDb[i] = db_[i];
				}
			}
		}
	}
	return tempDb;
}

//----< Browsing specific file funtionality >--------------------

template<typename T>
inline void RepositoryCore<T>::BrowseFiles(const std::string &browseKey, int version)
{
	int specVersion;
	int latestVerion = -1;
	std::string filePath;
	specVersion = Version::getLatestVersion(db_, browseKey, latestVerion);

	if (version == 0)
	{
		filePath = br_.browseByName(browseKey + "." + std::to_string(specVersion), RepositoryCore<T>::ROOT_PATH, RepositoryCore<T>::CHECKOUT_PATH);
		br_.openNotePad(filePath);
	}
	else if (version <= specVersion)
	{
		filePath = br_.browseByName(browseKey + "." + std::to_string(version), RepositoryCore<T>::ROOT_PATH, RepositoryCore<T>::CHECKOUT_PATH);
		br_.openNotePad(filePath);
	}
	else
		std::cout << "File not present for browsing";

}

//utility to check if file is present at destination
template<typename T>
inline int RepositoryCore<T>::isFilePresent(std::string key)
{
	return ck_.isFilePresent(key);
}



template<typename T>
//This function class Browse by catoegory of browse class.
inline std::vector<std::string> RepositoryCore<T>::BrowseOnMutipleInput(std::vector<std::string> & category_name, std::vector<std::string> & filename, std::vector<std::string> & version, std::vector<std::string> & children)
{
	DbCore<T> db;
	if (category_name.size() > 0)
		db = BrowseOnCategory(category_name);
	else
		db = this->getDbCore();
	if (version.size() > 0)
		db = BrowseOnVersion(version, db);

	if (filename.size() > 0)
		db = BrowseOnFileName(filename, db);

	
	if (children.size() > 0)
		db = BrowseOnChildren(children, db);

	return db.keys(); 
}


template<typename T>
//This function class Browse by catoegory of browse class.
inline DbCore<T> RepositoryCore<T>::BrowseOnCategory(std::vector<std::string> & category_name, int flag)
{
	DbCore<T> temp_db;
	if (category_name.size() > 0)
	{
		for (auto y : category_name)
		{
			std::vector<std::string> keys;
			br_.browseOnCategory(this->getDbCore(), y, keys); 
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					if (flag == 1)
					{
						if (this->getDbCore()[x].children().size() == 0)
							temp_db[x] = this->getDbCore()[x];
					}
					else
						temp_db[x] = this->getDbCore()[x];
				}
			}
		}
	}
	else
	{
		if (flag == 1)
		{
			for (auto x : this->getDbCore())
				if (x.second.children().size() == 0)
					temp_db[x.first] = this->getDbCore()[x.first];
		}
	}
	return temp_db; 
}

template<typename T>
//This function to browse  using file name
inline DbCore<T> RepositoryCore<T>::BrowseOnFileName(std::vector<std::string> & filename, DbCore<T>& db)
{
	DbCore<T> tempdb;
	if (filename.size() > 0)
	{
		for (auto y : filename)
		{
			std::vector<std::string> keys;
			br_.browseOnFileName(db, y, keys); 
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					tempdb[x] = db[x];
				}
			}
		}
	}
	return tempdb; 
}


template<typename T>
//This function to initiate browse funtionality
inline DbCore<T> RepositoryCore<T>::BrowseOnVersion(std::vector<std::string> & version, DbCore<T>& db)
{
	DbCore<T> temp_db;
	if (version.size() > 0)
	{
		for (auto y : version)
		{
			std::vector<std::string> keys;
			br_.browseOnVersion(db, y, keys); 
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					temp_db[x] = db[x];
				}
			}
		}
	}
	return temp_db; 
}

template<typename T>
//This function to call browse on children
inline DbCore<T> RepositoryCore<T>::BrowseOnChildren(std::vector<std::string> & children, DbCore<T>& db)
{
	DbCore<T> temp_db;
	if (children.size() > 0)
	{
		for (auto y : children)
		{
			std::vector<std::string> keys;
			br_.browseOnChildren(db, y, keys); 
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					temp_db[x] = db[x];
				}
			}
		}
	}
	return temp_db; 
}

//constructor
template <typename T>
RepositoryCore<T>::~RepositoryCore()
{
}
