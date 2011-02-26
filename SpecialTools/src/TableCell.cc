// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableCell.cc,v 1.1 2011/02/08 21:31:38 eusebi Exp $


//My libraries
#include "TAMUWW/SpecialTools/interface/TableCell.hh"

//C++ libraries
#include <string>

using std::string;

TableCell::TableCell(){
  SetName("Cell");
}

//----------------------------------------------------------------------------
TableCell::TableCell(string name){
  SetName(name.c_str());
}


ClassImp(TableCell)
