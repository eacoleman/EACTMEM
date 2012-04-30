//////////////////////////////////////////////////////////////////////
//// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
//// The diagrams can be compared with madgraph using the processes
//// GG final state (suppressed by x~5 vs the leading diagrams): d~ u -> e+ ve g g (u d~ -> e+ ve g g) for W+, d u~ -> e- ve~ g g (u~ d -> e- ve~ g g) for W-
//////////////////////////////////////////////////////////////////////
#include "TAMUWW/MatrixElement/interface/WggEventProb2Jet.hh"

#include <iostream>

#include <cmath>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

// Set this flag to test the output madgraph would give
//#define MADGRAPH_TEST
using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST
extern "C"
{
  void* wpggm_(double[][4], const double*, double*); // lepQ>0, d~ u -> e+ ve g g
  void* wpggaltm_ (double[][4], const double*, double*); // lepQ>0, u d~ -> e+ ve g g instead
  void* wmggm_(double[][4], const double*, double*); // lepQ<0, d u~ -> e- ve~ g g
  void* wmggaltm_ (double[][4], const double*, double*); // lepQ<0, u~ d -> e- ve~ g g
  // to use the altm_ functions for just one iteration initialize the swapPartonMom(true) in WggEventProb2Jet
}
#endif

// ------------------------------------------------------------------
WggEventProb2Jet::WggEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::Wgg, integrator, 3, 4, tf), swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
}
// ------------------------------------------------------------------

void WggEventProb2Jet::changeVars(const vector<double>& parameters)
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

double WggEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGG[2]  = {doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0),
				   doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0)};
   doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::GetAdjusted_sgg(alphas_process), 0),
   			   doublecomplex(0, 0)};

   // This should equal the number of helicity combinations.
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

      ///See if we've switched the quarks
      Array1 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
	vec2 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
      } else {
	vec1 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
	vec2 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
      }
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
      Array2 vec9 = DHELAS::fvixxx(vec8, vec5, factorGG, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec9, vec1, vec6, factorGG);

      Array2 vec10 = DHELAS::fvoxxx(vec1, vec5, factorGG, 0, 0);
      Array2 vec11 = DHELAS::fvoxxx(vec10, vec7, factorGWF, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec2, vec11, vec6, factorGG);

      Array1 vec12 = DHELAS::jioxxx(vec8, vec1, factorGG, 0, 0);
      OutputType output3 = DHELAS::vvvxxx(vec6, vec5, vec12, factorSGG);

      OutputType output4 = DHELAS::iovxxx(vec8, vec10, vec6, factorGG);

      Array2 vec13 = DHELAS::fvixxx(vec2, vec5, factorGG, 0, 0);
      Array2 vec14 = DHELAS::fvixxx(vec13, vec7, factorGWF, 0, 0);
      OutputType output5 = DHELAS::iovxxx(vec14, vec1, vec6, factorGG);

      Array1 vec15 = DHELAS::fvoxxx(vec1, vec7, factorGWF, 0, 0);
      Array2 vec16 = DHELAS::fvoxxx(vec15, vec5, factorGG, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec2, vec16, vec6, factorGG);

      Array1 vec17 = DHELAS::jioxxx(vec2, vec15, factorGG, 0, 0);
      OutputType output7 = DHELAS::vvvxxx(vec6, vec5, vec17, factorSGG);

      OutputType output8 = DHELAS::iovxxx(vec13, vec15, vec6, factorGG);

      std::swap(output3[1], output3[2]);
      std::swap(output7[1], output7[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = -output1[i] - output3[i] - output5[i] - output7[i] - output8[i];
	doublecomplex temp2 = -output2[i] + output3[i] - output4[i] - output6[i] + output7[i];
	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
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
      Array1 vec1;
      Array1 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, +1);
	vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, +1);
	vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      }
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array2 vec9 = DHELAS::fvoxxx(vec8, vec5, factorGG, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec1, vec9, vec6, factorGG);

      Array2 vec10 = DHELAS::fvixxx(vec1, vec5, factorGG, 0, 0);
      Array2 vec11 = DHELAS::fvixxx(vec10, vec7, factorGWF, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec11, vec2, vec6, factorGG);

      Array1 vec12 = DHELAS::jioxxx(vec1, vec8, factorGG, 0, 0);
      OutputType output3 = DHELAS::vvvxxx(vec6, vec5, vec12, factorSGG);

      OutputType output4 = DHELAS::iovxxx(vec10, vec8, vec6, factorGG);

      Array2 vec13 = DHELAS::fvoxxx(vec2, vec5, factorGG, 0, 0);
      Array2 vec14 = DHELAS::fvoxxx(vec13, vec7, factorGWF, 0, 0);
      OutputType output5 = DHELAS::iovxxx(vec1, vec14, vec6, factorGG);

      Array1 vec15 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
      Array2 vec16 = DHELAS::fvixxx(vec15, vec5, factorGG, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec16, vec2, vec6, factorGG);

      Array1 vec17 = DHELAS::jioxxx(vec15, vec2, factorGG, 0, 0);
      OutputType output7 = DHELAS::vvvxxx(vec6, vec5, vec17, factorSGG);

      OutputType output8 = DHELAS::iovxxx(vec15, vec13, vec6, factorGG);

      std::swap(output3[1], output3[2]);
      std::swap(output7[1], output7[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = -output1[i] + output3[i] - output5[i] + output7[i] - output8[i];
	doublecomplex temp2 = -output2[i] - output3[i] - output4[i] - output6[i] - output7[i];
	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
      }

   }


   answer /= 72;

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
  if (partons->getLepCharge() > 0) {
    wpggm_(fortranArray , &mw, &an);
    //wpggaltm_(fortranArray , &mw, &an);
  } else {
    wmggm_(fortranArray , &mw, &an);
    //wmggaltm_(fortranArray , &mw, &an);
  }
  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif


   return answer;
// //   const PartonColl* partons = getPartonColl();
// //
//    double fortranArray[6][4];
//    makeFortranArray(fortranArray);
// //
// //   double topMass = partons->getTopMass();
// // KLUDGE
// //   const double wMass = partons->getTopMass();
// //   const double wMass = MEConstants::wMass;
// //
// //   double answer;
// //
// ////   for (int i = 0; i < 7; ++i)
// ////      for (int j = 0; j < 4; ++j)
// ////      {
// ////         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
// ////      }
// ////   exit+(1);
// ////
// ////   std::cerr << "Top mass: " << topMass << std::endl;
// ////   std::cerr << "W mass: " << wMass << std::endl;
// //   
// ////   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
// //
//    if (partons->getLepCharge() > 0)
//       mywgg_(fortranArray, &wMass, &answer);
//    else
//       mywgg2_(fortranArray, &wMass, &answer);

// //   std::cerr << "Old answer: " << answer << std::endl;
// //   exit(1);
//   return answer;
}

// ------------------------------------------------------------------
void WggEventProb2Jet::setPartonTypes() const
{
   if (getPartonColl()->getLepCharge() > 0)
   {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kAntiDown);
      getMeasuredColl()->setParton2Type(kUp);
    } else {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kAntiDown);
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

// ------------------------------------------------------------------
void WggEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

// ------------------------------------------------------------------


bool WggEventProb2Jet::onSwitch()
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
