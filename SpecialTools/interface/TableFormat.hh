// Ricardo Eusebi
// FNAL eusebi@fnal.gov
// created: Monday February 05, 2007
// $Id: TableFormat.hh,v 1.2 2012/06/07 15:02:12 aperloff Exp $

#ifndef TABLEFORMAT_DEF
#define TABLEFORMAT_DEF

//My libraries

//C++ libraries
#include <string>

//ROOT libraries

//----------------------------------------------------------------------------
// The TableCell is the basic unit of a table.
class TableFormat {
public:
  std::string style;
  std::string Row1HeaderPre;
  std::string Row1HeaderPos;
  std::string separator;
  std::string end_row  ;
  std::string end_table;
  std::string begin_table;
  std::string line;
   
   int precision;
   bool noError;

   static TableFormat getFormat(std::string formatName, int numCol);

};

#endif
