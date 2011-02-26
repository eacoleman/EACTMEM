// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableCell.hh,v 1.1 2011/02/08 21:31:38 eusebi Exp $

#ifndef TABLECELLVAL_DEF
#define TABLECELLVAL_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/TableCell.hh"
#include "TAMUWW/SpecialTools/interface/TableFormat.hh"

//C++ libraries
#include <string>

//----------------------------------------------------------------------------
// The TableCell is the basic unit of a table.
class TableCellVal : public TableCell{

public:
  TableCellVal();
  TableCellVal(std::string);

  // reset the information for this cell
  void reset();

  // return the information to be displayed for this cell
  virtual std::string print(TableFormat style);

  // parse the information in str into the cell contents.
  virtual bool parseFromFile(std::string str, TableFormat style);

  // explain how to do addition of cells, inherited from bass class
  virtual TableCell & operator+=(const TableCell &rhs) ;
  virtual TableCell & operator-=(const TableCell &rhs) ;

  // Native to this class
  virtual TableCellVal operator+(const TableCell &rhs) const;
  virtual TableCellVal operator-(const TableCell &rhs) const;

  virtual TableCell & operator*=(const double & rhs) ;
  TableCellVal operator*(double rhs) const;
  virtual TableCell & operator/=(const double & rhs) ;
  TableCellVal operator/(double rhs) const;
  virtual TableCell & operator*=(const Value & rhs) ;
  TableCellVal operator*(Value rhs) const;
  virtual TableCell & operator/=(const Value & rhs) ;
  TableCellVal operator/(Value rhs) const;

  // provide a virtual clone method to all the derived classes.
  virtual TableCellVal * clone() const { return new TableCellVal(*this);}

  // the value stored in the cell
  Value val;

 ClassDef (TableCellVal,1)
};

#endif
