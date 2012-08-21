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

ClassImp(EventNtuple)

