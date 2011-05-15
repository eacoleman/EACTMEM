#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/TopLepType.hh"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "TF1.h"


using std::cout;
using std::endl;
using std::make_pair;

MicroNtuple::indexMap2 MicroNtuple::indexMap;

MicroNtuple::MicroNtuple()
{
   nJets = 2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets];
   bProb1 = new Double_t[nJets];
   bProb2 = new Double_t[nJets];

   clear();
   fillIndexMap();
}

MicroNtuple::MicroNtuple(int ijets) :
   nJets(ijets)
{
   knntag = new Double_t[ijets];
   tag = new Int_t[ijets];
   bProb0 = new Double_t[ijets];
   bProb1 = new Double_t[ijets];
   bProb2 = new Double_t[ijets];

   clear();
   fillIndexMap();
}

MicroNtuple::MicroNtuple(const MicroNtuple& rhs)
{
  //   nJets = rhs.nJets;
  nJets = 2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets]; //[ntag][nJets]
   bProb1 = new Double_t[nJets]; //[ntag][nJets]
   bProb2 = new Double_t[nJets]; //[ntag][nJets]

   for (unsigned i = 0; i < nEventProb; ++i)
   {
     eventProb[i] = rhs.eventProb[i];
     //eventProb[i] = m_tProbStat.tEventProb[i];
   }

   for (int i = 0; i < nJets; ++i)
   {
      knntag[i] = rhs.knntag[i];
      tag[i] = rhs.tag[i];
      bProb0[i] = rhs.bProb0[i];
      bProb1[i] = rhs.bProb1[i];
      bProb2[i] = rhs.bProb2[i];
      
   }
   nTagExt            = rhs.nTagExt;
   nTagExtLooseSV     = rhs.nTagExtLooseSV;
   tagProbExt0        = rhs.tagProbExt0;
   tagProbExt1        = rhs.tagProbExt1;
   tagProbExt2        = rhs.tagProbExt2;
   tagProbExtLooseSV0 = rhs.tagProbExtLooseSV0;
   tagProbExtLooseSV1 = rhs.tagProbExtLooseSV1;
   tagProbExtLooseSV2 = rhs.tagProbExtLooseSV2;


   weight = rhs.weight;
   epd1tag = rhs.epd1tag;
   epd2tag = rhs.epd2tag;
   epd1tagSchan = rhs.epd1tagSchan;
   epd2tagSchan = rhs.epd2tagSchan;
   epd1tagTchan = rhs.epd1tagTchan;
   epd2tagTchan = rhs.epd2tagTchan;

   h = rhs.h;
   indexMap = rhs.indexMap;
}

MicroNtuple& MicroNtuple::operator=(const MicroNtuple& rhs)
{
   nJets = rhs.nJets;

   delete [] knntag;
   delete [] tag;
   delete [] bProb0;
   delete [] bProb1;
   delete [] bProb2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets];
   bProb1 = new Double_t[nJets];
   bProb2 = new Double_t[nJets];

   for (unsigned i = 0; i < nEventProb; ++i)
   {
     eventProb[i] = rhs.eventProb[i];
     //eventProb[i] = m_tProbStat.tEventProb[i];
   }

   for (int i = 0; i < nJets; ++i)
   {
      knntag[i] = rhs.knntag[i];
      tag[i] = rhs.tag[i];
      bProb0[i] = rhs.bProb0[i];
      bProb1[i] = rhs.bProb1[i];
      bProb2[i] = rhs.bProb2[i];
      
   }

   nTagExt            = rhs.nTagExt;
   nTagExtLooseSV     = rhs.nTagExtLooseSV;
   tagProbExt0        = rhs.tagProbExt0;
   tagProbExt1        = rhs.tagProbExt1;
   tagProbExt2        = rhs.tagProbExt2;
   tagProbExtLooseSV0 = rhs.tagProbExtLooseSV0;
   tagProbExtLooseSV1 = rhs.tagProbExtLooseSV1;
   tagProbExtLooseSV2 = rhs.tagProbExtLooseSV2;
   weight = rhs.weight;
   epd1tag = rhs.epd1tag;
   epd2tag = rhs.epd2tag;
   epd1tagSchan = rhs.epd1tagSchan;
   epd2tagSchan = rhs.epd2tagSchan;
   epd1tagTchan = rhs.epd1tagTchan;
   epd2tagTchan = rhs.epd2tagTchan;

   h = rhs.h;

   indexMap = rhs.indexMap;

   return *this;
}

MicroNtuple::~MicroNtuple()
{
   delete [] knntag;
   delete [] tag;
   delete [] bProb0;
   delete [] bProb1;
   delete [] bProb2;
}



void MicroNtuple::clear()
{
   for (unsigned i = 0; i < nEventProb; ++i)
   {
      eventProb[i] = 0;
   }

   delete [] knntag;
   delete [] tag;
   delete [] bProb0;
   delete [] bProb1;
   delete [] bProb2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets];
   bProb1 = new Double_t[nJets];
   bProb2 = new Double_t[nJets];

   
   bProb0[0] = 0.;
   bProb0[1] = 0.;
   bProb1[0] = 0.;
   bProb1[1] = 0.;
   bProb2[0] = 0.;
   bProb2[1] = 0.;

   nTagExt            = 0;
   nTagExtLooseSV     = 0;
   tagProbExt0        = 0;
   tagProbExt1        = 0;
   tagProbExt2        = 0;
   tagProbExtLooseSV0 = 0;
   tagProbExtLooseSV1 = 0;
   tagProbExtLooseSV2 = 0;
   weight = 0;
   epd1tag = 0;
   epd2tag = 0;
   epd1tagSchan = 0;
   epd2tagSchan = 0;
   epd1tagTchan = 0;
   epd2tagTchan = 0;

   h.clear();
}


void MicroNtuple::fillIndexMap(){
  
  // Map all the processes, except those with Higgs. 
  //NOTE that I'm not putting the top mass as I should... 
  indexMap[DEFS::EP::TopS  ].insert ( make_pair (0,6));
  indexMap[DEFS::EP::TopT  ].insert ( make_pair (0,5));
  indexMap[DEFS::EP::TTbar ].insert ( make_pair (0,4));
  indexMap[DEFS::EP::WLight].insert ( make_pair (0,2));
  indexMap[DEFS::EP::ZLight].insert ( make_pair (0,3));
  indexMap[DEFS::EP::WW    ].insert ( make_pair (0,0));
  indexMap[DEFS::EP::WZ    ].insert ( make_pair (0,1));
  indexMap[DEFS::EP::QCD   ].insert ( make_pair (0,7));	
  
  // Map the HWW ME's
  indexMap.insert (make_pair (DEFS::EP::HWW, indexMap1 ()));
  indexMap[DEFS::EP::WH].insert ( make_pair (115, 8));
  indexMap[DEFS::EP::WH].insert ( make_pair (120, 9));
  indexMap[DEFS::EP::WH].insert ( make_pair (125, 10));
  indexMap[DEFS::EP::WH].insert ( make_pair (130, 11));
  indexMap[DEFS::EP::WH].insert ( make_pair (135, 12));	
  
  // Map the WH ME's
  indexMap.insert (make_pair (DEFS::EP::WH, indexMap1 ()));
  indexMap[DEFS::EP::HWW].insert ( make_pair (120, 13));
  indexMap[DEFS::EP::HWW].insert ( make_pair (130, 14));
  indexMap[DEFS::EP::HWW].insert ( make_pair (140, 15));
  indexMap[DEFS::EP::HWW].insert ( make_pair (150, 16));
  indexMap[DEFS::EP::HWW].insert ( make_pair (160, 17));
  
  
  // get the index like this
  // int index = map[DEFS::EP::WH][120]; //should be 13
  // int index = m[DEFS::EP::WZ][0] // should be 1
  
}// fillMap

// This method functions as a map which maps from the eventProbs to the ProbsForEPD.
// If you don't care about the higgs mass set it to zero.
ProbsForEPD  MicroNtuple::getEventProbs(double mhiggs, const double evtProb[nEventProb]) {


  ProbsForEPD res;

  // The HWW which depends on the higgs mass
  res.hww   = evtProb[indexMap[DEFS::EP::HWW][mhiggs]];

  // The WH which depends on the higgs mass
  res.wh    = evtProb[indexMap[DEFS::EP::WH][mhiggs]];

  // The rest which don't depend on the higgs mass
  res.schan  = evtProb[indexMap[DEFS::EP::TopS][0]];
  res.tchan  = evtProb[indexMap[DEFS::EP::TopT][0]];
  res.tt     = evtProb[indexMap[DEFS::EP::TTbar][0]];
  res.wlight = evtProb[indexMap[DEFS::EP::WLight][0]]; 
  res.zlight = evtProb[indexMap[DEFS::EP::ZLight][0]];
  res.ww     = evtProb[indexMap[DEFS::EP::WW][0]];
  res.wz     = evtProb[indexMap[DEFS::EP::WZ][0]];
  res.qcd    = evtProb[indexMap[DEFS::EP::QCD][0]];

  return res;

}//getEventProbs


// This method just calls the overloaded getEventProbs(mhiggs, evtProbs[nEventProb])
// with the MicroNtuple's eventProb
ProbsForEPD MicroNtuple::getEventProbs(double mhiggs) const {
  return getEventProbs(mhiggs,eventProb);
}


Double_t MicroNtuple::getKNNTagProb(double knn)
{
   double tagProb = .5 * (1 + knn);
   double corrTag = 1 / (1 + (1 / tagProb - 1) * 1.5477);   
   return (corrTag >= 0 && corrTag <= 1) ? corrTag : 0;
}

//------------------------------------------------------------------------------
// This is the core of the code where all the parameters are mixed to obtain the EPD
ProbsForEPD MicroNtuple::getProbsForEPD(const ProbsForEPD & meProbs, 
					const ProbsForEPD & coeffs,
					const double bProb[], unsigned ntag){


  // The returning probabilities
  ProbsForEPD probs;

  // Start with whatever we have for the ME's 
  probs = meProbs;

  // Multiply each by their correspondent parameters
  probs *= coeffs;

  // Depending on the number of tags multiply by the bProbs
  if (ntag == 1){
    
    if (bProb[0] < 0){
      cout<<" ERROR MicroNtuple::getProbsForEPD has ntag==1 and bProb0<0. Exiting..."<<endl;
      exit(1);
    }
    
    probs.wh     *= bProb[0]; 
    probs.hww    *= bProb[0]; 
    probs.schan  *= bProb[0]; 
    probs.tchan  *= bProb[0]; 
    probs.tchan2 *= bProb[0]; 
    probs.tt     *= bProb[0];
    probs.wlight *= 1 - bProb[0]; 
    probs.zlight *= 1 - bProb[0];
    probs.wbb    *= bProb[0]; 
    probs.wc     *= 1 - bProb[0]; 
    probs.wgg    *= 1 - bProb[0]; 
    probs.ww     *= 1 - bProb[0]; 
    probs.wz     *= 1 - bProb[0]; 
    probs.zz     *= 1 - bProb[0]; 
    probs.qcd    *= 1 - bProb[0];

  }else if (ntag == 2){

    if (bProb[0] < 0 || bProb[1] < 0){
      cout<<" ERROR MicroNtuple::getProbsForEPD has ntag==2 and (bProb[0]<0 || bProb[1]<0) Exiting."<<endl;
      exit(1);
    }

    probs.wh     *= bProb[0] * bProb[1];
    probs.hww    *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.schan  *= bProb[0] * bProb[1];
    probs.tchan  *= bProb[0] * (1 - bProb[1]);
    probs.tchan2 *= bProb[0] * (1 - bProb[1]);
    probs.tt     *= bProb[0] * bProb[1];
    probs.wlight *= (1 - bProb[0]) * (1 - bProb[1]); 
    probs.zlight *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.wbb    *=  bProb[0] * bProb[1];
    probs.wc     *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.wgg    *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.ww     *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.wz     *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.zz     *= (1 - bProb[0]) * (1 - bProb[1]);
    probs.qcd    *= (1 - bProb[0]) * (1 - bProb[1]);
    
  }

  return probs;

}//getProbsForEPD



//------------------------------------------------------------------------------
//-----------------------------  THE WH METHODS      ---------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWHEPD(unsigned ntag, double mass) const
{

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(0);

  if(ntag==0)
    return calcWHEPD(ntag, mass, meProbs, bProb0, nJets);
  else if(ntag==1)
    return calcWHEPD(ntag, mass, meProbs, bProb1, nJets);

  return calcWHEPD(ntag, mass, meProbs, bProb2, nJets);
  
}//calcWHEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWHEPD(unsigned ntag, double mass,
				const ProbsForEPD & meProbs,
				const double bProb[], Int_t nJets){

   
   // Translate mass to WH index
   unsigned indexWH  = getWHMassIndex(mass);
  
   // Get the Higgs Coefficients
   ProbsForEPD coeffs = getWHEPDCoefficients(indexWH, ntag, nJets);  

   // Get the probabilities for the EPD
   ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, ntag);

   // There is less than three events per MC sample in which all the 
   // probabilities are zero.  To avoid returning a NaN just 
   // return zero for those
   if (probs.wh == 0) return 0;

   // Return the wh probability
   return probs.wh / (probs.wh + probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
		      probs.wc + probs.qcd + probs.tt +
		      probs.ww + probs.wz);      

}// calcWHEPD

//------------------------------------------------------------------------------
unsigned int MicroNtuple::getWHMassIndex(double mass){

  if (mass == 100)
      return 0;
   else if (mass == 105)
      return 1;
   else if (mass == 110)
      return 2;
   else if (mass == 115)
      return 3;
   else if (mass == 120)
      return 4;
   else if (mass == 125)
      return 5;
   else if (mass == 130)
      return 6;
   else if (mass == 135)
      return 7;
   else if (mass == 140)
      return 8;
   else if (mass == 145)
      return 9;
   else if (mass == 150)
      return 10;

  //fail with error
  throw std::runtime_error("MicroNtuple::getWHMassIndex wrong mass requested");
  return 0;

}//getWHMassIndex

//------------------------------------------------------------------------------
const ProbsForEPD MicroNtuple::getWHEPDCoefficients(int indexWH, unsigned ntag, int nJets){

  // The returning object
  ProbsForEPD coeffs;

  if (ntag == 0) {
  
    if (indexWH == 0) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 1) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 2) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 3) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 4) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    
  }else if (ntag == 1){

    if (indexWH == 0) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 1) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 2) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 3) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 4) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else if (ntag == 2){

    if (indexWH == 0) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 1) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 2) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 3) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 4) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else
    throw std::runtime_error("ERROR MicroNtuple::getWHEPDCoefficients. Invalid tag requested");

  return coeffs;

}//getWHEPDCoefficients



//------------------------------------------------------------------------------
//-----------------------------  THE H->WW METHODS      ------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHWWEPD(unsigned ntag, double mass) const
{

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(0);

  if(ntag==0)
    return calcWHEPD(ntag, mass, meProbs, bProb0, nJets);
  else if(ntag==1)
    return calcWHEPD(ntag, mass, meProbs, bProb1, nJets);

  return calcWHEPD(ntag, mass, meProbs, bProb2, nJets);
  
}//calcWHEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHWWEPD(unsigned ntag, double mass,
				const ProbsForEPD & meProbs,
				const double bProb[], Int_t nJets){

   
   // Translate mass to WH index
   unsigned indexWH  = getHWWMassIndex(mass);
  
   // Get the Higgs Coefficients
   ProbsForEPD coeffs = getHWWEPDCoefficients(indexWH, ntag, nJets);  

   // Get the probabilities for the EPD
   ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, ntag);

   // There is less than three events per MC sample in which all the 
   // probabilities are zero.  To avoid returning a NaN just 
   // return zero for those
   if (probs.wh == 0) return 0;

   // Return the wh probability
   return probs.wh / (probs.wh + probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
		      probs.wc + probs.qcd + probs.tt +
		      probs.ww + probs.wz);      

}// calcHWWEPD

//------------------------------------------------------------------------------
unsigned int MicroNtuple::getHWWMassIndex(double mass){

  if (mass == 100)
      return 0;
   else if (mass == 105)
      return 1;
   else if (mass == 110)
      return 2;
   else if (mass == 115)
      return 3;
   else if (mass == 120)
      return 4;
   else if (mass == 125)
      return 5;
   else if (mass == 130)
      return 6;
   else if (mass == 135)
      return 7;
   else if (mass == 140)
      return 8;
   else if (mass == 145)
      return 9;
   else if (mass == 150)
      return 10;

  //fail with error
  throw std::runtime_error("MicroNtuple::getHWWMassIndex wrong mass requested");
  return 0;

}//getHWWMassIndex

//------------------------------------------------------------------------------
const ProbsForEPD MicroNtuple::getHWWEPDCoefficients(int indexWH, unsigned ntag, int nJets){

  // The returning object
  ProbsForEPD coeffs;

  if (ntag == 0) {
  
    if (indexWH == 0) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 1) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 2) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 3) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 4) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    
  }else if (ntag == 1){

    if (indexWH == 0) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 1) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 2) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 3) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 4) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else if (ntag == 2){

    if (indexWH == 0) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 1) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 2) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 3) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (indexWH == 4) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else
    throw std::runtime_error("ERROR MicroNtuple::getWHEPDCoefficients. Invalid tag requested");

  return coeffs;

}//getHWWEPDCoefficients


//------------------------------------------------------------------------------
//-----------------------------  THE WZ METHODS      ---------------------------
//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWZEPD(unsigned ntag) const
{

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(0);

  if(ntag==0)
    return calcWZEPD(ntag, meProbs, bProb0, nJets);
  else if(ntag==1)
    return calcWZEPD(ntag, meProbs, bProb1, nJets);

  return calcWZEPD(ntag, meProbs, bProb2, nJets);
  
}

//------------------------------------------------------------------------------
// Without Higgs
Double_t MicroNtuple::calcWZEPD(unsigned ntag, const ProbsForEPD & meProbs,
				const double bProb[], Int_t nJets){

  
  // Get the WZ Coefficients.
  ProbsForEPD coeffs = getWZEPDCoefficients(ntag, nJets);  
  
  // Get the probabilities for the EPD
  ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, ntag);

  // Return the wh probability
  return (probs.wz+probs.ww) / (probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
				probs.wc + probs.qcd + probs.tt +
				probs.ww + probs.wz);    
        
}//calcWZEPD 

//------------------------------------------------------------------------------
const ProbsForEPD MicroNtuple::getWZEPDCoefficients(unsigned ntag, int nJets){

  // The returning object
  ProbsForEPD coeffs;

  if (ntag == 0) 
    return ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
  else if (ntag == 1)
    return ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
  else if (ntag != 2)
    throw std::runtime_error("ERROR MicroNtuple::getWHEPDCoefficients. Invalid tag requested");


  // the two tag case
  return ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

}//getWZEPDCoefficients



//------------------------------------------------------------------------------
//-----------------------------  THE SingleTop METHODS      --------------------
//------------------------------------------------------------------------------

Double_t MicroNtuple::calcSingleTopEPD(unsigned ntag, EPDType type) const {

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(0);
  
  if(ntag==0)
    return calcSingleTopEPD(ntag, type, meProbs, bProb0, nJets);
  else if(ntag==1)
    return calcSingleTopEPD(ntag, type, meProbs, bProb1, nJets);
 
  return calcSingleTopEPD(ntag, type, meProbs, bProb2, nJets);
  
} // calcSingleTopEPD



//------------------------------------------------------------------------------
const ProbsForEPD MicroNtuple::getSingleTopEPDCoefficients (unsigned ntag, EPDType type){
  
  static const ProbsForEPD params1tag[3] = {ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0)};
  
  static const ProbsForEPD params2tag[3] = {ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0)};

  if (ntag==1)
    return params1tag[type];

  return  params2tag[type];

}//getSingleTopEPDCoefficients

//------------------------------------------------------------------------------
//EPD for SingleTop.
Double_t MicroNtuple::calcSingleTopEPD(unsigned ntag, EPDType type,
				       const ProbsForEPD & meProbs,
				       const double bProb[], Int_t nJets){
 
  if (nJets == 2){

    // Get the Higgs Coefficients
    const ProbsForEPD coeffs = getSingleTopEPDCoefficients(ntag,type);  
    
    // Get the probabilities for the EPD. the indexWH is set to zero and it's irrelevant here 
    ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, ntag);
    
    // There is less than three events per MC sample in which all the probabilities are zero. 
    // To avoid returning a NaN just return zero for those
    if (probs.schan + probs.tchan + probs.tchan2 == 0) return 0;

    // Return the wh probability. Interestingly there is not diboson here
    return (probs.schan+probs.tchan+probs.tchan2)/ (probs.schan +  probs.tchan + probs.tchan2 + probs.wbb + 
						    probs.wc + probs.qcd + probs.tt );       
   
   
  }//njets==2
   
  return 0;

}//calcEPD


//------------------------------------------------------------------------------
//-----------------------------  OTHER METHODS      ---------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Double_t MicroNtuple::eventDump(const DumbClassToTrickRoot* h, double epd,
                                int ALPtype, double lepPt)
{
   std::cout << "Run " << h->run << " Event " << h->event << " " << h->ntag
             << " tags  Alpgentype " << ALPtype << " Detector " << h->det 
             << " LeptonPt: " << lepPt << " EPD: " << epd << std::endl;

   return h->run + h->event;
}


Double_t MicroNtuple::triggerTO(double detector, double etalep, double metraw,
                                double etlep)
{
   if (detector != TopLepType::TPHX)
      return 1;

   using std::abs;

   static TF1 turnOnMETL3("turnOnMETL3", "1/(1.+TMath::Exp(-[2]*(x-[1])))", 0, 
                         500);
  
   static TF1 turnOnPEML2("turnOnPEML2", "1/(1.+TMath::Exp(-[2]*(x-[1])))", 0, 
                         500);
    
   static TF1 turnOnPEML3("turnOnPEML3", "1/(1.+TMath::Exp(-[2]*(x-[1])))", 0, 
                         500);

   double weight = 0.;

   const int Ntrigger = 3; //L1_MET15_L3, L2_PEM20, L3_PEM20
  
   const int Nperiods = 2; //0d+0h and 0i..mj
   const double periodweight[Nperiods] = {.316, .684};

   double alpha[Nperiods][Ntrigger];
   double beta[Nperiods][Ntrigger];
  
   //L1_MET15_L3

   alpha[0][0] = 14.79;
   beta[0][0] = 0.37;

   alpha[1][0] = 14.00;
   beta[1][0] = 0.37;

   //L3_PEM20

   alpha[0][2] = 19.44;
   beta[0][2] = 1.99;

   alpha[1][2] = 20.91;
   beta[1][2] = 1.66;

   //L2_PEM20, eta dependend

   //0h
   if (abs(etalep) < 1.52)
   {
      alpha[0][1] = 20.15;
      beta[0][1] = 0.4772;
   }
   if (abs(etalep) >= 1.52 && abs(etalep) < 2.11)
   {
      alpha[0][1] = 20.85;
      beta[0][1] = 0.4237;
   }
   if (abs(etalep) >= 2.11 && abs(etalep) < 2.61)
   {
      alpha[0][1] = 22.96;
      beta[0][1] = 0.415;
   }
   if (abs(etalep) >= 2.61)
   {
      alpha[0][1] = 29.09;
      beta[0][1] = 0.2842;
   }

   //0i
   if (abs(etalep) < 1.52)
   {
      alpha[1][1] = 22.03;
      beta[1][1] = 0.5684;
   }
   if (abs(etalep) >= 1.52 && abs(etalep) < 2.11)
   {
      alpha[1][1] = 21.64;
      beta[1][1] = 0.4601;
   }
   if (abs(etalep) >= 2.11 && abs(etalep) < 2.61)
   {
      alpha[1][1] = 23.57;
      beta[1][1] = 0.4407;
   }
   if (abs(etalep) >= 2.61)
   {
      alpha[1][1] = 29.61;
      beta[1][1] = 0.5717;
   }

   //averaging over periods
   for(int i=0; i< Nperiods; i++)
   {
      turnOnMETL3.SetParameter(1,alpha[i][0]);
      turnOnMETL3.SetParameter(2,beta[i][0]);
                   
      turnOnPEML2.SetParameter(1,alpha[i][1]);
      turnOnPEML2.SetParameter(2,beta[i][1]);
                   
      turnOnPEML3.SetParameter(1,alpha[i][2]);
      turnOnPEML3.SetParameter(2,beta[i][2]);

      weight += periodweight[i] * turnOnMETL3.Eval(metraw)
         * turnOnPEML2.Eval(etlep) * turnOnPEML3.Eval(etlep);
   }

   return weight;
}


DumbClassToTrickRoot::DumbClassToTrickRoot()
{
   clear();
}

void DumbClassToTrickRoot::clear()
{
   tagProb1 = 0;
   tagProb2 = 0;
   det = 0;   
   ntag = 0;
   run = 0;
   event = 0;
}

ClassImp(MicroNtuple)
   ClassImp(DumbClassToTrickRoot)
