#include "TAMUWW/SpecialTools/interface/FileLocationTable.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"

// C++ libraries
#include <vector>
#include <string>

using std::string;
using std::vector;
 
FileLocationTable::FileLocationTable(std::string name){
  SetName(name.c_str());
  basePathAdded = false;
  basePath = "";
}

void FileLocationTable::addBasePath(){
  if(basePathAdded) return;
  // loop over rows. 
  for (tableRows_it it=tableRows.begin();it!=tableRows.end(); it++){
        
      // get the cells and loop over them
      vector<TableCell*> cells = it-> getCellEntries();
      for (unsigned int col = 0 ;col < cells.size();col++){

        TableCellText * ce = (TableCellText*) cells[col];
        if (ce) 
          ce->text = basePath+ce->text;
      	
      }//for cells in row
      
  }//for rows
  
  basePathAdded = true;
}//addBasePath

bool FileLocationTable::parseFromFile(string filename, string cellClass, string formatStyle, string columnSuffix){
  bool toReturn = Table::parseFromFile(filename, cellClass, formatStyle);

  if((TableCellText*) getCellRowColumn("BasePath","FilePath"+columnSuffix))
    basePath = ((TableCellText*) getCellRowColumn("BasePath","FilePath"+columnSuffix))->text;
  
  // loop over rows and remove the BasePath row. 
  for (tableRows_it it=tableRows.begin();it!=tableRows.end(); it++){
    // Find the row with title BasePath
    if (string(it->GetName()).compare("BasePath") == 0){
      tableRows.erase(it);
    }
    break;
  }//for rows

  return toReturn;
}

ClassImp(FileLocationTable)
