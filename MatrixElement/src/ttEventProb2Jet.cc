// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 10/26/2010
// The diagrams can be compared with madgraph using the processes 
// g g -> b e+ ve w- b~, g g -> b~ e- ve~ w+ b

#include "TAMUWW/MatrixElement/interface/ttEventProb2Jet.hh"

//#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <math.h>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

// Set this flag to test the output madgraph would give
//#define MADGRAPH_TEST

using std::cout;
using std::endl;
using std::vector;

using AuxFunctions::Math::square;

#ifdef MADGRAPH_TEST
extern "C"
{
  void* ttlpwbbm_(double[][4], const double*, double*); // lepQ>0, g g -> b e+ ve w- b~
  void* ttlmwbbm_(double[][4], const double*, double*); // lepQ<0, g g -> b~ e- ve~ w+ b
  //   void* mytt2_(double[][4], double*, const double*, double*);
}
#endif

// ------------------------------------------------------------------
ttEventProb2Jet::ttEventProb2Jet(Integrator& integrator,
                                 const TransferFunction& tf,
                                 double tMass) :
  EventProb2Jet(DEFS::EP::TTbar, integrator, 6, 2, tf), 
  alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  // Set the top mass and width
  setTopMassAndWidth(tMass);
}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void ttEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);

  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------
// This is the default one for regular ME calculation
void ttEventProb2Jet::changeVars(const vector<double>& parameters)
{
 
   using MEConstants::wMass;

   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);
   jet1.SetRho(parameters[1]);
   jet1.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[1])));
   jet2.SetRho(parameters[2]);
   jet2.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[2])));
   getPartonColl()->getNeutrino().SetPz(parameters[0]);

   double r = parameters[3];
   double phi = parameters[4];
   double theta = parameters[5];

   double x = r * cos(phi) * sin(theta);
   double y = r * sin(phi) * sin(theta);
   double z = r * cos(theta);

   m_W.SetXYZM(x, y, z, wMass);

}//changeVars


void ttEventProb2Jet::met()
{
   getPartonColl()->setMet();
   TLorentzVector& vec = getPartonColl()->getNeutrino();
   double px = vec.Px() - m_W.Px();
   double py = vec.Py() - m_W.Py();
   double energy = std::sqrt(square(px) + square(py)
                             + square(vec.Pz()));
   vec.SetPxPyPzE(px, py, vec.Pz(), energy);
}

void ttEventProb2Jet::getTotalLV(TLorentzVector& vec) const
{
   EventProb::getTotalLV(vec);
   vec += m_W;

}

// ------------------------------------------------------------------
double ttEventProb2Jet::matrixElement() const
{

  //cout << "calling ttbar ME" << endl;
  //  cout << "m_W=" << m_W << endl;
  // cout << "m_W.M()=" << m_W.M() << endl;
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

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGG[2]  = {doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0),
				 doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0)};
//    doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::GetAdjusted_sgg(alphas_process), 0),
//    			   doublecomplex(0, 0)};

   enum {vecSize = 48};
   typedef SimpleArray<DHELAS::HelArray, 3> Array3;
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

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

      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
      Array2 vec2 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
      Array2 vec3 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array3 vec6 = DHELAS::vxxxxx<3>(m_W, wMass, 1);
      Array2 vec7 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec8 = DHELAS::jioxxx(vec4, vec5, factorGWF, wMass, wWidth);
      Array2 vec9 = DHELAS::fvoxxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);
      Array4 vec10 = DHELAS::fvoxxx(vec9, vec2, factorGG, m_massTop, m_widthTop);
      Array8 vec11 = DHELAS::fvoxxx(vec10, vec1, factorGG, m_massTop, m_widthTop);

      OutputType output1 = DHELAS::iovxxx(vec7, vec11, vec6, factorGWF);

      Array4 vec12 = DHELAS::fvoxxx(vec9, vec1, factorGG, m_massTop, m_widthTop);
      Array8 vec13 = DHELAS::fvoxxx(vec12, vec2, factorGG, m_massTop, m_widthTop);
      OutputType output2 = DHELAS::iovxxx(vec7, vec13, vec6, factorGWF);

//       Array4 vec14 = DHELAS::jvvxxx(vec1, vec2, factorSGG, 0, 0);
//       Array8 vec15 = DHELAS::fvoxxx(vec9, vec14, factorGG, m_massTop, m_widthTop);
//       OutputType output3 = DHELAS::iovxxx(vec7, vec15, vec6, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
// 	doublecomplex temp1 = -output1[i]+output3[i];
// 	doublecomplex temp2 = -output2[i]-output3[i];
	doublecomplex temp1 = -output1[i];
	doublecomplex temp2 = -output2[i];
	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
	//	cout << "i=" << i << " helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;

      }
   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec4;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec4 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
         lepE = partons->getLepton().E();
      }
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);

      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
      Array2 vec2 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
      Array2 vec3 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array3 vec6 = DHELAS::vxxxxx<3>(m_W, wMass, 1);
      Array2 vec7 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, +1);

      Array1 vec8 = DHELAS::jioxxx(vec5, vec4, factorGWF, wMass, wWidth);
      Array2 vec9 = DHELAS::fvixxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);
      Array4 vec10 = DHELAS::fvixxx(vec9, vec2, factorGG, m_massTop, m_widthTop);
      Array8 vec11 = DHELAS::fvixxx(vec10, vec1, factorGG, m_massTop, m_widthTop);

      OutputType output1 = DHELAS::iovxxx(vec11, vec7, vec6, factorGWF);

      Array4 vec12 = DHELAS::fvixxx(vec9, vec1, factorGG, m_massTop, m_widthTop);
      Array8 vec13 = DHELAS::fvixxx(vec12, vec2, factorGG, m_massTop, m_widthTop);
      OutputType output2 = DHELAS::iovxxx(vec13, vec7, vec6, factorGWF);

//       Array4 vec14 = DHELAS::jvvxxx(vec1, vec2, factorSGG, 0, 0);
//       Array8 vec15 = DHELAS::fvoxxx(vec9, vec14, factorGG, m_massTop, m_widthTop);
//       OutputType output3 = DHELAS::iovxxx(vec7, vec15, vec6, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
// 	doublecomplex temp1 = -output1[i]-output3[i];
// 	doublecomplex temp2 = -output2[i]+output3[i];
	doublecomplex temp1 = -output1[i];
	doublecomplex temp2 = -output2[i];
	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
	//	cout << "i=" << i << " helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;

      }

   }

//   for (DHELAS::CompVec::const_iterator it = output.begin();
//        it != output.end(); ++it)
//   {
//      answer += std::norm(-(*it)) * 2;
//   }
   answer /= 256;

#ifdef MADGRAPH_TEST
  // -----------------------------------------------------------
  // This code reports our answer as well as the madgraph answer
  // -----------------------------------------------------------
  // Report our answers
  cout<<" My answer= "<<answer<<endl;

  // Make a fortran array, format which is needed for the fortran calls
  double fortranArray[7][4];
  makeFortranArray_qlvWq(fortranArray,m_W);
   
  // Evalute the matrix element according to madgraph
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;
  if (partons->getLepCharge() > 0)
    ttlpwbbm_(fortranArray, &mw, &an);
  else
    ttlmwbbm_(fortranArray, &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

//   int zero = 0;
//   answer = 1/zero;
//   //if ( (!std::isfinite(answer)) || isnan(answer)) return 0;
//   // if (!isfinite(answer)) return 0;
//   //  if (anwers is weird) return 0;
//   if (std::fpclassify(answer) !=  FP_NORMAL) return 0;
  return answer;
//    assert ( answer>=0 );
//    //return answer;
//    //return 1.5;
//    return (1/0);



}

double ttEventProb2Jet::phaseSpace() const
{
   // For an explanation of why this should be so, see the parallel
   // function in the 3-jet bin
   return EventProb::phaseSpace() * m_W.E() * std::sin(m_W.Theta());
}

void ttEventProb2Jet::setPartonTypes() const
{
   getMeasuredColl()->setParton1Type(kGluon);
   getMeasuredColl()->setParton2Type(kGluon);
}

void ttEventProb2Jet::setJetTypes()
{

  if ( getPartonColl()->getLepCharge() > 0 ) {
    m_JetType[0]=kBottom;
    m_JetType[1]=kAntiBottom;
  } else {
    m_JetType[0]=kAntiBottom;
    m_JetType[1]=kBottom;
  }

  if ( getSwappedJet0Jet1Status() ) {
    int tempType=m_JetType[0];
    m_JetType[0]=m_JetType[1];
    m_JetType[1]=tempType;
  }

}

void ttEventProb2Jet::getScale(double& scale1, double& scale2) const
{
//   scale1 = scale2 = m_massTop;
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool ttEventProb2Jet::onSwitch()
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

