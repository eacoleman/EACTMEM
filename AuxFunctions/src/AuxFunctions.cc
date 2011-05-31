#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;

void AuxFunctions::copyHistStyle(const TH1* from, TH1* to)
{
  to->SetFillColor(from->GetFillColor());
  to->SetFillStyle(from->GetFillStyle());
  to->SetLineColor(from->GetLineColor());
  to->SetLineStyle(from->GetLineStyle());
  to->SetLineWidth(from->GetLineWidth());
  to->SetMarkerColor(from->GetMarkerColor());
  to->SetMarkerStyle(from->GetMarkerStyle());
  to->SetMarkerSize(from->GetMarkerSize());
}

string AuxFunctions::removeFunnyChar(const string& input)
{
  const string excluded = " ()[]/\\~;:{}*&$<>`!@#%^+|\'\",?";
  string answer(input);

  string::size_type pos = answer.find_first_of(excluded);
  while (pos != string::npos)
    {
      answer.erase(pos, 1);
      pos = answer.find_first_of(excluded);
    }

  return answer;
}
bool AuxFunctions::stringSub(std::string& input, std::string replace,
                               std::string with)
{
   bool replaced = false;
   std::string::size_type position = input.find(replace, 0);
   
   while (position != std::string::npos)
   {
      input.erase(position, replace.length());
      input.insert(position, with);
      
      position = input.find(replace, 0);
      replaced = true;
   }
   return replaced;
}


vector<string> AuxFunctions::splitLineIntoWords(string line, const string & match){

  // the returning vector of words
  vector<string> words;

  unsigned int last = 0;
  size_t current;

  do {

    // get the rest of the line
    unsigned int ss = line.length();
    line = line.substr(last, ss);
  
    // find the next match, and the substring
    current = line.find(match);

    string part = line.substr(0, current);
    
    // include 0-length strings.
    words.push_back(part);
    
    // if current is already string::npos then the loop would break
    last = current + match.length();
   
  }while (current != string::npos)  ;

  return words;
  
}//splitLineIntoWords

void AuxFunctions::trimSpaces(string& str) {

  // Trim Both leading and trailing spaces
  size_t startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
  size_t endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse af
  
  // if all spaces or empty return an empty string
  if(( string::npos == startpos ) || ( string::npos == endpos))
    {
      str = "";
    }
  else
    str = str.substr( startpos, endpos-startpos+1 );

} // trimSpaces

 
