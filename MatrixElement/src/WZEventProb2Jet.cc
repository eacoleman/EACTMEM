// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 10/19/2010
// The diagrams can be compared with madgraph using the processes 
// u d~ -> e+ ve u u~ (d~ u -> e+ ve u u~), d u~ -> e- ve~ u u~ (u~ d -> e- ve~ u u~)
// u d~ -> e+ ve d d~ (d~ u -> e+ ve d d~), d u~ -> e- ve~ d d~ (u~ d -> e- ve~ d d~)

#include "TAMUWW/MatrixElement/interface/WZEventProb2Jet.hh"

#include <iostream>

#include <cmath>
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
  void* wzlpuum_(double[][4], const double*, const double*, double*);// lepQ>0, u d~ -> e+ ve u u~
  void* wzlpuualtm_(double[][4], const double*, const double*, double*);// d~ u -> e+ ve u u~
  void* wzlmuum_(double[][4], const double*, const double*, double*);// lepQ<0, d u~ -> e- ve~ u u~
  void* wzlmuualtm_(double[][4], const double*, const double*, double*);// u~ d -> e- ve~ u u~
  void* wzlpddm_(double[][4], const double*, const double*, double*);// lepQ>0, u d~ -> e+ ve d d~
  void* wzlpddaltm_(double[][4], const double*, const double*, double*);// d~ u -> e+ ve d d~
  void* wzlmddm_(double[][4], const double*, const double*, double*);// lepQ<0, d u~ -> e- ve~ d d~
  void* wzlmddaltm_(double[][4], const double*, const double*, double*);// u~ d -> e- ve~ d d~
}
#endif

// ------------------------------------------------------------------
WZEventProb2Jet::WZEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet("WZ", integrator, 3, 8, tf), swapPartonMom(false), swapQuarkFlavor(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{}

// ------------------------------------------------------------------


// ------------------------------------------------------------------
void WZEventProb2Jet::changeVars(const vector<double>& parameters)
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


double WZEventProb2Jet::matrixElement() const
{
  double answer = 0;
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::zMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zWidth;

   //   MEConstants::PrintAllConstants(alphas_process);

   const PartonColl* partons = getPartonColl();


   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex zFactorU[2] = {doublecomplex(-MEConstants::gzu1, 0),
                                doublecomplex(-MEConstants::gzu2, 0)};
   doublecomplex zFactorD[2] = {doublecomplex(-MEConstants::gzd1, 0),
                                doublecomplex(-MEConstants::gzd2, 0)};
   doublecomplex factorZWW[2] = {doublecomplex(-MEConstants::gwwz, 0),
                               doublecomplex(0, 0)};

   //select the specific constants (based on whether we have a uu~ or a dd~ final state)
   doublecomplex zFactorJJ[2];
   if ( !swapQuarkFlavor ) {
     for ( int i=0; i<2; i++) {
       zFactorJJ[i]=zFactorU[i];
     }
   } else {
     for ( int i=0; i<2; i++) {
       zFactorJJ[i]=zFactorD[i];
     }
   }

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
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array1 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
        vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
        vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      }
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), 0, -1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec1, vec8, zFactorU, zMass, zWidth);
      OutputType output1 = DHELAS::iovxxx(vec6, vec5, vec9, zFactorJJ);

      Array1 vec10 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array1 vec11 = DHELAS::jioxxx(vec10, vec2, zFactorD, zMass, zWidth);
      OutputType output2 = DHELAS::iovxxx(vec6, vec5, vec11, zFactorJJ);

      Array1 vec12 = DHELAS::jioxxx(vec1, vec2, factorGWF, wMass, wWidth);
      Array1 vec13 = DHELAS::jvvxxx(vec12, vec7, factorZWW, zMass, zWidth);
      OutputType output3 = DHELAS::iovxxx(vec6, vec5, vec13, zFactorJJ);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	doublecomplex temp1 = -output1[i]-output2[i]-output3[i];
	double m1 = 9.0*std::norm(temp1);

	answer+= m1;
	//cout << "current helicity 'amplitude'" << m1 << endl;
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
     Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
     Array1 vec1;
     Array1 vec2;
     if ( !swapPartonMom ) {
       vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
       vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
     } else {
       vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
       vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
     }
     Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
     Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), 0, -1);
     
     Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
     Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
     Array1 vec9 = DHELAS::jioxxx(vec1, vec8, zFactorD, zMass, zWidth);
     OutputType output1 = DHELAS::iovxxx(vec6, vec5, vec9, zFactorJJ);

     Array1 vec10 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
     Array1 vec11 = DHELAS::jioxxx(vec10, vec2, zFactorU, zMass, zWidth);
     OutputType output2 = DHELAS::iovxxx(vec6, vec5, vec11, zFactorJJ);

     Array1 vec12 = DHELAS::jioxxx(vec1, vec2, factorGWF, wMass, wWidth);
     Array1 vec13 = DHELAS::jvvxxx(vec7, vec12, factorZWW, zMass, zWidth);
     OutputType output3 = DHELAS::iovxxx(vec6, vec5, vec13, zFactorJJ);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	doublecomplex temp1 = output1[i]+output2[i]+output3[i];
	double m1 = 9.0*std::norm(temp1);

	answer+= m1;
	//cout << "current helicity 'amplitude'" << m1 << endl;
      }

   }

   answer /= 36; //relative weight
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
   
  // Evalute the matrix element according to madgraph
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw = wMass; // to get rid of the const identifier
  double mz = zMass;
  double an = 0;
  if (partons->getLepCharge() > 0)
    //wzlpuum_(fortranArray, &zMass, &wMass, &an);
    wzlpuualtm_(fortranArray, &zMass, &wMass, &an);
  else
    //wzlmuum_(fortranArray, &zMass, &wMass, &an);
    wzlmuualtm_(fortranArray, &zMass, &wMass, &an);
  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

  return answer;
  
}

void WZEventProb2Jet::setPartonTypes() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
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

void WZEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool WZEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
    swapQuarkFlavor=false;
    swapPartonMom=false; 
    //swapPartonMom=true; //when testing alternate functions
    //swapQuarkFlavor=true; //when testing with dd~ instead of uu~
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
  case 4:
    swapQuarkFlavor=true;
    swapPartonMom=false; 
    break;
  case 5:
    swapJets(0, 1);
    break;
  case 6:
    swapPartonMom=true;
    break;
  case 7:
    swapJets(0, 1);
    break;
  default:
    return false;
  }

  return true;

}
