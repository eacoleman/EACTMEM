#ifndef PLOTMAP_DEF
#define PLOTMAP_DEF

// Our libraries
#include "TAMUWW/Tools/interface/Plots.hh"

// C++ libraries
#include <string>
#include <map>

using std::string;
using std::map;
using std::cout;
using std::endl;

// ##################################################
// ################# PLOT MAP CLASS #################
// ##################################################

class PlotMap: public std::map<std::string, Plot>
{
public:
   Plot operator[](std::string name);
   
   Plot set(std::string name, Plot plot);
};

#endif
