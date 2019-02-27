/////////////////////////////////////////////////////////////////////
// CheckIn.cpp -   Test stub for CheckIn Project                       //
// ver 1.0                                                         //
// Language:    C++, Visual Studio 2017                            //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018   //
/////////////////////////////////////////////////////////////////////

/*

Required Files:
* ---------------
* CheckIn.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 2 Mar 2018
*/

#include "Checkin.h"

//Test stub
#ifdef TEST_CHECK


int main()
{
	Checkin<std::string>ck ;
	ck.copyFile("../DB_Input.xml", "../root/TEST_CHECKIN/DB_Input.xml", "TEST_CHECKIN", "../root");
	getchar();
	return 0;
}

#endif // TEST_CHECKIN


