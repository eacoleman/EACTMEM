#ifndef CSVREWEIGHT_DEF
#define CSVREWEIGHT_DEF

// TAMUWW libraries
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

// C++ libraries
#include <iostream>
#include <string>
#include <cmath>
#include <string>

// ROOT libraries
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH1D.h"

class CSVreweight{

public:

   CSVreweight();
   // Returns the CSV event weight
   double getWeight(EventNtuple * ntuple);
   // Returns true if both files are open (otherwise returns false)
   bool checkFiles();
   // Opens the two configuration files
   void openFiles();
   // Closes the two configuration files
   void closeFiles();
   // Gets pointers to the histograms in the configuration files
   void getHistosFromFiles(); 
   
private:

   //Pointers to the files needed by the constructor
   TFile * f_CSVwgt_HF;
   TFile * f_CSVwgt_LF;

   //Histogram storage containers
   TH1D* h_csv_wgt_hf[9][5];
   TH1D* c_csv_wgt_hf[9][5];
   TH1D* h_csv_wgt_lf[9][3][3];

};

#endif
