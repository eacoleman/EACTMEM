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

ClassImp(EventNtuple)

