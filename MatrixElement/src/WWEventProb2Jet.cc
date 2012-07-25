// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 10/15/2010
// The diagrams can be compared with madgraph using the processes 
// u u~ -> e+ ve u~ d (u~ u -> e+ ve u~ d), u u~ -> e- ve~ u d~ (u~ u -> e- ve~ u d~)


#include "TAMUWW/MatrixElement/interface/WWEventProb2Jet.hh"


#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

//  C++ libraries
#include <vector>
#include <iostream>
#include <cmath>


// Set this flag to test the output madgraph would give
//#define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

// #ifdef MADGRAPH_TEST
// extern "C"
// {
//   void* wwlpm_(double[][4], const double*, const double*, double*); // lepQ>0 ( u u~ -> e+ ve u~ d )
//   void* wwlpaltm_(double[][4], const double*, const double*, double*); // for  u~ u -> e+ ve u~ d  instead
//   void* wwlmm_(double[][4], const double*, const double*, double*); // lepQ<0 ( u u~ -> e- ve~ u d~)
//   void* wwlmaltm_(double[][4], const double*, const double*, double*); // for  u~ u -> e- ve~ u d~ instead
// }
// #endif

// ------------------------------------------------------------------
WWEventProb2Jet::WWEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::WW, integrator, 3, 4, tf), 
  swapPartonMom(false),
  alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{

}

// ------------------------------------------------------------------


// ------------------------------------------------------------------
void WWEventProb2Jet::changeVars(const vector<double>& parameters)
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
double WWEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::zMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zWidth;

   //   MEConstants::PrintAllConstants(alphas_process);

   const PartonColl* partons = getPartonColl();

   double answer = 0;
   //cout << "Setting Coefficients" << endl;
   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};

   enum {vecSize = 4};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   if (partons->getLepCharge() > 0)
   {
      // Calculate the lepton only once per integration
      static Array1 vec3; //wavefunction
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
         lepE = partons->getLepton().E();
      }

      Array2 vec1;
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
        vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
        vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), 0, -1);
      }
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array2 vec8 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array4 vec9 = DHELAS::jioxxx(vec8, vec2, factorGWF, wMass, wWidth);

      OutputType output1 = DHELAS::iovxxx(vec5, vec6, vec9, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = output1[i];
	//	doublecomplex temp2 =  output1[i] + output3[i] + output5[i] + output7[i] + output8[i];

	doublecomplex m1 = ( temp1*9.0 )*std::conj(temp1)/1.0;
	//doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;
	doublecomplex m2 =0;

	answer+= (m1+m2).real();
	//	cout << "current helicity 'amplitude'" << m1.real() << " + " << m1.imag() << "i" << endl;
      }
      //setJetTypes(+1);


   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec3; //wavefunction
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
         lepE = partons->getLepton().E();
      }

      Array2 vec1;
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
        vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
        vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), 0, -1);
      }
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, +1);
      Array1 vec6 = DHELAS::ixxxxx<1>(partons->getJet(1), 0, -1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
      Array2 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array4 vec9 = DHELAS::jioxxx(vec1, vec8, factorGWF, wMass, wWidth);

      OutputType output1 = DHELAS::iovxxx(vec6, vec5, vec9, factorGWF);
      
      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = output1[i];
	//	doublecomplex temp2 =  output1[i] + output3[i] + output5[i] + output7[i] + output8[i];

	doublecomplex m1 = ( temp1*9.0 )*std::conj(temp1)/1.0;
	//doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;
	doublecomplex m2 =0;

	answer+= (m1+m2).real();
	//	cout << "current helicity 'amplitude'" << m1.real() << " + " << m1.imag() << "i" << endl;
      }
      //setJetTypes(-1);

   }


   answer /= 36; //relative weight

//   std::cerr << "New Answer: " << answer << std::endl;
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
//   //double mhiggs = m_massHiggs; // to get rid of the const identifier
//   double mw  = wMass; // to get rid of the const identifier
//   double mz = zMass;
//   double an = 0;
//   if (partons->getLepCharge() > 0)
//     //wwlpm_(fortranArray , &mz, &mw, &an);
//     wwlpaltm_(fortranArray , &mz, &mw, &an);
//   else
//     //wwlmm_(fortranArray , &mz, &mw, &an);
//     wwlmaltm_(fortranArray , &mz, &mw, &an);
//   cout << "Madgraph answer= " << an << endl;
   
//   // Exit right away
//   exit(1);  

// #endif

//    std::cout << "Jet0E=" << getPartonColl()->getJet(0).E() << std::endl;
//    std::cout << "Jet1E=" << getPartonColl()->getJet(1).E() << std::endl;



   return answer;

}

void WWEventProb2Jet::setPartonTypes() const
{

  //cout << "Setting Parton Types, Lepton Charge =" << getPartonColl()->getLepCharge() << endl;
  if ( !swapPartonMom ) {
    getMeasuredColl()->setParton1Type(kUp);
    getMeasuredColl()->setParton2Type(kAntiUp);
  } else {
    getMeasuredColl()->setParton1Type(kAntiUp);
    getMeasuredColl()->setParton2Type(kUp);
  }
}

void WWEventProb2Jet::setJetTypes()
{

  if ( getPartonColl()->getLepCharge() > 0 ) {
    m_JetType[0]=kAntiUp;
    m_JetType[1]=kDown;
  } else {
    m_JetType[0]=kUp;
    m_JetType[1]=kAntiDown;
  }

  if ( getSwappedJet0Jet1Status() ) {
    int tempType=m_JetType[0];
    m_JetType[0]=m_JetType[1];
    m_JetType[1]=tempType;
  }

}

void WWEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool WWEventProb2Jet::onSwitch()
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
