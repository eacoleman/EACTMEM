//////////////////////////////////////////////////////////////////////
//// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
//// The diagrams can be compared with madgraph using the processes
//// bb final state (suppressed by x~20 vs the leading diagrams): u d~ -> e+ ve b b~ (d~ u -> e+ ve b b~) for W+, d u~ -> e- ve~ b b~ (u~ d -> e- ve~ b b~ ) for W-
//////////////////////////////////////////////////////////////////////
#include "TAMUWW/MatrixElement/interface/WbbEventProb2Jet.hh"

//  C++ libraries
#include <vector>
#include <iostream>
#include <cmath>
#include <strstream>

// //for output on the screen
#include <iomanip>
using namespace std;

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

// Set this flag to test the output madgraph would give
//#define MADGRAPH_TEST
using std::vector;
//using std::cout;
//using std::endl;

#ifdef MADGRAPH_TEST
extern "C"
{
  void* wpbbm_(double[][4], const double*, double*); // lepQ>0, u d~ -> e+ ve b b~
  void* wpbbaltm_ (double[][4], const double*, double*); // lepQ>0, d~ u -> e+ ve b b~ instead
  void* wmbbm_(double[][4], const double*, double*); // lepQ<0, d u~ -> e- ve~ b b~
  void* wmbbaltm_ (double[][4], const double*, double*); // lepQ<0, u~ d -> e- ve~ b b~ instead
  // to use the altm_ functions for just one iteration initialize the swapPartonMom(true) 
}
#endif

// ------------------------------------------------------------------
WbbEventProb2Jet::WbbEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::Wbb, integrator, 3, 4, tf), swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
}
// ------------------------------------------------------------------

void WbbEventProb2Jet::changeVars(const vector<double>& parameters)
{
  using AuxFunctions::Math::square;

  TLorentzVector& jet1 = getPartonColl()->getJet(0);
  TLorentzVector& jet2 = getPartonColl()->getJet(1);
  
  jet1.SetRho(parameters[1]);
  jet1.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[1])));
  jet2.SetRho(parameters[2]);
  jet2.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[2])));

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}
// ------------------------------------------------------------------

double WbbEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGG[2]  = {doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0),
				   doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0)};
   //   doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::GetAdjusted_sgg(alphas_process), 0),
   //   			   doublecomplex(0, 0)};

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

      ///Initial State Partons
      Array1 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
	vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
	vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      }
      ///Final State Neutrino & Partons
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec1, vec8, factorGG, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec6, vec5, vec9, factorGG);

      Array1 vec12 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array1 vec13 = DHELAS::jioxxx(vec12, vec2, factorGG, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec6, vec5, vec13, factorGG);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = -output1[i] - output2[i];
	//doublecomplex temp2 = 0;
	double m1 = 2.0*std::norm(temp1);

	answer+=m1;
      }

   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec3;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, +1);
         lepE = partons->getLepton().E();
      }

      ///Initial State Partons
      Array1 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
	vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
	vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      }
      ///Final State Neutrino & Partons
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, +1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec1, vec8, factorGG, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec6, vec5, vec9, factorGG);

      Array1 vec12 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array1 vec13 = DHELAS::jioxxx(vec12, vec2, factorGG, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec6, vec5, vec13, factorGG);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = +output1[i] + output2[i];
	//doublecomplex temp2 = 0;
	double m1 = 2.0*std::norm(temp1);

	answer+=m1;
      }

   }

   answer /= 36;

#ifdef MADGRAPH_TEST
  // -----------------------------------------------------------
  // This code reports our answer as well as the madgraph answer
  // -----------------------------------------------------------
   
  // Report our answers
   std::cout<<" My answer= "<<answer<< std::endl;

  // Make a fortran array, format which is needed for the fortran calls
  double fortranArray[6][4];
  makeFortranArray(fortranArray);

  // Evalute the matrix element according to madgraph
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;
  if (partons->getLepCharge() > 0) {
    if ( !swapPartonMom ) {
      wpbbm_(fortranArray , &mw, &an);
    } else {
      wpbbaltm_(fortranArray , &mw, &an);
    }
  } else {
    if ( !swapPartonMom ) {
      wmbbm_(fortranArray , &mw, &an);
    } else {
      wmbbaltm_(fortranArray , &mw, &an);
    }
  }
  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif


   return answer;

}


// ------------------------------------------------------------------
void WbbEventProb2Jet::setPartonTypes() const
{
   if (getPartonColl()->getLepCharge() > 0)
   {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kAntiDown);
    } else {
      getMeasuredColl()->setParton1Type(kAntiDown);
      getMeasuredColl()->setParton2Type(kUp);
    }
   }
   else
   {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kAntiUp);
    } else {
      getMeasuredColl()->setParton1Type(kAntiUp);
      getMeasuredColl()->setParton2Type(kDown);
    }
   }

}

void WbbEventProb2Jet::setJetTypes()
{
  m_JetType[0]=kBottom;
  m_JetType[1]=kAntiBottom;

  if ( getSwappedJet0Jet1Status() ) {
    int tempType=m_JetType[0];
    m_JetType[0]=m_JetType[1];
    m_JetType[1]=tempType;
  }
}

// ------------------------------------------------------------------
void WbbEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

// ------------------------------------------------------------------


bool WbbEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
    //swapPartonMom=true; //when testing alternate functions
    swapPartonMom=false;
    setSwapJet0Jet1Status(false);
    break;
  case 1:
    swapJets(0, 1);
    setSwapJet0Jet1Status(true);
    break;
  case 2:
    swapPartonMom=true;
    break;
  case 3:
    swapJets(0, 1);
    setSwapJet0Jet1Status(false);
    break;
  default:
    return false;
  }

  return true;

}


