// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id:$

//My libraries
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"

//C++ libraries
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip> //setw and others

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostringstream;

//----------------------------------------------------------------------------
Table::Table(string tableName) {

  SetName(tableName.c_str());
   
}//C'tor
 
//----------------------------------------------------------------------------
Table::~Table() {} //D'tor
   

//----------------------------------------------------------------------------
void Table::reset(){

  for (unsigned int i=0;i<tableRows.size();i++)
    tableRows[i].reset();

}//reset


//----------------------------------------------------------------------------
//The provided string determines the characters used to print the table. 
// style is case sensitive, and it must be any of the followings
// style = "Normal" Default
// style = "Latex" 
// style = "Tiki"   
// style = "Twiki" equivalent to Tiki
void Table::printTable(string style){

  // get the format delimiters
  TableFormat format  = TableFormat::getFormat(style);

  //Start from a fresh line
  cout<<endl;

  // The string to where we print
  ostringstream oss;

  // Create and find the max size of each column
  vector<size_t> maxSizes;

  // start by including the table name in the first column
  maxSizes.push_back(string(GetName()).length());

  // loop over rows. maxSizes[0] must exist
  for (tableRows_it it=tableRows.begin();it!=tableRows.end(); it++){

    size_t lenR = string(it->GetName()).length();
    if (lenR > maxSizes[0] )
      maxSizes[0] =  lenR;
    
    // get the cells and loop over them
    vector<TableCell> cells = it-> getCellEntries();
    for (unsigned int col = 0 ;col < cells.size();col++){
      size_t len = cells[col].print(format).length();
	
      if ((col+1) < maxSizes.size()){
	if (len < maxSizes[col+1] )
	  maxSizes[col+1] =  len;
      }else
	maxSizes.push_back(len);
	
    }//for cells in column

  }//for rows


  // Now that we know the actual size of each column just print the table
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  // This is the number of spaces between the information in each column
  // and the separator
  const size_t pad = 1;

  //Print the table name followed by the column names of the first row
  oss<<std::setw(maxSizes[0]+pad)<<format.Row1HeaderPre+GetName()<<" "<<format.separator;

  
  if (tableRows.size()>0){
 
   vector<TableCell> cells = tableRows[0].getCellEntries();
  
    // Print the cell's names  
    for (int col=0;col<(int)cells.size();col++){
      if (col < (int)cells.size()-1 )
	oss << std::setw(maxSizes[col+1]+pad) << cells[col].GetName() << " "<<format.separator.c_str() ;
      else{
	oss << std::setw(maxSizes[col+1]+pad) << cells[col].GetName();
      }
    }// for cells
    oss<<format.end_row+format.Row1HeaderPos<<endl;    

    //Loop over all rows printing their info
    for (tableRows_it it=tableRows.begin();it!=tableRows.end(); it++) {
      vector<TableCell> cells = it->getCellEntries();
      
      // Print the Cut name and separator
      oss << std::setw(maxSizes[0]+pad)<<it->GetName() << " " << format.separator;
      
      // Print the info in each cell followed by the separator when needed
      for (int col=0;col<(int)cells.size();col++){
	if (col < (int)cells.size()-1 )
	  oss << std::setw(maxSizes[col+1]+pad) << cells[col].print(format) << " " << format.separator;
	else
	  oss << std::setw(maxSizes[col+1]+pad) << cells[col].print(format);
      }

      oss<<format.end_row<<endl;
    }// for rows

  } 

  oss<<format.end_table<<endl;

  // Print the string stream to the default output
  cout<<oss.str()<<endl;

}//printTable


//----------------------------------------------------------------------------
Table & Table::operator+=(const Table & rhs){

  vector<TableRow> table2Cuts = rhs.getRows();

  //Check
  if (table2Cuts.size() != tableRows.size()){
    cout<<"ERROR  Table::AddTable Tables have two different number of rows,"<<endl
	<<" Returning without adding."<<endl;
    return *this;
  }

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    tableRows[it] += table2Cuts[it];

  return *this;

}//operator+=

//----------------------------------------------------------------------------
Table Table::operator+(const Table &rhs) const {

  // Make a copy in which I add the right hand side
  Table res = *this;
  res += rhs;
  return res;

}//operator+


//----------------------------------------------------------------------------
Table & Table::operator-=(const Table & rhs){

  vector<TableRow> table2Cuts = rhs.getRows();

  //Check
  if (table2Cuts.size() != tableRows.size()){
    cout<<"ERROR  Table::AddTable Tables have two different number of rows,"<<endl
	<<" Returning without adding."<<endl;
    return *this;
  }

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    tableRows[it] -= table2Cuts[it];

  return *this;

}//operator-=

//----------------------------------------------------------------------------
Table Table::operator-(const Table &rhs) const {

  // Make a copy in which I add the right hand side
  Table res = *this;
  res -= rhs;
  return res;

}//operator-

//----------------------------------------------------------------------------
Table Table::operator*(double rhs) const {

  // Make a copy in which I add the right hand side
  Table res = *this;
  res *= rhs;
  return res;

}//operator*

//----------------------------------------------------------------------------
Table & Table::operator*=(double rhs) {

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    tableRows[it] *= rhs;

  return *this;

}//operator*=

//----------------------------------------------------------------------------
Table Table::operator/(double rhs) const {

  // Make a copy in which I add the right hand side
  Table res = *this;
  res /= rhs;
  return res;

}//operator/

//----------------------------------------------------------------------------
Table & Table::operator/=(double rhs) {

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    tableRows[it] /= rhs;

  return *this;

}//operator/=


//----------------------------------------------------------------------------
Table Table::operator*(Value rhs) const {

  // Make a copy in which I add the right hand side
  Table res = *this;
  res *= rhs;
  return res;

}//operator*

//----------------------------------------------------------------------------
Table & Table::operator*=(Value rhs) {

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    tableRows[it] *= rhs;

  return *this;

}//operator*=

//----------------------------------------------------------------------------
Table Table::operator/(Value rhs) const {

  // Make a copy in which I add the right hand side
  Table res = *this;
  res /= rhs;
  return res;

}//operator/

//----------------------------------------------------------------------------
Table & Table::operator/=(Value rhs) {

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    tableRows[it] /= rhs;

  return *this;

}//operator/=

//----------------------------------------------------------------------------
void Table::addTable(Table & table2, int omitFirstCuts){

  vector<TableRow> table2Cuts = table2.getRows();

  //Check
  if (table2Cuts.size()!=tableRows.size()){
    cout<<"ERROR  Table::AddTable Tables have two different number of rows,"<<endl
	<<" Returning without adding."<<endl;
    return ;
  }

  //Loop over all the cuts adding the information of the other table
  for (int it=0;it<(int)tableRows.size(); it++)
    if (it>=omitFirstCuts)
      tableRows[it].addTableRow(table2Cuts[it]);

}//addTable

//----------------------------------------------------------------------------
Value Table::getValueAtRowColumnStrings (string row, string col){ 

  //Loop over all the rows trying to find one named row
  for (int it = 0; it < (int)tableRows.size(); it++){

    if (row.compare(tableRows[it].GetName()) == 0){

      // Get the vector of cells
      vector<TableCell> cells = tableRows[it].getCellEntries();

      // found it, now found the cell with name col
      for (int cc = 0; cc < (int) cells.size(); cc++){
	if (col.compare(cells[cc].GetName()) == 0) {
	  return cells[cc].val;
	}
      }//for cells

    }//if row

  }//for rows
  
  cout<<"ERROR Table::getValueAtRowColumnStrings failed to find row="<<row<<" and col="<<col<<endl;
  return Value(0,0);

}//getValueAtRowColumnStrings 

//----------------------------------------------------------------------------
// A test table.
void Table::fillWithTest(){

  vector<TableRow> rows;
  for (int r=0;r<15;r++){
    
    vector<TableCell> cells ;
    // Create 
    for (int c=0;c<5;c++){
      ostringstream oss;
      oss<< c<<"jets";
      TableCell cell(oss.str());
      cell.val.value = c;
      cell.val.value = c*0.5;
      
      cells.push_back(cell);
    }//for cells
    
    ostringstream oss;
    oss <<"cut "<<r;
    TableRow row(oss.str());
    row.setCellEntries(cells);
    rows.push_back(row);
  }//for rows

  tableRows = rows;
}

//----------------------------------------------------------------------------
// reports true if the file was parsed successfully. This method
// just loops over lines and pass them to parseLine
bool Table::parseFromFile(string filename, string style){

  ifstream inputFile (filename.c_str());
  
  // check that the file is open
  if (inputFile.is_open()){
    
    TableFormat format  = TableFormat::getFormat(style);

    // clear the current table
    tableRows.clear();

    // Read lines until the eof is reached
    int lineCounter = 0;
    int goodLineCounter = 0;
    string currentLine;

    while (inputFile.good()){

      // get the next line from the file
      getline(inputFile, currentLine);
      
      // remove leading and trailing spaces on the line
      PeterFunctions::trimSpaces(currentLine);

      lineCounter++;

      // if this is an empty line or a comment (which starts with a "#" char) 
      // continue with the while loop
      if (currentLine.length() == 0 || currentLine.find_first_of('#') == 0)
	continue;

      goodLineCounter ++;

      if (!parseLine(currentLine, lineCounter, goodLineCounter, format)){
	cout<<"ERROR  Table::ParseFromFile() file "<<filename
	    <<" could not be parsed at line "<<lineCounter<<endl;
	return false;
      }
      
    }// while

    // Close the file
    inputFile.close();
    
  }else{
    cout<<"ERROR  Table::ParseFromFile() could not open file="<<filename<<endl;
  }
  return true;

}//ParseFromFile

//----------------------------------------------------------------------------
// reports true if the file was parsed successfully
bool Table::parseLine(string currentLine, int lineCounter, 
		      int goodLineCounter, TableFormat format){
  
  static vector<string> colNames; 
 
  // split the line into fields separated by "|"
  vector<string> fields = PeterFunctions::splitLineIntoWords(currentLine,format.separator);
  
  // remove leading and trailing spaces on all fields
  for (int ff=0;ff < (int) fields.size(); ff++)
    PeterFunctions::trimSpaces(fields[ff]);
  

  // If it is the first good line 
  if (goodLineCounter == 1 && fields.size() > 0) {

    // set the name of the table 
    SetName(fields[0].c_str());
 
    // save the names of the columns for later use.
    colNames = fields; // field 0th is the table name and shouldn't be used.
    
  }else{ 
    // These are the different rows. 
    
    // Check that field size must be smaller than the number of columns
    if (fields.size() > colNames.size()) {
      cout<<"ERROR Table::parseFromFile. Line number "<< (lineCounter+1)
	  <<"  has "<< fields.size() <<" which is more than the "
	  << colNames.size()<< " column names"<<endl;
      return false;
    }
    
    // make sure there is at least two fields (rowname and one entry)
    if (fields.size() > 0 && fields[0].size() > 0 ){
      
      // Create the cells in this row
      vector <TableCell> cells;
      for (int cc = 1; cc <  (int) fields.size() ; cc++){
	
	// for each field create a cell with name colNames[cc]
	TableCell cell(colNames[cc]);
	// set the information from the file
	if (!cell.parseFromFile(fields[cc],format)){
	  cout<<" ERROR in Table::parseFromFile, method TableCell::parseFromFile(string str) could not parse string="<<fields[cc]<<"="<<endl;
	  cout<<" Table::parseFromFile() failed!"<<endl;
	  return false;
	}
	cells.push_back(cell);
	
      }	
      
      // Create the row and put the cells in
      TableRow row(fields[0]);
      row.setCellEntries(cells);
      tableRows.push_back(row);
      
    }// fields.size >0
  }// else
  
  return true;
  
}//ParseLine

ClassImp(Table)
