// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: Table.hh,v 1.4 2011/04/27 17:21:09 aperloff Exp $

#ifndef TABLE_DEF
#define TABLE_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/TableFormat.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCell.hh"

//C++ libraries
#include <string>
#include <vector>
#include <ostream>
#include <fstream>

//ROOT libraries
#include "TNamed.h"
#include "TCollection.h"

//----------------------------------------------------------------------------
// a Table is just a collection of rows
class  Table : public TNamed {
  
public :
  
  Table(std::string tableName = "default_Table");  

  ~Table();
  
  // The operators  
  Table operator+(const Table &rhs) const;
  Table & operator+=(const Table &rhs);

  Table operator-(const Table &rhs) const;
  Table & operator-=(const Table &rhs);

  Table operator*(double rhs) const;
  Table & operator*=(double rhs);

  Table operator/(double rhs) const;
  Table & operator/=(double rhs);

  Table operator*(Value rhs) const;
  Table & operator*=(Value rhs);

  Table operator/(Value rhs) const;
  Table & operator/=(Value rhs);

  // Reset the contents of the table. Keep the name and structure. 
  void reset();
  
  //print the table to a file
  void printToFile(std::string filename, std::string style = "Normal");

  // print the Table in different formats
  void printTable(std::ostream &out,std::string style = "Normal");
  
  // add two tables together
   void addTable(Table &, unsigned int omitFirstRows = 0);

  // add a single row
  void addRow(TableRow row) { tableRows.push_back(row);}

  // set the vector of rows
  void setRows(std::vector<TableRow> rows) { tableRows = rows;}

  // get the vector of rows
  std::vector<TableRow> getRows() const { return tableRows;} ;

  // get the tableCell object for a given Row and Column
  TableCell* getCellRowColumn(std::string row, std::string col);
      
  // A test to fill the table
  void fillWithTest();

  // reports true if the file was parsed successfully
  virtual bool parseFromFile(std::string filename, std::string cellClass = "TableCellText", std::string formatStyle = "Normal");

  // reports true if the Line was parsed successfully
  bool parseLine(std::string filename, int lineCounter, 
		 int goodLineCounter, std::string cellClass,
		 TableFormat format);

  // get the origin of the table
  std::string getTableOrigin() {return tableOrigin;}

  // merge tables into a single table if they have the same name, column, and row titles
  int Merge(TCollection *list);

 protected:

  // Create a newCell of type determined by cellClass
  TableCell* createNewCell(std::string cellClass, std::string cellName);
  
  // The table class is just a vector of rows
  std::vector<TableRow> tableRows;

  // The origin of the table:
  // - The file this table was sucessfuly parsed from, if any.
  //   includes tables that were copied and modified.
  // - Automatic otherwise.
  std::string tableOrigin;

  // Provide an interator for code simplification.
  typedef  std::vector<TableRow>::iterator tableRows_it;

   ClassDef (Table,1)
};

#endif
