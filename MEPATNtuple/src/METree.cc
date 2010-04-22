#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctionsRoot.hh"

#include <iostream>
#include <stdexcept>
#include <TClonesArray.h>


METree::METree() :
   m_tProbStat(new TClonesArray("ProbStat")),
   m_tSize(0),
   m_run(0),
   m_event(0)
{}

METree::METree(const METree& rhs) :
   m_tProbStat(new TClonesArray("ProbStat")),
   m_tSize(rhs.m_tSize),
   m_run(rhs.m_run),
   m_event(rhs.m_event)
{
   PeterFunctions::copyTCA<METree>(rhs.m_tProbStat, m_tProbStat);
}

const METree& METree::operator=(const METree& rhs)
{
   m_tProbStat->Clear();
   PeterFunctions::copyTCA<ProbStat>(rhs.m_tProbStat, m_tProbStat);
   m_tSize = rhs.m_tSize;
   m_run = rhs.m_run;
   m_event = rhs.m_event;

   return *this;
}

METree::~METree()
{
   delete m_tProbStat;
}

void METree::clear()
{
   m_tProbStat->Clear();
   m_tSize = 0;
   m_run = 0;
   m_event = 0;
}

void METree::addProbStat(const ProbStat& stat)
{
   new((*m_tProbStat)[m_tSize++]) ProbStat(stat);
}

Float_t METree::getEPD(double btag) const
{
//   static unsigned count = 0;
//   std::cerr << "count " << count++ << std::endl;
//   return 1;
//   std::cerr << m_tProbStat->GetEntries() << std::endl;

   float isbtag = btag ? btag : 1;
   float isnotbtag = btag ? (1 - btag) : 1;

   float schan = isbtag * dynamic_cast<ProbStat*>(m_tProbStat->At(0))->tEventProb * 1.3;
   float tchan = isbtag * dynamic_cast<ProbStat*>(m_tProbStat->At(1))->tEventProb * 2;
   float wbb = isbtag * dynamic_cast<ProbStat*>(m_tProbStat->At(2))->tEventProb  * 1.1;
   float wc = isnotbtag * dynamic_cast<ProbStat*>(m_tProbStat->At(3))->tEventProb * .45;

   return (schan + tchan) / (schan + tchan + wbb + wc);
}

const ProbStat* METree::getProbStat(unsigned index) const
{
   if (index >= static_cast<unsigned>(m_tSize))
      throw std::runtime_error("Invalid index for getProbStat");

   return dynamic_cast<ProbStat*>((*m_tProbStat)[index]);
}

ProbStat::ProbStat() :
   tN(0),
   tNevl(0),
   tRelerr(0),
   tIfail(0),
   tRealTime(0),
   tCpuTime(0),
   tProb(0),
   tMaxProb(0),
   tEventProb(0),
   tEventErr(0),
   tEventMaxProb(0)
{}

ProbStat::ProbStat(int number) :
   tN(number),
   tNevl(new Double_t[number]),
   tRelerr(new Double_t[number]),
   tIfail(new Double_t[number]),
   tRealTime(0),
   tCpuTime(0),
   tProb(new Double_t[number]),
   tMaxProb(new Double_t[number]),
   tEventProb(0),
   tEventErr(0),
   tEventMaxProb(0)
{}

ProbStat::ProbStat(const ProbStat& rhs) :
   tN(rhs.tN),
   tNevl(new Double_t[tN]),
   tRelerr(new Double_t[tN]),
   tIfail(new Double_t[tN]),
   tRealTime(rhs.tRealTime),
   tCpuTime(rhs.tCpuTime),
   tProb(new Double_t[tN]),
   tMaxProb(new Double_t[tN]),
   tEventProb(rhs.tEventProb),
   tEventErr(rhs.tEventErr),
   tEventMaxProb(rhs.tEventMaxProb){

  for (Int_t i = 0; i < tN; ++i)  {
    
    tNevl   [i] = rhs.tNevl[i];
    tRelerr [i] = rhs.tRelerr[i];
    tIfail  [i] = rhs.tIfail[i];
    tProb   [i] = rhs.tProb[i];
    tMaxProb[i] = rhs.tMaxProb[i];
  }//for
  
}//C'tor

const ProbStat& ProbStat::operator=(const ProbStat& rhs){

   tN = rhs.tN;
   
   delete [] tNevl;
   delete [] tRelerr;
   delete [] tIfail;
   delete [] tProb;
   delete [] tMaxProb;

   tNevl    = new Double_t[tN];
   tRelerr  = new Double_t[tN];
   tIfail   = new Double_t[tN];
   tProb    = new Double_t[tN];
   tMaxProb = new Double_t[tN];

   for (Int_t i = 0; i < tN; ++i){

      tNevl   [i] = rhs.tNevl[i];
      tRelerr [i] = rhs.tRelerr[i];
      tIfail  [i] = rhs.tIfail[i];
      tProb   [i] = rhs.tProb[i];
      tMaxProb[i] = rhs.tMaxProb[i];

   }//for

   tRealTime     = rhs.tRealTime;
   tCpuTime      = rhs.tCpuTime;
   tEventProb    = rhs.tEventProb;
   tEventErr     = rhs.tEventErr;
   tEventMaxProb = rhs.tEventMaxProb;

   return *this;
}

ProbStat::~ProbStat(){

   delete [] tNevl;
   delete [] tRelerr;
   delete [] tIfail;
   delete [] tProb;
   delete [] tMaxProb;
}

ClassImp(METree)
ClassImp(ProbStat)
