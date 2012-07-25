//////////////////////////////////////////////////////////////////////
//// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
//// The diagrams can be compared with madgraph using the processes
//// Lb final state (suppressed by x~40 vs the leading diagrams): u b -> e+ ve d b (b u -> e+ ve d b) for W+, d b~ -> e- ve~ u b~ (b~ d -> e- ve~ u b~) for W-
//////////////////////////////////////////////////////////////////////
#include "TAMUWW/MatrixElement/interface/WLbEventProb2Jet.hh"

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
  void* wplbm_(double[][4], const double*, double*); //lepQ>0, u b -> e+ ve d b 
  void* wplbaltm_ (double[][4], const double*, double*); //lepQ>0, b u -> e+ ve d b instead
  void* wmlbm_(double[][4], const double*, double*); //lepQ<0, d b~ -> e- ve~ u b~
  void* wmlbaltm_ (double[][4], const double*, double*); //lepQ<0, b~ d -> e- ve~ u b~ instead
  // to use the altm_ functions for just one iteration initialize the swapPartonMom(true) 
}
#endif

// ------------------------------------------------------------------
WLbEventProb2Jet::WLbEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf, const TransferFunction& btf) :
  EventProb2Jet(DEFS::EP::WLb, integrator, 3, 4, lighttf),
  m_bTF(btf),
  swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);
}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void WLbEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;
  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);
  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------

void WLbEventProb2Jet::changeVars(const vector<double>& parameters)
{
   using AuxFunctions::Math::square;

  TLorentzVector& jet1 = getPartonColl()->getJet(0);
  TLorentzVector& jet2 = getPartonColl()->getJet(1);
  
  jet1.SetRho(parameters[1]);
  jet1.SetE(parameters[1]);
  jet2.SetRho(parameters[2]);
  jet2.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[2])));

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

// ------------------------------------------------------------------
void WLbEventProb2Jet::setDynamicBounds()
///Add this function when using more than one TF
{
   const float lowPercent = .01;
   const float highPercent = .02;
   double lower, upper;
   getDefaultTF().getBounds(getMeasuredColl()->getFullJet(0), lowPercent,
                      highPercent, lower, upper);
   std::cout << "\tSetting jet 1 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(1, lower, upper);
   m_bTF.getBounds(getMeasuredColl()->getFullJet(1), lowPercent,
                       highPercent, lower, upper);
   std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(2, lower, upper);
}

// ------------------------------------------------------------------

double WLbEventProb2Jet::matrixElement() const
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
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
	vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), bMass, +1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
	vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), bMass, +1);
      }
      ///Final State Neutrino & Partons
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, +1);
      Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, +1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec1, vec5, factorGWF, wMass, wWidth);
      Array2 vec12 = DHELAS::fvixxx(vec2, vec8, factorGWF, m_massTop, m_widthTop);
      OutputType output1 = DHELAS::iovxxx(vec12, vec6, vec7, factorGWF);


      Array1 vec13 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array1 vec14 = DHELAS::jioxxx(vec13, vec5, factorGG, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec2, vec6, vec14, factorGG);

      Array1 vec17 = DHELAS::fvoxxx(vec5, vec7, factorGWF, 0, 0);
      Array1 vec18 = DHELAS::jioxxx(vec1, vec17, factorGG, 0, 0);
      OutputType output3 = DHELAS::iovxxx(vec2, vec6, vec18, factorGG);


      for (unsigned i = 0; i < vecSize; ++i) {
	doublecomplex temp1 = -output1[i];
	doublecomplex temp2 = -output2[i] - output3[i];
	double m1 = 9.0*std::norm(temp1);
	double m2 = 2.0*std::norm(temp2);
	
	answer+= m1+m2;
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
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
	vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), bMass, -1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
	vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), bMass, -1);
      }
      ///Final State Neutrino & Partons
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, +1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec1, vec5, factorGWF, wMass, wWidth);
      Array2 vec12 = DHELAS::fvoxxx(vec2, vec8, factorGWF, m_massTop, m_widthTop);
      OutputType output1 = DHELAS::iovxxx(vec6, vec12, vec7, factorGWF);

      Array1 vec13 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array1 vec14 = DHELAS::jioxxx(vec13, vec5, factorGG, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec6, vec2, vec14, factorGG);

      Array1 vec17 = DHELAS::fvoxxx(vec5, vec7, factorGWF, 0, 0);
      Array1 vec18 = DHELAS::jioxxx(vec1, vec17, factorGG, 0, 0);
      OutputType output3 = DHELAS::iovxxx(vec6, vec2, vec18, factorGG);

      for (unsigned i = 0; i < vecSize; ++i) {
	doublecomplex temp1 =  output1[i];
	doublecomplex temp2 = -output2[i] - output3[i];
	double m1 = 9.0*std::norm(temp1);
	double m2 = 2.0*std::norm(temp2);
	
	answer+= m1+m2;
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
      wplbm_(fortranArray , &mw, &an);
    } else {
      wplbaltm_(fortranArray , &mw, &an);
    }
  } else {
    if ( !swapPartonMom ) {
      wmlbm_(fortranArray , &mw, &an);
    } else {
      wmlbaltm_(fortranArray , &mw, &an);
    }
  }
  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif


   return answer;

}


// ------------------------------------------------------------------
void WLbEventProb2Jet::setPartonTypes() const
{
   if (getPartonColl()->getLepCharge() > 0)
   {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kBottom);
    } else {
      getMeasuredColl()->setParton1Type(kBottom);
      getMeasuredColl()->setParton2Type(kUp);
    }
   }
   else
   {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kAntiBottom);
    } else {
      getMeasuredColl()->setParton1Type(kAntiBottom);
      getMeasuredColl()->setParton2Type(kDown);
    }

   }
}

void WLbEventProb2Jet::setJetTypes()
{

  if ( getPartonColl()->getLepCharge() > 0 ) {
    m_JetType[0]=kDown;
    m_JetType[1]=kBottom;
  } else {
    m_JetType[0]=kUp;
    m_JetType[1]=kAntiBottom;
  }

  if ( getSwappedJet0Jet1Status() ) {
    int tempType=m_JetType[0];
    m_JetType[0]=m_JetType[1];
    m_JetType[1]=tempType;
  }

}
// ------------------------------------------------------------------
void WLbEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

// ------------------------------------------------------------------
double WLbEventProb2Jet::totalTF() const
/// Add when using more than one TF
{
   return getDefaultTF().getTF(getPartonColl()->getFullJet(0),
                         getMeasuredColl()->getFullJet(0))
      * m_bTF.getTF(getPartonColl()->getFullJet(1),
                        getMeasuredColl()->getFullJet(1));
}

// ------------------------------------------------------------------


bool WLbEventProb2Jet::onSwitch()
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

// ------------------------------------------------------------------
void WLbEventProb2Jet::setupIntegral(){

  std::cout<<"\tTop mass= "<< m_massTop<<" width= "<<m_widthTop<<std::endl;

}

