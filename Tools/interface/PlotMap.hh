#ifndef PLOTMAP_DEF
#define PLOTMAP_DEF

// Out Libraries
#include "TAMUWW/Tools/interface/Plots.hh"

// C++ libraries
#include <iostream>
#include <string>
#include <map>

using std::string;
using std::map;
using std::cout;
using std::endl;

// ##################################################
// ################ PLOT MAP CLASS ##################
// ##################################################

class PlotMap: public std::map<string, Plot*>
{
public:
   Plot* operator[](string name);
   
   Plot* set(string name, Plot* plot);
};

// ##################################################
// ############ FORMATTED PLOT MAP CLASS ############
// ##################################################

class FormattedPlotMap: public std::map<string, FormattedPlot*>
{
public:
   FormattedPlot* operator[](string name);
   
   FormattedPlot* set(string name, FormattedPlot* plot);
};

#endif
