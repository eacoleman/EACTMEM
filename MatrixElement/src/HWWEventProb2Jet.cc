// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 03/24/2011
// The diagrams can be compared with madgraph using the processes 
// g g -> e+ ve u~ d, g g -> e- ve~ u d~ 


//  This package libraries
#include "TAMUWW/MatrixElement/interface/HWWEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

//  C++ libraries
#include <vector>
#include <iostream>
#include <cmath>

// Set this flag to test the output madgraph would give
// #define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

// #ifdef MADGRAPH_TEST
// extern "C" {
//   void* hwwlpm_ (double[][4], double*, const double*, double*); // lepQ>0 
//   void* hwwlmm_ (double[][4], double*, const double*, double*); // lepQ<0
// }
// #endif

// ------------------------------------------------------------------
HWWEventProb2Jet::HWWEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf, 
				   double higgsMass) :
  EventProb2Jet(DEFS::EP::HWW, integrator, 3, 4, tf), 
  alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  setHiggsMassAndWidth(higgsMass);
  // Set GH
  setGH(higgsMass,MEConstants::topMass,alphas_process,MEConstants::v);
}

// ------------------------------------------------------------------
// This method sets the Higgs mass and the Higgs width
void  HWWEventProb2Jet::setHiggsMassAndWidth(double mHiggs) {

  // Set the mass
  m_massHiggs = mHiggs;

  // Use the theoretical Higgs width for the given mass 
  // multiplied by a factor of 100
  m_widthHiggs = 100.0 * calcHiggsWidth(m_massHiggs);

  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massHiggs);

}//setHiggsMassAndWidth
// ------------------------------------------------------------------
// This method sets the effective higgs coupling to two gluons
void  HWWEventProb2Jet::setGH(double mHiggs, double mTop, double alphas, double v) {
  double tau = mHiggs*mHiggs/(4*mTop*mTop);
  double series_t = 1 + tau*7/30 + tau*tau*2/21 + tau*tau*tau*26/525;
  double sgg = MEConstants::GetAdjusted_sgg(alphas);
  m_gh = sgg*sgg*series_t/(12.0 * TMath::Pi() * TMath::Pi() * v);
}//setGH
// ------------------------------------------------------------------
void HWWEventProb2Jet::changeVars(const vector<double>& parameters)
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
double HWWEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::zMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zWidth;

//    MEConstants::PrintAllConstants(alphas_process);
//    cout << "temp_gh = " << m_gh << endl;
//    cout << "hMass = " << m_massHiggs << endl;
//    cout << "hWidth = " << m_widthHiggs << endl;

   const PartonColl* partons = getPartonColl();

   double answer = 0;
   
   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGH[2]    = {doublecomplex(m_gh, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGWWH[2]  = {doublecomplex(MEConstants::gwwh, 0),
				   doublecomplex(0, 0)};


   // This should equal the number of Feynman diagrams.
   enum {vecSize = 4};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   if (partons->getLepCharge() > 0)
   {
      // Calculate the lepton only once per integration
      static Array1 vec3;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
         lepE = partons->getLepton().E();
      }

      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
      Array2 vec2 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, +1);

      Array4 vec7 = DHELAS::hvvhxx(vec1, vec2, factorGH, m_massHiggs, m_widthHiggs);
      Array1 vec8 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array4 vec9 = DHELAS::jvsxxx(vec8, vec7, factorGWWH, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec5, vec6, vec9, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	doublecomplex m1 = 4*std::norm(output[i]) * 6;
	answer += m1.real();
	//cout << "current helicity 'amplitude'" << (m1).real() << " + " << (m1).imag() << "i" << endl;
      }

   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec3;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
         lepE = partons->getLepton().E();
      }

      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
      Array2 vec2 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
//      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, 1);
      Array1 vec6 = DHELAS::ixxxxx<1>(partons->getJet(1), 0, -1);
      Array4 vec7 = DHELAS::hvvhxx(vec1, vec2, factorGH, m_massHiggs, m_widthHiggs);
      Array1 vec8 = DHELAS::jioxxx(vec4, vec3, factorGWF , wMass, wWidth);
      Array4 vec9 = DHELAS::jvsxxx(vec8, vec7, factorGWWH, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec6, vec5, vec9, factorGWF);

      
      for (unsigned i = 0; i < vecSize; ++i)
      {
	doublecomplex m1 = 4*std::norm(output[i]) * 6;
	answer += m1.real();
	//cout << "current helicity 'amplitude'" << (m1).real() << " + " << (m1).imag() << "i" << endl;
      }

   }// if charge is neg or pos

   // divide by variable IDEN
   answer /= 256;

// #ifdef MADGRAPH_TEST
//   // -----------------------------------------------------------
//   // This code reports our answer as well as the madgraph answer
//   // -----------------------------------------------------------
//   // Report our answers
//   cout<<" My answer= "<<answer<<endl;
  
//   // Make a fortran array, format which is needed for the fortran calls
//   double fortranArray[6][4];
//   makeFortranArray(fortranArray);
   
//   // Evalute the matrix element according to madgraph
//   double mhiggs = m_massHiggs; 
//   double whiggs = m_widthHiggs;
//   double an = 0;
//   if (partons->getLepCharge() > 0)
//     hwwlpm_(fortranArray , &mhiggs, &whiggs, &an);
//   else
//     hwwlmm_(fortranArray , &mhiggs, &whiggs, &an);

//   cout << "Madgraph answer= " << an << endl;
   
//   // Exit right away
//   exit(1);  

// #endif


  return answer;

}

// ------------------------------------------------------------------
void HWWEventProb2Jet::setPartonTypes() const
{   
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kGluon);
}

// ------------------------------------------------------------------
void HWWEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

// ------------------------------------------------------------------
void HWWEventProb2Jet::setupIntegral() {
  // just report it to screen
  // Set GH
  setGH(m_massHiggs,MEConstants::topMass,alphas_process,MEConstants::v);
  cout << "\tHiggs mass: " << m_massHiggs
       <<"  width: "       << m_widthHiggs 
       <<"  m_gh=  "       << m_gh << endl;

}//setup Integral

bool HWWEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
    break;
  case 1:
    swapJets(0, 1);
    break;
  default:
    return false;
  }
  
  return true;

}
