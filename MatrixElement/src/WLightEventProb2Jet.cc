// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 07/20/2010
// The diagrams can be compared with madgraph using the processes 
// g u -> e+ ve d g for W+, g d -> e- ve~ u g for W-
// $Id$

//  This package libraries
#include "TAMUWW/MatrixElement/interface/WLightEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

//  C++ libraries
#include <vector>
#include <iostream>

// Set this flag to test the output madgraph would give
#define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST
extern "C" {
  void* wpjjm_ (double[][4], const double*, double*); // lepQ>0 
  void* wpjjaltm_ (double[][4], const double*, double*); // for the u g -> e+ ve d g instead of g u -> e+ ve d g (lepQ>0) 
  void* wmjjm_ (double[][4], const double*, double*); // lepQ<0 
  void* wmjjaltm_ (double[][4], const double*, double*); // for the d g -> e- ve~ u g instead of g d -> e- ve~ u g (lepQ<0) 
  // to use the altm_ functions for just one iteration initialize the swapPartonMom(true) in WLightEventProb2Jet
}
#endif

// ------------------------------------------------------------------
WLightEventProb2Jet::WLightEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet("WLight2Jet", integrator, 3, 4, tf), swapPartonMom(false)
{

}

// ------------------------------------------------------------------


// ------------------------------------------------------------------
void WLightEventProb2Jet::changeVars(const vector<double>& parameters)
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
double WLightEventProb2Jet::matrixElement() const
{

   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::zMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zWidth;


   MEConstants::PrintAllConstants();

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGG[2]  = {doublecomplex(MEConstants::gg, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::sgg, 0),
				   doublecomplex(0, 0)};

   // This should equal the number of Feynman diagrams.
   enum {vecSize = 4};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;
   //OutputType output[5000];


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

      ///See if we've switched the quark and the gluon
      Array2 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::vxxxxx<2>(partons->getQuark1(), 0, -1);
        vec2 = DHELAS::ixxxxx<1>(partons->getQuark2(), 0, +1);
      } else {
	vec1 = DHELAS::vxxxxx<2>(partons->getQuark2(), 0, -1);
        vec2 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, +1);
      }

//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);

      OutputType output1 = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);
      
      Array2 vec10 = DHELAS::fvoxxx(vec5,vec1,factorGG,0,0);
      Array2 vec11 = DHELAS::fvoxxx(vec10,vec7,factorGWF,0,0);

      OutputType output2 = DHELAS::iovxxx(vec2,vec11,vec6,factorGG);

      Array2 vec12 = DHELAS::fvixxx(vec8,vec1,factorGG,0,0);

      OutputType output3 = DHELAS::iovxxx(vec12,vec5,vec6,factorGG);
      OutputType output4 = DHELAS::iovxxx(vec8,vec10,vec6,factorGG);

      Array1 vec13 = DHELAS::fvoxxx(vec5,vec7,factorGWF,0,0);
      Array1 vec14 = DHELAS::jioxxx(vec2,vec13,factorGG,0,0);

      OutputType output5 = DHELAS::vvvxxx(vec6,vec1,vec14,factorSGG);

      Array2 vec15 = DHELAS::fvoxxx(vec13,vec1,factorGG,0,0);

      OutputType output6 = DHELAS::iovxxx(vec2,vec15,vec6,factorGG);

      Array2 vec16 = DHELAS::fvixxx(vec2,vec1,factorGG,0,0);
      Array2 vec17 = DHELAS::fvixxx(vec16,vec7,factorGWF,0,0);

      OutputType output7 = DHELAS::iovxxx(vec17,vec5,vec6,factorGG);
      OutputType output8 = DHELAS::iovxxx(vec16,vec13,vec6,factorGG);          

      std::swap(output1[1], output1[2]);
      std::swap(output5[1], output5[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = -output1[i] + output2[i] + output4[i] - output5[i] + output6[i];
 	doublecomplex temp2 =  output1[i] + output3[i] + output5[i] + output7[i] + output8[i];

//      	doublecomplex temp1 = output1[i];
//   	doublecomplex temp2 = 0;
        cout << "output1=" << output1[i] << endl;
	//<< " output2=" << output2[i] << endl; 
	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
	cout << "current helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;
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

      ///See if we've switched the quark and the gluon
      Array2 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::vxxxxx<2>(partons->getQuark1(), 0, -1);
        vec2 = DHELAS::ixxxxx<1>(partons->getQuark2(), 0, +1);
      } else {
	vec1 = DHELAS::vxxxxx<2>(partons->getQuark2(), 0, -1);
        vec2 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, +1);
      }

//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);

      OutputType output1 = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);
      
      Array2 vec10 = DHELAS::fvoxxx(vec5,vec1,factorGG,0,0);
      Array2 vec11 = DHELAS::fvoxxx(vec10,vec7,factorGWF,0,0);

      OutputType output2 = DHELAS::iovxxx(vec2,vec11,vec6,factorGG);

      Array2 vec12 = DHELAS::fvixxx(vec8,vec1,factorGG,0,0);

      OutputType output3 = DHELAS::iovxxx(vec12,vec5,vec6,factorGG);
      OutputType output4 = DHELAS::iovxxx(vec8,vec10,vec6,factorGG);

      Array1 vec13 = DHELAS::fvoxxx(vec5,vec7,factorGWF,0,0);
      Array1 vec14 = DHELAS::jioxxx(vec2,vec13,factorGG,0,0);

      OutputType output5 = DHELAS::vvvxxx(vec6,vec1,vec14,factorSGG);

      Array2 vec15 = DHELAS::fvoxxx(vec13,vec1,factorGG,0,0);

      OutputType output6 = DHELAS::iovxxx(vec2,vec15,vec6,factorGG);

      Array2 vec16 = DHELAS::fvixxx(vec2,vec1,factorGG,0,0);
      Array2 vec17 = DHELAS::fvixxx(vec16,vec7,factorGWF,0,0);

      OutputType output7 = DHELAS::iovxxx(vec17,vec5,vec6,factorGG);
      OutputType output8 = DHELAS::iovxxx(vec16,vec13,vec6,factorGG);     

      
      std::swap(output1[1], output1[2]);
      std::swap(output5[1], output5[2]);


      for (unsigned i = 0; i < vecSize; ++i)
      {
	doublecomplex temp1 = (-output1[i] + output2[i] + output4[i] - output5[i] + output6[i])*-1.0;
	doublecomplex temp2 = ( output1[i] + output3[i] + output5[i] + output7[i] + output8[i])*-1.0;
         
	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
	cout << "current helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;
      }
    

   }// if charge is neg or pos

   // divide by variable IDEN
   answer /= 96;



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
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;
  if (partons->getLepCharge() > 0)
    wpjjm_(fortranArray , &mw, &an);
  //wpjjaltm_(fortranArray , &mw, &an);
  else
    wmjjm_(fortranArray , &mw, &an);
  // wmjjaltm_(fortranArray , &mw, &an);
  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

  return answer;

}

// ------------------------------------------------------------------
void WLightEventProb2Jet::setQuarkIDs() const
{   
  if ( getPartonColl()->getLepCharge() > 0 ) {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setProtonType(kGluon);
      getMeasuredColl()->setAntiprotonType(kUp);
    } else {
      getMeasuredColl()->setProtonType(kUp);
      getMeasuredColl()->setAntiprotonType(kGluon);
    }
    
  } else {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setProtonType(kGluon);
      getMeasuredColl()->setAntiprotonType(kDown);
    } else {
      getMeasuredColl()->setProtonType(kDown);
      getMeasuredColl()->setAntiprotonType(kGluon);
    }
  }
}

// ------------------------------------------------------------------
void WLightEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

// ------------------------------------------------------------------


bool WLightEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
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
