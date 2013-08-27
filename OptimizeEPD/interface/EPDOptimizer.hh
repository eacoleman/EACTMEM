// This macro is intended to calculate the most discriminating EPD coefficients
// Author: Ricardo Eusebi, November 10th, 2008

//ROOT libraries
#include <TStyle.h>
#include "TDirectory.h"
#include "TH1.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TMath.h"
#include "TGraph.h"
#include "TMinuit.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

//C++ libraries
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

//Other libraries
#include "TAMUWW/OptimizeEPD/interface/PhysicsProcessForOpt.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/SpecialTools/interface/FigureOfMerit.hh"
#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"

class EPDOptimizer{

  //input values
  static DEFS::Ana::Type anaType;
  static DEFS::TagCat tagcat;
  static std::vector<PhysicsProcessForOpt*> processes; //must be loaded outside of this class!
  static double mhiggs; //NOTE: mhiggs is used in two places: to calculate FOM and when loading... in calculating it, mhiggs = 0????
  static DEFS::JetBin jetBin; //NOTE: optimizeEPDCoefficients.C seems to only call ONE type of this.... make it a variable or no?
  static bool debug;

  //return values
  double bestFOM;
  ProbsForEPD bestEPD;
  TH1* bestSignalHisto;
  TH1* bestBackHisto;

  //intermediate values
  static TH1* signalHisto;
  static TH1* allBackHisto;
  static ProbsForEPD normEPDcoeffs;

public:

  //constructor
  EPDOptimizer(DEFS::Ana::Type ana, DEFS::TagCat tc, std::vector<PhysicsProcessForOpt*> prc,
	       double mh = 0, DEFS::JetBin jetBin = DEFS::jets2, bool debug = false);

  //processes the data, calls minuit, sets the results
  void optimize();
  
  //return functions, needs optimize() to be called first
  double getFOM() {return bestFOM;}
  ProbsForEPD getEPD() {return bestEPD;}

private:

  ProbsForEPD optimizeEPDCoeffs(DEFS::Ana::Type anaType, vector<PhysicsProcessForOpt*> processes, 
				DEFS::TagCat tagcat, double & bestFigOfMerit);

  static Double_t createHistoAndGetFOM( DEFS::Ana::Type anaType,
			     vector<PhysicsProcessForOpt*> processes, 
			     const ProbsForEPD & coeffs, 
			     DEFS::TagCat tagcat, double mhiggs,
			     TH1 * signalHisto,  TH1 * allBackHisto);


  //Minuit2 input function
  static Double_t calculateFOM(const Double_t *EPDcoeffs);

  //ClassDef(EPDOptimizer, 0)

};

