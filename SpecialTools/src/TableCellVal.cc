// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableCellVal.cc,v 1.3 2012/06/08 14:28:24 aperloff Exp $


//My libraries
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

//C++ libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstdlib> // atof

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostringstream;
using std::setprecision;

TableCellVal::TableCellVal(){
  SetName("CellVal");
}

//----------------------------------------------------------------------------
TableCellVal::TableCellVal(string name){
  SetName(name.c_str());
}

//----------------------------------------------------------------------------
string TableCellVal::print(TableFormat format){
  ostringstream oss;

  oss << setprecision(format.precision);
  if (format.style == "LATEX" ) 
  {
     oss<< val.value;
     if(!format.noError)
        oss <<" $\\pm$ " << val.error ;
  }
  else
  {
    oss<< val.value;
    if(!format.noError)
       oss << " +/- " << val.error;
  }
  return oss.str();
}
//----------------------------------------------------------------------------
// return true if the cell sucessfully parsed the string into its contents
bool TableCellVal::parseFromFile(string str, TableFormat format){

  vector<string> fields ;
  if (format.style == "LATEX" ) 
    fields = AuxFunctions::splitLineIntoWords(str,"$\\pm$");
  else
    fields = AuxFunctions::splitLineIntoWords(str,"+/-");
  if (fields.size() == 2){
    val.value = std::atof(fields[0].c_str());
    val.error = std::atof(fields[1].c_str());
    return true;
  } 

  return false;

}

//----------------------------------------------------------------------------
void TableCellVal::reset(){
  val.value = 0;
  val.error = 0;
}


//----------------------------------------------------------------------------
TableCell & TableCellVal::operator+=(const TableCell &rhsa) {
  TableCellVal * rhs = (TableCellVal *) &rhsa ;
  val += rhs->val;
  return *this;
}

//----------------------------------------------------------------------------
TableCellVal TableCellVal::operator+(const TableCell &rhs) const {
  TableCellVal res = *this;
  res += rhs;
  return res ;
}

//----------------------------------------------------------------------------
TableCell & TableCellVal::operator-=(const TableCell &rhsa) {
  TableCellVal * rhs = (TableCellVal *) &rhsa ;
  val -= rhs->val;
  return *this;
}

//----------------------------------------------------------------------------
TableCellVal TableCellVal::operator-(const TableCell &rhs) const {
  TableCellVal res = *this;
  res -= rhs;
  return res;
}

//----------------------------------------------------------------------------
TableCell & TableCellVal::operator*=(const double & rhs) {
  val *= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellVal TableCellVal::operator*(double rhs) const {
  TableCellVal res = *this;
  res *= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCellVal::operator/=(const double & rhs) {
  val /= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellVal TableCellVal::operator/(double rhs) const {
  TableCellVal res = *this;
  res /= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCellVal::operator*=(const Value & rhs) {
  val *= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellVal TableCellVal::operator*(Value rhs) const {
  TableCellVal res = *this;
  res *= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCellVal::operator/=(const Value & rhs) {
  val /= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellVal TableCellVal::operator/(Value rhs) const {
  TableCellVal res = *this;
  res /= rhs;
  return res;
}


ClassImp(TableCellVal)
