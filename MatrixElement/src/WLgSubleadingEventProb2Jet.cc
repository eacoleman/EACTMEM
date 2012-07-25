///////////////////////////////////////////////////////////////////////
//// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
//// The diagrams can be compared with madgraph using the processes:
//// LightGluon final state - subleading processes (suppressed by x~2-4 vs the leading processes) : g d~ -> e+ ve u~ g (d~ g -> e+ ve u~ g) for W+, g u~ -> e- ve~ d~ g (u~ g -> e- ve~ d~ g) for W-
//////////////////////////////////////////////////////////////////////

//  This package libraries
#include "TAMUWW/MatrixElement/interface/WLgSubleadingEventProb2Jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

//  C++ libraries
#include <vector>
#include <iostream>

// Set this flag to test the output madgraph would give
//#define MADGRAPH_TEST

using std::vector;
// using std::cout;
// using std::endl;

#ifdef MADGRAPH_TEST
extern "C" {
  void* wpjjsm_ (double[][4], const double*, double*); // lepQ>0, g d~ -> e+ ve u~ g
  void* wpjjsaltm_ (double[][4], const double*, double*); // lepQ>0, d~ g -> e+ ve u~ g instead
  void* wmjjsm_ (double[][4], const double*, double*); // lepQ<0, g u~ -> e- ve~ d~ g
  void* wmjjsaltm_ (double[][4], const double*, double*); // lepQ<0, u~ g -> e- ve~ d~ g  instead 
  // to use the altm_ functions for just one iteration set swapPartonMom=true in onSwitch
}
#endif

// ------------------------------------------------------------------
WLgSubleadingEventProb2Jet::WLgSubleadingEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf, const TransferFunction& gluontf) :
  EventProb2Jet(DEFS::EP::WLg, integrator, 3, 4, lighttf), m_gluonTF(gluontf),
  swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{

}

// ------------------------------------------------------------------


// ------------------------------------------------------------------
void WLgSubleadingEventProb2Jet::changeVars(const vector<double>& parameters)
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
void WLgSubleadingEventProb2Jet::setDynamicBounds()
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
   m_gluonTF.getBounds(getMeasuredColl()->getFullJet(1), lowPercent,
                       highPercent, lower, upper);
   std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(2, lower, upper);
}

// ------------------------------------------------------------------
double WLgSubleadingEventProb2Jet::matrixElement() const
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

   doublecomplex factorGWF[2]   = {doublecomplex(MEConstants::gwf, 0),
				   doublecomplex(0, 0)};
   doublecomplex factorGG[2]  = {doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0),
				   doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0)};
   doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::GetAdjusted_sgg(alphas_process), 0),
   			   doublecomplex(0, 0)};

   // This should equal the number of helicity combinations.
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
	vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
        vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
        vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      }

      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, +1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec5, vec8, factorGG, 0, 0);

      OutputType output1 = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);
      
      Array2 vec10 = DHELAS::fvixxx(vec5,vec1,factorGG,0,0);
      Array2 vec11 = DHELAS::fvixxx(vec10,vec7,factorGWF,0,0);

      OutputType output2 = DHELAS::iovxxx(vec11,vec2,vec6,factorGG);

      Array2 vec12 = DHELAS::fvoxxx(vec8,vec1,factorGG,0,0);

      OutputType output3 = DHELAS::iovxxx(vec5,vec12,vec6,factorGG);
      OutputType output4 = DHELAS::iovxxx(vec10,vec8,vec6,factorGG);

      Array1 vec13 = DHELAS::fvixxx(vec5,vec7,factorGWF,0,0);
      Array1 vec14 = DHELAS::jioxxx(vec13,vec2,factorGG,0,0);

      OutputType output5 = DHELAS::vvvxxx(vec6,vec1,vec14,factorSGG);

      Array2 vec15 = DHELAS::fvixxx(vec13,vec1,factorGG,0,0);

      OutputType output6 = DHELAS::iovxxx(vec15,vec2,vec6,factorGG);

      Array2 vec16 = DHELAS::fvoxxx(vec2,vec1,factorGG,0,0);
      Array2 vec17 = DHELAS::fvoxxx(vec16,vec7,factorGWF,0,0);

      OutputType output7 = DHELAS::iovxxx(vec5,vec17,vec6,factorGG);
      OutputType output8 = DHELAS::iovxxx(vec13,vec16,vec6,factorGG);          

      std::swap(output1[1], output1[2]);
      std::swap(output5[1], output5[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
 	doublecomplex temp1 = output1[i] - output3[i] + output5[i] - output7[i] - output8[i];
	doublecomplex temp2 = -output1[i] - output2[i] - output4[i] - output5[i] - output6[i];

	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
	//	cout << "current helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;
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
	vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
        vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
      } else {
	vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
        vec2 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      }

      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, +1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factorGWF, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec5, vec8, factorGG, 0, 0);
      OutputType output1 = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);
      
      Array2 vec10 = DHELAS::fvixxx(vec5,vec1,factorGG,0,0);
      Array2 vec11 = DHELAS::fvixxx(vec10,vec7,factorGWF,0,0);
      OutputType output2 = DHELAS::iovxxx(vec11,vec2,vec6,factorGG);

      Array2 vec12 = DHELAS::fvoxxx(vec8,vec1,factorGG,0,0);
      OutputType output3 = DHELAS::iovxxx(vec5,vec12,vec6,factorGG);
      OutputType output4 = DHELAS::iovxxx(vec10,vec8,vec6,factorGG);

      Array1 vec13 = DHELAS::fvixxx(vec5,vec7,factorGWF,0,0);
      Array1 vec14 = DHELAS::jioxxx(vec13,vec2,factorGG,0,0);
      OutputType output5 = DHELAS::vvvxxx(vec6,vec1,vec14,factorSGG);

      Array2 vec15 = DHELAS::fvixxx(vec13,vec1,factorGG,0,0);
      OutputType output6 = DHELAS::iovxxx(vec15,vec2,vec6,factorGG);

      Array2 vec16 = DHELAS::fvoxxx(vec2,vec1,factorGG,0,0);
      Array2 vec17 = DHELAS::fvoxxx(vec16,vec7,factorGWF,0,0);
      OutputType output7 = DHELAS::iovxxx(vec5,vec17,vec6,factorGG);
      OutputType output8 = DHELAS::iovxxx(vec13,vec16,vec6,factorGG);     

      
      std::swap(output1[1], output1[2]);
      std::swap(output5[1], output5[2]);


      for (unsigned i = 0; i < vecSize; ++i)
      {
	doublecomplex temp1 = -output1[i] + output3[i] - output5[i] + output7[i] + output8[i];
	doublecomplex temp2 =  output1[i] + output2[i] + output4[i] + output5[i] + output6[i];

	doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;

	answer+= (m1+m2).real();
	//	cout << "current helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;
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

  if (partons->getLepCharge() > 0) {
    if ( !swapPartonMom ) {
      wpjjsm_(fortranArray , &mw, &an);
    } else {
      wpjjsaltm_(fortranArray , &mw, &an);
    }
  } else {
    if ( !swapPartonMom ) {
      wmjjsm_(fortranArray , &mw, &an);
    } else {
      wmjjsaltm_(fortranArray , &mw, &an);
    }
  }

  cout << "Madgraph answer= " << an << endl;
   
  // Exit right away
  exit(1);  

#endif

  return answer;

}

// ------------------------------------------------------------------
void WLgSubleadingEventProb2Jet::setPartonTypes() const
{ 
 
  if ( getPartonColl()->getLepCharge() > 0 ) {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kAntiDown);
    } else {
      getMeasuredColl()->setParton1Type(kAntiDown);
      getMeasuredColl()->setParton2Type(kGluon);
    }
    
  } else {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kAntiUp);
    } else {
      getMeasuredColl()->setParton1Type(kAntiUp);
      getMeasuredColl()->setParton2Type(kGluon);
    }
  }

}

void WLgSubleadingEventProb2Jet::setJetTypes()
{

  if ( getPartonColl()->getLepCharge() > 0 ) {
    m_JetType[0]=kAntiUp;
    m_JetType[1]=kGluon;
  } else {
    m_JetType[0]=kAntiDown;
    m_JetType[1]=kGluon;
  }

  if ( getSwappedJet0Jet1Status() ) {
    int tempType=m_JetType[0];
    m_JetType[0]=m_JetType[1];
    m_JetType[1]=tempType;
  }

}

// ------------------------------------------------------------------
void WLgSubleadingEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

// ------------------------------------------------------------------
double WLgSubleadingEventProb2Jet::totalTF() const
/// Add when using more than one TF
{
   return getDefaultTF().getTF(getPartonColl()->getFullJet(0),
                         getMeasuredColl()->getFullJet(0))
      * m_gluonTF.getTF(getPartonColl()->getFullJet(1),
                        getMeasuredColl()->getFullJet(1));
}

// ------------------------------------------------------------------


bool WLgSubleadingEventProb2Jet::onSwitch()
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
