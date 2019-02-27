#ifndef PAYLOAD_H
#define PAYLOAD_H
///////////////////////////////////////////////////////////////////////
// PayLoad.h - application defined payload                           //
// ver 1.1                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides a single class, PayLoad:
*  - holds a payload string and vector of categories
*  - provides means to set and access those values
*  - provides methods used by Persist<PayLoad>:
*    - Sptr toXmlElement();
*    - static PayLoad fromXmlElement(Sptr elem);
*  - provides a show function to display PayLoad specific information
*  - PayLoad processing is very simple, so this package contains only
*    a header file, making it easy to use in other packages, e.g.,
*    just include the PayLoad.h header.
*
*  Required Files:
*  ---------------
*    PayLoad.h, PayLoad.cpp - application defined package
*    DbCore.h, DbCore.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.1 : 19 Feb 2018
*  - added inheritance from IPayLoad interface
*  Ver 1.0 : 10 Feb 2018
*  - first release
*
*/

#include <string>
#include <vector>
#include <iostream>
/*
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../DbCore/Definitions.h"
#include "../DbCore/DbCore.h"
*/

//#include "../../NoSQLDB/XmlDocument/XmlDocument/XmlDocument.h"
//#include "../../NoSQLDB/XmlDocument/XmlElement/XmlElement.h"
//#include "../../NoSQLDB/DbCore/Definitions.h"
//#include "../../NoSQLDB/DbCore/DbCore.h"

#include "../../NoSQLDB/Executive/NoSqlDb.h"
#include "IPayLoad.h"

///////////////////////////////////////////////////////////////////////
// PayLoad class provides:
// - a std::string value which, in Project #2, will hold a file path
// - a vector of string categories, which for Project #2, will be 
//   Repository categories
// - methods used by Persist<PayLoad>:
//   - Sptr toXmlElement();
//   - static PayLoad fromXmlElement(Sptr elem);


enum CheckinStatus
{
	closed,
	open,
	closing
	
};

namespace NoSqlDb
{
  class PayLoad : public IPayLoad<PayLoad>
  {

  public:
    PayLoad() = default;
    PayLoad(const std::string& val) : value_(val) {}
    static void identify(std::ostream& out = std::cout);
	const std::string ChekinEnumToString(CheckinStatus v);
	CheckinStatus ChekinStringToEnum(std::string s);

	PayLoad& operator=(const std::string& val)
    {
      value_ = val;
      return *this;
    }
    operator std::string() { return value_; }

    std::string value() const { return value_; }
    std::string& value() { return value_; }
    void value(const std::string& value) { value_ = value; }

	CheckinStatus getCheckinStatus() const { return checkinStatus_; }
	CheckinStatus& getCheckinStatus() { return checkinStatus_; }
	void setCheckinStatus(const CheckinStatus& value) { checkinStatus_ = value; }

	std::string getNamespace() const { return namespace_; }
	std::string& getNamespace() { return namespace_; }
	void setNamespace(const std::string& value) { namespace_ = value; }

	std::vector<std::string>& categories() { return categories_; }
    std::vector<std::string> categories() const { return categories_; }

    bool hasCategory(const std::string& cat)
    {
      return std::find(categories().begin(), categories().end(), cat) != categories().end();
    }

    Sptr toXmlElement();
    static PayLoad fromXmlElement(Sptr elem);

    static void showPayLoadHeaders(std::ostream& out = std::cout);
    static void showElementPayLoad(std::string& key, NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out = std::cout);
    static void showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out = std::cout);
  private:
    std::string value_;
    std::vector<std::string> categories_;
	CheckinStatus checkinStatus_;
	std::string namespace_;


  };


  //----< show file name >---------------------------------------------

  inline void PayLoad::identify(std::ostream& out)
  {
    out << "\n  \"" << __FILE__ << "\"";
  }
  //----< converting enum values to string >---------
  inline const std::string PayLoad::ChekinEnumToString(CheckinStatus v)
  {
	  switch (v)
	  {
	  case open:   return "open";
	  case closing:   return "closing";
	  case closed: return "closed";
	  default:      return "[Unknown type]";
	  }
  }
  //----< converting string values to enum for checkinString >---------
  inline CheckinStatus PayLoad::ChekinStringToEnum(std::string s)
  {
	  if (s=="open")
	  {
		  return open;
	  }
	  if (s=="closing")
	  {
		  return closing;
	  }
	  if (s=="closed")
	  {
		  return closed;
	  }
	  return open;
	  
  }
  //----< create XmlElement that represents PayLoad instance >---------
  /*
  * - Required by Persist<PayLoad>
  */
  inline Sptr PayLoad::toXmlElement()
  {
    Sptr sPtr = XmlProcessing::makeTaggedElement("payload");
    XmlProcessing::XmlDocument doc(makeDocElement(sPtr));
    Sptr sPtrVal = XmlProcessing::makeTaggedElement("value",value_);
    sPtr->addChild(sPtrVal);
	Sptr sPtrNamespace = XmlProcessing::makeTaggedElement("namespace", namespace_);
	sPtr->addChild(sPtrNamespace);
	Sptr sPtrCheckinState = XmlProcessing::makeTaggedElement("status", ChekinEnumToString(checkinStatus_));
	sPtr->addChild(sPtrCheckinState);

    Sptr sPtrCats = XmlProcessing::makeTaggedElement("categories");
    sPtr->addChild(sPtrCats);
	for (auto cat : categories_)
    {
      Sptr sPtrCat = XmlProcessing::makeTaggedElement("category", cat);
      sPtrCats->addChild(sPtrCat);
    }
    return sPtr;
  }
  //----< create PayLoad instance from XmlElement >--------------------
  /*
  * - Required by Persist<PayLoad>
  */

  inline PayLoad PayLoad::fromXmlElement(Sptr pElem)
  {
    PayLoad pl;
    for (auto pChild : pElem->children())
    {
      std::string tag = pChild->tag();
      std::string val = pChild->children()[0]->value();
      if (tag == "value")
      {
        pl.value(val);
      }
	  if (tag == "namespace")
	  {
		  pl.setNamespace(val);
	  }
	  if (tag == "status")
	  {
		  pl.setCheckinStatus(pl.ChekinStringToEnum(val));
	  }
      if (tag == "categories")
      {
        std::vector<Sptr> pCategories = pChild->children();
        for (auto pCat : pCategories)
        {
          pl.categories().push_back(pCat->children()[0]->value());
        }
      }
    }
    return pl;
  }
  /////////////////////////////////////////////////////////////////////
  // PayLoad display functions

  inline void PayLoad::showPayLoadHeaders(std::ostream& out)
  {
    out << "\n  ";
	out << std::setw(40) << std::left << "key";

    out << std::setw(10) << std::left << "Name";
    out << std::setw(45) << std::left << "Payload Value";
	out << std::setw(25) << std::left << "Payload Status";
    out << std::setw(25) << std::left << "Categories";
    out << "\n  ";
	out << std::setw(40) << std::left << "--------";

    out << std::setw(10) << std::left << "--------";
    out << std::setw(40) << std::left << "--------------------------------------";
	out << std::setw(25) << std::left << "-----------------------";
    out << std::setw(25) << std::left << "-----------------------";
  }


  inline void PayLoad::showElementPayLoad(std::string& key, NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out)
  {
    out << "\n  ";
	out << std::setw(40) << std::left << key.substr(0, 40);
    out << std::setw(10) << std::left << elem.name().substr(0, 8);
    out << std::setw(40) << std::left << elem.payLoad().value().substr(0, 38);
	out << std::setw(25) << std::left << elem.payLoad().ChekinEnumToString(elem.payLoad().getCheckinStatus());
    for (auto cat : elem.payLoad().categories())
    {
      out << cat << " ";
    }
  }

  inline void PayLoad::showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out)
  {
    showPayLoadHeaders(out);
    for (auto item : db)
    {
      NoSqlDb::DbElement<PayLoad> temp = item.second;
	  std::string s = item.first;
      PayLoad::showElementPayLoad(s,temp, out);
    }
  }
}
#endif
