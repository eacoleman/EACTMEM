#ifndef MICRONTUPLE_HH
#define MICRONTUPLE_HH

#include "TObject.h"
#include "string"

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

class ProbsForEPD {
public:
  ProbsForEPD();

  double ww     ;
  double wz     ;
  double wjg    ;
  double zjg    ;
  double tt     ;
  double tchan  ;
  double schan  ;
  double qcd    ;

  double tchan2 ;
  double wbb    ;
  double wcc    ;
  double wc     ;
  double wh     ;

  // This just print all the elements to screen
  void Show(std::string);

  // This divides each member to the sum of all members
  void Normalize();

  // This divides each member by the maximum member
  void NormalizeToMax();

};

class MicroNtuple : public TObject
{
public:


  MicroNtuple(); // I HATE ROOT!
  MicroNtuple(int nJets);
  MicroNtuple(const MicroNtuple&);
  MicroNtuple& operator=(const MicroNtuple&);
  ~MicroNtuple();
      
  //enum {nEventProb = 21};
  enum {nEventProb = 22};

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

  enum EPDType {kSchan = 0, kTchan = 1, kCombined = 2};

  // void fillBProb();
  static Double_t getKNNTagProb(double);
  Double_t calcEPD(unsigned ntag, EPDType type = kCombined) const;
  static Double_t calcEPD(unsigned ntag, EPDType type,
			  const double eventProb[nEventProb],
			  const double bProb[], Int_t arraySize);

  // Get the coefficients neeed to calculate the EPD
  static const double * getSingleTopEPDCoefficients (unsigned ntag, EPDType type);

  // Translate from mass to index
  static unsigned getHiggsMassIndex(double mass);

  // Get the coefficients neeed to calculate the EPD
  static const double * getHiggsEPDCoefficients(int indexWH, unsigned tagIndex, int nJets);

  // Get the coefficients neeed to calculate the EPD with maxProb
  static const double * getHiggsEPDMaxProbCoefficients(int indexWH, unsigned ntag);

  // Get the probabilities for each process
  static ProbsForEPD getProbsForEPD(const double eventProb[nEventProb], 
				    const double * params,
				    //const vector<double> & params,
				    unsigned int indexWH,
				    const double bProb[], unsigned ntag);
  

  //This one is the regular EPD 
  Double_t calcHiggsEPD(unsigned ntag, double mass) const;
  static Double_t calcHiggsEPD(unsigned ntag,  double mass,
			       const double eventProb[nEventProb], 
			       const double bProb[], Int_t arraySize);

  // This one is MaxProb EPD
  Double_t calcHiggsMaxProbEPD(unsigned ntag, double mass) const;
  static Double_t calcHiggsMaxProbEPD(unsigned ntag,  double mass,
			       const double eventProb[nEventProb], 
			       const double bProb[], Int_t arraySize);

  //This one is EPD + Max Prob EPD
  Double_t calcHiggsSuperEPD(unsigned ntag, double mass) const;
  static Double_t calcHiggsSuperEPD(unsigned ntag,  double mass,
				    const double eventProb[nEventProb], 
				    const double bProb[], Int_t arraySize);

  Double_t calcWZEPD(unsigned ntag, unsigned secvtxtag) const;
  static Double_t calcWZEPD(unsigned ntag,  unsigned secvtxtag,
			    const double eventProb[nEventProb], 
			    const double bProb[], Int_t arraySize);


  static Double_t eventDump(const DumbClassToTrickRoot* h, double epd,
			    int ALPTtype, double lepPt);

  static Double_t triggerTO(double detector, double etalep, double metraw,
			    double etlep);

  Bool_t cutEvents() const;

  void clear();

  ClassDef(MicroNtuple, 7)

};

#endif
