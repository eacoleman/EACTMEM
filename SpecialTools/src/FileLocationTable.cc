#include "TAMUWW/SpecialTools/interface/FileLocationTable.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"

// C++ libraries
#include <vector>
#include <string>

using std::string;
using std::vector;
 
FileLocationTable::FileLocationTable(std::string name){
  SetName(name.c_str());
}

void FileLocationTable::addBasePath(string basepath, string rowNameToOmit){

  // loop over rows. 
  for (tableRows_it it=tableRows.begin();it!=tableRows.end(); it++){
        
    // if we shouldn't omit this row
    if (string(it->GetName()).compare(rowNameToOmit) != 0){

      // get the cells and loop over them
      vector<TableCell*> cells = it-> getCellEntries();
      for (unsigned int col = 0 ;col < cells.size();col++){

	TableCellText * ce = (TableCellText*) cells[col];
	if (ce) 
	  ce->text = basepath+ce->text;
      	
      }//for cells in row

    } // do not omit row

  }//for rows

}//addBasePath

ClassImp(FileLocationTable)
