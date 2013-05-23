#include "TSystem.h"
#include "TString.h"

#include <utility>

#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"

using namespace std;

namespace HelperFunctions
{
   ////////////////////////////////////////////////////////////////////////////////
   //  Helper Variables
   ////////////////////////////////////////////////////////////////////////////////
   
   ////////////////////////////////////////////////////////////////////////////////
   //  helper Functions
   ////////////////////////////////////////////////////////////////////////////////
   
   /// returns the cross section for the given process
   pair<double,double> getCrossSectionAndError(TString channelName);
   
   /// returns the branching ratio for the given process
   double getBranchingRatio(TString channelName);
   
   /// returns the number of Monte Carlo events for the given process
   double getNumMCEvts(TString channelName);

   /// returns the index of the location of b within a, or -1 if b is not found in a
   int vfind(vector<string> a, string b);
}
