#pragma once
////////////////////////////////////////////////////////////////////////////
// Version.h - Has design of Version helps to version file   //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                  //
// Parag Taneja, CSE687 - Object Oriented Design, Spring 2018          //
///////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- Version : Has design of Version helps to version file.

Public Functions :
static int getVersion(DbCore<T>& param_dbcore,const std::string & pram_value , const std::string & parma_namespace,const DbElement<T> &param_dbelem); -
This gets the version of file by querying namespace and filename in PayLoad value field.
static int getLatestVersionOfChildren(DbCore<T>& param_dbcore,const std::string & parent_key, int & latest_version_status); -
This returns the latest version and its status for all the parents associated to a key

* Required Files:
* ---------------
* DbCore.h, PayLoad.h

*
*
* Maintenance History:
* --------------------
* ver 1.0 : 2nd Mar 2018

*/

#include <iostream>
#include <sstream>
#include <algorithm>
#include "../NoSQLDB/DbCore/DbCore.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"

using namespace NoSqlDb;
template<typename T>
class Version
{
private:
	int x;
public:
	static int getVersion(DbCore<T>& param_dbcore,const std::string & pram_value , const std::string & parma_namespace,const DbElement<T> &param_dbelem);
	static int getLatestVersionOfChildren(DbCore<T>& param_dbcore,const std::string & parent_key, int & latest_version_status);
};

template<typename T>
//This gets the version of file by querying namespace and filename in PayLoad value field.
inline int Version<T>::getVersion(DbCore<T>& param_dbcore,const std::string & param_key, const std::string & parma_namespace, const DbElement<T> &param_dbelem)
{
	T temp_t = param_dbelem.payLoad();
	std::string value = temp_t.value();
	Query<T> q1(param_dbcore);
	auto hasValue = [&value](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).value() == value;
	}; //Querying on basis of src path.
	std::vector<std::string> temp_query_key = q1.select(hasValue).keys();
	std::string temp_nmspc = temp_t.nmspc();
	auto hasNameSpace = [&temp_nmspc](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).nmspc() == temp_nmspc;
	}; //Querying on basis of namespace.
	q1.from(temp_query_key);
	std::vector<std::string> query_key = q1.select(hasNameSpace).keys(); //using compound query.
	if (query_key.size() == 0)
		return 1;
	else
	{
		size_t max = 0;
		for (auto x : query_key) {
			size_t indexofdot = x.find_last_of(".");
			std::string version = x.substr(indexofdot+1,x.size());
			std::stringstream stream(version);
			size_t version_int = 0;
			stream >> version_int;
			if (max < version_int) {
				max = version_int; //finding maximum version.
			}
		}
		return max;
	}
}

template<typename T>
//This returns the latest version and its status for all the parents associated to a key
inline int Version<T>::getLatestVersionOfChildren(DbCore<T>& param_dbcore, const std::string &parent_key,int & latest_version_status)
{
	std::regex e("(" + parent_key +").*");
	size_t max=0;
	for (auto x : param_dbcore.keys())
	{
		if (std::regex_match(x, e))
		{
			size_t indexofdot = x.find_last_of(".");
			std::string version = x.substr(indexofdot + 1, x.size());
			std::stringstream stream(version);
			size_t version_int = 0;
			stream >> version_int;
			if (max < version_int) {
				max = version_int; //finding maximum version
				T t = param_dbcore[x].payLoad();
				latest_version_status = t.objstatus(); //finding status of key
			}
		}
	}
	return max;

}
