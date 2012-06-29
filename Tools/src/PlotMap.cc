#include "TAMUWW/Tools/interface/PlotMap.hh"

// ##################################################
// ################# PLOT MAP CLASS #################
// ##################################################

Plot PlotMap::operator[](string name)
{
   if(end() == find(name))
   {
      cout << endl << "##### WARNING #####" << endl;
      cout << "Element: " << name << " could not be found in PlotMap." << endl;
      cout << "Aborting..." << endl;
      cout << "###################" << endl << endl;
      
      throw;
   }
   
   return map<string, Plot>::operator[](name);
}

Plot PlotMap::set(string name, Plot plot)
{
   return map<string, Plot>::operator[](name) = plot;
}
