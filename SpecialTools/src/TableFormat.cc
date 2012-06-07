// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableFormat.cc,v 1.1 2011/02/08 21:31:38 eusebi Exp $

//My libraries
#include "TAMUWW/SpecialTools/interface/TableFormat.hh"

//C++ libraries
#include <string>
#include <iostream>
#include <algorithm> // for std::transform

using std::cout;
using std::endl;
using std::string;


//----------------------------------------------------------------------------
//This method return a TableFormat object containing all the elements
// that determines the characters used to print the table. 
// style is case sensitive, and it must be any of the followings
// style = "Normal" Default
// style = "Latex" 
// style = "Tiki"   
// style = "Twiki" equivalent to Tiki
TableFormat TableFormat::getFormat(string style, int numCol){

  TableFormat format;
  format.style = "Undefined";
  format.Row1HeaderPre= "";
  format.Row1HeaderPos= "";
  format.separator = "";
  format.end_row   = "";
  format.end_table = "";
  format.begin_table = "";

  // transform style to uppercase
  std::transform(style.begin(), style.end(), style.begin(),  (int(*)(int)) toupper);

  if(style == "NORMAL") {
    format.style="NORMAL";
    format.separator = "|";
    format.end_row   = " ";
  }else if (style == "LATEX"){
    format.style="LATEX";
    format.separator = "&";
    format.end_row   = "\\\\";
    format.begin_table = "\\begin{tabular}{";
    for (int i = 0; i<numCol; i++)
       format.begin_table+=" c ";
    format.begin_table+="}\n";
    format.end_table = "\\end{tabular}";
  } else if (style == "TIKI" || style == "TWIKI") {
    format.style="TIKI";
    format.Row1HeaderPre = "{FANCYTABLE(head=>";
    format.Row1HeaderPos = ")}";
    format.separator = "~|~";
    format.end_row   = " ";
    format.end_table = "{FANCYTABLE}";
  }else{
    cout<<"ERROR TableFormat::getFormat was called with a unknown style="<<style<<endl;
  }

  return format;
}
