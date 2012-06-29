// Alexx Perloff
// TAMU aperloff@physics.tamu.edu
// created: Tuesday June 26, 2012
// $Id: TableCellInt.hh,v 1.0 2012/06/26 19:20:48 aperloff Exp $

#ifndef TABLECELLINT_DEF
#define TABLECELLINT_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/TableCell.hh"
#include "TAMUWW/SpecialTools/interface/TableFormat.hh"

//C++ libraries
#include <string>

//----------------------------------------------------------------------------
// The TableCell is the basic unit of a table.
class TableCellInt : public TableCell{

public:
  TableCellInt();
  TableCellInt(std::string);

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
  virtual TableCellInt operator+(const TableCell &rhs) const;
  virtual TableCellInt operator-(const TableCell &rhs) const;

  virtual TableCell & operator*=(const double & rhs) ;
  TableCellInt operator*(double rhs) const;
  virtual TableCell & operator/=(const double & rhs) ;
  TableCellInt operator/(double rhs) const;
  virtual TableCell & operator*=(const int & rhs) ;
  TableCellInt operator*(int rhs) const;
  virtual TableCell & operator/=(const int & rhs) ;
  TableCellInt operator/(int rhs) const;

  virtual TableCell & operator++(int unused) ;
  TableCellInt & operator++(int unused) const;
  virtual TableCell & operator=(int rhs) ;
  TableCellInt & operator=(int rhs) const;

  // provide a virtual clone method to all the derived classes.
  virtual TableCellInt * clone() const { return new TableCellInt(*this);}

  // the value stored in the cell
  int val;

 ClassDef (TableCellInt,1)
};

#endif
