////////////////////////////////////////////////////////////////////////////
// RepoXmlUtil.cpp - Is used to read-write xml file                       //
// ver 1.0                                                                //
// Language:    C++, Visual Studio 2017                                   //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018             //
///////////////////////////////////////////////////////////////////////////

#include "RepoXmlUtil.h"


//test stub
#ifdef TEST_XML
int main()
{
	RepoXmlUtil x;
	std::map<std::string, std::vector<std::string>> map_temp;
	map_temp["Key1"].push_back("Dependency1");
	map_temp["Key1"].push_back("Dependency1");
	map_temp["Key2"].push_back("Dependency1");
	std::string path = "../test_xml.xml";
	x.readDependencyMapfromXML(temp_map, temp_path);
	std::cout << "XML created with name TEST_XMLMAP";
	std::getchar();
	return 0;
}
#endif
