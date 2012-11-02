#ifndef METREE_HH
#define METREE_HH

#include "TAMUWW/MatrixElement/interface/EventProbDefs.hh"

#include <TObject.h>
#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif
#include <TClonesArray.h>

#include <string>
#include <map>
#include <vector>

class ProbStat : public TObject{

public:

  ProbStat();
  ProbStat(int number);
  ProbStat(const ProbStat&);
  const ProbStat& operator=(const ProbStat&);
  ~ProbStat();

  // The Matrix Element whose results are contained here
  DEFS::EP::Type tmeType;

  // A relevant parameter used in the computation of the ME.
  // For Higgs (or top) matrix elements this is typically the 
  // Higgs (top) mass.
  double tmeParam;
 
  //The number of permutation for a given ME probability
  Int_t tN;

  Double_t* tNevl;    //[tN]
  Double_t* tRelerr;  //[tN]
  Double_t* tIfail;   //[tN]
  Double_t  tRealTime;
  Double_t  tCpuTime;
  Double_t* tProb;    //[tN]
  Double_t* tJet0Type;    //[tN]
  Double_t* tJet1Type;    //[tN]
  Double_t* tJet2Type;    //[tN]
  Double_t* tJet3Type;    //[tN]
  Double_t* tJet4Type;    //[tN]
  Double_t* tJet5Type;    //[tN]
  Double_t* tMaxProb; //[tN]
  Double_t  tEventProb;
  Double_t  tEventErr;
  Double_t  tEventMaxProb; 
  
  ClassDef(ProbStat, 4)

};



class METree : public TObject{

public:
   METree();
   METree(const METree&);
   const METree& operator=(const METree&);
   ~METree();
   
   void clear();
   
   void addProbStat(const ProbStat& stat);
   
   Int_t getNProbStat() const {return m_tSize;}
   const ProbStat* getProbStat(unsigned index) const;
   
   Float_t getEPD(double btag = 0) const;
   
   Int_t getRun() const {return m_run;}
   Int_t getEvent() const {return m_event;}
   
   void setRunEvent(Int_t run, Int_t event) {m_run = run; m_event = event;}
   
   // This method sets up the MVA reader.
   // The weights read in by this method must be previously trained.
   // dir is the directory to the weightfiles.
   void setMVAReader(std::vector<TString> MVAMethods, TString dir);

   // This method returns the response, error, probability, and rarity of 
   // the MVA classifier specified by the user.
   // effS is used for the efficiency calculator for cut method.
   // The outputs can be accessed using the map keys "response", "error", "probability", and "rarity".
   std::vector<std::map<TString,Double_t> > getMVAOutput(std::vector<TString> MVAMethods, Double_t effS = 0.7);
   std::vector<Double_t> getMVAOutput(std::vector<TString> MVAMethods, TString specificOutput, Double_t effS = 0.7);
   Double_t getMVAOutput(TString MVAMethod, TString specificOutput, Double_t effS = 0.7);

   // This method returns the efficiency for the CutsGA MVA method.
   // nSelCutsGA is the number of cuts that passed.
   Double_t getCutsGAEfficiency(Int_t nSelCutsGA, Int_t nTreeEntries);

   // Retrieve cuts for particular signal efficiency
   // CINT ignores dynamic_casts so we have to use a cuts-secific Reader function to acces the pointer
   void getMVACuts(std::vector<TString> &inputVars, std::vector<TString> &inputLabels, 
                   std::vector<TString> &inputTitles, std::vector<Double_t> &cutsMin, 
                   std::vector<Double_t> &cutsMax, Double_t effS = 0.7);
   void setMjjMVA(double mjj);

private:
   TClonesArray* m_tProbStat;
   
   Int_t m_tSize;
   
   Int_t m_run;
   Int_t m_event;
   
   TMVA::Reader *reader;
   
   //std::vector<Float_t> tEventProbMVA;
   Float_t tEventProbMVA[100];
   Float_t MjjMVA;
   
   ClassDef(METree, 3)
};

#endif
