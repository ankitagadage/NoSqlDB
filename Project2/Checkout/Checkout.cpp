/////////////////////////////////////////////////////////////////////
// Checkout.cpp -   Test stub for CheckOut Project                       //
// ver 1.0                                                         //
// Language:    C++, Visual Studio 2017                            //
// Ankita Gadage, CSE687 - Object Oriented Design, Spring 2018   //
/////////////////////////////////////////////////////////////////////

/*

Class Checkout : helps to checkout file .

Required Files:
* ---------------
* Checkout.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 2 Mar 2018
*/




#include "Checkout.h"





//test stub
#ifdef TEST_CHOUT

int main()
{
	std::vector<std::string> temp_vec;
	temp_vec.push_back("FileManager::FileMgr.h.2");
	std::string src = "../root";
	std::string dst = "../stage";
	Checkout ck;
	ck.copyFileToStage(temp_vec, src, dst);
	std::getchar();
	return 0;
}

#endif