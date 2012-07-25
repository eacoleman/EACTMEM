// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 12/12/2010
// The diagrams can be compared with madgraph using the processes 
// g g -> g g g g - Use the leading diagram only

#include "TAMUWW/MatrixElement/interface/QCDEventProb2Jet.hh"


#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

//  C++ libraries
//#include "TLorentzVector.h"

#include <vector>
#include <iostream>
#include <cmath>

#include <strstream>

//for output on the screen
#include <iomanip>
using namespace std;

// Set this flag to test the output madgraph would give
// #define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST
extern "C"
{
  void* ggggm_(double[][4], const double*, const double*, double*); // lepQ>0 or lepQ<0 
}
#endif

// ------------------------------------------------------------------
QCDEventProb2Jet::QCDEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::QCD, integrator, 3, 2, tf), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{}

// ------------------------------------------------------------------


// ------------------------------------------------------------------
void QCDEventProb2Jet::changeVars(const vector<double>& parameters)
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
double QCDEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;
   typedef SimpleArray<DHELAS::HelArray, 8> Array8;
   typedef SimpleArray<DHELAS::HelArray, 16> Array16;
   typedef SimpleArray<DHELAS::HelArray, 32> Array32;

   //MEConstants::PrintAllConstants(alphas_process);

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::GetAdjusted_sgg(alphas_process), 0),
				    doublecomplex(0, 0)};

   // Calculate the lepton only once per integration
   static Array2 vec3; //wavefunction
   static double lepE = 0;
   Array2 vec4;
   
   if (lepE != partons->getLepton().E())
     {
       vec3 = DHELAS::vxxxxx<2>(partons->getLepton(), 0, +1);
       lepE = partons->getLepton().E();
       }
   vec4 = DHELAS::vxxxxx<2>(partons->getNeutrino(), 0, +1);

   Array2 vec1=DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);     
   Array2 vec2=DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
 
   Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(0), 0, +1);
   Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, +1);
   
   Array4 vec7 = DHELAS::jvvxxx(vec3, vec2, factorSGG, 0, 0);
   Array8 vec8 = DHELAS::jvvxxx(vec4, vec7, factorSGG, 0, 0);
   Array16 vec9 = DHELAS::jvvxxx(vec5, vec8, factorSGG, 0, 0);
   OutputType output1 = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);

   
   for (unsigned i = 0; i < vecSize; ++i)
     {
       double m1= 648*std::norm(output1[i]);
       //648=16*(3641+454-50+572-50-76+428+184+454+68-76+40+572+40+184+176)*4/648
       answer+= m1;
       //cout << "current helicity 'amplitude'" << m1 << endl;

     }
   
   
   answer /= 6144; //relative weight
//   std::cerr << "New Answer: " << answer << std::endl;
#ifdef MADGRAPH_TEST
  // -----------------------------------------------------------
  // This code reports our answer as well as the madgraph answer
  // -----------------------------------------------------------
  // Report our answers
  cout<<" My answer= "<<answer<<endl;

  // Make a fortran array, format which is needed for the fortran calls
  double fortranArray[6][4];
  makeFortranArray(fortranArray);
   
//   double fortranArray_vl[6][4];
//   makeFortranArray_qqvl(fortranArray_vl);
  // Evalute the matrix element according to madgraph
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = 0; // to get rid of the const identifier
  double mz = 0;
  double an = 0;
  //  if (partons->getLepCharge() > 0)
  ggggm_(fortranArray, &mz, &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

   return answer;

}

void QCDEventProb2Jet::setPartonTypes() const
{
  getMeasuredColl()->setParton1Type(kGluon);
  getMeasuredColl()->setParton2Type(kGluon);
}

void QCDEventProb2Jet::setJetTypes()
{

  m_JetType[0]=kGluon;
  m_JetType[1]=kGluon;

//   if ( getSwappedJet0Jet1Status() ) {
//     int tempType=m_JetType[0];
//     m_JetType[0]=m_JetType[1];
//     m_JetType[1]=tempType;
//   }

}


void QCDEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool QCDEventProb2Jet::onSwitch()
{

  switch (getLoop()) {
  case 0:
    setSwapJet0Jet1Status(false);
    break;
  case 1:
    swapJets(0, 1);
    setSwapJet0Jet1Status(true);
    break;
  default:
    return false;
  }

  return true;

}
