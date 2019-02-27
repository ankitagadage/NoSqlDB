/////////////////////////////////////////////////////////////////////
// Browse.cpp -   Test stub for Browse Project                       //
// ver 1.0                                                         //
// Language:    C++, Visual Studio 2017                            //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018   //
/////////////////////////////////////////////////////////////////////


/*

Required Files:
* ---------------
* Browse.h ,Payload.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 2 Mar 2018
*/

#include "Browse.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"


//test stub
#ifdef TEST_BROWSE
int main()
{
	std::string temp_key = "Browse::Browse.cpp.2";
	std::string temp_rootpath = "../root";
	std::string temp_Stagepath = "../stage";
	std::string temp_string = Browse<PayLoad>::browseBySpecificName(temp_key, temp_rootpath, temp_Stagepath);
	br.openNotePad(temp_string);
	std::getchar();
	return 0;
}
#endif