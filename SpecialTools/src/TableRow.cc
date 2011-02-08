// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id:$

//My libraries
#include "TAMUWW/SpecialTools/interface/TableRow.hh"

//C++ libraries
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;

//----------------------------------------------------------------------------
TableRow::TableRow(){  
  SetName("defaultTableRow");
}

//----------------------------------------------------------------------------
TableRow::TableRow(string _rowName) {
  SetName(_rowName.c_str());
}//C'tor

//----------------------------------------------------------------------------
TableRow::~TableRow(){} //D'tor

//----------------------------------------------------------------------------
TableRow & TableRow::operator+=(const TableRow &rhs){

  vector<TableCell> table2Entries = rhs.getCellEntries();

  // Check
  if (table2Entries.size() != cellEntries.size()){
    cout<<"ERROR  TableRow::operator+= TableRow's have two different number of entries"<<endl
	<<" Returning without adding."<<endl;
    return *this;
  }

  // Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)cellEntries.size(); it++)
      cellEntries[it] += table2Entries[it];

  return *this;

}//operator +=

//----------------------------------------------------------------------------
TableRow TableRow::operator+(const TableRow &rhs) const{

  TableRow res = *this;
  res += rhs;
  return res;

}//operator+

//----------------------------------------------------------------------------
TableRow & TableRow::operator-=(const TableRow &rhs){

  vector<TableCell> table2Entries = rhs.getCellEntries();

  // Check
  if (table2Entries.size() != cellEntries.size()){
    cout<<"ERROR  TableRow::operator+= TableRow's have two different number of entries"<<endl
	<<" Returning without adding."<<endl;
    return *this;
  }

  // Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)cellEntries.size(); it++)
      cellEntries[it] -= table2Entries[it];

  return *this;

}//operator -=

//----------------------------------------------------------------------------
TableRow TableRow::operator-(const TableRow &rhs) const{

  TableRow res = *this;
  res -= rhs;
  return res;

}//operator+-

//----------------------------------------------------------------------------
TableRow & TableRow::operator*=(double rhs){

  // Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)cellEntries.size(); it++)
    cellEntries[it] *= rhs;

  return *this;

}//operator *=

//----------------------------------------------------------------------------
TableRow TableRow::operator*(double rhs) const{

  TableRow res = *this;
  res *= rhs;
  return res;

}//operator*

//----------------------------------------------------------------------------
TableRow & TableRow::operator/=(double rhs){

  // Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)cellEntries.size(); it++)
    cellEntries[it] /= rhs;

  return *this;

}//operator /=

//----------------------------------------------------------------------------
TableRow TableRow::operator/(double rhs) const{

  TableRow res = *this;
  res /= rhs;
  return res;

}//operator/

//----------------------------------------------------------------------------
TableRow & TableRow::operator*=(Value rhs){

  // Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)cellEntries.size(); it++)
    cellEntries[it] *= rhs;

  return *this;

}//operator *=

//----------------------------------------------------------------------------
TableRow TableRow::operator*(Value rhs) const{

  TableRow res = *this;
  res *= rhs;
  return res;

}//operator*

//----------------------------------------------------------------------------
TableRow & TableRow::operator/=(Value rhs){

  // Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)cellEntries.size(); it++)
    cellEntries[it] /= rhs;

  return *this;

}//operator /=

//----------------------------------------------------------------------------
TableRow TableRow::operator/(Value rhs) const{

  TableRow res = *this;
  res /= rhs;
  return res;

}//operator/

//----------------------------------------------------------------------------
void TableRow::reset(){
  
  for (unsigned int i=0;i<cellEntries.size();i++)
    cellEntries[i].reset();
  
}//Reset


//----------------------------------------------------------------------------
// This adds the information of tableCut2 to this cut
void TableRow::addTableRow(TableRow tableRow2){

  vector<TableCell> entriesToAdd =  tableRow2.getCellEntries();

  //Check
  if (getCellEntries().size() != entriesToAdd.size()){
    cout<<"ERROR  Table::AddTableRow Rows have different number of cells"<<endl
	<<" Returning without adding."<<endl;
    return ;
  }
  
  for (int it=0; it < (int) getCellEntries().size(); it++)
    cellEntries[it] += entriesToAdd[it];

}//addTableRow

ClassImp(TableRow)
