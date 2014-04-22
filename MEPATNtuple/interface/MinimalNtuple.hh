/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// For testing matrix elements
//// Created by Perloff, Alexx : aperloff@physics.tamu.edu
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MINIMALNTUPLE_HH
#define MINIMALNTUPLE_HH

//
// User Defined Includes
//
#include "TAMUWW/SpecialTools/interface/Defs.hh"

//
// ROOT includes
//
#include "TObject.h"
#include "TH3.h"
#include "TProfile3D.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"
#include "TBenchmark.h"
#include "TAxis.h"

//
// Standard Library Includes
//
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

//
// Namespace
//
using std::vector;
using std::map;
using std::string;
using std::pair;
using std::cout;
using std::setw;
using std::left;
using std::cout;
using std::endl;
using std::max;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class MinimalNtuple : public TObject
{
public:
   //
   // Construction/Destruction
   //
   MinimalNtuple();
   MinimalNtuple(int num_profile_bins);
   ~MinimalNtuple();

   //
   // Fill the ntuple with random numbers
   //
   void fillRandom(TRandom* rand);

   //
   // Print the average time taken in the fillRandom function,
   // where entries is the number of times that function was called.
   //
   void printAvgTime(int entries);

   int run;
   int lumi;
   int event;

   int nbins;
   TProfile3D* profile;

   double realTime;
   double cpuTime;

   ClassDef(MinimalNtuple,1)
};

#endif