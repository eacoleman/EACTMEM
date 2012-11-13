#ifndef METREE_HH
#define METREE_HH

#include "TAMUWW/MatrixElement/interface/EventProbDefs.hh"

#include <TObject.h>
#include <TClonesArray.h>

#include <string>

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

private:
   TClonesArray* m_tProbStat;
   
   Int_t m_tSize;
   
   Int_t m_run;
   Int_t m_event;
   
   ClassDef(METree, 4)
};

#endif
