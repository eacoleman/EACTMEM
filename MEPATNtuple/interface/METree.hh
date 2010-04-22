#ifndef METREE_HH
#define METREE_HH

#include <TObject.h>
#include <TClonesArray.h>

struct ProbStat : public TObject{

  ProbStat();
  ProbStat(int number);
  ProbStat(const ProbStat&);
  const ProbStat& operator=(const ProbStat&);
  ~ProbStat();

  //The number of permutation for a given ME probability
  Int_t tN;
  
  Double_t* tNevl;    //[tN]
  Double_t* tRelerr;  //[tN]
  Double_t* tIfail;   //[tN]
  Double_t  tRealTime;
  Double_t  tCpuTime;
  Double_t* tProb;    //[tN]
  Double_t* tMaxProb; //[tN]
  Double_t  tEventProb;
  Double_t  tEventErr;
  Double_t  tEventMaxProb; 
  
  ClassDef(ProbStat, 2)

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

      ClassDef(METree, 2)
};

#endif
