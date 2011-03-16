// Author: Ricardo Eusebi, based on the similar classes for WH 2 jets 
// Created : 4/17/2009
// For information of the diagrams see the file MatrixElement/fsrc/wh3jets_matrix.ps (lepQ>0)
// which was made by madgraph using the process ud~ > W+h > e+ve b b~ g
// and the file MatrixElement/fsrc/wh3jets2_matrix.ps (lepQ<0)
// which was made by madgraph using the process u~d > W-h > e-ve~ b~ b g
//
// This class was expanded to allow the simultaneous computation of several masses at 
// the same time, thus reducing the total CPU time by a factor of 4 or so.

//  This package libraries
#include "TAMUWW/MatrixElement/interface/WHEventProb3Jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

//  C++ libraries#include <cmath>
#include <vector>
#include <iostream>

// Set this flag to test the output madgraph would give
// #define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST
extern "C" {
  void* mywh3jet_(double[][4], double*, const double*, double*);
  void* mywh3jet2_(double[][4], double*, const double*, double*);
}
#endif

// ------------------------------------------------------------------
WHEventProb3Jet::WHEventProb3Jet(Integrator& integrator,
                                 const TransferFunction& bTF,
                                 const TransferFunction& lightTF,
				 double higgsMass) :

  // base class with 7 variables and up to 4 permutations 
  EventProb3Jet(DEFS::EP::WH, integrator, 7, 4, bTF, lightTF){

  // set the Higgs mass and width
  setHiggsMassAndWidth(higgsMass);

#ifdef MADGRAPH_TEST
  std::cout<<" WARNING, WHEventProb3Jet compiled with MADGRAPH_TEST flag!!"<<std::endl;
#endif

}//C'tor

// ------------------------------------------------------------------
// This method sets the Higgs mass and the Higgs width
void  WHEventProb3Jet::setHiggsMassAndWidth(double mHiggs) {

  // Set the mass
  m_massHiggs = mHiggs;

  // Use the theoretical Higgs width for the given mass 
  // multiplied by a factor of 1000. This is 10 times more than the 2-jet bin
  // to make the integral go faster.
  m_widthHiggs = 1000.0 * calcHiggsWidth(m_massHiggs);

  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massHiggs);

}//setHiggsMassAndWidth


// ------------------------------------------------------------------
//Make sure this returns the number of permutations on the C'tor
unsigned WHEventProb3Jet::getProbMax() const {
  return 4;
}

// ------------------------------------------------------------------
// This method takes information from Madgraph, particularly from the 
// file matrix.f corresponding for the process  u d~ -> e+ ve b b~ g 
double WHEventProb3Jet::matrixElement() const{

  typedef SimpleArray<DHELAS::HelArray, 1> Array1;
  typedef SimpleArray<DHELAS::HelArray, 2> Array2;
  typedef SimpleArray<DHELAS::HelArray, 4> Array4;

  using MEConstants::bMass;
  using MEConstants::wMass;
  using MEConstants::wWidth;

  // the returning answer
  double answer = 0;

  const PartonColl* partons = getPartonColl();

  doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
			     doublecomplex(0, 0)};
  doublecomplex gFactor[2] = {doublecomplex(MEConstants::sgg, 0),
			      doublecomplex(MEConstants::sgg, 0)};

  doublecomplex factorGWWH[1] = {doublecomplex(MEConstants::gwwh, 0)};

  doublecomplex factorGHBOT[2] = {doublecomplex(MEConstants::ghbot, 0),
				  doublecomplex(MEConstants::ghbot, 0)};

  enum {vecSize = 8};
  typedef SimpleArray<doublecomplex, vecSize> OutputType;
  static OutputType output3, output4, output5, output6;

  //separate by charge
  if (partons->getLepCharge() > 0){  // default is Charge() > 0

    // Calculate the lepton only once per integration
    static Array1 vec3;
    static double lepE = 0;
    if (lepE != partons->getLepton().E()){
      vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      lepE = partons->getLepton().E();
    }

    // Starting from 1 to match madgraph ordering
    // Keep in mind that ixxxxx<1>( , , -1) = - ixxxxx<1>( , , 1)
    Array1 vec1 = DHELAS::ixxxxx<1>(partons->getParton1()  , 0    , +1);
    Array1 vec2 = DHELAS::oxxxxx<1>(partons->getParton2()  , 0    , -1);
    //    Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton()  , 0    , -1);
    Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0    , +1);
    Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0)    , bMass, +1);
    Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1)    , bMass, -1);
    Array2 vec7 = DHELAS::vxxxxx<2>(partons->getJet(2)    , 0    , +1);

    Array1 vec8 = DHELAS::jioxxx(vec3, vec4, factor, wMass, wWidth);

    // Diagrams number 1,2 and 7 involve the ggh vertex, and are therefore suppresed

    //Diagram number 3 (ISR really)
    Array2 vec15 = DHELAS::fvixxx(vec1, vec7, gFactor, 0, 0);
    Array2 vec16 = DHELAS::jioxxx(vec15, vec2, factor, wMass, wWidth);
    Array2 vec17 = DHELAS::hvvxxx(vec16, vec8, factorGWWH, m_massHiggs, m_widthHiggs);
    output3 = DHELAS::iosxxx(vec6, vec5, vec17, factorGHBOT);

    // Diagram number 4 (ISR really)
    Array2 vec18 = DHELAS::fvoxxx(vec2, vec7 , gFactor, 0,0);
    Array2 vec19 = DHELAS::jioxxx(vec1, vec18, factor, wMass, wWidth);
    Array2 vec20 = DHELAS::hvvxxx(vec19, vec8, factorGWWH,  m_massHiggs,m_widthHiggs);
    output4 = DHELAS::iosxxx(vec6, vec5, vec20, factorGHBOT);

    // Diagram number 5 (FSR really)
    Array1 vec21  = DHELAS::jioxxx(vec1, vec2, factor ,wMass ,wWidth);
    Array1 vec22  = DHELAS::hvvxxx(vec21,vec8, factorGWWH,  m_massHiggs, m_widthHiggs);
    Array4 vec23  = DHELAS::fvoxxx(vec5, vec7, gFactor, bMass , 0);
    output5 = DHELAS::iosxxx(vec6, vec23, vec22, factorGHBOT);

    // Diagram number 6 (FSR really)
    Array2 vec24  = DHELAS::fsoxxx(vec5, vec22, factorGHBOT ,bMass, 0);
    output6 = DHELAS::iovxxx(vec6, vec24, vec7, gFactor);      
    
    answer = 0;
 
    // Do the sum, separating the different colors diagrams
    for (unsigned i = 0; i < vecSize; ++i) {
      
      // Lump together diagrams 1,2,3 & 4 and 5,6 & 7.
      doublecomplex temp  = -output3[i] - output4[i];
      doublecomplex temp2 = -output5[i] - output6[i];         
      
      // Multiply by variable CF(i,i)
      answer += (std::norm(temp) + std::norm(temp2))* 12;
    }//for
    
      // divide by variable IDEN
      answer /= 36;
   
  }else{

     
    // Diagrams number 1,2 and 7 involve the ggh vertex, and are therefore suppresed
    // This is basically as the case of lepton.Charge >0, except that:
    // - swap Quark1 and Quark2, because antiquarks always come from the antiproton
    // - swap all the oxxxxx's with ixxxxx's
    // - swap all the initial helicities, except vector bosons
    // - in all the jioxxx's and iosxxx swap the first two parameters
    // - swap all the fvixxx's with fvoxxx's, keep all the params identical 
    // - swap all the fsoxxx's with fsixxx's, keep all the params identical 

    // Calculate the lepton only once per integration
    static Array1 vec3;
    static double lepE = 0;
    if (lepE != partons->getLepton().E()){
      vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, +1);
      lepE = partons->getLepton().E();
    }

    Array1 vec1 = DHELAS::oxxxxx<1>(partons->getParton2()  , 0    , -1);
    Array1 vec2 = DHELAS::ixxxxx<1>(partons->getParton1()  , 0    , +1);
    //   Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton()  , 0    , +1);
    Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0    , -1);
    Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(0)    , bMass, -1);
    Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1)    , bMass, +1);
    Array2 vec7 = DHELAS::vxxxxx<2>(partons->getJet(2)    , 0    , +1); 

    Array1 vec8 = DHELAS::jioxxx(vec4, vec3, factor, wMass, wWidth);

    //Diagram number 3 (ISR really)
    Array2 vec15 = DHELAS::fvoxxx(vec1, vec7, gFactor, 0, 0);
    Array2 vec16 = DHELAS::jioxxx(vec2, vec15, factor, wMass, wWidth);
    Array2 vec17 = DHELAS::hvvxxx(vec8, vec16, factorGWWH, m_massHiggs, m_widthHiggs);
    output3 = DHELAS::iosxxx(vec5, vec6, vec17, factorGHBOT);

    // Diagram number 4 (ISR really)
    Array2 vec18 = DHELAS::fvixxx(vec2, vec7 , gFactor, 0,0);
    Array2 vec19 = DHELAS::jioxxx(vec18, vec1, factor, wMass, wWidth);
    Array2 vec20 = DHELAS::hvvxxx(vec8, vec19, factorGWWH,  m_massHiggs, m_widthHiggs);
    output4 = DHELAS::iosxxx(vec5, vec6, vec20, factorGHBOT);
     
    // Diagram number 5 (FSR really)
    Array1 vec21  = DHELAS::jioxxx(vec2, vec1, factor ,wMass ,wWidth);
    Array1 vec22  = DHELAS::hvvxxx(vec8,vec21, factorGWWH, m_massHiggs, m_widthHiggs);
    Array4 vec23  = DHELAS::fvixxx(vec5, vec7, gFactor, bMass , 0);
    output5 = DHELAS::iosxxx(vec23, vec6, vec22, factorGHBOT);

    // Diagram number 6 (FSR really)
    Array2 vec24  = DHELAS::fsixxx(vec5, vec22, factorGHBOT ,bMass, 0);
    output6 = DHELAS::iovxxx(vec24, vec6, vec7, gFactor);

    answer = 0;

    // Do the sum, separating the different colors diagrams
    for (unsigned i = 0; i < vecSize; ++i) {
      
      // Lump together diagrams 1,2,3 & 4 and 5,6 & 7.
      doublecomplex temp  = -output3[i] - output4[i];
      doublecomplex temp2 = -output5[i] - output6[i];         
      
      // Multiply by variable CF(i,i)
	answer += (std::norm(temp) + std::norm(temp2))* 12;
	
    }//for
    
    // divide by variable IDEN
    answer /= 36;
    
  }// if charge is neg or pos
   

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
  double mhiggs = m_higgsMass; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;
  if (partons->getLepCharge() > 0)
    mywh3jet_(fortranArray, &mhiggs, &mw, &an);
  else
    mywh3jet2_(fortranArray, &mhiggs, &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

  return answer;

}//matrixElement


// ------------------------------------------------------------------
void WHEventProb3Jet::setPartonTypes() const{

   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kDown);
   }
   else
   {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kUp);
   }

}

// ------------------------------------------------------------------
void WHEventProb3Jet::getScale(double& scale1, double& scale2) const {

  double scale = getPartonColl()->sHat();
  if (scale < 0)
    scale1 = scale2 = 0;
  else
    scale1 = scale2 = std::sqrt(scale);
}

// ------------------------------------------------------------------
void WHEventProb3Jet::setupIntegral() {
  // ONLY if running a mass scan un-comment the line below
  // setHiggsMassAndWidth(getMeasuredColl()->getHiggsMass());

  // just report it to screen
  cout << "\tHiggs mass: " << m_massHiggs
       <<"  width: "       << m_widthHiggs << endl;

}//setup Integral



