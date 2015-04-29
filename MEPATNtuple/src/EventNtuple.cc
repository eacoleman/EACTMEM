/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/TopLepType.hh"
#include "SHyFT/TemplateMakers/bin/AngularVars.h"
#include "SHyFT/TemplateMakers/interface/KinFit.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "TF1.h"
#include "TLorentzVector.h"
#include "TMath.h"

using std::cout;
using std::endl;
using std::max;

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
GenParticle::GenParticle() {
   charge = 0;
   pdgId = 0;
   status = 0;
   particlePosition = -1;
   numberOfMothers = -1;
   numberOfDaughters = -1;
}


//______________________________________________________________________________
GenParticle::~GenParticle() {}


//______________________________________________________________________________
EventNtuple::EventNtuple() {}


//______________________________________________________________________________
EventNtuple::~EventNtuple() {}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool EventNtuple::baseCuts()
{
   return (METEtMin());
}


//______________________________________________________________________________
bool EventNtuple::METEtMin(double EtMin)
{
   return METLV[0].Et()>EtMin;
}


//______________________________________________________________________________
/*
//Minimal Example:
EventNtuple* ntuple = new EventNtuple();
jets2p->SetBranchAddress("EvtNtuple", &ntuple);
jets2p->GetEntry(9)
ntuple->getQGLikelihood(0,new QGLikelihoodCalculator())
 */
double EventNtuple::getQGLikelihood(unsigned int index, QGLikelihoodCalculator* qglikeli)
{
   if (qglikeli==0) {
      cout << "\tERROR::EventNtuple::getQGLikelihood The QGLikelihoodCalculator is NULL" << endl
           << "\tPlease fix this. The program will now terminate." << endl;
      assert(qglikeli);
   }
   if(index >= jLV.size()) {
      cout << "\tERROR::EventNtuple::getQGLikelihood The requested jet index is out of range" << endl
           << "\tReturning -1.0 as the QGLikelihood" << endl; 
      return -1.0;
   }

   double qglikelihood = 0.0;
   qglikelihood = qglikeli->computeQGLikelihoodPU(jLV[index].Pt(), rho, jLV[index].jChargedMultiplicity,
                                                  jLV[index].jNeutralMultiplicity, jLV[index].jPtD);
 
   return qglikelihood;
}


//______________________________________________________________________________
/*
//Minimal Example:
EventNtuple* ntuple = new EventNtuple();
jets2p->SetBranchAddress("EvtNtuple", &ntuple);
jets2p->GetEntry(9)
ntuple->getQGLikelihoods(new QGLikelihoodCalculator())
 */
vector<double> EventNtuple::getQGLikelihoods(QGLikelihoodCalculator* qglikeli)
{
   if (qglikeli==0) {
      cout << "\tERROR::EventNtuple::getQGLikelihoods The QGLikelihoodCalculator is NULL" << endl
           << "\tPlease fix this. The program will now terminate." << endl;
      assert(qglikeli);
   }
   if (jLV.size()==0)
      cout << "\tWARNING::EventNtuple::getQGLikelihoods jLV, jChargedMultiplicity, jNeutralMultiplicity, or jPtD has size 0." << endl << "\tTherefore, no QGLikelihoods will be returned." << endl;
   
   vector<double> qglikelihoods;
   for (unsigned int i=0; i<jLV.size(); i++) {
      qglikelihoods.push_back(getQGLikelihood(i,qglikeli));
   }

   return qglikelihoods;
}


//______________________________________________________________________________
double EventNtuple::getJERfactor(double pt, double eta, double ptgen){

  double jer = 1;

  if (fabs(eta) < 0.5)
    jer = 1.052;
  else if (fabs(eta) < 1.1)
    jer = 1.057;
  else if (fabs(eta) < 1.7)
    jer = 1.096;
  else if (fabs(eta) < 2.3)
    jer = 1.134;
  else if (fabs(eta) < 5)
    jer = 1.288;

  double corr = ptgen / pt;

  return  max(0.0,corr + jer * (1 - corr));

}


//______________________________________________________________________________
// This corrects the resolution of all jets and updates the MET.
// The computation of new met follows. Using subscripts _o for old and _n for new
// we get from the def of met that met_o + j_0 =0 and met_n + j_n=0
// => met_n + j_n - j_o + j_o = 0
//    met_n + j_n - j_o - met_o = 0 => met_n  = met_o + j_o - j_n
// and using j_n = c * j_o we get
//    met_n = met_0 + j_o ( 1 - c) 
void EventNtuple::doJER(){

  // Loop over jets
  for (unsigned int j=0; j < jLV.size() ; j++){

    // get the correction factor for this jet
    double cor = getJERfactor(jLV[j].Pt(), jLV[j].Eta(), jLV[j].refLV.Pt());
    
    // recompute the met for this change
    double newMetX = (1 - cor)*jLV[j].X() + METLV[0].X();
    double newMetY = (1 - cor)*jLV[j].Y() + METLV[0].Y();
    METLV[0].SetX(newMetX);
    METLV[0].SetY(newMetY);

    // correct the jet
    jLV[j] = jLV[j]*cor;

  }// for jets

  // sort the vector of jets here
  std::sort(jLV.begin(), jLV.end(), Jet::sortInDecreasingPt);

}// doJER


//______________________________________________________________________________
//Types:
//       pfMEtSysShiftCorrParameters_2011runAvsSumEt_data
//       pfMEtSysShiftCorrParameters_2011runAvsSumEt_mc
//       pfMEtSysShiftCorrParameters_2011runBvsSumEt_data
//       pfMEtSysShiftCorrParameters_2011runBvsSumEt_mc
//       pfMEtSysShiftCorrParameters_2011runAplusBvsSumEt_data
//       pfMEtSysShiftCorrParameters_2011runAplusBvsSumEt_mc
//       pfMEtSysShiftCorrParameters_2012runAvsSumEt_data
//       pfMEtSysShiftCorrParameters_2012runAvsSumEt_mc
//       pfMEtSysShiftCorrParameters_2011runAvsNvtx_data
//       pfMEtSysShiftCorrParameters_2011runAvsNvtx_mc
//       pfMEtSysShiftCorrParameters_2011runBvsNvtx_data
//       pfMEtSysShiftCorrParameters_2011runBvsNvtx_mc
//       pfMEtSysShiftCorrParameters_2011runAplusBvsNvtx_data
//       pfMEtSysShiftCorrParameters_2011runAplusBvsNvtx_mc
//       pfMEtSysShiftCorrParameters_2012runAvsNvtx_data
//       pfMEtSysShiftCorrParameters_2012runAvsNvtx_mc
pair<double,double> EventNtuple::getMETPhiCorrection(TString eraType){
   
   pair<double,double> METCor = std::make_pair(0,0);
/*
   // parametrization of MET x/y shift vs. sumEt
   if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAvsSumEt_data")==0) {
      METCor.first  = -3.365e-1 + (4.801e-3*sumEt);
      METCor.second = +2.578e-1 - (6.124e-3*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAvsSumEt_mc")==0) {
      METCor.first  = -9.389e-2 + (1.815e-4*sumEt);
      METCor.second = +1.571e-1 - (3.710e-3*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runBvsSumEt_data")==0) {
      METCor.first  = -3.265e-1 + (5.162e-3*sumEt);
      METCor.second = -1.956e-2 - (6.299e-3*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runBvsSumEt_mc")==0) {
      METCor.first  = -1.070e-1 + (9.587e-5*sumEt);
      METCor.second = -1.517e-2 - (3.357e-3*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAplusBvsSumEt_data")==0) {
      METCor.first  = -5.65217e-01 + (5.42436e-03*sumEt);
      METCor.second = +4.54054e-01 - (6.73607e-03*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAplusBvsSumEt_mc")==0) {
      METCor.first  = -4.53909e-02 - (2.55863e-05*sumEt);
      METCor.second = +1.27947e-01 - (3.62604e-03*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2012runAvsSumEt_data")==0) {
      METCor.first  = -7.67892e-01 + (5.76983e-03*sumEt);
      METCor.second = +5.54005e-01 - (2.94046e-03*sumEt);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2012runAvsSumEt_mc")==0) {
      METCor.first  = +1.77344e-01 - (1.34333e-03*sumEt);
      METCor.second = +8.08402e-01 - (2.84264e-03*sumEt);
   }
*/
   // parametrization of MET x/y shift vs. Nvtx
   if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAvsNvtx_data")==0) {
      METCor.first  = +3.87339e-1 + (2.58294e-1*vLV[0].npv);
      METCor.second = -7.83502e-1 - (2.88899e-1*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAvsNvtx_mc")==0) {
      METCor.first  = -1.94451e-2 - (4.38986e-3*vLV[0].npv);
      METCor.second = -4.31368e-1 - (1.90753e-1*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runBvsNvtx_data")==0) {
      METCor.first  = +6.64470e-1 + (2.71292e-1*vLV[0].npv);
      METCor.second = -1.239990e0 - (3.18661e-1*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runBvsNvtx_mc")==0) {
      METCor.first  = -9.89706e-2 + (6.64796e-3*vLV[0].npv);
      METCor.second = -5.32495e-1 - (1.82195e-1*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAplusBvsNvtx_data")==0) {
      METCor.first  = +3.64118e-01 + (2.93853e-01*vLV[0].npv);
      METCor.second = -7.17757e-01 - (3.57309e-01*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2011runAplusBvsNvtx_mc")==0) {
      METCor.first  = -4.79178e-02 + (8.62653e-04*vLV[0].npv);
      METCor.second = -4.54408e-01 - (1.89684e-01*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2012runAvsNvtx_data")==0) {
      METCor.first  = +3.54233e-01 + (2.65299e-01*vLV[0].npv);
      METCor.second = +1.88923e-01 - (1.66425e-01*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2012runAvsNvtx_mc")==0) {
      METCor.first  = -2.99576e-02 - (6.61932e-02*vLV[0].npv);
      METCor.second = +3.70819e-01 - (1.48617e-01*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2012runAvsNvtx_TAMUWW_data")==0) {
      METCor.first  = +1.62143 + (0.489774*vLV[0].npv);
      METCor.second = -1.41580 - (0.314019*vLV[0].npv);
   }
   else if (eraType.CompareTo("pfMEtSysShiftCorrParameters_2012runAvsNvtx_TAMUWW_mc")==0) {
      METCor.first  = +0.496732 - (0.0156985*vLV[0].npv);
      METCor.second = +0.379505 - (0.2656620*vLV[0].npv);
   }
   
   METCor.first*=-1;
   METCor.second*=-1;

   return  METCor;

}// getMETPhiCorrection


//______________________________________________________________________________
void EventNtuple::doMETPhiCorrection(TString eraType){
   
   //Loop over the MET lorentz vectors
   for (unsigned int m=0; m < METLV.size(); m++){
      
      // get the correction factor for this MET
      pair<double,double> cor = getMETPhiCorrection(eraType);
      //cout << "\tPx before correction = " << METLV[0].Px() << endl;
      //cout << "\tPy before correction = " << METLV[0].Py() << endl;
      //cout << "\tPx correction = " << cor.first << endl;
      //cout << "\tPy correction = " << cor.second << endl;
      METLV[0].SetX(METLV[0].Px()+cor.first);
      METLV[0].SetY(METLV[0].Py()+cor.second);
      //cout << "\tPx after correction = " << METLV[0].Px() << endl;
      //cout << "\tPy after correction = " << METLV[0].Py() << endl;
   }// for METs

}// doMETPhiCorrection


//______________________________________________________________________________
////////////////FNAL CUTs
bool EventNtuple::FNALcutsElectron(){
 
  if (METLV[0].Et() <= 30)
     return false;
  
  TLorentzVector ll = lLV[0];
  TLorentzVector met = METLV[0];
  TLorentzVector mt(ll.Px() + met.Px(),ll.Py() + met.Py(),0,ll.E() + met.E());

  if (mt.M() <= 30)
    return false;

  if (ll.Pt() <= 35)
    return false;
  
  if (fabs(ll.Eta()) >= 2.5)
    return false;


  if ((int)jLV.size() > 0 && jLV[0].Pt() <= 30)
    return false;
  if ((int)jLV.size() > 1 && jLV[1].Pt() <= 30)
    return false;
  if (abs(jLV[0].Eta()) >= 2.4)
    return false;
  if (abs(jLV[1].Eta()) >= 2.4)
    return false;

  for (unsigned int i = 0; i < jLV.size(); i++){
    if (jLV[i].DeltaR(ll) <= 0.3)
      return false;
  }
  TLorentzVector jj = jLV[0] + jLV[1];
    if (jj.M() <= 65 || jj.M() >= 95)
      return false;

    return true;
}

//______________________________________________________________________________
//First FNAL Cut (electron)
bool EventNtuple::FNALcutsMuon(){

  if (METLV[0].Et() <= 30)
    return false;

  TLorentzVector ll = lLV[0];
  TLorentzVector met = METLV[0];
  TLorentzVector mt(ll.Px() + met.Px(),ll.Py() + met.Py(),0,ll.E() + met.E());

  if (mt.M() <= 30)
    return false;
 
  if (ll.Pt() <= 25)
    return false;

  if (fabs(ll.Eta()) >= 2.1)
    return false;


  if ((int)jLV.size() > 0 && jLV[0].Pt() <= 30)
    return false;

  if ((int)jLV.size() > 1 && jLV[1].Pt() <= 30)
    return false;

  if (abs(jLV[0].Eta()) >= 2.4)
    return false;

  if (abs(jLV[1].Eta()) >= 2.4)
    return false;
  
  for (unsigned int i = 0; i < jLV.size(); i++){
    if (jLV[i].DeltaR(ll) <= 0.3)
      return false;
  }
  TLorentzVector jj = jLV[0] + jLV[1];
  if (jj.M() < 65 || jj.M() >= 95)
    return false;

  return true;

}//First FNAL Cut muon


void printPrimeFactors(int num, int div = 2)
{
        if (num % div == 0) {
                std::cout << div << " ";
                printPrimeFactors(num / div, div);
        } else if (div <= num) {
                printPrimeFactors(num, div + 1);
        }
}

//______________________________________________________________________________
void EventNtuple::printDecayInformation(int decayParticle, Int_t instance, Int_t depth, TString option) {

   option.ToLower();
   
   if(option.Contains("debug")){
      for(int i=depth; i>-1;i--)
         cout << "\t";
      cout << "printDecayInformation::At depth = " << depth << "\tdecayParticle = " << decayParticle << endl;
   }

   if (genParticleCollection.size()==0) {
      cout << "WARNING::No genParticleCollection present." << endl;
      return;
   }

   int i=0;
   int tempInst = 1;
   bool foundParticle = false;
   for (unsigned int p=0; p<genParticleCollection.size(); p++) {
      if (genParticleCollection[p].pdgId==decayParticle && tempInst==instance) {
         i=p;
         foundParticle = true;
      }
      else if(genParticleCollection[p].pdgId==decayParticle && tempInst!=instance) {
         tempInst++;
      }
      else if(tempInst!=instance && p==genParticleCollection.size()-1){
         cout << " " << decayParticle << " ";
         return;
      }
   }

   if(!foundParticle) {
      cout << " " <<decayParticle << " ";
      return;
   }

   if(genParticleCollection[i].daughterPositions.size()==0) {
      cout << " " <<decayParticle << " ";
      return;
   }
   if (0 >= depth) {
      cout << " " << decayParticle << " ";
      return;
   }
   // print the decay 
   cout << " ( " << decayParticle << " ->" ;
   for (std::vector<unsigned long>::iterator daughters = genParticleCollection[i].daughterPositions.begin(); genParticleCollection[i].daughterPositions.end() != daughters ; ++daughters ) {
      if(*daughters<=500)
         printDecayInformation(genParticleCollection[*daughters].pdgId, 1, depth - 1, option) ; 
   }// RECURSION
   cout << ") ";
   return;
}

//______________________________________________________________________________
void EventNtuple::printHiggsDecayInformation() {
   printDecayInformation(25,1,10,"");
}

//______________________________________________________________________________
bool EventNtuple::triggered(char * triggerName, bool andor){
   if(TString(triggerName).Contains("*")==1) {
      TRegexp trigRegexp(triggerName);
      bool matchTrigName = false;
      bool retAnd = true;
      bool retOr = false;
      for(map<string,bool>::iterator it = triggerMap.begin(); it!=triggerMap.end(); it++) {
         TString thisTrigPath = TString(it->first);
         matchTrigName = thisTrigPath.Contains(trigRegexp);
         if(matchTrigName == true) {
            if(andor)
               retAnd = retAnd && triggerMap[string(thisTrigPath)];
            else
               retOr = retOr || triggerMap[string(thisTrigPath)];
         }
      }
      if(andor)
         return retAnd;
      else
         return retOr;
   }
   else if (triggerMap.find(triggerName) != triggerMap.end())
      return triggerMap[triggerName];
   
   return false;
}

//______________________________________________________________________________
bool EventNtuple::findSpecificTrigger(string triggerName) {
   return triggerMap[triggerName];
}

//______________________________________________________________________________
void EventNtuple::printSpecificTrigger(string triggerName) {
   cout << "EventNtuple::printSpecificTrigger:" << endl
        << left << setw(90) << "Trigger Name" << setw(5) << "" << setw(9) << "wasAccept" << endl;
   char prev = std::cout.fill ('_');
   cout << setw(90) << "" << std::setfill(prev) << setw(5) << "" << std::setfill('_') << setw(9) << "" << endl;
   std::cout.fill(prev);
   cout << left << setw(90) << triggerName << setw(5) << "" << setw(9) << triggerMap[triggerName] << endl;
}

//______________________________________________________________________________
bool EventNtuple::findTriggers(TString triggerName, bool andor) {
   TRegexp trigRegexp(triggerName);
   bool matchTrigName = false;
   bool retAnd = true;
   bool retOr = false;
   for(map<string,bool>::iterator it = triggerMap.begin(); it!=triggerMap.end(); it++) {
      TString thisTrigPath = TString(it->first);
      matchTrigName = thisTrigPath.Contains(trigRegexp);
      if(matchTrigName == true) {
         if(andor)
            retAnd = retAnd && triggerMap[string(thisTrigPath)];
         else
            retOr = retOr || triggerMap[string(thisTrigPath)];
      }
   }
   if(andor)
      return retAnd;
   else
      return retOr;
}

//______________________________________________________________________________
void EventNtuple::printTriggers(TString triggerName) {
   TRegexp trigRegexp(triggerName);
   bool matchTrigName = false;

   cout << "EventNtuple::printSpecificTrigger:" << endl
        << left << setw(90) << "Trigger Name" << setw(5) << "" << setw(9) << "wasAccept" << endl;
   char prev = std::cout.fill ('_');
   cout << setw(90) << "" << std::setfill(prev) << setw(5) << "" << std::setfill('_') << setw(9) << "" << endl;
   std::cout.fill(prev);
   
   for(map<string,bool>::iterator it = triggerMap.begin(); it!=triggerMap.end(); it++) {
      TString thisTrigPath = TString(it->first);
      matchTrigName = thisTrigPath.Contains(trigRegexp);
      if(matchTrigName == true) {
         cout << left << setw(90) << thisTrigPath << setw(5) << "" << setw(9) << triggerMap[string(thisTrigPath)] << endl;
      }
   }
}

//______________________________________________________________________________
int EventNtuple::getNBTags() {
  int nBtag = 0;
  for(unsigned int j=0; j<jLV.size();j++) {
     //if(jLV[j].jBtagCSV==1)
     if(jLV[j].jBtagDiscriminatorCSV>0.4)
      nBtag++;
  }
  return nBtag;
}

//______________________________________________________________________________
double EventNtuple::getDeltaPhiJetJet(){
   if(jLV.size() >= 2)
      return fabs(jLV[0].DeltaPhi(jLV[1]));
   else
      return 0.0;
}

//______________________________________________________________________________
double EventNtuple::getDeltaPhiMETJet(){
  double dPhiMetJet = 999;
  for(unsigned int j=0; j<jLV.size() && j<10; j++) {
    if((j+1) == 1 ){dPhiMetJet = METLV[0].DeltaPhi(jLV[j].Phi());}
  }

  if(jLV.size() >= 2)
    return dPhiMetJet;
  else
    return 0.0;
}

//______________________________________________________________________________
double EventNtuple::getMinDeltaPhiMETJet(){
  double minDPhiMetJet = 9e20;
  for(unsigned int j=0; j<jLV.size() && j<10; j++) {
    double tempDPhi = METLV[0].DeltaPhi(jLV[j]);
    if ( fabs(tempDPhi) < minDPhiMetJet ) minDPhiMetJet = tempDPhi;
  }
  return minDPhiMetJet;
}

//______________________________________________________________________________
void EventNtuple::getAngularVariables(Float_t &cosdPhiWW, Float_t &cosdPhiWH, Float_t &costhetal, 
                                      Float_t &costhetaj, Float_t &costhetaWH, Float_t &jacksonAngle,
                                      bool verbose){
  double massW = 80.4 ;
  double massMuon = 0.1;
  double nvPz1 = 0;
  double nvPz2 = 0; nvPz2=nvPz2;
  double coeffA = (massW*massW - massMuon*massMuon)/2 + lLV[0].Px()*METLV[0].Px() + lLV[0].Py()*METLV[0].Py();
  double coeffa= lLV[0].E()*lLV[0].E() - lLV[0].Pz()*lLV[0].Pz();
  double coeffb = (-2)*coeffA*lLV[0].Pz() ;
  double coeffc = lLV[0].E()*lLV[0].E()*(METLV[0].Px()*METLV[0].Px()+METLV[0].Py()*METLV[0].Py()) - coeffA*coeffA;
  double rootDelta = coeffb*coeffb - 4*coeffa*coeffc ;

  // imaginary roots                                                                                                  
  if (rootDelta<0){
     nvPz1 = (-1)*coeffb/(2*coeffa);
     nvPz2 = (-1)*coeffb/(2*coeffa);
  }
  //real roots                                                                                                        
  else {
     double root1 = (-1)*coeffb/(2*coeffa) + sqrt(rootDelta)/(2*coeffa);
     double root2 = (-1)*coeffb/(2*coeffa) - sqrt(rootDelta)/(2*coeffa);
     if (fabs(root1)>fabs(root2)) {
        nvPz1= root2; nvPz2= root1 ;
     }
     else {
        nvPz1= root1; nvPz2= root2 ;
     }
  }

  double nvEt = TMath::Sqrt(METLV[0].E()*METLV[0].E() + nvPz1*nvPz1);
  TLorentzVector pu(lLV[0].Px(),lLV[0].Py(),lLV[0].Pz(),lLV[0].E());
  TLorentzVector pv(METLV[0].Px(),METLV[0].Py(),nvPz1,nvEt);
  TLorentzVector pj1(jLV[0].Px(),jLV[0].Py(),jLV[0].Pz(),jLV[0].E());
  TLorentzVector pj2(jLV[1].Px(),jLV[1].Py(),jLV[1].Pz(),jLV[1].E());

  TLorentzVector fit_mup(0,0,0,0); 
  TLorentzVector fit_nvp(0,0,0,0);
  TLorentzVector fit_ajp(0,0,0,0); 
  TLorentzVector fit_bjp(0,0,0,0);

  Float_t fit_chi2;
  Int_t   fit_NDF;
  Int_t   fit_status;

  bool isMuon = false ;
  if ( lLV[0].leptonCat == DEFS::muon ) isMuon = true ;
       
  KinFit::doKinematicFit(isMuon, 1,  pu,  pv,  pj1,  pj2, fit_mup,  fit_nvp, fit_ajp,  fit_bjp, 
                 fit_chi2,  fit_NDF,  fit_status);

  dg_kin_Wuv_Wjj( pu, pv, pj1, pj2, cosdPhiWW, cosdPhiWH, costhetal, costhetaj, costhetaWH);
  jacksonAngle = JacksonAngle( pj1,pj2);

  if (TMath::IsNaN(costhetal)) {
    if(verbose)
      cout << "WARNING::EventNtuple::getAngularVariables costhetal is a NaN" << endl
           << "Setting its value to -999.0" << endl;
    costhetal = -999.0;
  }
}

//______________________________________________________________________________
double EventNtuple::getJacobePeak(){
  if(jLV.size() >= 2)
    return jLV[1].Pt() / ((jLV[0]+jLV[1]).M());
  else
    return 0.0;
}

//______________________________________________________________________________
double EventNtuple::getDeltaRlepjj(){
  if(jLV.size() >= 2) {
    double minDeta = 9999;
    double dRljj = 0.0;
    for (int iJet = 0; iJet < (int)jLV.size()-1; ++iJet) {
       for (int jJet = iJet+1; jJet < (int)jLV.size(); ++jJet) {
          double deltaEta = fabs( jLV[iJet].Eta() - jLV[jJet].Eta() );
          if ( deltaEta < minDeta ) {
             minDeta = deltaEta ;
             TLorentzVector m2jj_an = jLV[iJet] + jLV[jJet];
             dRljj = lLV[0].DeltaR(m2jj_an);
          }
       }
    }
    return dRljj;
  }
  else
    return 0.0;
}

//______________________________________________________________________________
double EventNtuple::getMinDPhiLepJet(){
  if(jLV.size() >= 2) {
    double dPhilj_f = 0;
    if ( fabs(lLV[0].DeltaPhi(jLV[0])) < fabs(lLV[0].DeltaPhi(jLV[1])) ) {
       dPhilj_f = fabs(lLV[0].DeltaPhi(jLV[0]));
    }
    else {  
       dPhilj_f = fabs(lLV[0].DeltaPhi(jLV[1]));
    }
    return dPhilj_f ;
  }
  else
    return 0.0;
}

//______________________________________________________________________________
double EventNtuple::getSumJetEt() {
  double sumJetEt = 0.0;
  for(unsigned int j=0; j<jLV.size() && j<10; j++) {
    sumJetEt += jLV[j].Pt();
  }
  return sumJetEt;
}

ClassImp(EventNtuple)

