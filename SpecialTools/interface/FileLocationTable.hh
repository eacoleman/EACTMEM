// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: FileLocationTable.hh,v 1.1 2011/02/08 21:31:38 eusebi Exp $

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
 
  void addBasePath(std::string, std::string rowNameToOmit="BasePath");

  ClassDef (FileLocationTable,1)
};

#endif
