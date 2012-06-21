/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/TopLepType.hh"

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

GenParticle::GenParticle() {
   charge = 0;
   pdgId = 0;
   status = 0;
   particlePosition = -1;
   numberOfMothers = -1;
   numberOfDaughters = -1;
}

GenParticle::~GenParticle() {}

EventNtuple::EventNtuple() {}

EventNtuple::~EventNtuple() {}

bool EventNtuple::baseCuts()
{
   return (METEtMin());
}

bool EventNtuple::METEtMin(double EtMin)
{
   return METLV[0].Et()>EtMin;
}

double EventNtuple::getQGLikelihood(unsigned int index, QGLikelihoodCalculator* qglikeli)
{
   if(index >= jLV.size())
      return -1.0;

   double qglikelihood = 0.0;
   bool del = false;

   if (!qglikeli) {
      qglikeli = new QGLikelihoodCalculator();
      del = true;
   }

   qglikelihood = qglikeli->computeQGLikelihoodPU(jLV[index].Pt(), rho, jChargedMultiplicity[index],
                                                  jNeutralMultiplicity[index], jPtD[index]);
   if (del)
      delete qglikeli;
   
   return qglikelihood;
}

vector<double> EventNtuple::getQGLikelihoods(QGLikelihoodCalculator* qglikeli)
{
   vector<double> qglikelihoods;
   bool del = false;
   
   if (!qglikeli) {
      qglikeli = new QGLikelihoodCalculator();
      del = true;
   }

   for (unsigned int i=0; i<jLV.size(); i++) {
      qglikelihoods.push_back(getQGLikelihood(i,qglikeli));
   }
   
   if (del)
      delete qglikeli;

   return qglikelihoods;
}

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

  return  corr + jer * (1 - corr);

}

void EventNtuple::doJER(){

  if (jLV.size()<2 || refjLV.size()<2){
    cout<<"ERROR EventNtuple::doJER cannot be called with zero-size vectors"<<endl;
    return ;
  }

  double c0 = getJERfactor(jLV[0].Pt(), jLV[0].Eta(), refjLV[0].Pt());
  double c1 = getJERfactor(jLV[1].Pt(), jLV[1].Eta(), refjLV[1].Pt());

  double newMetX = (c0 - 1)*jLV[0].X() + (c1 - 1)*jLV[1].X() + METLV[0].X();
  double newMetY = (c0 - 1)*jLV[0].Y() + (c1 - 1)*jLV[1].Y() + METLV[0].Y();
  //Jet 1
  jLV[0]= jLV[0]*c0;

  //Jet 2
  jLV[1]= jLV[1]*c1;


  METLV[0].SetX(newMetX);
  METLV[0].SetY(newMetY);

  METEt = sqrt(pow(newMetX, 2) + pow(newMetY, 2));

}

////////////////FNAL CUTs

bool EventNtuple::FNALcutsElectron(){
 
  if (METEt <= 30)
    return false;
  
  TLorentzVector ll(lLV[0].Px(),lLV[0].Py(),lLV[0].Pz(),lLV[0].E());
  TLorentzVector met(METLV[0].Px(),METLV[0].Py(),METLV[0].Py(),METLV[0].E());
  TLorentzVector mt(lLV[0].Px()+METLV[0].Px(),lLV[0].Py()+METLV[0].Py(),0,lLV[0].E()+METLV[0].E());

  if (mt.M() <= 30)
    return false;

  if (lLV[0].Pt() <= 35)
    return false;
  
  if (abs(lLV[0].Eta()) >= 2.5)
    return false;

  int h = 0;
  int nh = 0;
  int Nj = jLV.size();
	
  for (int i = 0; i<Nj; i++) {
    if (jLV[i].Pt() >= jLV[h].Pt()) {
      nh = h;
      h = i;
    }
    else if (jLV[i].Pt() < jLV[h].Pt() && jLV[i].Pt() >= jLV[nh].Pt()) {
      nh = i;
    }
    else
      continue;
  }
  if ((int)jLV.size() > h && jLV[h].Pt() <= 30)
    return false;
  if ((int)jLV.size() > nh && jLV[nh].Pt() <= 30)
    return false;
  if (abs(jLV[h].Eta()) >= 2.4)
    return false;
  if (abs(jLV[nh].Eta()) >= 2.4)
    return false;

  bool dr = true;
  for (int i = 0; i<Nj; i++){
    TLorentzVector j(jLV[i].Px(),jLV[i].Py(),jLV[i].Pz(),jLV[i].E());
    if (j.DeltaR(ll) <= 0.5)
      dr = false;
    else
      continue;
  }
  if (dr) {
    TLorentzVector jj = jLV[h] + jLV[nh];
    if (jj.M()<=65 || jj.M()>=95)
      return false;
  }
  return true;
}//First FNAL Cut (electron)

bool EventNtuple::FNALcutsMuon(){

  if (METEt <= 30)
    return false;

  TLorentzVector ll(lLV[0].Px(),lLV[0].Py(),lLV[0].Pz(),lLV[0].E());
  TLorentzVector met(METLV[0].Px(),METLV[0].Py(),METLV[0].Py(),METLV[0].E());
  TLorentzVector mt(lLV[0].Px()+METLV[0].Px(),lLV[0].Py()+METLV[0].Py(),0,lLV[0].E()+METLV[0].E());

  if (mt.M() <= 30)
    return false;
 
  if (lLV[0].Pt() <= 25)
    return false;

  if (abs(lLV[0].Eta()) >= 2.1)
    return false;

  int h = 0;
  int nh = 0;
  int Nj = jLV.size();
  
  for (int i = 0; i<Nj; i++) {
    if (jLV[i].Pt() >= jLV[h].Pt()) {
      nh = h;
      h = i;
    }
    else if (jLV[i].Pt() < jLV[h].Pt() && jLV[i].Pt() >= jLV[nh].Pt()) {
      nh = i;
    }
    else
      continue;
  }

  if ((int)jLV.size() > h && jLV[h].Pt() <= 30)
    return false;

  if ((int)jLV.size() > nh && jLV[nh].Pt() <= 30)
    return false;

  if (abs(jLV[h].Eta()) >= 2.4)
    return false;

  if (abs(jLV[nh].Eta()) >= 2.4)
    return false;
  
  bool dr = true;
  for (int i = 0; i<Nj; i++){
    TLorentzVector j(jLV[i].Px(),jLV[i].Py(),jLV[i].Pz(),jLV[i].E());
    if (j.DeltaR(ll) <= 0.5)
      dr = false;
    else
      continue;
  }
  if (dr) {
    TLorentzVector jj = jLV[h] + jLV[nh];
    if (jj.M()<65 || jj.M()>=95)
      return false;
  }
  return true;
}//First FNAL Cut muon

ClassImp(EventNtuple)

