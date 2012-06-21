#ifndef PUREWEIGHT_DEF
#define PUREWEIGHT_DEF

// C++ libraries
#include <string>

// ROOT libraries
#include "TString.h"
#include "TH1.h"

class PUreweight{

 public:

  PUreweight(TString dataFile, TString MCFile, TString dataHisto, TString MCHisto);
  double getWeight(double tnpu);
  TH1 *getHistoFromFile(TString filename, TString histoname); 

 private:


  TH1 *weights_;
};

#endif
