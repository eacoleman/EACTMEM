////////////////////////////////////////////////////////////////////////////////
//
// MVAVar
// ------
//
//                         11/20/2014 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class defines all of the variables used in TAMUWW for the MVA training
// and reading. This is the single place to store these variable names,
// definitions, units, and types.
////////////////////////////////////////////////////////////////////////////////

#ifndef MVAVar_HH
#define MVAVar_HH

//
// TAMUWW Libraries
//
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"

//
// ROOT Libraries
//
#include "TString.h"

//
// Standard Libraries
//
#include <iostream>
#include <map>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class MVAVar{
public:
    //
    //Constructor/Destructor
    //
    MVAVar();
    MVAVar(TString n, TString def, TString un = "", char t = 'F',
           float v = -9999, bool u = false, bool s = false);
    ~MVAVar();

    //
    // Member Functions
    //
    /// returns the number of variables that will be trained upon
    static int getTSize();

    /// sets up the variables available for training, which are stored in the varMap
    static void setVarMap(std::map<TString,MVAVar> & varMap);

    /// get an iterator to the first used variable
    static std::map<TString,MVAVar>::iterator getFirstUsed(std::map<TString,MVAVar> & varMap, bool spectator = false);

    /// set the variables and spectators to use from a table
    static void setUseFromTable(std::map<TString,MVAVar> & varMap, TString tableRow);

    //
    // Member Data
    //
    TString name, definition, unit;
    char type;
    static const int TSize = 13;
    float value;
    bool use, isSpectator;
    int index, maxIndex;
};

typedef map<TString,MVAVar> MVAVarMap;
typedef vector<map<TString,MVAVar> > MVAVarMapCollection;

#endif
