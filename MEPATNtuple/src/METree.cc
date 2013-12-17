#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctionsRoot.hh"

#include <iostream>
#include <stdexcept>
#include <TClonesArray.h>

using std::cout;
using std::endl;
using std::make_pair;

//------------------------------------------------------------------------------
METree::METree() :
  m_tProbStat(new TClonesArray("ProbStat")),
  m_tSize(0),
  m_run(0),
  m_event(0)
{}

//------------------------------------------------------------------------------
METree::METree(const METree& rhs) :
  m_tProbStat(new TClonesArray("ProbStat")),
  m_tSize(rhs.m_tSize),
  m_run(rhs.m_run),
  m_event(rhs.m_event)
{
  AuxFunctionsRoot::copyTCA<METree>(rhs.m_tProbStat, m_tProbStat);
}

//------------------------------------------------------------------------------
const METree& METree::operator=(const METree& rhs)
{
  m_tProbStat->Clear();
  AuxFunctionsRoot::copyTCA<ProbStat>(rhs.m_tProbStat, m_tProbStat);
  m_tSize = rhs.m_tSize;
  m_run = rhs.m_run;
  m_event = rhs.m_event;

  return *this;
}

//------------------------------------------------------------------------------
METree::~METree()
{
  delete m_tProbStat;
}

//------------------------------------------------------------------------------
void METree::clear()
{
  m_tProbStat->Clear();
  m_tSize = 0;
  m_run = 0;
  m_event = 0;
}

//------------------------------------------------------------------------------
void METree::addProbStat(const ProbStat& stat)
{
  new((*m_tProbStat)[m_tSize++]) ProbStat(stat);
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
const ProbStat* METree::getProbStat(unsigned index) const
{
  if (index >= static_cast<unsigned>(m_tSize))
    throw std::runtime_error("Invalid index for getProbStat");

  return dynamic_cast<ProbStat*>((*m_tProbStat)[index]);
}

//------------------------------------------------------------------------------
indexMap2 METree::fillIndexMap(){

   indexMap.clear();
   //cout << "fillIndexMap" << endl;
   for(int i=0; i<getNProbStat(); i++) {
      //indexMap[getProbStat(i)->tmeType].insert(make_pair(getProbStat(i)->tmeParam,i));
      //cout << "tmeType = " << getProbStat(i)->tmeType << " tmeParam = " <<  getProbStat(i)->tmeParam << " index = " << i << endl;
      indexMap[getProbStat(i)->tmeType].insert(std::pair<double,int>(getProbStat(i)->tmeParam,i));
   }

   return indexMap;
  
  // get the index like this
  // int index = indexMap[DEFS::EP::WH][120];
  // int index = indexMap[DEFS::EP::WZ][0];
  
}// fillMap

//------------------------------------------------------------------------------
ProbStat::ProbStat() :
  tmeType(DEFS::EP::UNKNOWN),
  tmeParam(0),
  tN(0),
  tNevl(0),
  tRelerr(0),
  tIfail(0),
  tRealTime(0),
  tCpuTime(0),
  tProb(0),
  tJet0Type(0),
  tJet1Type(0),
  tJet2Type(0),
  tJet3Type(0),
  tJet4Type(0),
  tJet5Type(0),
  tMaxProb(0),
  tEventProb(0),
  tEventErr(0),
  tEventMaxProb(0)
{}

//------------------------------------------------------------------------------
ProbStat::ProbStat(int number) :
  tmeType(DEFS::EP::UNKNOWN),
  tmeParam(0),
  tN(number),
  tNevl(new Double_t[number]),
  tRelerr(new Double_t[number]),
  tIfail(new Double_t[number]),
  tRealTime(0),
  tCpuTime(0),
  tProb(new Double_t[number]),
  tJet0Type(new Double_t[number]),
  tJet1Type(new Double_t[number]),
  tJet2Type(new Double_t[number]),
  tJet3Type(new Double_t[number]),
  tJet4Type(new Double_t[number]),
  tJet5Type(new Double_t[number]),
  tMaxProb(new Double_t[number]),
  tEventProb(0),
  tEventErr(0),
  tEventMaxProb(0)
{}

//------------------------------------------------------------------------------
ProbStat::ProbStat(const ProbStat& rhs) :
  tmeType(rhs.tmeType),
  tmeParam(rhs.tmeParam),
  tN(rhs.tN),
  tNevl(new Double_t[tN]),
  tRelerr(new Double_t[tN]),
  tIfail(new Double_t[tN]),
  tRealTime(rhs.tRealTime),
  tCpuTime(rhs.tCpuTime),
  tProb(new Double_t[tN]),
  tJet0Type(new Double_t[tN]),
  tJet1Type(new Double_t[tN]),
  tJet2Type(new Double_t[tN]),
  tJet3Type(new Double_t[tN]),
  tJet4Type(new Double_t[tN]),
  tJet5Type(new Double_t[tN]),
  tMaxProb(new Double_t[tN]),
  tEventProb(rhs.tEventProb),
  tEventErr(rhs.tEventErr),
  tEventMaxProb(rhs.tEventMaxProb){

  for (Int_t i = 0; i < tN; ++i)  {
    
    tNevl   [i] = rhs.tNevl[i];
    tRelerr [i] = rhs.tRelerr[i];
    tIfail  [i] = rhs.tIfail[i];
    tProb   [i] = rhs.tProb[i];
    tJet0Type[i] = rhs.tJet0Type[i];
    tJet1Type[i] = rhs.tJet1Type[i];
    tJet2Type[i] = rhs.tJet2Type[i];
    tJet3Type[i] = rhs.tJet3Type[i];
    tJet4Type[i] = rhs.tJet4Type[i];
    tJet5Type[i] = rhs.tJet5Type[i];

    tMaxProb[i] = rhs.tMaxProb[i];
  }//for
  
}//C'tor

//------------------------------------------------------------------------------
const ProbStat& ProbStat::operator=(const ProbStat& rhs){

  tmeType = rhs.tmeType;
  tmeParam = rhs.tmeParam;
  tN = rhs.tN;
   
  delete [] tNevl;
  delete [] tRelerr;
  delete [] tIfail;
  delete [] tProb;
  delete [] tJet0Type;
  delete [] tJet1Type;
  delete [] tJet2Type;
  delete [] tJet3Type;
  delete [] tJet4Type;
  delete [] tJet5Type;
  delete [] tMaxProb;

  tNevl    = new Double_t[tN];
  tRelerr  = new Double_t[tN];
  tIfail   = new Double_t[tN];
  tProb    = new Double_t[tN];
  tJet0Type = new Double_t[tN];
  tJet1Type = new Double_t[tN];
  tJet2Type = new Double_t[tN];
  tJet3Type = new Double_t[tN];
  tJet4Type = new Double_t[tN];
  tJet5Type = new Double_t[tN];

  tMaxProb = new Double_t[tN];

  for (Int_t i = 0; i < tN; ++i){

    tNevl   [i] = rhs.tNevl[i];
    tRelerr [i] = rhs.tRelerr[i];
    tIfail  [i] = rhs.tIfail[i];
    tProb   [i] = rhs.tProb[i];
    tJet0Type[i] = rhs.tJet0Type[i];
    tJet1Type[i] = rhs.tJet1Type[i];
    tJet2Type[i] = rhs.tJet2Type[i];
    tJet3Type[i] = rhs.tJet3Type[i];
    tJet4Type[i] = rhs.tJet4Type[i];
    tJet5Type[i] = rhs.tJet5Type[i];

    tMaxProb[i] = rhs.tMaxProb[i];

  }//for

  tRealTime     = rhs.tRealTime;
  tCpuTime      = rhs.tCpuTime;
  tEventProb    = rhs.tEventProb;
  tEventErr     = rhs.tEventErr;
  tEventMaxProb = rhs.tEventMaxProb;

  return *this;
}

/*
//------------------------------------------------------------------------------
void ProbStat::addtMEProfile(const TProfile3D& stat)
{
  new((*tME)[tME_tSize]) TProfile3D(stat);
  tME_tSize++;

}
void ProbStat::addtTFProfile(const TProfile3D& stat)
{
  new((*tTF)[tTF_tSize++]) TProfile3D(stat);
 tTF_tSize++;
}
void ProbStat::addtPDFProfile(const TProfile3D& stat)
{
  new((*tPDF)[tPDF_tSize++]) TProfile3D(stat);
  tPDF_tSize++;
}
void ProbStat::addtPSProfile(const TProfile3D& stat)
{
  new((*tPS)[tPS_tSize++]) TProfile3D(stat);
  tPS_tSize++;
}

//------------------------------------------------------------------------------
const TProfile3D* ProbStat::getMEProfile(unsigned index) const
{
  if (index >= static_cast<unsigned>(tME->GetSize()))
    throw std::runtime_error("Invalid index for getMEProfile");

  return dynamic_cast<TProfile3D*>((*tME)[index]);
}
const TProfile3D* ProbStat::getTFProfile(unsigned index) const
{
  if (index >= static_cast<unsigned>(tTF->GetSize()))
    throw std::runtime_error("Invalid index for getTFProfile");

  return dynamic_cast<TProfile3D*>((*tTF)[index]);
}
const TProfile3D* ProbStat::getPDFProfile(unsigned index) const
{
  if (index >= static_cast<unsigned>(tPDF->GetSize()))
    throw std::runtime_error("Invalid index for getPDFProfile");

  return dynamic_cast<TProfile3D*>((*tPDF)[index]);
}
const TProfile3D* ProbStat::getPSProfile(unsigned index) const
{
  if (index >= static_cast<unsigned>(tPS->GetSize()))
    throw std::runtime_error("Invalid index for getPSProfile");

  return dynamic_cast<TProfile3D*>((*tPS)[index]);
}
*/

//------------------------------------------------------------------------------
ProbStat::~ProbStat(){

  delete [] tNevl;
  delete [] tRelerr;
  delete [] tIfail;
  delete [] tProb;
  delete [] tJet0Type;
  delete [] tJet1Type;
  delete [] tJet2Type;
  delete [] tJet3Type;
  delete [] tJet4Type;
  delete [] tJet5Type;
  delete [] tMaxProb;
}

ClassImp(METree)
ClassImp(ProbStat)
