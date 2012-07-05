#include "TAMUWW/Tools/interface/PlotMap.hh"

// ##################################################
// ################# PLOT MAP CLASS #################
// ##################################################

Plot* PlotMap::operator[](string name)
{
   if(end() == find(name))
   {
      cout << endl << "##### WARNING #####" << endl;
      cout << "Element: " << name << " could not be found in ProtectedMap." << endl;
      cout << "Aborting..." << endl;
      cout << "###################" << endl << endl;
      
      throw;
   }
   
   return map<string, Plot*>::operator[](name);
}

Plot* PlotMap::set(string name, Plot* plot)
{
   return map<string, Plot*>::operator[](name) = plot;
}

// ##################################################
// ########### FORMMATTED PLOT MAP CLASS ############
// ##################################################

FormattedPlot* FormattedPlotMap::operator[](string name)
{
   if(end() == find(name))
   {
      cout << endl << "##### WARNING #####" << endl;
      cout << "Element: " << name << " could not be found in ProtectedMap." << endl;
      cout << "Aborting..." << endl;
      cout << "###################" << endl << endl;
      
      throw;
   }
   
   return map<string, FormattedPlot*>::operator[](name);
}

FormattedPlot* FormattedPlotMap::set(string name, FormattedPlot* plot)
{
   return map<string, FormattedPlot*>::operator[](name) = plot;
}
