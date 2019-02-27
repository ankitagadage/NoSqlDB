#pragma once

////////////////////////////////////////////////////////////////////////////
// Browse.h - Has design of Browse class                                 //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                  //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018          //
///////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- Browse : Provides browse Functionality

Public Functions :
void getFile(DbCore<T>& db, std::string fileNamespace, std::string dest, int version);
std::vector<std::string> browseOnCategory(DbCore<T>& db, std::string & category);
std::string browseByName(std::string  file_key, const std::string& srcrootpath, const std::string& stagefolder);
void openNotePad(std::string & filepath);



* Required Files:
* ---------------
* Executive.h, FileSystem.h
* Utility.h ,Process.h

*
*
* Maintenance History:
* --------------------
* ver 1.0 : 2nd Mar 2018

*/
#include "../NoSQLDB/Executive/Executive.h"
#include "../Process/Process.h"
#include <string>
using namespace NoSqlDb;

//Design of Browse class
template<typename T>
class Browse
{
public:
	Browse();
	void getFile(DbCore<T>& db, std::string fileNamespace, std::string dest, int version);
	std::vector<std::string> browseOnCategory(DbCore<T>& db, std::string & category);
	void browseOnCategory(DbCore<T>& db, std::string & category, std::vector<std::string> & vec);
	void browseOnFileName(DbCore<T>& db, std::string & filename, std::vector<std::string> & vec);
	void browseOnVersion(DbCore<T>& db, std::string & version, std::vector<std::string> & values);
	void browseOnChildren(DbCore<T>& db, std::string & child, std::vector<std::string> & vec);


	std::string browseByName(std::string  file_key, const std::string& srcrootpath, const std::string& stagefolder);
	void openNotePad(std::string & filepath);

	~Browse();
};

//constructor
template<typename T>
Browse<T>::Browse()
{
}

//Destructor
template<typename T>
Browse<T>::~Browse()
{
}



template<typename T>
//Browse on Children
inline void Browse<T>::browseOnChildren(DbCore<T>& db, std::string & child, std::vector<std::string> & vec)
{
	vec.clear();
	Query<T> q1(db);
	Keys keys{ child };
	Conditions<PayLoad> conds0;
	conds0.children(keys);
	vec = q1.select(conds0).keys();//Querying DB on basis of query.
}

template<typename T>
//Browsing on version
inline void Browse<T>::browseOnVersion(DbCore<T>& db, std::string & version, std::vector<std::string> & values)
{
	values.clear();
	for (auto x : db)
	{
		size_t indexofVersion = x.first.find_last_of(".");
		std::string key_version = x.first.substr(indexofVersion + 1, x.first.size());
		if (key_version == version)
			values.push_back(x.first);
	}
}


template<typename T>
//Query DB on Category
inline void Browse<T>::browseOnCategory(DbCore<T>& db, std::string & category, std::vector<std::string> & values)
{
	values.clear();
	Query<T> q1(db);
	auto isPresent = [&category](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).hasCategory(category);
	};
	values = q1.select(isPresent).keys(); //Querying DB on basis of query.
}



template<typename T>
//Query DB using FileName
inline void Browse<T>::browseOnFileName(DbCore<T>& db, std::string & filename, std::vector<std::string> & vec)
{
	vec.clear();
	std::string exp = "(.*)(" + filename + ")(.*)";
	std::regex temp_regex(exp);
	for (auto x : db)
	{
		if (std::regex_match(x.first, temp_regex))
			vec.push_back(x.first);
	}
}

//starting a new process to open file in notepad
template<typename T>
inline void Browse<T>::openNotePad(std::string & filepath)
{
	Process pro;
	std::string notepadPath = "c:/windows/system32/notepad.exe";
	pro.application(notepadPath);

	std::string cmdLine = "/A " + filepath;
	pro.commandLine(cmdLine);

	std::cout << "\n  starting process: \"" << notepadPath << "\"";
	std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
	pro.create();

}



//get file to open it in notepad
template<typename T>
inline void Browse<T>::getFile(DbCore<T>& db, std::string fileNamespace, std::string dest, int version)         
{
	Query q(db);
	Query q1(db);

	std::vector<std::string> result;
	if (version == 0)
	{
		version = Version::getFileVersion(q1.getKeys(fileNamespace + fileName).keys());
		result = q.getKeys(fileNamespace + fileName + "." + version).keys();
	}
	else
	{
		result = q.getKeys(fileNamespace + fileName + "." + version).keys();
	}
	for (int i = 0; i < result.size(); i++)
	{
		db[result].show();
		Process p;
		p.title("test application");
		std::string appPath = "c:/windows/system32/notepad.exe";
		p.application(appPath);

		std::string cmdLine = db[result].payLoad().value();
		p.commandLine(cmdLine);

		std::cout << "\n  starting process: \"" << appPath << "\"";
		std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
		p.create();

		CBP callback = []() { std::cout << "\n  --- child process exited with this message ---"; };
		p.setCallBackProcessing(callback);
		p.registerCallback();

	}
}


//queries the db to extract files based on category
template<typename T>
inline std::vector<std::string> Browse<T>::browseOnCategory(DbCore<T>& db, std::string & category)
{
	Query<T> q1(db);
	auto hasCategory = [&category](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).hasCategory(category);
	};
	return q1.select(hasCategory).keys(); 
}


template<typename T>
inline std::string Browse<T>::browseByName(std::string  fKey, const std::string& rPath, const std::string& stagePath)
{
	fKey = regex_replace(fKey, std::regex("::"), "/");
	std::string src = rPath + "/" + fKey;
	size_t fIndex = fKey.find_last_of("/");
	std::string fname = fKey.substr(fIndex + 1, fKey.size());
	size_t dIndex = fname.find_last_of(".");
	std::string dfilename = fname.substr(0, dIndex);
	std::string dest = stagePath + "/" + dfilename;
	FileSystem::File::copy(src, dest, false); 
	return dest;
}

