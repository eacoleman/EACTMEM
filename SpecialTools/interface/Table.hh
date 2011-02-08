// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id:$

#ifndef TABLE_DEF
#define TABLE_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/Value.hh"
#include "TAMUWW/SpecialTools/interface/TableFormat.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"

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

  // Reset the contents of the table. Keep the name and structure 
  void reset();
  
  // print the Table in different formats
  void printTable(std::string style = "Normal");
  
  // add two tables together
   void addTable(Table &,int omitFirstRows = 0);

  // set the vector of rows
  void addRow(TableRow row) { tableRows.push_back(row);}

  // set the vector of rows
  void setRows(std::vector<TableRow> rows) { tableRows = rows;}
  std::vector<TableRow> getRows()          { return tableRows;}

  // get the vector of rows
  std::vector<TableRow> getRows() const { return tableRows;} ;

  Value getValueAtRowColumnStrings (std::string, std::string);
  
  // A test to fill the table
  void fillWithTest();

  // reports true if the file was parsed successfully
  bool parseFromFile(std::string filename, std::string formatStyle = "Normal");

  // reports true if the Line was parsed successfully
  bool parseLine(std::string filename, int lineCounter, 
		 int goodLineCounter, TableFormat format);

 private:
  
   typedef  std::vector<TableRow>::iterator tableRows_it;
   std::vector<TableRow> tableRows;

   ClassDef (Table,1)
};

#endif
