// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: Table.hh,v 1.1 2011/02/08 21:31:38 eusebi Exp $

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

//ROOT libraries
#include "TNamed.h"

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
  
  // print the Table in different formats
  void printTable(std::string style = "Normal");
  
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
  bool parseFromFile(std::string filename, std::string formatStyle = "Normal");

  // reports true if the Line was parsed successfully
  bool parseLine(std::string filename, int lineCounter, 
		 int goodLineCounter, std::string cellClass,
		 TableFormat format);

  // get the origin of the table
  std::string getTableOrigin() {return tableOrigin;}

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
