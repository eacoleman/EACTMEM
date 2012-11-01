#ifndef MICRONTUPLE_HH
#define MICRONTUPLE_HH

#include "TObject.h"

#include <string>
#include <map>
#include <vector>

#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"
#include "TAMUWW/MatrixElement/interface/EventProbDefs.hh"


class MicroNtuple : public TObject
{
public:


  MicroNtuple(); 
  MicroNtuple(int nJets);
  MicroNtuple(const MicroNtuple&);
  MicroNtuple& operator=(const MicroNtuple&);
  ~MicroNtuple();
      
  enum {nEventProb = 29};

  // ===================================
  //  Member Variables
  // ===================================

  Int_t nJets;
  Double_t eventProb[nEventProb];
  Double_t eventMaxProb[nEventProb];

  Double_t* bProb; //[nJets]
  Double_t weight;

  Double_t epd1tag;
  Double_t epd2tag;
  Double_t epd1tagSchan;
  Double_t epd2tagSchan;
  Double_t epd1tagTchan;
  Double_t epd2tagTchan;
  Double_t epdPretagWWandWZ;
  Double_t epd0tagWWandWZ;
  Double_t epd1tagWWandWZ;
  Double_t epd2tagWWandWZ;


  enum {nWHmasses = 11};
  Double_t epd1tagWH[nWHmasses];
  Double_t epd2tagWH[nWHmasses];
 
  enum {nHWWmasses = 14};
  Double_t epd1tagHWW[nHWWmasses];
  Double_t epd2tagHWW[nHWWmasses];

  // Create a map that contains the index of eventProb to be used for a
  // given DEFS::EP::Type and parameter type. This map is static and 
  // could be filled on demand calling fillIndexMap(). 
  // int index = map[DEFS::EP::WH][120]; //should be 13
  // int index = m[DEFS::EP::WZ][0] // should be 1
  typedef std::map<double, int> indexMap1;
  typedef std::map<DEFS::EP::Type , indexMap1> indexMap2;
  static indexMap2 indexMap;

  // The static method that fills the map
  static void fillIndexMap();
  static indexMap2 getIndexMap(){ return indexMap;}


  enum EPDType {kSchan = 0, kTchan = 1, kCombined = 2};

  // ===================================
  //  Members Methods
  // ===================================

  // This method functions as a map which maps from the eventProbs to the ProbsForEPD.
  // If you don't care about the higgs mass set it to zero. It returns the raw meProbs.
  static ProbsForEPD getEventProbs(double mhiggs, const double evtProbs[nEventProb]) ;

  // This method just calls the above method with evtProbs as a parameter
  ProbsForEPD getEventProbs(double mhiggs) const;

  // Get the probabilities for each process
  static ProbsForEPD getProbsForEPD(const ProbsForEPD & meProbs, 
				    const ProbsForEPD & coeffs,
				    const double bProb[],  
				    DEFS::TagCat tagcat);
  
  

  // ===================================
  //  Methods for the particular analyses
  // ===================================

  // Methods for the WH 
  static const ProbsForEPD getWHEPDCoefficients(double mhiggs, DEFS::TagCat tagcat, int nJets); //  WH coefficients 
  Double_t calcWHEPD(DEFS::TagCat tagcat, double mass) const;
  static Double_t calcWHEPD(DEFS::TagCat tagcat,  double mass,
			    const ProbsForEPD & meProbs, 
			    const double bProb[], Int_t arraySize);

  // Methods for the H->WW 
  static const ProbsForEPD getHWWEPDCoefficients(double mhiggs, DEFS::TagCat tagcat, int nJets); //  H->WW coefficients 
  Double_t calcHWWEPD(DEFS::TagCat tagcat, double mass) const;
  static Double_t calcHWWEPD(DEFS::TagCat tagcat,  double mass,
			     const ProbsForEPD & meProbs, 
			     const double bProb[], Int_t arraySize);


  // Methods for the WZ+WW 
  static const ProbsForEPD getWZEPDCoefficients(DEFS::TagCat tagcat, int nJets); // WZ+WW coefficients
  Double_t calcWZEPD(DEFS::TagCat tagcat) const;
  static Double_t calcWZEPD(DEFS::TagCat tagcat, 
			    const ProbsForEPD & meProbs, 
			    const double bProb[], Int_t arraySize);

  // Methods for the SingleTop
  static const ProbsForEPD getSingleTopEPDCoefficients (DEFS::TagCat tagcat, EPDType type);
  Double_t calcSingleTopEPD(DEFS::TagCat tagcat, EPDType type = kCombined) const;
  static Double_t calcSingleTopEPD(DEFS::TagCat tagcat, EPDType type,
				   const ProbsForEPD & meProbs,
				   const double bProb[], Int_t arraySize);


  // ===================================
  //  Other methods
  // ===================================
  static Double_t triggerTO(double detector, double etalep, double metraw,
			    double etlep);


  void clear();

  ClassDef(MicroNtuple, 10)

};

#endif
