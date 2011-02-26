// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableCell.cc,v 1.1 2011/02/08 21:31:38 eusebi Exp $


//My libraries
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
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


TableCellText::TableCellText(){
  SetName("TextCell");
  text = "";
}

//----------------------------------------------------------------------------
TableCellText::TableCellText(string name){
  SetName(name.c_str());
  text = "";
}

//----------------------------------------------------------------------------
string TableCellText::print(TableFormat format){
  return text;
}
//----------------------------------------------------------------------------
// return true if the cell sucessfully parsed the string into its contents
bool TableCellText::parseFromFile(string str, TableFormat format){

  text = str;
  return true;

}

//----------------------------------------------------------------------------
void TableCellText::reset(){
  text = "";

}

//----------------------------------------------------------------------------
TableCell & TableCellText::operator+=(const TableCell &rhsa) {
  const TableCellText * rhs= (TableCellText*)&rhsa;
  text += rhs->text;
  return *this;
}

//----------------------------------------------------------------------------
TableCellText TableCellText::operator+(const TableCell &rhs) const {
  TableCellText res = *this;
  res += rhs;
  return res;
}

//----------------------------------------------------------------------------
TableCell & TableCellText::operator-=(const TableCell &rhs) {
  cout<<"ERROR  TableCellText::operator-= not defined"<<endl;
  return *this;
}

//----------------------------------------------------------------------------
TableCellText TableCellText::operator-(const TableCell &rhs) const {
  cout<<"ERROR  TableCellText::operator- not defined"<<endl;
  TableCellText res = *this;
  return res;
}


ClassImp(TableCellText)
