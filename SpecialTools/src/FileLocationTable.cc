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
          ce->text = basePath[col]+ce->text;
      	
      }//for cells in row
      
  }//for rows
  
  basePathAdded = true;
}//addBasePath

string FileLocationTable::getBasePath(string col){

   vector<TableCell*> cells = tableRows.begin()->getCellEntries();
   for (unsigned int c=0; c<cells.size(); c++){
      if (string(cells[c]->GetName()).compare(col) == 0)
         return basePath[c];
   }
   std::cout << "WARNING::FileLocationTable::getBasePath() could not find "
             << "the BasePath associated with column " << col << std::endl;
   return "";
}

bool FileLocationTable::parseFromFile(string filename, string cellClass, string formatStyle){
  bool toReturn = Table::parseFromFile(filename, cellClass, formatStyle);

  // loop over rows and remove the BasePath row. 
  for (tableRows_it it=tableRows.begin();it!=tableRows.end(); it++){
    // Find the row with title BasePath
    if (string(it->GetName()).compare("BasePath") == 0){
       // Get the vector of cells
       vector<TableCell*> cells = it->getCellEntries();
       for (unsigned int c=0; c<cells.size(); c++){
          basePath.push_back(((TableCellText*)cells[c])->text);
       }
       tableRows.erase(it);
    }
    break;
  }//for rows

  return toReturn;
}

ClassImp(FileLocationTable)
