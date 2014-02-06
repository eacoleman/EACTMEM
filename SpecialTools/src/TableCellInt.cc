// Alexx Perloff
// TAMU aperloff@physics.tamu.edu
// created: Tuesday June 26, 2012
// $Id: TableCellInt.cc,v 1.0 2012/06/26 19:20:48 aperloff Exp $


//My libraries
#include "TAMUWW/SpecialTools/interface/TableCellInt.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

//C++ libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstdlib> // atof & atoi
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostringstream;
using std::setprecision;

TableCellInt::TableCellInt(){
  SetName("CellInt");
  val = 0;
}

//----------------------------------------------------------------------------
TableCellInt::TableCellInt(string name){
  SetName(name.c_str());
  val = 0;
}

//----------------------------------------------------------------------------
string TableCellInt::print(TableFormat format){
  ostringstream oss;

  oss << setprecision(format.precision);
  oss<< val;
  return oss.str();
}
//----------------------------------------------------------------------------
// return true if the cell sucessfully parsed the string into its contents
bool TableCellInt::parseFromFile(string str, TableFormat format){

  vector<string> fields ;
  str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
  fields = AuxFunctions::splitLineIntoWords(str," ");
  if (fields.size() == 1){
    val = std::atof(fields[0].c_str());
    return true;
  } 

  return false;

}

//----------------------------------------------------------------------------
void TableCellInt::reset(){
  val = 0;
}


//----------------------------------------------------------------------------
TableCell & TableCellInt::operator+=(const TableCell &rhsa) {
  TableCellInt * rhs = (TableCellInt *) &rhsa ;
  val += rhs->val;
  return *this;
}

//----------------------------------------------------------------------------
TableCellInt TableCellInt::operator+(const TableCell &rhs) const {
  TableCellInt res = *this;
  res += rhs;
  return res ;
}

//----------------------------------------------------------------------------
TableCell & TableCellInt::operator-=(const TableCell &rhsa) {
  TableCellInt * rhs = (TableCellInt *) &rhsa ;
  val -= rhs->val;
  return *this;
}

//----------------------------------------------------------------------------
TableCellInt TableCellInt::operator-(const TableCell &rhs) const {
  TableCellInt res = *this;
  res -= rhs;
  return res;
}

//----------------------------------------------------------------------------
TableCell & TableCellInt::operator*=(const double & rhs) {
  val *= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellInt TableCellInt::operator*(double rhs) const {
  TableCellInt res = *this;
  res *= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCellInt::operator/=(const double & rhs) {
  val /= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellInt TableCellInt::operator/(double rhs) const {
  TableCellInt res = *this;
  res /= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCellInt::operator*=(const int & rhs) {
  val *= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellInt TableCellInt::operator*(int rhs) const {
  TableCellInt res = *this;
  res *= rhs;
  return res;
}
//----------------------------------------------------------------------------
TableCell & TableCellInt::operator/=(const int & rhs) {
  val /= rhs;
  return *this;
}

//----------------------------------------------------------------------------
TableCellInt TableCellInt::operator/(int rhs) const {
  TableCellInt res = *this;
  res /= rhs;
  return res;
}

//----------------------------------------------------------------------------
TableCell & TableCellInt::operator++(int unused) {
   val++;
   return *this;
}

//----------------------------------------------------------------------------
TableCellInt & TableCellInt::operator++(int unused) const {
   TableCellInt res = *this;
   res++;
   return res;
}

//----------------------------------------------------------------------------
TableCell & TableCellInt::operator=(int rhs) {
   val = rhs;
   return *this;
}

//----------------------------------------------------------------------------
TableCellInt & TableCellInt::operator=(int rhs) const {
   TableCellInt res = *this;
   res = rhs;
   return res;
}

//----------------------------------------------------------------------------
int TableCellInt::getValue() {
  return val;

  //ostringstream oss;
  //oss<< val;
  //return std::atoi(oss.str().c_str());
}

ClassImp(TableCellInt)
