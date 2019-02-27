

/////////////////////////////////////////////////////////////////////
// RepositoryCore.cpp -   Test stub for RepositoryCore Project                       //
// ver 1.0                                                         //
// Language:    C++, Visual Studio 2017                            //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018   //
/////////////////////////////////////////////////////////////////////


/*

Class RepositoryCore : helps to browse,checkIn,verion,checkout of file.

Required Files:
* ---------------
* RepositoryCore.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 2 Mar 2018
*/
#include "RepositoryCore.h"

#ifdef TEST_CODEREPO
//This file contains one test stub which implements one of the function mentioned in CodeRepo .
//Here in this test stub , user is trying to check In File by creating a DB element.
int main()
{
	RepositoryCore<PayLoad> c;
	std::vector<DbElement<PayLoad>> vec_of_dbElements;
	DbElement<PayLoad> test1_dbElement;
	test1_dbElement.name("Author_TC5");
	test1_dbElement.descrip("This file containes the code of Checkout.cpp");
	test1_dbElement.dateTime(DateTime().now());
	test1_dbElement.payLoad().value("../CheckOut/CheckOut.cpp");
	test1_dbElement.payLoad().categories().push_back("CheckOut1");
	test1_dbElement.payLoad().nmspc("TestStub");
	vec_of_dbElements.push_back(test1_dbElement);	 //Checkout.cpp.1 in closed state.

	DbElement<PayLoad> test2_dbElement;
	test2_dbElement.name("Author_TC5");
	test2_dbElement.descrip("This file containes the code of CheckOut.h");
	test2_dbElement.dateTime(DateTime().now());
	test2_dbElement.payLoad().value("../CheckOut/CheckOut.h");
	test2_dbElement.payLoad().categories().push_back("CheckOut1");
	test2_dbElement.payLoad().nmspc("TestStub");
	vec_of_dbElements.push_back(test2_dbElement);//Checkout.h.1 in closed state.

	c.checkinFiles(vec_of_dbElements);
	c.displayDb();
	std::getchar();
	return 0;
}
#endif // TEST_CODEREPO



