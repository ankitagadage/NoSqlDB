#pragma once
///////////////////////////////////////////////////////////////////////
// TestUtilities.h - provides single-user test harness               //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2017                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes:
* - TestExecutor    Executes single test in the context of a try-catch block
* - TestExecutive   Uses TestExecutor to run a sequence of tests
*
* Required Files:
* ---------------
*   TestUtilities.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Jan 2018
* - first release
* - refactored from earlier Utilities.h
*
* Notes:
* ------
* - Designed to provide all functionality in header file.
* - Implementation file only needed for test and demo.
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include <vector>

/////////////////////////////////////////////////////////////////////
// TestExecutor class
// - supports execution of callable objects for testing in the
//   context of a try-catch block.

template<typename T>
class TestExecutor
{
public:
  bool execute(T t, const std::string& name, std::ostream& out = std::cout);
private:
  void check(bool result, std::ostream& out);
};
//----< execute tests in the context of a try-catch block >----------

template <typename T>
bool TestExecutor<T>::execute(T t, const std::string& name, std::ostream& out)
{
  bool result = false;
  try
  {
    result = t();
    check(result, out);
    out << " -- \"" << name << "\"\n";
  }
  catch (std::exception& ex)
  {
    check(false, out);
    out << " -- \"" << name << "\" ";
    out << ex.what() << "\n";
  }
  return result;
}
//----< display test results >---------------------------------------

template<typename T>
void TestExecutor<T>::check(bool result, std::ostream& out)
{
  if (result)
    out << "  passed";
  else
    out << "  failed";
}

///////////////////////////////////////////////////////////////////////
// TestExecutive class
// - executes a sequence of tests with the help of TestExecutor

class TestExecutive
{
public:
  using Test = std::function<bool()>;
  

  bool doTests();
  void registerTest(Test t, const std::string& testName);
private:
  
};




