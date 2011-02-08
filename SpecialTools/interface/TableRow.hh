// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id:$

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

  TableRow ();
  TableRow (std::string rowName);
  ~TableRow();

  // The operator + 
  TableRow operator+(const TableRow &rhs) const;
  TableRow & operator+=(const TableRow &rhs);
  TableRow operator-(const TableRow &rhs) const;
  TableRow & operator-=(const TableRow &rhs);
  TableRow operator*(double rhs) const;
  TableRow & operator*=(double rhs);
  TableRow operator/(double rhs) const;
  TableRow & operator/=(double rhs);
  TableRow operator*(Value rhs) const;
  TableRow & operator*=(Value rhs);
  TableRow operator/(Value rhs) const;
  TableRow & operator/=(Value rhs);

  // Set and Get the vector of entries
  void setCellEntries(std::vector<TableCell> ent){ cellEntries = ent; }

  std::vector<TableCell> getCellEntries() const { return cellEntries;}

  void reset();
  
  // Define how to add rows
  void addTableRow(TableRow);


 private:

  std::vector<TableCell>   cellEntries; // The entries in each cell of this row 

  ClassDef (TableRow,1)

};// TableRow


#endif
