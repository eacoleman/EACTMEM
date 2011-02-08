// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id:$


//My libraries
#include "TAMUWW/SpecialTools/interface/TableCell.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"

//C++ libraries
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib> // atof

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostringstream;


TableCell::TableCell(){
  SetName("Cell");
}

//----------------------------------------------------------------------------
TableCell::TableCell(string name){
  SetName(name.c_str());
}

//----------------------------------------------------------------------------
string TableCell::print(TableFormat format){
  ostringstream oss;

  if (format.style == "LATEX" ) 
    oss<< val.value<<" \\pm "<<val.error ;
  else
    oss<< val.value<<" +/- "<<val.error ;
  return oss.str();
}
//----------------------------------------------------------------------------
// return true if the cell sucessfully parsed the string into its contents
bool TableCell::parseFromFile(string str, TableFormat format){

  vector<string> fields ;
  if (format.style == "LATEX" ) 
    fields = PeterFunctions::splitLineIntoWords(str,"\\pm");
  else
    fields = PeterFunctions::splitLineIntoWords(str,"+/-");
  if (fields.size() == 2){
    val.value = std::atof(fields[0].c_str());
    val.error = std::atof(fields[1].c_str());
    return true;
  } 

  return false;

}

//----------------------------------------------------------------------------
void TableCell::reset(){
  val.value = 0;
  val.error = 0;
}

//----------------------------------------------------------------------------
TableCell & TableCell::operator+=(const TableCell &rhs) {
  val += rhs.val;
  return *this;
}

//----------------------------------------------------------------------------
TableCell TableCell::operator+(const TableCell &rhs) const {
  TableCell res = *this;
  res += rhs;
  return res;
}

//----------------------------------------------------------------------------
TableCell & TableCell::operator-=(const TableCell &rhs) {
  val -= rhs.val;
  return *this;
}

//----------------------------------------------------------------------------
TableCell TableCell::operator-(const TableCell &rhs) const {
  TableCell res = *this;
  res -= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCell::operator*=(double rhs) {
  val *= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCell TableCell::operator*(double rhs) const {
  TableCell res = *this;
  res *= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCell::operator/=(double rhs) {
  val /= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCell TableCell::operator/(double rhs) const {
  TableCell res = *this;
  res /= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCell::operator*=(Value rhs) {
  val *= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCell TableCell::operator*(Value rhs) const {
  TableCell res = *this;
  res *= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCell::operator/=(Value rhs) {
  val /= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCell TableCell::operator/(Value rhs) const {
  TableCell res = *this;
  res /= rhs;
  return res;
}


ClassImp(TableCell)
