#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "TF1.h"


using std::cout;
using std::endl;
using std::make_pair;
using std::map;
using std::vector;

MicroNtuple::indexMap2 MicroNtuple::indexMap;

MicroNtuple::MicroNtuple()
{
  MicroNtuple(2);
}

MicroNtuple::MicroNtuple(int ijets) : nJets(ijets)
{
   bProb = new Double_t[ijets];
   clear();
   fillIndexMap();
}

MicroNtuple::MicroNtuple(const MicroNtuple& rhs)
{
  nJets = rhs.nJets;

  bProb = new Double_t[nJets]; //[ntag][nJets]
  
  for (unsigned i = 0; i < nEventProb; ++i)
    eventProb[i] = rhs.eventProb[i];
    
  
  for (int i = 0; i < nJets; ++i)
    bProb[i] = rhs.bProb[i];

  weight = rhs.weight;
  epd1tag = rhs.epd1tag;
  epd2tag = rhs.epd2tag;
  epd1tagSchan = rhs.epd1tagSchan;
  epd2tagSchan = rhs.epd2tagSchan;
  epd1tagTchan = rhs.epd1tagTchan;
  epd2tagTchan = rhs.epd2tagTchan;
  
  indexMap = rhs.indexMap;
}

MicroNtuple& MicroNtuple::operator=(const MicroNtuple& rhs)
{
   nJets = rhs.nJets;

   delete [] bProb;
   bProb = new Double_t[nJets];

   for (unsigned i = 0; i < nEventProb; ++i)
     eventProb[i] = rhs.eventProb[i];

   for (int i = 0; i < nJets; ++i)
     bProb[i] = rhs.bProb[i];   

   weight = rhs.weight;
   epd1tag = rhs.epd1tag;
   epd2tag = rhs.epd2tag;
   epd1tagSchan = rhs.epd1tagSchan;
   epd2tagSchan = rhs.epd2tagSchan;
   epd1tagTchan = rhs.epd1tagTchan;
   epd2tagTchan = rhs.epd2tagTchan;
   
   indexMap = rhs.indexMap;

   return *this;
}

MicroNtuple::~MicroNtuple()
{
  // I don't understand ROOT. Technically I should be able to 
  // delete bProb right here, however the destructor of MicroNtuple 
  // seems to be called by ROOT when using TTreeFormula even though
  // the constructor was never called. Another ROOT mistery...
  //if (bProb)
  //delete [] bProb;
}



void MicroNtuple::clear()
{
  for (unsigned i = 0; i < nEventProb; ++i)
    eventProb[i] = 0;
  

  delete [] bProb;
  bProb = new Double_t[nJets];
  for (int i = 0; i < nJets; ++i)
    bProb[i] = 0.;   
  
  weight  = 0;
  epd1tag = 0;
  epd2tag = 0;
  epd1tagSchan = 0;
  epd2tagSchan = 0;
  epd1tagTchan = 0;
  epd2tagTchan = 0;
  
}


void MicroNtuple::fillIndexMap(){
  
  // Map all the processes, except those with Higgs. 
  //NOTE that I'm not putting the top mass as I should... 
  indexMap[DEFS::EP::STopS  ].insert ( make_pair (0,6));
  indexMap[DEFS::EP::STopT  ].insert ( make_pair (0,5));
  indexMap[DEFS::EP::TTbar ].insert ( make_pair (0,4));
  indexMap[DEFS::EP::WLight].insert ( make_pair (0,2));
  indexMap[DEFS::EP::ZLight].insert ( make_pair (0,3));
  indexMap[DEFS::EP::WW    ].insert ( make_pair (0,0));
  indexMap[DEFS::EP::WZ    ].insert ( make_pair (0,1));
  indexMap[DEFS::EP::QCD   ].insert ( make_pair (0,7));	
  
  // Map the HWW ME's
  indexMap.insert (make_pair (DEFS::EP::WH, indexMap1 ()));
  indexMap[DEFS::EP::WH].insert ( make_pair (115, 8));
  indexMap[DEFS::EP::WH].insert ( make_pair (120, 9));
  indexMap[DEFS::EP::WH].insert ( make_pair (125, 10));
  indexMap[DEFS::EP::WH].insert ( make_pair (130, 11));
  indexMap[DEFS::EP::WH].insert ( make_pair (135, 12));	
  
  // Map the WH ME's
  indexMap.insert (make_pair (DEFS::EP::HWW, indexMap1 ()));
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
  res.schan  = evtProb[indexMap[DEFS::EP::STopS][0]];
  res.tchan  = evtProb[indexMap[DEFS::EP::STopT][0]];
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



//------------------------------------------------------------------------------
// This is the core of the code where all the parameters are mixed to obtain the EPD
ProbsForEPD MicroNtuple::getProbsForEPD(const ProbsForEPD & meProbs, 
					const ProbsForEPD & coeffs,
					const double bProb[], DEFS::TagCat tagcat){


  // The returning probabilities
  ProbsForEPD probs;

  // Start with whatever we have for the ME's 
  probs = meProbs;

  // Multiply each by their correspondent parameters
  probs *= coeffs;

  // for the pretag category, in which we don't care about tags at all
  // just return as we have it 
  if (tagcat == DEFS::pretag) 
    return probs;

  // Otherwise for the tagged categories do this: 
  // If bProb[i] represents the probability that jet i is a b-jet 
  // Then for processes in which the number of actual b-quarks at 
  // tree level is:
  //   0 => multiply by (1 - bProb[0]) * (1 - bProb[1]);
  //   1 => multiply by bProb[0] * (1 - bProb[1]) + bProb[1] * (1 - bProb[0]);
  //   2 => multiply by bProb[0] * bProb[1];
  probs.wh     *=      bProb[0]  *  bProb[1];
  probs.hww    *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.schan  *=      bProb[0]  *  bProb[1];
  probs.tchan  *=  0.5*(bProb[0] * (1 - bProb[1]) + bProb[1] * (1 - bProb[0]));
  probs.tchan2 *=  0.5*(bProb[0] * (1 - bProb[1]) + bProb[1] * (1 - bProb[0]));
  probs.tt     *=       bProb[0] * bProb[1];
  probs.wlight *= (1 - bProb[0]) * (1 - bProb[1]); 
  probs.zlight *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.wbb    *=       bProb[0] * bProb[1];
  probs.wc     *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.wgg    *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.ww     *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.wz     *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.zz     *= (1 - bProb[0]) * (1 - bProb[1]);
  probs.qcd    *= (1 - bProb[0]) * (1 - bProb[1]);

  return probs;

}//getProbsForEPD



//------------------------------------------------------------------------------
//-----------------------------  THE WH METHODS      ---------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWHEPD(DEFS::TagCat tagcat, double mhiggs) const
{

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(mhiggs);
  return calcWHEPD(tagcat, mhiggs, meProbs, bProb, nJets);
  
}//calcWHEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWHEPD(DEFS::TagCat tagcat, double mass,
				const ProbsForEPD & meProbs,
				const double bProb[], Int_t nJets){

  // Get the Higgs Coefficients
  ProbsForEPD coeffs = getWHEPDCoefficients(mass, tagcat, nJets);  
  
  // Get the probabilities for the EPD
  ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, tagcat);
  
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
const ProbsForEPD MicroNtuple::getWHEPDCoefficients(double mhiggs, DEFS::TagCat tagcat, int nJets){

  // The returning object
  ProbsForEPD coeffs;

  if (tagcat == DEFS::pretag || tagcat == DEFS::eq0TSV){
  
    if      (mhiggs <= 115) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 120) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 125) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 130) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else                    coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    
  }else if (tagcat ==  DEFS::eq1TSV){

    if      (mhiggs <= 115) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 120) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 125) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 130) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else                    coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else if (tagcat == DEFS::eq2TSV){

    if      (mhiggs <= 115) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 120) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 125) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 130) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else                    coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else
    throw std::runtime_error("ERROR MicroNtuple::getWHEPDCoefficients. Invalid tag requested");

  return coeffs;

}//getWHEPDCoefficients



//------------------------------------------------------------------------------
//-----------------------------  THE H->WW METHODS      ------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHWWEPD(DEFS::TagCat tagcat, double mhiggs) const
{

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(mhiggs);
  return calcWHEPD(tagcat, mhiggs, meProbs, bProb, nJets);
  
}//calcWHEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHWWEPD(DEFS::TagCat tagcat, double mass,
				const ProbsForEPD & meProbs,
				const double bProb[], Int_t nJets){

   
   // Translate mass to WH index
   //unsigned indexWH  = getHWWMassIndex(mass);
  
  cout<<" \t\t SFSG 1.1"<<endl;
  
   // Get the Higgs Coefficients
   ProbsForEPD coeffs = getHWWEPDCoefficients(mass, tagcat, nJets);  

   cout<<" \t\t SFSG 1.2"<<endl;

   // Get the probabilities for the EPD
   ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, tagcat);

  cout<<" \t\t SFSG 1.3"<<endl;

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
const ProbsForEPD MicroNtuple::getHWWEPDCoefficients(double mhiggs, DEFS::TagCat tagcat, int nJets){

  // The returning object
  ProbsForEPD coeffs;

  if (tagcat == DEFS::pretag || tagcat == DEFS::eq0TSV){
  
    if      (mhiggs <= 120) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 130) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 140) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 150) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else                    coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
 
  }else if (tagcat ==  DEFS::eq1TSV){   

    if      (mhiggs <= 120) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 130) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 140) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 150) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else                    coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else if (tagcat == DEFS::eq2TSV){

    if      (mhiggs <= 120) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 130) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 140) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else if (mhiggs <= 150) coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
    else                    coeffs = ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

  } else
    throw std::runtime_error("ERROR MicroNtuple::getWHEPDCoefficients. Invalid tag requested");
    
  return coeffs;

}//getHWWEPDCoefficients


//------------------------------------------------------------------------------
//-----------------------------  THE WZ METHODS      ---------------------------
//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWZEPD(DEFS::TagCat tagcat) const
{

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(0);
  return calcWZEPD(tagcat, meProbs, bProb, nJets);
  
}

//------------------------------------------------------------------------------
// Without Higgs
Double_t MicroNtuple::calcWZEPD(DEFS::TagCat tagcat, const ProbsForEPD & meProbs,
				const double bProb[], Int_t nJets){

  
  // Get the WZ Coefficients.
  ProbsForEPD coeffs = getWZEPDCoefficients(tagcat, nJets);  
  
  // Get the probabilities for the EPD
  ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, tagcat);

  // Return the wh probability
  return (probs.wz+probs.ww) / (probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
				probs.wc + probs.qcd + probs.tt +
				probs.ww + probs.wz);    
        
}//calcWZEPD 

//------------------------------------------------------------------------------
const ProbsForEPD MicroNtuple::getWZEPDCoefficients(DEFS::TagCat tagcat, int nJets){

  // The returning object
  ProbsForEPD coeffs;

  if(tagcat == DEFS::pretag || tagcat == DEFS::eq0TSV)
    return ProbsForEPD(2.85714e+09,2e+10,1.20062e+08,1.36023e+07,0,110.948,4000,357143,0,0,0,2.80857e+06,182765,0,3.99459); // DEFS::TagCat=0 figOfMerit=0.136722
  else if (tagcat == DEFS::eq1TSV)
    return ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);
  else if (tagcat != DEFS::eq2TSV)
    throw std::runtime_error("ERROR MicroNtuple::getWZEPDCoefficients. Invalid tag requested");


  // the two tag case
  return ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0);

}//getWZEPDCoefficients



//------------------------------------------------------------------------------
//-----------------------------  THE SingleTop METHODS      --------------------
//------------------------------------------------------------------------------

Double_t MicroNtuple::calcSingleTopEPD(DEFS::TagCat tagcat, EPDType type) const {

  // Get the meProbs
  ProbsForEPD meProbs = getEventProbs(0);
  return calcSingleTopEPD(tagcat, type, meProbs, bProb, nJets);
  
} // calcSingleTopEPD



//------------------------------------------------------------------------------
const ProbsForEPD MicroNtuple::getSingleTopEPDCoefficients (DEFS::TagCat tagcat, EPDType type){
  
  static const ProbsForEPD params1tag[3] = {ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0)};
  
  static const ProbsForEPD params2tag[3] = {ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0),
					    ProbsForEPD(2.5, 0, 0, .5, .01, .165, 0, 0, 0, 0,0,0,0,0,0)};

  if (tagcat == DEFS::eq1TSV)
    return params1tag[type];
  else if (tagcat != DEFS::eq2TSV)
    throw std::runtime_error("ERROR MicroNtuple::getSingleTopEPDCoefficients. Invalid tag requested");

  return  params2tag[type];

}//getSingleTopEPDCoefficients

//------------------------------------------------------------------------------
//EPD for SingleTop.
Double_t MicroNtuple::calcSingleTopEPD(DEFS::TagCat tagcat, EPDType type,
				       const ProbsForEPD & meProbs,
				       const double bProb[], Int_t nJets){
 
  if (nJets == 2){

    // Get the Higgs Coefficients
    const ProbsForEPD coeffs = getSingleTopEPDCoefficients(tagcat,type);  
    
    // Get the probabilities for the EPD. the indexWH is set to zero and it's irrelevant here 
    ProbsForEPD probs = getProbsForEPD(meProbs, coeffs, bProb, tagcat);
    
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
Double_t MicroNtuple::triggerTO(double detector, double etalep, double metraw,
                                double etlep)
{
  return 1;
}

//------------------------------------------------------------------------------
void MicroNtuple::setMVAReader(vector<TString> MVAMethods, TString dir, Int_t m_run, Int_t m_event) {
   // This loads the library
   TMVA::Tools::Instance();

   // --- Create the Reader object
   reader = new TMVA::Reader( "!Color:!Silent" );

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
   /*************************************************************/
   int m_tSize = 28; //////////FIX THIS!!! FIX THIS!!! FIX THIS!!!
   /*************************************************************/
   vector<Float_t> localVar(m_tSize,0);
   char name[1024];
   for (int i=0; i<m_tSize; i++) {
      sprintf(name,"%d",i);
      TString var = TString("tEventProb") + name + " := eventProb[" + name + "]";
      localVar[i] = eventProb[i];
      reader->AddVariable(var, &localVar[i]);
   }

   // Spectator variables declared in the training have to be added to the reader, too
   reader->AddSpectator("run := m_run", &m_run);
   reader->AddSpectator("event := m_event", &m_event);

   // --- Book the MVA methods
   //TString dir    = "weights/";
   TString prefix = "TMVAClassification";

   // Book method(s)
   for (unsigned int i=0; i<MVAMethods.size(); i++) {
      TString methodName = MVAMethods[i] + TString(" method");   
      TString weightfile = dir + prefix + TString("_") + MVAMethods[i] + TString(".weights.xml");
      reader->BookMVA( methodName, weightfile );
   }
}

//------------------------------------------------------------------------------
vector<map<TString,Double_t> > MicroNtuple::getMVAOutput(vector<TString> MVAMethods, Double_t effS) {
   // This loads the library
   TMVA::Tools::Instance();

   // Retrieve MVA output, error, probability, and rarity.
   vector<map<TString,Double_t> > outputs;

   for (unsigned int i=0; i<MVAMethods.size(); i++) {
      map<TString,Double_t> output;
      TString methodName = MVAMethods[i] + TString(" method");  
      
      if (MVAMethods[i].CompareTo("CutsGA")==0)
         output["response"] = reader->EvaluateMVA( methodName, effS );
      else
         output["response"] = reader->EvaluateMVA( methodName );
      output["error"] = reader->GetMVAError();
      output["probability"] = reader->GetProba ( methodName );
      output["rarity"] = reader->GetRarity( methodName );

      outputs.push_back(output);
   }

   return outputs;
}

//------------------------------------------------------------------------------
Double_t MicroNtuple::getCutsGAEfficiency(Int_t nSelCutsGA, Int_t nTreeEntries){
   return Double_t(nSelCutsGA)/nTreeEntries;
}

//------------------------------------------------------------------------------
void MicroNtuple::getMVACuts(std::vector<TString> &inputVars, std::vector<TString> &inputLabels, 
                             std::vector<TString> &inputTitles, std::vector<Double_t> &cutsMin, 
                             std::vector<Double_t> &cutsMax, Double_t effS) {
   TMVA::MethodCuts* mcuts = reader->FindCutsMVA( "CutsGA method" ) ;
   
   if (mcuts) {      
      mcuts->GetCuts( effS, cutsMin, cutsMax );
      for (UInt_t ivar=0; ivar<cutsMin.size(); ivar++) {
         inputVars.push_back(mcuts->GetInputVar(ivar));
         inputLabels.push_back(mcuts->GetInputLabel(ivar));
         inputTitles.push_back(mcuts->GetInputTitle(ivar));
      }
   }
}
   
ClassImp(MicroNtuple)
