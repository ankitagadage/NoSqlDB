#pragma once
////////////////////////////////////////////////////////////////////////////
// Version.h - Has design of Version requirement   //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                  //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018          //
///////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- Version : Has design of Version class

Public Functions :
static size_t getFileVersion(std::vector<std::string> keys);
template<typename T>
static int getLatestVersion(DbCore<T>& dbcore, const std::string &key, int & latestVersionStatus);
template<typename T>
static int getVersion(DbCore<T>& param_dbcore, const std::string & param_key, const std::string & parma_namespace, const DbElement<T> &param_dbelem);

* Required Files:
* ---------------
* DbCore.h, PayLoad.h

*
*
* Maintenance History:
* --------------------
* ver 1.0 : 2nd Mar 2018

*/
#include <string>
#include <sstream>
#include <vector>
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"
using namespace NoSqlDb;

class Version
{
public:
	static size_t getFileVersion(std::vector<std::string> keys);
	template<typename T>
	static int getLatestVersion(DbCore<T>& dbcore, const std::string &key, int & latestVersionStatus);
	template<typename T>
	static int getVersion(DbCore<T>& param_dbcore, const std::string & param_key, const std::string & parma_namespace, const DbElement<T> &param_dbelem);


};



//overloaded function to get file version with different parameters
template<typename T>
inline int Version::getLatestVersion(DbCore<T>& dbcore, const std::string &key, int & latestVersionStatus)
{
	size_t max = 0;
	std::regex e("(" + key + ").*");
	for (auto x : dbcore.keys())
	{
		if (std::regex_match(x, e))
		{
			size_t indexofdot = x.find_last_of(".");
			std::string version = x.substr(indexofdot + 1, x.size());
			std::stringstream stream(version);
			size_t lVersion = 0;
			stream >> lVersion;
			if (max < lVersion) {
				T t = dbcore[x].payLoad();
				latestVersionStatus = t.getCheckinStatus();
				max = lVersion;
			}
		}
	}
	return max;

}

//get the specific file version by query on Db
template<typename T>
inline int Version::getVersion(DbCore<T>& dbcore, const std::string & pkey, const std::string & pnamespace, const DbElement<T> &dbelem)
{
	Query<T> q1(dbcore);
	T t = dbelem.payLoad();
	std::string iValue = t.value();
	std::string nmspc = t.getNamespace();
	auto hasValue = [&iValue](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).value() == iValue;
	}; 
	std::vector<std::string> queryKey = q1.select(hasValue).keys();
	auto hasNameSpace = [&nmspc](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).getNamespace() == nmspc;
	}; 
	q1.from(queryKey);
	//std::vector<std::string> queryKey1 = q1.select(hasNameSpace).keys(); 
	if (queryKey.size() == 0)
		return 1;
	else
	{
		size_t max = 0;
		for (auto x : queryKey) {
			size_t indexofdot = x.find_last_of(".");
			std::string ver = x.substr(indexofdot + 1, x.size());
			std::stringstream stream(ver);
			size_t iVersion = 0;
			stream >> iVersion;
			if (max < iVersion) {
				max = iVersion;
			}
		}
		return max;
	}
}

//return file version just from a collection of keys
size_t Version::getFileVersion(std::vector<std::string> keys)
{

	size_t max = 0;
	for (auto x : keys) {
		size_t indexofdot = x.find_last_of(".");
		std::string version = x.substr(indexofdot + 1, x.size());
		std::stringstream stream(version);
		size_t version_int = 0;
		stream >> version_int;
		if (max < version_int) {
			max = version_int;
		}
	}
	return max;
}




