// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 11/19/2010
// The diagrams can be compared with madgraph using the processes 
// u b -> b e+ ve d (b u -> b e+ ve d), d b~ -> b~ e- ve~ u

#include "TAMUWW/MatrixElement/interface/tChannelEventProb2Jet.hh"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

//#define MADGRAPH_TEST
using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST

extern "C"
{
  void* tchanlpm_(double[][4], const double*, double*);// lepQ>0, u b -> b e+ ve d 
  void* tchanlpaltm_(double[][4], const double*, double*);// b u -> b e+ ve d 
  void* tchanlmm_(double[][4], const double*, double*);// lepQ<0, d b~ -> b~ e- ve~ u
  void* tchanlmaltm_(double[][4], const double*, double*);// 
}
#endif

// ------------------------------------------------------------------
tChannelEventProb2Jet::tChannelEventProb2Jet(Integrator& integrator, const TransferFunction& btf, const TransferFunction& lighttf) :
  EventProb2Jet(DEFS::EP::TopT, integrator, 3,4, btf), 
  m_lightTF(lighttf), //m_lightTF is stored locally (in the .hh)
  swapPartonMom(false), 
  alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);
}

// // ------------------------------------------------------------------
// tChannelEventProb2Jet::tChannelEventProb2Jet(Integrator& integrator, const TransferFunction& btf, const TransferFunction& lighttf, const std::string& name) :
//   EventProb2Jet("t-channel", integrator, 3, 8, btf), m_lightTF(lighttf), swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas 
// {
//   // Set the top mass and width
//   setTopMassAndWidth(MEConstants::topMass);
// } // Protected C'tor 

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void tChannelEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;
  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);
  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------
void tChannelEventProb2Jet::setDynamicBounds()
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
   m_lightTF.getBounds(getMeasuredColl()->getFullJet(1), lowPercent,
                       highPercent, lower, upper);
   std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(2, lower, upper);
}

// // ------------------------------------------------------------------
// unsigned tChannelEventProb2Jet::getProbMax() const
// {

//   //cout << "calling getProbMax" << endl;
//   //return getMeasuredColl()->getNBtags() == 1 ? 1 : 2;
//   cout << "calling getProbMax, getNBtags=" << getMeasuredColl()->getNBtags() << endl;
//   return getMeasuredColl()->getNBtags();
// }

// ------------------------------------------------------------------
void tChannelEventProb2Jet::changeVars(const vector<double>& parameters)
{
   using AuxFunctions::Math::square;

   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[1])));
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

// ------------------------------------------------------------------
double tChannelEventProb2Jet::matrixElement() const
{

   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;
   typedef SimpleArray<DHELAS::HelArray, 8> Array8;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

//    MEConstants::PrintAllConstants(alphas_process);
//    cout << "tMass = " << m_massTop << endl;
//    cout << "tWidth = " << m_widthTop << endl;
   const PartonColl* partons = getPartonColl();

   double answer = 0;

   enum {vecSize = 4};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   doublecomplex factorGWF[2] = {doublecomplex(MEConstants::gwf, 0),
				 doublecomplex(0, 0)};

   if (partons->getLepCharge() > 0)
   {
      // Calculate the lepton only once per integration
      static Array1 vec4;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec4 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
         lepE = partons->getLepton().E();
      }
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);

      Array1 vec1;
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
	vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), bMass, 1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, 1);
	vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), bMass, 1);
      }

      Array2 vec3 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec5, factorGWF, wMass, wWidth);
      Array2 vec8 = DHELAS::fvoxxx(vec3, vec7, factorGWF, m_massTop, m_widthTop);
      Array4 vec9 = DHELAS::jioxxx(vec2, vec8, factorGWF, wMass, wWidth);

      OutputType output1 = DHELAS::iovxxx(vec1, vec6, vec9, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	 double temp1 =  std::norm(output1[i]) * 9;
	 answer+= temp1;
	 // cout << "i=" << i << " helicity 'amplitude'" << temp1 << " + " << "0" << "i" << endl;
      }
   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec4;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec4 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, +1);
         lepE = partons->getLepton().E();
      }
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);

      Array1 vec1;
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
	vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), bMass, -1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, 1);
	vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), bMass, -1);
      }

      Array2 vec3 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec5, vec4, factorGWF, wMass, wWidth);
      Array2 vec8 = DHELAS::fvixxx(vec3, vec7, factorGWF, m_massTop, m_widthTop);
      Array4 vec9 = DHELAS::jioxxx(vec8, vec2, factorGWF, wMass, wWidth);

      OutputType output1 = DHELAS::iovxxx(vec1, vec6, vec9, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	 double temp1 =  std::norm(output1[i]) * 9;
	 answer+= temp1;
	 //	 cout << "i=" << i << " helicity 'amplitude'" << temp1 << " + " << "0" << "i" << endl;
      }
   }

   answer /= 36;


 #ifdef MADGRAPH_TEST
  // -----------------------------------------------------------
  // This code reports our answer as well as the madgraph answer
  // -----------------------------------------------------------
   
  // Report our answers
  cout<<" My answer= "<<answer<<endl;

  // Make a fortran array, format which is needed for the fortran calls
  double fortranArray[6][4];
  makeFortranArray_qlvq(fortranArray);

  // Evalute the matrix element according to madgraph
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;
  if (partons->getLepCharge() > 0)
    tchanlpm_(fortranArray , &mw, &an);
  //tchanlpaltm_(fortranArray , &mw, &an);
  else
    tchanlmm_(fortranArray , &mw, &an);
  //tchanlmaltm_(fortranArray , &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

  return answer;

}

// ------------------------------------------------------------------
void tChannelEventProb2Jet::setPartonTypes() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kBottom);
   }
   else
   {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kAntiBottom);
   }
}

// ------------------------------------------------------------------
void tChannelEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = MEConstants::wMass / 2;
//   scale2 = MEConstants::wMass / 2 + m_massTop;
   double lightLine = std::sqrt(getPartonColl()->Q2());
   double topLine = std::sqrt(getPartonColl()->Q2()
                              + square(m_massTop));

   if (getMeasuredColl()->getLepCharge() > 0)
   {
      scale1 = lightLine;
      scale2 = topLine;
   }
   else
   {
      scale1 = topLine;
      scale2 = lightLine;
   }
//   std::cerr << "t-channel scales: " << scale1 << " " << scale2 << std::endl;
}

// ------------------------------------------------------------------
double tChannelEventProb2Jet::totalTF() const
/// Add when using more than one TF
{
   return getDefaultTF().getTF(getPartonColl()->getFullJet(0),
                         getMeasuredColl()->getFullJet(0))
      * m_lightTF.getTF(getPartonColl()->getFullJet(1),
                        getMeasuredColl()->getFullJet(1));
}

// ------------------------------------------------------------------
bool tChannelEventProb2Jet::onSwitch()
{

  switch (getLoop()) {
  case 0:
    //swapPartonMom=true; //when testing alternate functions
    swapPartonMom=false; 
    break;
  case 1:
    swapJets(0, 1);
    break;
  case 2:
    swapPartonMom=true;
    break;
  case 3:
    swapJets(0, 1);
    break;
  default:
    return false;
  }

  return true;

}

// ------------------------------------------------------------------
void tChannelEventProb2Jet::setupIntegral(){

  std::cout<<"\tTop mass= "<< m_massTop<<" width= "<<m_widthTop<<std::endl;

}


