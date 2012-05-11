// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 10/27/2010
// The diagrams can be compared with madgraph using the processes 
// u d~ -> b e+ ve b~ (d~ u -> b e+ ve b~), d u~ -> b~ e- ve~ b (u~ d -> b~ e- ve~ b)


#include "TAMUWW/MatrixElement/interface/sChannelEventProb2Jet.hh"

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
  void* schanlpm_(double[][4], const double*, double*);// lepQ>0, u d~ -> b e+ ve b~
  void* schanlpaltm_(double[][4], const double*, double*);// d~ u -> b e+ ve b~
  void* schanlmm_(double[][4], const double*, double*);// lepQ<0, d u~ -> b~ e- ve~ b
  void* schanlmaltm_(double[][4], const double*, double*);// u~ d -> b~ e- ve~ b
}
#endif

// ------------------------------------------------------------------
sChannelEventProb2Jet::sChannelEventProb2Jet(Integrator& integrator,const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::STopS, integrator, 3,4, tf), 
  swapPartonMom(false), 
  alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);
}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void sChannelEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);

  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);


}//setTopMassAndWidth

// ------------------------------------------------------------------
void sChannelEventProb2Jet::changeVars(const vector<double>& parameters)
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
double sChannelEventProb2Jet::matrixElement() const
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

   enum {vecSize = 16};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
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
     Array2 vec1;
     Array2 vec2;
     if ( !swapPartonMom ) {
       vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, 1);
       vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), 0, -1);
     } else {
       vec1 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, 1);
       vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), 0, -1);
     }
     Array2 vec3 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
     Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);
     
     Array4 vec7 = DHELAS::jioxxx(vec1, vec2, factorGWF, wMass, wWidth);
     Array1 vec8 = DHELAS::jioxxx(vec4, vec5, factorGWF, wMass, wWidth);
     Array2 vec9 = DHELAS::fvoxxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);

     OutputType output1 = DHELAS::iovxxx(vec6, vec9, vec7, factorGWF);
     
     for (unsigned i = 0; i < vecSize; ++i)
       {
	 double temp1 =  std::norm(-output1[i]) * 9;
	 answer+= temp1;
	 //	 cout << "i=" << i << " helicity 'amplitude'" << temp1 << " + " << "0" << "i" << endl;
       }
   }
   else
   {

      static Array1 vec4;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
	vec4 = DHELAS::oxxxxx<1>(partons->getLepton(),0, +1);
         lepE = partons->getLepton().E();
      }
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec1;
      Array2 vec2;
      if ( !swapPartonMom ) {
	vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
	vec2 = DHELAS::oxxxxx<2>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	vec2 = DHELAS::oxxxxx<2>(partons->getParton1(), 0, -1);
      }
      Array2 vec3 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, +1);
      
      Array4 vec7 = DHELAS::jioxxx(vec1, vec2, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec5, vec4, factorGWF, wMass, wWidth);
      Array2 vec9 = DHELAS::fvixxx(vec3, vec8, factorGWF, m_massTop, m_widthTop);

      OutputType output1 = DHELAS::iovxxx(vec9, vec6, vec7, factorGWF);

      for (unsigned i = 0; i < vecSize; ++i)
      {
	double temp1 =  std::norm(+output1[i]) * 9;
	answer+= temp1;
	//	cout << "i=" << i << " helicity 'amplitude'" << temp1 << " + " << "0" << "i" << endl;
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
    schanlpm_(fortranArray , &mw, &an);
    //schanlpaltm_(fortranArray , &mw, &an);
  else
    schanlmm_(fortranArray , &mw, &an);
  //schanlmaltm_(fortranArray , &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

   return answer;

}


void sChannelEventProb2Jet::setPartonTypes() const
{
  if (getMeasuredColl()->getLepCharge() > 0) {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kAntiDown);
    } else {
      getMeasuredColl()->setParton1Type(kAntiDown);
      getMeasuredColl()->setParton2Type(kUp);
    }
  } else {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kAntiUp);
    } else {
      getMeasuredColl()->setParton1Type(kAntiUp);
      getMeasuredColl()->setParton2Type(kDown);
    }
  }
}

void sChannelEventProb2Jet::getScale(double& scale1, double& scale2) const
{
//   scale1 = scale2 = m_massTop;
   // Q = sqrt(sHat)
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool sChannelEventProb2Jet::onSwitch()
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

