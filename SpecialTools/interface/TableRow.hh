// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableRow.hh,v 1.2 2011/02/26 19:10:07 eusebi Exp $

#ifndef TABLEROW_DEF
#define TABLEROW_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/TableCell.hh"

//C++ libraries
#include <string>
#include <vector>

//ROOT libraries
#include "TNamed.h"


//----------------------------------------------------------------------------
// A TableRow is just a row-vector  of cells.
class TableRow  : public TNamed {

 public:

  //TableRow ();
  TableRow (const TableRow &);
  TableRow (std::string rowName ="defaultTableRow");
  ~TableRow();

  // Set and Get the vector of entries
  void addCellEntries(TableCell* ent){ cellEntries.push_back(ent); }
  void setCellEntries(std::vector<TableCell*> ent){ cellEntries = ent; }
  std::vector<TableCell*> getCellEntries() const { return cellEntries;}
  TableRow & operator=(const TableRow & rhs);
  TableCell* operator[](std::string col);

 private:

  void deleteAllCells();

  std::vector<TableCell*>  cellEntries; // The entries in each cell of this row 

  ClassDef (TableRow,1)

};// TableRow


#endif
