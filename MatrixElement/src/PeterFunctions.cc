#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"

bool PeterFunctions::stringSub(std::string& input, std::string replace,
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
