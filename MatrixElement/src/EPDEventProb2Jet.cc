// Author: Ricardo Eusebi, Texas A&M University.
// Created : 12/3/2009
//
// This class computes the integrated epd by taking the correlation 
// of the three integration variables correlated between all 
// the ME's. This must be an improvement over the previous 
// EPD in which the integration is carried out in each ME independently.

//  This package libraries
#include "TAMUWW/MatrixElement/interface/EPDEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
//#include "TAMUWW/MatrixElement/interface/sChannelEventProb2Jet.hh"
// #include "TAMUWW/MatrixElement/interface/SingleTopNtuple/MicroNtuple.hh"

//  C++ libraries
#include <iostream>
#include <cmath>
#include <vector>

// Use this flag to check the ME values by themselves 
// #define CHECKMES

// using std::vector;
// using std::cout;
// using std::endl;

using std::vector;
using std::cout;
using std::endl;

PartonColl EPDEventProb2Jet::m_measuredColl;
double EPDEventProb2Jet::evtMEProb[MicroNtuple::nEventProb];


// ------------------------------------------------------------------
EPDEventProb2Jet::EPDEventProb2Jet(Integrator & integrator, 
				   vector<EventProb*> evtProbs,
				   const TransferFunction & tf, 
				   double topMass, double higgsMass):
  EventProb2Jet(DEFS::EP::CorrEPD, integrator, 6, 1, tf),
  eventProbs(evtProbs)
{
  setTopMassAndWidth(topMass);
  setHiggsMassAndWidth(higgsMass);

  //Initialize the bProbs
  bProb[0] = 0.5;
  bProb[1] = 0.5;

  //Initialize all probs to zero
  for (unsigned e = 0; e < MicroNtuple::nEventProb; e++)
    evtMEProb[e] =0;

}


// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void EPDEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(mTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------
// This method sets the Higgs mass and the Higgs width
void  EPDEventProb2Jet::setHiggsMassAndWidth(double mHiggs) {

  // Set the mass
  m_massHiggs = mHiggs;

  // Use the theoretical Higgs width for the given mass 
  // multiplied by a factor of 100
  m_widthHiggs = 100.0 * calcHiggsWidth(mHiggs);

}//setHiggsMassAndWidth



// ------------------------------------------------------------------
// This changes the variables in such a way that they represent the 
// momentum (not the energy) of the partons. The energy will be set 
// in each matrix element differently. (e.g. if the ME assumes it is
// a b-quark then it would set it to sqrt(m_b^2+p^2) )
void EPDEventProb2Jet::changeVars(const vector<double>& parameters)
{
   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(parameters[1]);
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

// ------------------------------------------------------------------
// Needed because it's a pure virtual
double EPDEventProb2Jet::matrixElement() const { 
  
  std::cout<<"WARNING EPDEventProb2Jet::matrixElement() "
	   <<"This method is empty, why would you call it ? "<<std::endl; 

  return 0;

}//matrixElement

// ------------------------------------------------------------------
//This returns the epd, or if the flag CHECKMES is defined it 
// returns the ME for that particular ME
double EPDEventProb2Jet::computeEPD(const double parameters[]) const
{

  // The integration variables in parameters[] are defined
  // between zero and one. So first scale those parameters 
  // to their real bounds.
  vector<double> trueParams(parameters, parameters + getDimension());
  adjustBounds(trueParams);

#ifdef CHECKMES
  setMeasuredColl(m_measuredColl);
  double epd = eventProbs[0]->matrixElementCorrelated(trueParams);
  return epd;
#endif

  // For the moment assume we always have two jets. 
  // This should be set before the integral.
  static const int nJets = 2;

  // For the moment assume we always have a single tag jet. 
  // This should be set before the integral.
  static const int ntag = 2;

  // Here we should get the prob from the parton collection. For 
  // now they are set both to 0.5.

  // Calculate the ME for the Higgs process. Since the vector
  // evtMEProb must match what is used in MicroNtuple we need to use
  // the index 3 which corresponds to 115 GeV.
  setMeasuredColl(m_measuredColl);
  evtMEProb[3] = eventProbs[0]->matrixElementCorrelated(trueParams);

  // Calculate the ME for all the processes. Since the vector
  // evtMEProb must match what is used in MicroNtuple we need to add 10 
  // because we only have one WH me here, and not 11.
  for (unsigned e=1;e<eventProbs.size();e++){
    setMeasuredColl(m_measuredColl);
    evtMEProb[e+10] = eventProbs[e]->matrixElementCorrelated(trueParams);
  }

  // The coefficients used inside MicroNtuple assume that the tt ME
  // has an extra multiplyier factor of 2 * Pi^2 * 980, due to its 
  // extended volume (the factor is actually wrong, but that is a
  // different story...)
  evtMEProb[19] *=  TMath::Pi() * 1960;

  // return the epd as obtained from the MicroNtuple, for mass 115
  return MicroNtuple::calcHiggsEPD(ntag, 115 , evtMEProb, bProb, nJets);

}//computeEPD

// ------------------------------------------------------------------
void EPDEventProb2Jet::setPartonTypes() const { 
  // needed because this is a pure virtual method 
}

// ------------------------------------------------------------------
void EPDEventProb2Jet::getScale(double &, double &) const {
  //needed because this is a pure virtual method
}

// ------------------------------------------------------------------
void EPDEventProb2Jet::setupIntegral()
{ 
  // so we can distribute it to all ME's
  m_measuredColl = *getMeasuredColl();

  // Set the mass of the first probability (WH) to the Higgs 
  // mass of this class.
     
  //cast the object to HiggsEventProb*
  HiggsEventProb * higgs_eventProb = dynamic_cast<HiggsEventProb*> 
    (eventProbs[0]);

  //if the object cast to HiggsEventProb sucessfully then change its mass
  if (higgs_eventProb)
    higgs_eventProb->setHiggsMassAndWidth(m_massHiggs);

  // reset the counting for debugging of this class
  debug = 0;

  //reset the counting for debugging for the other probs
  for (unsigned e=0;e<eventProbs.size();e++)
    eventProbs[e]->debug = 0;

  // just report it to screen
  cout << "\tHiggs mass: " << m_massHiggs
       <<"  width: "       << m_widthHiggs << endl;

  cout << "\tTop mass: " << m_massTop
       <<" width: "       << m_widthTop <<endl;


}//setup Integral


// ------------------------------------------------------------------
// Make sure I have a single combination
bool EPDEventProb2Jet::onSwitch()
{
  if (getLoop() ==1 )
    return false;

  return true;

}

// ------------------------------------------------------------------
// This overloads the execute function of the EventProb, and it differs 
// from it  in that we simply integrate. We might remove this almost
// empty method later to avoid spurious method calls
double EPDEventProb2Jet::execute(const double parameters[])
{
  
  // if (debug==0) cout<<" entering execute for the first time"<<endl;

   // This can be removed simply if virtual changeVars were 
   // using the m_measuredColl instead of the m_partonColl 
   //m_partonColl = m_measuredColl;
   //setPartonColl(*getMeasuredColl());

   // Change the variables for ONLY the two jets energy and the neutrino pz
   // according to the parameters and their bounds. It does not change
   // extra variables that might be present, like an lost extra jet.
   // This assumes we integrate over the parton momentum and not energy. 
   //m_changeVars(parameters);

   // Set the neutrino components Px and Py for the given 
   // jet energies and the neutrino Pz. This uses only the 
   // lepton and jets Px and Py components, which is independent of 
   // the ME's
   //met();

   // The incoming parton energies will be set inside the 
   // method matrixElementCorrelated() of each Event Prob.
   // This is because it depends whether we assume the partons 
   // are b's or not, which in turns depends on the ME under consideration

   //This is the actual calculation of the epd. When running with the 
   // flag CHECKME this is one ME (the one selected in matrixElement() 
   // above) already multiplied by the Transfer functions and the pdf's
   double epd = computeEPD(parameters);

   if (TMath::IsNaN(epd)) {
      epd  = 0;
      cout<<" WARNING, EPDEventProb2Jet::execute(...) epd is nan"<<endl;
   }

   /*
   //debug 
   if (debug % 50000 == 0) {
     cout<<"\tdebug="<<debug<<" \tepd="<<epd<<" m_volume="<<getVolume()<<endl;
   }
   */
   // the volume of the integration in GeV^6
   double corrVolume = getCorrectedVolume();

   // the transfer function
   double tf = totalTF();

   double result = epd * corrVolume * tf;

   if (result >  m_maxProbNumber)
     m_maxProbNumber = result;

   // return the epd times the volume of the integration in GeV^6
   debug++;
   return result;

}//execute

//The volume reported by the base class is wrong, just report it correctly.
// The base class reports the volume to be 2 * Pi^2 * 980
// when it should be 4 * pi /3 * 980 ^3
double EPDEventProb2Jet::getCorrectedVolume() {

  // getVolume() * (4 * pi /3 * 980 ^3)/ (2 * pi ^2 * 980) 
  return getVolume() * 2 * 980 / (TMath::Pi() *3);

}//getCorrectedVolume
