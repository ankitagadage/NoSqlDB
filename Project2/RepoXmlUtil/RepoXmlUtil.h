#pragma once
////////////////////////////////////////////////////////////////////////////
// RepoXmlUtil.h - Is used to read-write xml file                       //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                   //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018             //
///////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides contains  following class:
*
- RepoXmlUtil : Has declaration of funstions to read -write Depedency Map and DB to XML and vice versa.

Public Functions :
void saveDbToXml(DbCore<T>& db, std::string& path);
void readDbFromXml(DbCore<T>& db, std::string& path);
void saveDependencyMaptoXML(std::map<std::string, std::vector<std::string>>& map, std::string& path);
void readDependencyMapfromXML(std::map<std::string, std::vector<std::string>>& map, std::string& path);
* Required Files:
* ---------------
* Executive.h, XmlDocument.h
* XmlParser.h
*
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018

*/


#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "../NoSQLDB/Executive/Executive.h"
#include "../NoSQLDB/XmlDocument/XmlDocument/XmlDocument.h"

using namespace NoSqlDb;


template<typename T>
class RepoXmlUtil
{
public:
	void saveDbToXml(DbCore<T>& db, std::string& path);
	void readDbFromXml(DbCore<T>& db, std::string& path);
	void saveDependencyMaptoXML(std::map<std::string, std::vector<std::string>>& map, std::string& path);
	void readDependencyMapfromXML(std::map<std::string, std::vector<std::string>>& map, std::string& path);

};


//----< Saving Db to Xml >------------------------------------------

template<typename T>
inline void RepoXmlUtil<T>::saveDbToXml(DbCore<T>& db, std::string& path)
{
	Persist<T> persist(db);
	std::string xmlHeader = "< ? xml version = \"1.0\" encoding = \"utf-8\" ? >\n";
	Xml xmlStr = persist.toXml();
	xmlHeader = xmlHeader + xmlStr; // added header to XML
	std::string filename = path;
	std::ofstream outfile(filename); // writing into a file 
	outfile << xmlHeader << std::endl;
	outfile.close();
}


//----< reads XML file into DB >------------------------------------------

template<typename T>
inline void RepoXmlUtil<T>::readDbFromXml(DbCore<T>& db, std::string& path)
{
	Persist<PayLoad> persist(db);
	std::string filename = path;
	std::ifstream ifstream(filename);
	std::string xmlStr((std::istreambuf_iterator<char>(ifstream)),
		(std::istreambuf_iterator<char>()));
	persist.fromXml(xmlStr);
}

//----< saving Dependency Map to XML >------------------------------------------


template<typename T>
inline void RepoXmlUtil<T>::saveDependencyMaptoXML(std::map<std::string, std::vector<std::string>>& map, std::string& path)
{
	using sptr = std::shared_ptr<AbstractXmlElement>;
	sptr root = makeTaggedElement("Dependency");
	for (auto elem : map)
	{
		sptr row = makeTaggedElement("row");
		root->addChild(row);
		sptr key = makeTaggedElement("key", elem.first);
		row->addChild(key);
		sptr values = makeTaggedElement("values");
		row->addChild(values);
		for (auto x : elem.second)
		{
			sptr value_elem = makeTaggedElement("Value", x);
			values->addChild(value_elem);
		}
	}
	sptr xmlDoc = makeDocElement(root);
	std::string xmlHeader = "< ? xml version = \"1.0\" encoding = \"utf-8\" ? >\n";
	xmlHeader = xmlHeader + xmlDoc->toString();
	std::string filename = path;
	std::ofstream outfile(filename);
	outfile << xmlHeader << std::endl;
	outfile.close();
}

//----< reading Dependency Map from XML >------------------------------------------

template<typename T>
inline void RepoXmlUtil<T>::readDependencyMapfromXML(std::map<std::string, std::vector<std::string>>& map, std::string& path)
{

	XmlParser parser(path);
	XmlDocument* pDoc = parser.buildDocument();
	using sptr = std::shared_ptr<AbstractXmlElement>;
	std::vector<sptr> itr = pDoc->elements("Dependency").select();
	for (auto x : itr) {
		std::string key;
		for (auto y : x->children()) {
			if (y->tag() == "key")
			{
				key = y->children()[0]->value();
			}
			else
			{
				for (auto z : y->children())
				{
					map[key].push_back(z->children()[0]->value());
				}
			}
		}
	}
}


