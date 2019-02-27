
////////////////////////////////////////////////////////////////////////////
// Checkout.h - Has design of CheckOut which helps to checkout functionality  //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                  //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018          //
///////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- CheckOut : helps to checkout file copy to stage folder.

Public Functions :
void copyFileToStage(const std::vector<std::string>& paramVec, const std::string& rootPath, const std::string& destPath);




* Required Files:
* ---------------
* FileMgr.h, Executive.h, Version.h


*
*
* Maintenance History:
* --------------------
* ver 1.0 : 2nd Mar 2018

*/




#pragma once
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../FileMgr/FileMgr.h"
#include "../Version/Version.h"
#include <algorithm>
using namespace NoSqlDb;
class Checkout
{
public:
	void copyFileToStage(const std::vector<std::string>& paramVec, const std::string& rootPath, const std::string& destPath);

};

// copies the file to stage folder
void Checkout::copyFileToStage(const std::vector<std::string>& paramVec, const std::string & rootPath, const std::string & destPath)
{
	for (auto x : paramVec)
	{
		x = regex_replace(x, std::regex("::"), "/");
		std::string src = "../root/" + x;
		size_t indexoffile = x.find_last_of("/");
		std::string filename_ = x.substr(indexoffile + 1, x.size());
		size_t indexofdot = filename_.find_last_of(".");
		std::string dstfilename = filename_.substr(0, indexofdot);
		std::string dest = destPath + "/" + dstfilename;
		FileSystem::File::copy(src, dest, false); //Copying File to stage directory.
	}

}
