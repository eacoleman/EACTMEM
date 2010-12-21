// Author: Ricardo Eusebi, Texas A&M University.
// Created : 12/3/2009
// The diagrams can be compared with madgraph using the processes 
// pp > h > l-vl~jj (lepQ<0) and  pp > h > l+vl jj (lepQ>0)

//  This package libraries
#include "TAMUWW/MatrixElement/interface/HWWEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

//  C++ libraries
#include <vector>
#include <iostream>

// Set this flag to test the output madgraph would give
// #define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST
extern "C" {
  void* myhww2j_ (double[][4], double*, const double*, double*); // lepQ>0 
  void* myhww2j2_(double[][4], double*, const double*, double*); // lepQ<0
}
#endif

// ------------------------------------------------------------------
HWWEventProb2Jet::HWWEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf, 
				   double higgsMass) :
   EventProb2Jet("HWW", integrator, 3, 2, tf)
{
  setHiggsMassAndWidth(higgsMass);
}

// ------------------------------------------------------------------
// This method sets the Higgs mass and the Higgs width
void  HWWEventProb2Jet::setHiggsMassAndWidth(double mHiggs) {

  // Set the mass
  m_massHiggs = mHiggs;

  // Use the theoretical Higgs width for the given mass 
  // multiplied by a factor of 100
  m_widthHiggs = 100.0 * calcHiggsWidth(mHiggs);



}//setHiggsMassAndWidth

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

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGWWH[2]  = {doublecomplex(MEConstants::gwwh, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGH[2]    = {doublecomplex(MEConstants::gh, 0),
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
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1);

      Array4 vec7 = DHELAS::hvvhxx(vec1, vec2, factorGH, m_massHiggs, m_widthHiggs);
      Array1 vec8 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array4 vec9 = DHELAS::jvsxxx(vec8, vec7, factorGWWH, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec5, vec6, vec9, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	 // Multiply by variable CF(i,i)
         answer += std::norm(-output[i]) * 6;
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
	 // Multiply by variable CF(i,i)
         answer += std::norm(-output[i]) * 6;
      }

   }// if charge is neg or pos

   // divide by variable IDEN
   answer /= 256;

   // For some reason madgraph puts a 2 * 2 extra, in the line
   //JAMP(   1) =      2*( +AMP(   1))
   // (which later get squared resulting in an extra factor of 4.
   // I do the same, just in case.
   answer *= 4;

#ifdef MADGRAPH_TEST
  // -----------------------------------------------------------
  // This code reports our answer as well as the madgraph answer
  // -----------------------------------------------------------
   
  // Report our answers
  cout<<" My answer= "<<answer<<endl;

  // Make a fortran array, format which is needed for the fortran calls
  double fortranArray[6][4];
  makeFortranArray(fortranArray);

  // Evalute the matrix element according to madgraph
  double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;
  if (partons->getLepCharge() > 0)
    myhww2j_(fortranArray, &mhiggs, &mw, &an);
  else
    myhww2j2_(fortranArray, &mhiggs, &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

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
  // ONLY if running a mass scan un-comment the line below
  // setHiggsMassAndWidth(getMeasuredColl()->getHiggsMass());

  // just report it to screen
  cout << "\tHiggs mass: " << m_massHiggs
       <<"  width: "       << m_widthHiggs << endl;

}//setup Integral

