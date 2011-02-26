// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableCell.hh,v 1.1 2011/02/08 21:31:38 eusebi Exp $

#ifndef TABLECELLTEXT_DEF
#define TABLECELLTEXT_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/TableCell.hh"

//C++ libraries
#include <string>

//ROOT libraries
#include "TNamed.h"

//----------------------------------------------------------------------------
// The TableCell is the basic unit of a table.
class TableCellText : public TableCell{

public:
  TableCellText();
  TableCellText(std::string);

  // reset the information for this cell
  void reset();

  // return the information to be displayed for this cell
  std::string print(TableFormat style);

  // parse the information in str into the cell contents.
  bool parseFromFile(std::string str, TableFormat style);

  // explain how to do addition of cells, inherit from base class.
  virtual TableCell & operator+=(const TableCell &rhs) ;
  virtual TableCell & operator-=(const TableCell &rhs) ;

  // Native methods
  TableCellText operator+(const TableCell &rhs) const;
  TableCellText operator-(const TableCell &rhs) const;

  // the value stored in the cell
  std::string text;
  
  // provide a virtual clone method to all the derived classes.
  virtual TableCellText * clone() const { return new TableCellText(*this);}

  ClassDef (TableCellText,1)
};

#endif
