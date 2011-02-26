// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableRow.cc,v 1.1 2011/02/08 21:31:38 eusebi Exp $

//My libraries
#include "TAMUWW/SpecialTools/interface/TableRow.hh"

//C++ libraries
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;


ClassImp(TableRow)


//----------------------------------------------------------------------------
TableRow::TableRow(string _rowName) {
  SetName(_rowName.c_str());
}//C'tor


//----------------------------------------------------------------------------
// Copy constructor
TableRow::TableRow(const TableRow & rhs) {

  cellEntries.clear();

  // Set the name of this row
  SetName(rhs.GetName());
  
  // Get the vector of the new row
  const vector<TableCell*> rhsCells = rhs.getCellEntries();

  //and clone all the cells
  for (unsigned int it=0;it<rhsCells.size();it++){
    TableCell * newCell = (TableCell *) rhsCells[it]->clone();
    cellEntries.push_back(newCell);
  }   

}//C'tor


//----------------------------------------------------------------------------
TableRow::~TableRow(){
  deleteAllCells();
} //D'tor

//----------------------------------------------------------------------------
void TableRow::deleteAllCells(){

  for (unsigned int it=0;it<cellEntries.size();it++)
    delete cellEntries[it];

}

//----------------------------------------------------------------------------
TableRow & TableRow::operator=(const TableRow & rhs){

  // First delete all entries in this row
  deleteAllCells();

  // Clear the content of the cellentries
  cellEntries.clear();

  // Set the name of this row
  SetName(rhs.GetName());

  // Get the vector of the new row
  vector<TableCell*> rhsCells = rhs.getCellEntries();

  //and clone all the cells
  for (unsigned int it=0;it<rhsCells.size();it++){
    TableCell * newCell = (TableCell *) rhsCells[it]->clone();
    cellEntries.push_back(newCell);
  }
    
  return *this;

}//operator=
