//////////////////////////////////////////////////////////////////////
//// Author: Ilya Osipenkov Texas A&M University.
//// The diagrams can be compared with madgraph using the processes:
//// g b~ -> b~ u~ d e+ ve (b~ g -> b~ u~ d e+ ve), g b -> b e+ ve u~ d (b g -> b e+ ve u~ d) for W+; g b -> b u d~ e- ve~ (b g -> b u d~ e- ve~ ), g b~ -> b~ e- ve~ u d~ (b~ g -> b~ e- ve~ u d~ ) for W-
//// Actually contains two sets of matrix elements: one for W coming from the Top decaying hadronically (decTWjj=true) and the other corresponding to the W decaying leptonically (decTWjj=false).
//////////////////////////////////////////////////////////////////////
#include "TAMUWW/MatrixElement/interface/STopTWEventProb2Jet.hh"

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
using std::cout;
using std::endl;
using std::vector;


#ifdef MADGRAPH_TEST
extern "C"
{
  void* wlptwjjm_(double[][4], const double*, double*); //lepQ>0, g b~ -> b~ u~ d e+ ve
  void* wlptwjjaltm_ (double[][4], const double*, double*); //lepQ>0, b~ g -> b~ u~ d e+ ve instead
  void* wlmtwjjm_(double[][4], const double*, double*); //lepQ<0, g b -> b u d~ e- ve~
  void* wlmtwjjaltm_ (double[][4], const double*, double*); //lepQ<0, b g -> b u d~ e- ve~ instead
  void* wjjtwlpm_(double[][4], const double*, double*); //lepQ>0, g b -> b e+ ve u~ d
  void* wjjtwlpaltm_ (double[][4], const double*, double*); //lepQ>0, b g -> b e+ ve u~ d instead
  void* wjjtwlmm_(double[][4], const double*, double*); //lepQ<0, g b~ -> b~ e- ve~ u d~
  void* wjjtwlmaltm_ (double[][4], const double*, double*); //lepQ<0, b~ g -> b~ e- ve~ u d~ instead
  // to use the altm_ functions for just one iteration set swapPartonMom=true 
}
#endif

// ------------------------------------------------------------------
STopTWEventProb2Jet::STopTWEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf) :
  EventProb2Jet(DEFS::EP::STopTW, integrator, 6, 8, lighttf),
  decTWjj(false),swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);
}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void STopTWEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;
  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);
  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------

void STopTWEventProb2Jet::changeVars(const vector<double>& parameters)
{
   using AuxFunctions::Math::square;

  TLorentzVector& jet1 = getPartonColl()->getJet(0);
  TLorentzVector& jet2 = getPartonColl()->getJet(1);
  
  jet1.SetRho(parameters[1]);
  jet1.SetE(parameters[1]);
  jet2.SetRho(parameters[2]);
  jet2.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[2])));
  getPartonColl()->getNeutrino().SetPz(parameters[0]);

  double r = parameters[3];
  double phi = parameters[4];
  double theta = parameters[5];
  
  double x = r * cos(phi) * sin(theta);
  double y = r * sin(phi) * sin(theta);
  double z = r * cos(theta);

  //integrate over the b-quark 4 momentum
  m_b.SetXYZM(x, y, z, MEConstants::bMass);
}

void STopTWEventProb2Jet::met()
{
  using AuxFunctions::Math::square;

   getPartonColl()->setMet();
   TLorentzVector& vec = getPartonColl()->getNeutrino();
   double px = vec.Px() - m_b.Px();
   double py = vec.Py() - m_b.Py();
   double energy = std::sqrt(square(px) + square(py)
                             + square(vec.Pz()));
   vec.SetPxPyPzE(px, py, vec.Pz(), energy);
}

void STopTWEventProb2Jet::getTotalLV(TLorentzVector& vec) const
{
   EventProb::getTotalLV(vec);
   vec += m_b;

}

// // ------------------------------------------------------------------
// void STopTWEventProb2Jet::setDynamicBounds()
// ///Add this function when using more than one TF
// {
//    const float lowPercent = .01;
//    const float highPercent = .02;
//    double lower, upper;
//    getDefaultTF().getBounds(getMeasuredColl()->getFullJet(0), lowPercent,
//                       highPercent, lower, upper);
//    std::cout << "\tSetting jet 1 bounds from " << lower << " to " << upper
//              << std::endl;
//    setBounds(1, lower, upper);
//    m_bTF.getBounds(getMeasuredColl()->getFullJet(1), lowPercent,
//                        highPercent, lower, upper);
//    std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
//              << std::endl;
//    setBounds(2, lower, upper);
// }

// ------------------------------------------------------------------

double STopTWEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;
   typedef SimpleArray<DHELAS::HelArray, 8> Array8;
   typedef SimpleArray<DHELAS::HelArray, 16> Array16;
   typedef SimpleArray<DHELAS::HelArray, 32> Array32;

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

   enum {vecSize = 32};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   if ( decTWjj ) {
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

	 ///Initial State Partons
	 Array2 vec1;
	 Array2 vec2;
	 if ( !swapPartonMom ) {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), bMass, +1);
	 } else {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), bMass, +1);
	 }
	 ///Final State Neutrino & Partons
	 Array2 vec3 = DHELAS::oxxxxx<2>(m_b, bMass, +1);
	 Array1 vec5 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
	 Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(0), 0, -1);
	 Array2 vec7 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);

	 Array1 vec8 = DHELAS::jioxxx(vec4, vec5, factorGWF, wMass, wWidth);
	 Array2 vec9 = DHELAS::fvoxxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);
	 Array4 vec10 = DHELAS::fvoxxx(vec9, vec1, factorGG, m_massTop, m_widthTop);
	 Array8 vec11 = DHELAS::jioxxx(vec2, vec10, factorGWF, wMass, wWidth);
	 OutputType output1 = DHELAS::iovxxx(vec6, vec7, vec11, factorGWF);

	 Array4 vec12 = DHELAS::fvixxx(vec2, vec1, factorGG, bMass, 0);
	 Array8 vec13 = DHELAS::jioxxx(vec12, vec9, factorGWF, wMass, wWidth);
	 OutputType output2 = DHELAS::iovxxx(vec6, vec7, vec13, factorGWF);

	 std::swap(output2[17], output2[18]);

	 for (unsigned i = 0; i < vecSize; ++i) {
	   //cout << "i=" << i << " output1=" << output1[i] << " output2=" << output2[i] << endl;
	   doublecomplex temp1 = output1[i] + output2[i];
	   double m1 = 12.0*std::norm(temp1);
	   answer+= m1;
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

	 ///Initial State Partons
	 Array2 vec1;
	 Array2 vec2;
	 if ( !swapPartonMom ) {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
	   vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), bMass, -1);
	 } else {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
	   vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), bMass, -1);
	 }
	 ///Final State Neutrino & Partons
	 Array2 vec3 = DHELAS::ixxxxx<2>(m_b, bMass, -1);
	 Array1 vec5 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
	 Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec7 = DHELAS::ixxxxx<2>(partons->getJet(1), 0, -1);


	 Array1 vec8 = DHELAS::jioxxx(vec5, vec4, factorGWF, wMass, wWidth);
	 Array2 vec9 = DHELAS::fvixxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);
	 Array4 vec10 = DHELAS::fvixxx(vec9, vec1, factorGG, m_massTop, m_widthTop);
	 Array8 vec11 = DHELAS::jioxxx(vec10, vec2, factorGWF, wMass, wWidth);
	 OutputType output1 = DHELAS::iovxxx(vec7, vec6, vec11, factorGWF);

	 Array4 vec12 = DHELAS::fvoxxx(vec2, vec1, factorGG, bMass, 0);
	 Array8 vec13 = DHELAS::jioxxx(vec9, vec12, factorGWF, wMass, wWidth);
	 OutputType output2 = DHELAS::iovxxx(vec7, vec6, vec13, factorGWF);

	 for (unsigned i = 0; i < vecSize; ++i) {
	   //cout << "i=" << i << " output1=" << output1[i] << " output2=" << output2[i] << endl;
	   doublecomplex temp1 = +output1[i] + output2[i];
	   double m1 = 12.0*std::norm(temp1);
	   answer+= m1;
	 }
       }
     
   } else {
     if (partons->getLepCharge() > 0)
       {
	 // Calculate the lepton only once per integration
	 static Array1 vec6;
	 static double lepE = 0;
	 if (lepE != partons->getLepton().E())
	   {
	     vec6 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
	     lepE = partons->getLepton().E();
	   }

	 ///Initial State Partons
	 Array2 vec1;
	 Array2 vec2;
	 if ( !swapPartonMom ) {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
	   vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), bMass, -1);
	 } else {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
	   vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), bMass, -1);
	 }
	 ///Final State Neutrino & Partons
	 Array2 vec3 = DHELAS::ixxxxx<2>(m_b, bMass, -1);
	 Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), 0, -1);
	 Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);
	 Array1 vec7 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);

	 Array4 vec8 = DHELAS::jioxxx(vec4, vec5, factorGWF, wMass, wWidth);
	 Array8 vec9 = DHELAS::fvixxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);
	 Array16 vec10 = DHELAS::fvixxx(vec9, vec1, factorGG, m_massTop, m_widthTop);
	 Array32 vec11 = DHELAS::jioxxx(vec10, vec2, factorGWF, wMass, wWidth);
	 OutputType output1 = DHELAS::iovxxx(vec6, vec7, vec11, factorGWF);

	 Array4 vec12 = DHELAS::fvoxxx(vec2, vec1, factorGG, bMass, 0);
	 Array32 vec13 = DHELAS::jioxxx(vec9, vec12, factorGWF, wMass, wWidth);
	 OutputType output2 = DHELAS::iovxxx(vec6, vec7, vec13, factorGWF);


	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = -output1[i] - output2[i];
	   double m1 = 12.0*std::norm(temp1);
	   answer+= m1;
	 }

       }
     else
       {
	 // Calculate the lepton only once per integration
	 static Array1 vec6;
	 static double lepE = 0;
	 if (lepE != partons->getLepton().E())
	   {
	     vec6 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, +1);
	     lepE = partons->getLepton().E();
	   }

	 ///Initial State Partons
	 Array2 vec1;
	 Array2 vec2;
	 if ( !swapPartonMom ) {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), bMass, +1);
	 } else {
	   vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), bMass, +1);
	 }
	 ///Final State Neutrino & Partons
	 Array2 vec3 = DHELAS::oxxxxx<2>(m_b, bMass, +1);
	 Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(1), 0, -1);
	 Array1 vec7 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);

	 Array4 vec8 = DHELAS::jioxxx(vec5, vec4, factorGWF, wMass, wWidth);
	 Array8 vec9 = DHELAS::fvoxxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);
	 Array16 vec10 = DHELAS::fvoxxx(vec9, vec1, factorGG, m_massTop, m_widthTop);
	 Array32 vec11 = DHELAS::jioxxx(vec2, vec10, factorGWF, wMass, wWidth);
	 OutputType output1 = DHELAS::iovxxx(vec7, vec6, vec11, factorGWF);

	 Array4 vec12 = DHELAS::fvixxx(vec2, vec1, factorGG, bMass, 0);
	 Array32 vec13 = DHELAS::jioxxx(vec12, vec9, factorGWF, wMass, wWidth);
	 OutputType output2 = DHELAS::iovxxx(vec7, vec6, vec13, factorGWF);

	 std::swap(output2[2], output2[4]);
	 std::swap(output2[6], output2[12]);

	 for (unsigned i = 0; i < vecSize; ++i) {
	   //	cout << "i=" << i << " output1=" << output1[i] << " output2=" << output2[i] << endl;
	   doublecomplex temp1 = +output1[i] + output2[i];
	   double m1 = 12.0*std::norm(temp1);
	   answer+= m1;
	 }
       }
   }

   answer /= 96;

#ifdef MADGRAPH_TEST
  // -----------------------------------------------------------
  // This code reports our answer as well as the madgraph answer
  // -----------------------------------------------------------
   
  // Report our answers
   std::cout<<" My answer= "<<answer<< std::endl;

  // Make a fortran array, format which is needed for the fortran calls
  double fortranArray[7][4];

  // Evalute the matrix element according to madgraph
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double an = 0;

  if ( decTWjj ) {
    makeFortranArray_blvqq(fortranArray,m_b);
    if (partons->getLepCharge() > 0) {
      if ( !swapPartonMom ) {
	wjjtwlpm_(fortranArray , &mw, &an);
      } else {
	wjjtwlpaltm_(fortranArray , &mw, &an);
      }
    } else {
      if ( !swapPartonMom ) {
	wjjtwlmm_(fortranArray , &mw, &an);
      } else {
	wjjtwlmaltm_(fortranArray , &mw, &an);
      }
    }
  } else {
    makeFortranArray_bqqlv(fortranArray,m_b);
    if (partons->getLepCharge() > 0) {
      if ( !swapPartonMom ) {
	wlptwjjm_(fortranArray , &mw, &an);
      } else {
	wlptwjjaltm_(fortranArray , &mw, &an);
      }
    } else {
      if ( !swapPartonMom ) {
	wlmtwjjm_(fortranArray , &mw, &an);
      } else {
	wlmtwjjaltm_(fortranArray , &mw, &an);
      }
    }
  }

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif


   return answer;

}


// ------------------------------------------------------------------
double STopTWEventProb2Jet::phaseSpace() const
{
   // For an explanation of why this should be so, see the parallel
   // function in the 3-jet bin
   return EventProb::phaseSpace() * m_b.E() * std::sin(m_b.Theta());
}


// ------------------------------------------------------------------
void STopTWEventProb2Jet::setPartonTypes() const
{
  if ( decTWjj ) {
    if (getPartonColl()->getLepCharge() > 0) {
      if ( !swapPartonMom ) {
	getMeasuredColl()->setParton1Type(kGluon);
	getMeasuredColl()->setParton2Type(kBottom);
      } else {
	getMeasuredColl()->setParton1Type(kBottom);
	getMeasuredColl()->setParton2Type(kGluon);
      }
    } else {
      if ( !swapPartonMom ) {
	getMeasuredColl()->setParton1Type(kGluon);
	getMeasuredColl()->setParton2Type(kAntiBottom);
      } else {
	getMeasuredColl()->setParton1Type(kAntiBottom);
	getMeasuredColl()->setParton2Type(kGluon);
      }
    }
  } else {
    if (getPartonColl()->getLepCharge() > 0) {
      if ( !swapPartonMom ) {
	getMeasuredColl()->setParton1Type(kGluon);
	getMeasuredColl()->setParton2Type(kAntiBottom);
      } else {
	getMeasuredColl()->setParton1Type(kAntiBottom);
	getMeasuredColl()->setParton2Type(kGluon);
      }
    } else {
      if ( !swapPartonMom ) {
	getMeasuredColl()->setParton1Type(kGluon);
	getMeasuredColl()->setParton2Type(kBottom);
      } else {
	getMeasuredColl()->setParton1Type(kBottom);
	getMeasuredColl()->setParton2Type(kGluon);
      }
    }
  }

}


// ------------------------------------------------------------------
void STopTWEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}


// // ------------------------------------------------------------------
// double STopTWEventProb2Jet::totalTF() const
// /// Add when using more than one TF
// {
//    return getDefaultTF().getTF(getPartonColl()->getFullJet(0),
//                          getMeasuredColl()->getFullJet(0))
//       * m_bTF.getTF(getPartonColl()->getFullJet(1),
//                         getMeasuredColl()->getFullJet(1));
// }


// ------------------------------------------------------------------


bool STopTWEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
    //decTWjj=true; //when testing alternate functions
    //swapPartonMom=true; //when testing alternate functions
    decTWjj=false;
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
  case 4:
    decTWjj=true;
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

// ------------------------------------------------------------------
void STopTWEventProb2Jet::setupIntegral(){

  std::cout<<"\tTop mass= "<< m_massTop<<" width= "<<m_widthTop<<std::endl;

}

