/////////////////////////////////////////////////////////////////////
// CheckIn.h -    CheckIn Project                                 //
// ver 1.0                                                         //
// Language:    C++, Visual Studio 2017                            //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018    //
/////////////////////////////////////////////////////////////////////

/*

Class ChecIn :Facility to CheckIn file into Repo Structure.

Required Files:
* ---------------
* FileMgr.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 2 Mar 2018
*/


#pragma once
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include <map>
#include "../FileMgr/FileMgr.h"
#include "../Version/Version.h"

using namespace NoSqlDb;

//Provides support for checkin functions
template <typename T>
class Checkin
{
public:
	
	void setdb(DbCore<T>& db);
	void acceptFile(DbElement<T> elem,std::string dest,size_t version);
	void copyFile(std::string src, std::string dest, std::string _namespace, std::string root);

	void addFile(DbElement<T> elem,std::string dest);
	int isFilePresent(std::string key);
	void createDependencyMap();
	void checkDependentFile(std::string elemList);

private: 
	static std::map<std::string, std::vector<std::string>> objDependencyMap_;
	DbCore<T> db_;

};


template<typename T>
//Utility to copy files from source to destination
inline void Checkin<T>::copyFile(std::string src, std::string dest, std::string _namespace, std::string root)
{
	if (!FileSystem::Directory::exists(root + "/" + _namespace)) //Checking Existence.
	{
		FileSystem::Directory::create(root + "/" + _namespace); //If does not exist create that directory.
	}
	FileSystem::File::copy(src, dest, false); //copy file from source to destination
}

//this function is called new files are to be checked in
template<typename T>
void Checkin<T>::addFile(DbElement<T> elem ,std::string dest)
{
	size_t indexofFileNameStart = elem.payLoad().value().find_last_of("/");

	std::string fileName = elem.payLoad().value().substr(indexofFileNameStart + 1, elem.payLoad().value().size());

	dest = dest + fileName + "." + std::to_string(1);

	FileSystem::File::copy(elem.payLoad().value(), dest, false);
	elem.payLoad().value() = dest + fileName;
	db_[elem.payLoad().getNamespace() + fileName + "." + std::to_string(1)] = elem;
}

//this function finds if any file has dependencies
template<typename T>
void Checkin<T>::checkDependentFile(std::string key)
{
	//for (int i = 0; i < elemList.size(); i++)
	{
		for (unsigned int j = 0; j < db_[key].children().size() && db_[key].payLoad().getCheckinStatus()==open; j++)
		{
			bool isDependencyClosed = true;
			std::vector<std::string> parentList = objDependencyMap_[db_[key].children()[j]];
			for (unsigned int i = 0; i < parentList.size(); i++)
			{

				if (db_[parentList[i]].payLoad().getCheckinStatus() == closed)
				{
					isDependencyClosed = isDependencyClosed && true;
				}
				else
				{
					isDependencyClosed = isDependencyClosed && false;
				}
			}
			if (isDependencyClosed == true)
			{
				
				db_[db_[key].children()[j]].payLoad().setCheckinStatus(closed);
			}
		}
	}
	
}
//Creating new dependency map
template<typename T>
void Checkin<T>::createDependencyMap()
{
	for (auto const &ent1 : db_.dbStore()) {
		auto const &key = ent1.first;
		auto const &value = ent1.second;
		for (size_t i = 0; i <  ent1.second.children().size(); i++)
		{
			//if (objDependencyMap.find(ent1.second.children[i]) == objDependencyMap.end())
				//objDependencyMap.insert(ent1.second.children[i], ent1.first);
			//else
			{
				objDependencyMap_[ent1.second.children()[i]].push_back(ent1.first);
			}
		}
	}

}

//setting the db and creating dependency map
template<typename T>
inline void Checkin<T>::setdb(DbCore<T>& db)
{
	db_ = db;
	createDependencyMap();
}

//utility to accept files for new check in
template<typename T>
void Checkin<T>::acceptFile(DbElement<T> newElem, std::string dest, size_t version)
{
	std::string key;
	size_t indexofFileNameStart = newElem.payLoad().value().find_last_of("/");
	std::string fileName = newElem.payLoad().value().substr(indexofFileNameStart + 1, newElem.payLoad().value().size());
	key = newElem.payLoad().getNamespace() + fileName + "." + std::to_string(version);
	DbElement<T> elem = db_[newElem.payLoad().getNamespace() + fileName + "." + std::to_string(version)];
	if(version != 0){
		if (elem.payLoad().getCheckinStatus() == open){
			dest = dest + fileName + "." + std::to_string(version);
			key = elem.payLoad().getNamespace() + fileName + "." + std::to_string(version);
			bool isCopySuccess = FileSystem::File::copy(elem.payLoad().value(), dest, false);
		}
		else if (elem.payLoad().getCheckinStatus() == closed)
		{
			dest = dest + fileName + "." + std::to_string(version + 1);
			key = elem.payLoad().getNamespace() + fileName + "." + std::to_string(version+1);
			bool isCopySuccess = FileSystem::File::copy(elem.payLoad().value(), dest, false);

		}
	}
	
	elem.payLoad().value() = dest + fileName;
	bool isDependencyClosed = true;
	if (newElem.payLoad().getCheckinStatus() == closed)
	{
		std::vector<std::string> parentList = objDependencyMap_[key];
		for (unsigned int i = 0; i < parentList.size(); i++)
		{
			if (db_[parentList[i]].payLoad().getCheckinStatus() == closed)
			{
				isDependencyClosed = isDependencyClosed && true;
			}
			else
			{
				isDependencyClosed = isDependencyClosed && false;
			}
		}
		if (isDependencyClosed == false){
			newElem.payLoad().setCheckinStatus(closing);
		}
		else	{
			checkDependentFile(key);
		}
	}
	db_[key] = newElem ;
}

//utility to check if file is present at destination
template<typename T>
inline int Checkin<T>::isFilePresent(std::string key)
{
	Query<T> q(db_);
	q.getKeys(key).keys();
	if (q.getKeys(key).keys().size() > 0)
	{
		return Version::getFileVersion(q.getKeys(key).keys());
	}
	else
	{
		return 0;
	}

}


//static object initialization
template<typename T>
std::map<std::string, std::vector<std::string>> Checkin<T>::objDependencyMap_ = {};

