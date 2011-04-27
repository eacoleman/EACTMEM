// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: FileLocationTable.hh,v 1.1 2011/03/01 02:02:45 eusebi Exp $

#ifndef FILELOCATIONTABLE_DEF
#define FILELOCATIONTABLE_DEF

//My libraries
#include "TAMUWW/SpecialTools/interface/Table.hh"

//C++ libraries
#include <string>

//ROOT libraries
#include "TNamed.h"

//----------------------------------------------------------------------------
// a Table is just a collection of rows
class  FileLocationTable : public Table {
  
public :
  
  FileLocationTable(std::string name = "FileLocationTable");
 
  void addBasePath();

  std::string getBasePath() {return basePath;}

  virtual bool parseFromFile(std::string filename, std::string cellClass = "TableCellText", std::string formatStyle = "Normal");

private :

  std::string basePath;
  bool basePathAdded;

  ClassDef (FileLocationTable,1)
};

#endif
