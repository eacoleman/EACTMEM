// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id:$

#ifndef TABLECELL_DEF
#define TABLECELL_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/TableFormat.hh"

//C++ libraries
#include <string>

//ROOT libraries
#include "TNamed.h"

//----------------------------------------------------------------------------
// The TableCell is the basic unit of a table.
class TableCell : public TNamed{

public:
  TableCell();
  TableCell(std::string);

  // reset the information for this cell
  void reset();

  // return the information to be displayed for this cell
  std::string print(TableFormat style);

  // parse the information in str into the cell contents.
  bool parseFromFile(std::string str, TableFormat style);

  // explain how to do addition of cells 
  TableCell & operator+=(const TableCell &rhs) ;
  TableCell operator+(const TableCell &rhs) const;
  TableCell & operator-=(const TableCell &rhs) ;
  TableCell operator-(const TableCell &rhs) const;
  TableCell & operator*=(double rhs) ;
  TableCell operator*(double rhs) const;
  TableCell & operator/=(double rhs) ;
  TableCell operator/(double rhs) const;
  TableCell & operator*=(Value rhs) ;
  TableCell operator*(Value rhs) const;
  TableCell & operator/=(Value rhs) ;
  TableCell operator/(Value rhs) const;

  // the value stored in the cell
  Value val;

 ClassDef (TableCell,1)
};

#endif
