#ifndef MICRONTUPLE_HH
#define MICRONTUPLE_HH

#include "TObject.h"

#include <string>
#include <map>

#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"
#include "TAMUWW/MatrixElement/interface/EventProbDefs.hh"

class DumbClassToTrickRoot : public TObject
{
public:
  DumbClassToTrickRoot();

  Double_t tagProb1;
  Double_t tagProb2;
  Int_t det;
  Int_t ntag;
  Int_t run;
  Int_t event;

  void clear();

  ClassDef(DumbClassToTrickRoot, 1)

};


class MicroNtuple : public TObject
{
public:


  MicroNtuple(); 
  MicroNtuple(int nJets);
  MicroNtuple(const MicroNtuple&);
  MicroNtuple& operator=(const MicroNtuple&);
  ~MicroNtuple();
      
  enum {nEventProb = 18};

  // ===================================
  //  Member Variables
  // ===================================

  Int_t nJets;
  Double_t eventProb[nEventProb];
  Double_t eventMaxProb[nEventProb];
  Double_t* knntag; //[nJets]
  Int_t* tag; //[nJets]
  Double_t* bProb0; //[nJets]
  Double_t* bProb1; //[nJets]
  Double_t* bProb2; //[nJets]
  Double_t weight;
  Int_t nTagExt;
  Int_t nTagExtLooseSV;
  Double_t tagProbExt0;
  Double_t tagProbExt1;
  Double_t tagProbExt2;
  Double_t tagProbExtLooseSV0;
  Double_t tagProbExtLooseSV1;
  Double_t tagProbExtLooseSV2;
  Double_t epd1tag;
  Double_t epd2tag;
  Double_t epd1tagSchan;
  Double_t epd2tagSchan;
  Double_t epd1tagTchan;
  Double_t epd2tagTchan;
  Double_t epd0tagWWandWZ;
  Double_t epd1tagWWandWZ;
  Double_t epd2tagWWandWZ;


  enum {nWHmasses = 11};
  Double_t epd1tagWH[nWHmasses];
  Double_t epd2tagWH[nWHmasses];
  Double_t epdNoKITWH[nWHmasses];

  //Same as epd<x>tagWH, but using Max Prob. 
  //This is not optimized, so I don't expect it to be better than epd<x>tagWH
  Double_t MPepd1tagWH[nWHmasses];
  Double_t MPepd2tagWH[nWHmasses];

  //Multi-axis projection technique, using integral probability
  Double_t MAPTIPepd1tagWH[nWHmasses];
  Double_t MAPTIPepd2tagWH[nWHmasses];

  //Multi-axis projection technique, using maximal probability
  Double_t MAPTMPepd1tagWH[nWHmasses];
  Double_t MAPTMPepd2tagWH[nWHmasses];

  DumbClassToTrickRoot h;

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


  enum EPDType {kSchan = 0, kTchan = 1, kCombined = 2};

  // ===================================
  //  Members Methods
  // ===================================

  // void fillBProb();
  static Double_t getKNNTagProb(double); 

  // This method functions as a map which maps from the eventProbs to the ProbsForEPD.
  // If you don't care about the higgs mass set it to zero. It returns the raw meProbs.
  static ProbsForEPD getEventProbs(double mhiggs, const double evtProbs[nEventProb]) ;

  // This method just calls the above method with evtProbs as a parameter
  ProbsForEPD getEventProbs(double mhiggs) const;

  // Get the probabilities for each process
  static ProbsForEPD getProbsForEPD(const ProbsForEPD & meProbs, 
				    const ProbsForEPD & coeffs,
				    const double bProb[], unsigned ntag);
  
  

  // ===================================
  //  Methods for the particular analyses
  // ===================================

  // Methods for the WH 
  static unsigned getWHMassIndex(double mass); // Translate from mass to index
  static const ProbsForEPD getWHEPDCoefficients(int indexWH, unsigned tagIndex, int nJets); //  WH coefficients 
  Double_t calcWHEPD(unsigned ntag, double mass) const;
  static Double_t calcWHEPD(unsigned ntag,  double mass,
			    const ProbsForEPD & meProbs, 
			    const double bProb[], Int_t arraySize);

  // Methods for the H->WW 
  static unsigned getHWWMassIndex(double mass); // Translate from mass to index
  static const ProbsForEPD getHWWEPDCoefficients(int indexWH, unsigned tagIndex, int nJets); //  H->WW coefficients 
  Double_t calcHWWEPD(unsigned ntag, double mass) const;
  static Double_t calcHWWEPD(unsigned ntag,  double mass,
			     const ProbsForEPD & meProbs, 
			     const double bProb[], Int_t arraySize);


  // Methods for the WZ+WW 
  static const ProbsForEPD getWZEPDCoefficients(unsigned tagIndex, int nJets); // WZ+WW coefficients
  Double_t calcWZEPD(unsigned ntag) const;
  static Double_t calcWZEPD(unsigned ntag, 
			    const ProbsForEPD & meProbs, 
			    const double bProb[], Int_t arraySize);

  // Methods for the SingleTop
  static const ProbsForEPD getSingleTopEPDCoefficients (unsigned ntag, EPDType type);
  Double_t calcSingleTopEPD(unsigned ntag, EPDType type = kCombined) const;
  static Double_t calcSingleTopEPD(unsigned ntag, EPDType type,
				   const ProbsForEPD & meProbs,
				   const double bProb[], Int_t arraySize);


  // ===================================
  //  Other methods
  // ===================================
  static Double_t eventDump(const DumbClassToTrickRoot* h, double epd,
			    int ALPTtype, double lepPt);

  static Double_t triggerTO(double detector, double etalep, double metraw,
			    double etlep);


  void clear();

  ClassDef(MicroNtuple, 8)

};

#endif
