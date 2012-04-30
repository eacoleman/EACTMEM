//////////////////////////////////////////////////////////////////////
//// Author: Ilya Osipenkov Texas A&M University.
//// The diagrams can be compared with madgraph using the processes
//// LightLight final state (suppressed by x~5 vs the leading diagrams): 
//// Initial State Configuration 01: g g -> e+ ve u~ d for W+, g g -> e- ve~ u d~ W-
//// Initial State Configuration 02: u u -> e+ ve u d for W+, d d -> e- ve~ u d W-
//// Initial State Configuration 03: d u -> e+ ve d d (u d -> e+ ve d d) for W+, u d -> e- ve~ u u (d u -> e- ve~ u u) W-
//////////////////////////////////////////////////////////////////////
#include "TAMUWW/MatrixElement/interface/WLLEventProb2Jet.hh"

//  C++ libraries
#include <vector>
#include <iostream>
#include <cmath>
#include <strstream>

//for output on the screen
#include <iomanip>
using namespace std;


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
  void* wpllisc01m_(double[][4], const double*, double*); // lepQ>0, g g -> e+ ve u~ d
  void* wmllisc01m_(double[][4], const double*, double*); // lepQ<0, g g -> e- ve~ u d~
  void* wpllisc02m_(double[][4], const double*, double*); // lepQ>0, u u -> e+ ve u d
  void* wmllisc02m_(double[][4], const double*, double*); // lepQ<0, d d -> e- ve~ u d
  void* wpllisc03m_(double[][4], const double*, double*); // lepQ>0, d u -> e+ ve d d 
  void* wpllisc03altm_(double[][4], const double*, double*); // lepQ>0, u d -> e+ ve d d instead
  void* wmllisc03m_(double[][4], const double*, double*); // lepQ<0, u d -> e- ve~ u u
  void* wmllisc03altm_(double[][4], const double*, double*); // lepQ<0, d u -> e- ve~ u u
  // to use to use a specific configuration for just one iteration initialize the initialStateConfiguration(1,2,3)
  // to use the altm_ functions for just one iteration initialize the swapPartonMom(true)
}
#endif

// ------------------------------------------------------------------
WLLEventProb2Jet::WLLEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::WLL, integrator, 3, 8, tf), initialStateConfiguration(1), swapPartonMom(false), alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{
  ///ISC01:
  ///For g g -> e+ ve u~ d:
  int o_output_ggp[NOutputP1][NVec];
  AssignOrdering(o_output_ggp,1,"5 1 6 2");
  AssignOrdering(o_output_ggp,2,"5 1 2 6");
  AssignOrdering(o_output_ggp,3,"5 2 6 1");//default
  AssignOrdering(o_output_ggp,4,"5 2 1 6");
  AssignOrdering(o_output_ggp,5,"5 2 6 1");
  AssignOrdering(o_output_ggp,6,"5 1 6 2");
  AssignOrdering(o_output_ggp,7,"5 1 2 6");
  AssignOrdering(o_output_ggp,8,"5 6 1 2");
  int switchedlabel_ggp[NOutputP1][NVec];
  Determine_switchedlabels(switchedlabel_ggp,o_output_ggp[3],o_output_ggp,1);
  Determine_switchedindex(idxvec_reordered_ggp,switchedlabel_ggp,1);
  Determine_switchedlabels(switchedlabel_ggp,o_output_ggp[3],o_output_ggp,2);
  Determine_switchedindex(idxvec_reordered_ggp,switchedlabel_ggp,2);
  for (int v = 4; v < NOutputP1; v++) {
    Determine_switchedlabels(switchedlabel_ggp,o_output_ggp[3],o_output_ggp,v);
    Determine_switchedindex(idxvec_reordered_ggp,switchedlabel_ggp,v);
  }
  ///For g g -> e- ve~ u d~:
  int o_output_ggm[NOutputP1][NVec];
  AssignOrdering(o_output_ggm,1,"6 5 1 2");//default
  AssignOrdering(o_output_ggm,2,"6 5 1 2");
  AssignOrdering(o_output_ggm,3,"6 5 2 1");
  AssignOrdering(o_output_ggm,4,"6 5 2 1");
  AssignOrdering(o_output_ggm,5,"6 5 2 1");
  AssignOrdering(o_output_ggm,6,"6 5 1 2");
  AssignOrdering(o_output_ggm,7,"6 5 1 2");
  AssignOrdering(o_output_ggm,8,"6 5 1 2");
  int switchedlabel_ggm[NOutputP1][NVec];
  ///Only need to reorder 3,4,5
  for (int v = 3; v < 6; v++) {
    Determine_switchedlabels(switchedlabel_ggm,o_output_ggm[1],o_output_ggm,v);
    Determine_switchedindex(idxvec_reordered_ggm,switchedlabel_ggm,v);
  }

  ///ISC02:
  ///For u u -> e+ ve u d:
  int o_output_uup[NOutputP1][NVec];
  AssignOrdering(o_output_uup,1,"1 2 5 6");//default
  AssignOrdering(o_output_uup,2,"1 6 2 5");
  AssignOrdering(o_output_uup,3,"2 1 5 6");
  AssignOrdering(o_output_uup,4,"2 6 1 5");
  int switchedlabel_uup[NOutputP1][NVec];
  for (int v = 2; v < 5; v++) {
    Determine_switchedlabels(switchedlabel_uup,o_output_uup[1],o_output_uup,v);
    Determine_switchedindex(idxvec_reordered_uup,switchedlabel_uup,v);
  }
  ///For d d -> e- ve~ u d:
  int o_output_ddm[NOutputP1][NVec];
  AssignOrdering(o_output_ddm,1,"1 6 2 5");//default
  AssignOrdering(o_output_ddm,2,"1 6 2 5");
  AssignOrdering(o_output_ddm,3,"2 6 1 5");
  AssignOrdering(o_output_ddm,4,"2 6 1 5");
  int switchedlabel_ddm[NOutputP1][NVec];
  for (int v = 3; v < 5; v++) {
    Determine_switchedlabels(switchedlabel_ddm,o_output_ddm[1],o_output_ddm,v);
    Determine_switchedindex(idxvec_reordered_ddm,switchedlabel_ddm,v);
  }

  ///ISC03:
  ///For d u -> e+ ve d d (u d -> e+ ve d d)
  int o_output_ddp[NOutputP1][NVec];
  AssignOrdering(o_output_ddp,1,"1 6 2 5");//default
  AssignOrdering(o_output_ddp,2,"1 6 2 5");
  AssignOrdering(o_output_ddp,3,"2 1 5 6");
  AssignOrdering(o_output_ddp,4,"2 6 1 5");
  int switchedlabel_ddp[NOutputP1][NVec];
  for (int v = 3; v < 5; v++) {
    Determine_switchedlabels(switchedlabel_ddp,o_output_ddp[1],o_output_ddp,v);
    Determine_switchedindex(idxvec_reordered_ddp,switchedlabel_ddp,v);
  }
  ///For u d -> e- ve~ u u (d u -> e- ve~ u u)
  int o_output_uum[NOutputP1][NVec];
  AssignOrdering(o_output_uum,1,"1 6 2 5");//default
  AssignOrdering(o_output_uum,2,"1 6 2 5");
  AssignOrdering(o_output_uum,3,"2 1 5 6");
  AssignOrdering(o_output_uum,4,"2 6 1 5");
  int switchedlabel_uum[NOutputP1][NVec];
  for (int v = 3; v < 5; v++) {
    Determine_switchedlabels(switchedlabel_uum,o_output_uum[1],o_output_uum,v);
    Determine_switchedindex(idxvec_reordered_uum,switchedlabel_uum,v);
  }

}
// ------------------------------------------------------------------

void WLLEventProb2Jet::changeVars(const vector<double>& parameters)
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

double WLLEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;
   typedef SimpleArray<DHELAS::HelArray, 8> Array8;

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

   if ( initialStateConfiguration==1 ) {
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
	 
	 ///Initial State Partons
	 Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
	 Array2 vec2 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
	 ///Final State Neutrino & Partons
	 Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
	 Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(0), 0, -1);
	 Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);

	 Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
	 Array4 vec8 = DHELAS::fvixxx(vec5, vec1, factorGG, 0, 0);
	 Array4 vec9 = DHELAS::fvixxx(vec8, vec7, factorGWF, 0, 0);

	 OutputType output[NOutputP1];
	 output[1] = DHELAS::iovxxx(vec9, vec6, vec2, factorGG);//input order = 5 1 6 2

	 Array8 vec10 = DHELAS::fvixxx(vec8, vec2, factorGG, 0, 0);
	 output[2] = DHELAS::iovxxx(vec10, vec6, vec7, factorGWF);//input order = 5 1 2 6 

	 Array4 vec11 = DHELAS::fvixxx(vec5, vec2, factorGG, 0, 0);
	 Array4 vec12 = DHELAS::fvixxx(vec11, vec7, factorGWF, 0, 0);
	 output[3] = DHELAS::iovxxx(vec12, vec6, vec1, factorGG);//input order = 5 2 6 1 : default

	 Array8 vec13 = DHELAS::fvixxx(vec11, vec1, factorGG, 0, 0);
	 output[4] = DHELAS::iovxxx(vec13, vec6, vec7, factorGWF);//input order = 5 2 1 6

	 Array2 vec14 = DHELAS::fvixxx(vec5, vec7, factorGWF, 0, 0);
	 Array4 vec15 = DHELAS::fvixxx(vec14, vec2, factorGG, 0, 0);
	 output[5] = DHELAS::iovxxx(vec15, vec6, vec1, factorGG);//input order = 5 2 6 1

	 Array4 vec16 = DHELAS::fvixxx(vec14, vec1, factorGG, 0, 0);
	 output[6] = DHELAS::iovxxx(vec16, vec6, vec2, factorGG);//input order = 5 1 6 2

	 Array4 vec17 = DHELAS::jvvxxx(vec1, vec2, factorSGG, 0, 0);
	 Array8 vec18 = DHELAS::fvixxx(vec5, vec17, factorGG, 0, 0);
	 output[7] = DHELAS::iovxxx(vec18, vec6, vec7, factorGWF);//input order = 5 1 2 6
	 output[8] = DHELAS::iovxxx(vec14, vec6, vec17, factorGG);//input order = 5 6 1 2

	 ///Reorder all vectors to match the default
	 Reorder(output[1],idxvec_reordered_ggp,1);
	 Reorder(output[2],idxvec_reordered_ggp,2);
	 for (int v = 4; v < NOutputP1; v++) {
	   Reorder(output[v],idxvec_reordered_ggp,v);
	 }


	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = output[1][i] + output[2][i] + output[6][i] - output[7][i] - output[8][i];
	   doublecomplex temp2 = output[3][i] + output[4][i] + output[5][i] + output[7][i] + output[8][i];
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
	 
	 ///Initial State Partons
	 Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
	 Array2 vec2 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
	 ///Final State Neutrino & Partons
	 Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
	 Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), 0, -1);

	 Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
	 Array4 vec8 = DHELAS::fvoxxx(vec5, vec1, factorGG, 0, 0);
	 Array4 vec9 = DHELAS::fvoxxx(vec8, vec7, factorGWF, 0, 0);

	 OutputType output[NOutputP1];
	 output[1] = DHELAS::iovxxx(vec6, vec9, vec2, factorGG);//input order = 6 5 1 2 : default

	 Array8 vec10 = DHELAS::fvoxxx(vec8, vec2, factorGG, 0, 0);
	 output[2] = DHELAS::iovxxx(vec6, vec10, vec7, factorGWF);//input order = 6 5 1 2 

	 Array4 vec11 = DHELAS::fvoxxx(vec5, vec2, factorGG, 0, 0);
	 Array4 vec12 = DHELAS::fvoxxx(vec11, vec7, factorGWF, 0, 0);
	 output[3] = DHELAS::iovxxx(vec6, vec12, vec1, factorGG);//input order = 6 5 2 1

	 Array8 vec13 = DHELAS::fvoxxx(vec11, vec1, factorGG, 0, 0);
	 output[4] = DHELAS::iovxxx(vec6, vec13, vec7, factorGWF);//input order = 6 5 2 1

	 Array2 vec14 = DHELAS::fvoxxx(vec5, vec7, factorGWF, 0, 0);
	 Array4 vec15 = DHELAS::fvoxxx(vec14, vec2, factorGG, 0, 0);
	 output[5] = DHELAS::iovxxx(vec6, vec15, vec1, factorGG);//input order = 6 5 2 1

	 Array4 vec16 = DHELAS::fvoxxx(vec14, vec1, factorGG, 0, 0);
	 output[6] = DHELAS::iovxxx(vec6, vec16, vec2, factorGG);//input order = 6 5 1 2

	 Array4 vec17 = DHELAS::jvvxxx(vec1, vec2, factorSGG, 0, 0);
	 Array8 vec18 = DHELAS::fvoxxx(vec5, vec17, factorGG, 0, 0);
	 output[7] = DHELAS::iovxxx(vec6, vec18, vec7, factorGWF);//input order = 6 5 1 2
	 output[8] = DHELAS::iovxxx(vec6, vec14, vec17, factorGG);//input order = 6 5 1 2

 	 ///Reorder all vectors to match the default
	 for (int v = 3; v < 6; v++) {
	   Reorder(output[v],idxvec_reordered_ggm,v);
	 }


	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = output[1][i] + output[2][i] + output[6][i] + output[7][i] + output[8][i];
	   doublecomplex temp2 = output[3][i] + output[4][i] + output[5][i] - output[7][i] - output[8][i];
	   doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	   doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;
	   
	   answer+= (m1+m2).real();
	 }

       }
     answer /= 256;
   }


   if ( initialStateConfiguration==2 ) {
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
	 
	 ///Initial State Partons
	 Array2 vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
	 Array2 vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	 ///Final State Neutrino & Partons
	 Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
	 Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);

	 Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
	 Array2 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
	 Array4 vec14 = DHELAS::jioxxx(vec2, vec5, factorGG, 0, 0);
	 Array8 vec15 = DHELAS::fvixxx(vec1, vec14, factorGG, 0, 0);

	 OutputType output[NOutputP1];
	 output[1] = DHELAS::iovxxx(vec15, vec6, vec7, factorGWF);//input order = 1 2 5 6 : default

	 Array2 vec18 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
	 output[2] = DHELAS::iovxxx(vec18, vec6, vec14, factorGG);//input order = 1 6 2 5

	 Array4 vec23 = DHELAS::jioxxx(vec1, vec5, factorGG, 0, 0);
	 Array8 vec24 = DHELAS::fvixxx(vec2, vec23, factorGG, 0, 0);
	 output[3] = DHELAS::iovxxx(vec24, vec6, vec7, factorGWF);//input order = 2 1 5 6
	 output[4] = DHELAS::iovxxx(vec8, vec6, vec23, factorGG);//input order = 2 6 1 5

	 ///Reorder all vectors to match the default
	 for (int v = 2; v < 5; v++) {
	   Reorder(output[v],idxvec_reordered_uup,v);
	 }


	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = +output[1][i] + output[2][i];
	   doublecomplex temp2 = -output[3][i] - output[4][i];
	   doublecomplex m1 = ( temp1*6.0 -temp2*2.0)*std::conj(temp1)/3.0;
	   doublecomplex m2 = ( -temp1*2.0 +temp2*6.0)*std::conj(temp2)/3.0;
	   
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
	 
	 ///Initial State Partons
	 Array2 vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
	 Array2 vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	 ///Final State Neutrino & Partons
	 Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
	 Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);

	 Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
	 Array2 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
	 Array4 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);

	 OutputType output[NOutputP1];
	 output[1] = DHELAS::iovxxx(vec1, vec6, vec9, factorGG);//input order = 1 6 2 5 : default

	 Array2 vec15 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
	 Array2 vec16 = DHELAS::fvoxxx(vec5, vec7, factorGWF, 0, 0);
	 Array4 vec17 = DHELAS::jioxxx(vec2, vec16, factorGG, 0, 0);
	 output[2] = DHELAS::iovxxx(vec1, vec6, vec17, factorGG);//input order = 1 6 2 5

	 Array4 vec23 = DHELAS::jioxxx(vec15, vec5, factorGG, 0, 0);
	 output[3] = DHELAS::iovxxx(vec2, vec6, vec23, factorGG);//input order = 2 6 1 5

	 Array4 vec26 = DHELAS::jioxxx(vec1, vec16, factorGG, 0, 0);
	 output[4] = DHELAS::iovxxx(vec2, vec6, vec26, factorGG);//input order = 2 6 1 5

	 ///Reorder all vectors to match the default
	 for (int v = 3; v < 5; v++) {
	   Reorder(output[v],idxvec_reordered_ddm,v);
	 }

	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = -output[1][i] - output[2][i];
	   doublecomplex temp2 = +output[3][i] + output[4][i];
	   doublecomplex m1 = ( temp1*6.0 -temp2*2.0)*std::conj(temp1)/3.0;
	   doublecomplex m2 = ( -temp1*2.0 +temp2*6.0)*std::conj(temp2)/3.0;
	   answer+= (m1+m2).real();
	 }
       }
     answer /= 36;
   }

   if ( initialStateConfiguration==3 ) {
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
	 
	 ///Initial State Partons
	 Array2 vec1;
	 Array2 vec2;
	 if ( !swapPartonMom ) {
	   vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	 } else {
	   vec1 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);	   
	 }


	 ///Final State Neutrino & Partons
	 Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, +1);
	 Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);

	 Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factorGWF, wMass, wWidth);
	 Array2 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
	 Array4 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);

	 OutputType output[NOutputP1];
	 output[1] = DHELAS::iovxxx(vec1, vec6, vec9, factorGG);//input order = 1 6 2 5: default

	 Array2 vec16 = DHELAS::fvoxxx(vec5, vec7, factorGWF, 0, 0);
	 Array4 vec17 = DHELAS::jioxxx(vec2, vec16, factorGG, 0, 0);
	 output[2] = DHELAS::iovxxx(vec1, vec6, vec17, factorGG);//input order = 1 6 2 5

	 Array4 vec24 = DHELAS::jioxxx(vec1, vec5, factorGG, 0, 0);
	 Array8 vec25 = DHELAS::fvixxx(vec2, vec24, factorGG, 0, 0);
	 output[3] = DHELAS::iovxxx(vec25, vec6, vec7, factorGWF);//input order = 2 1 5 6
	 output[4] = DHELAS::iovxxx(vec8, vec6, vec24, factorGG);//input order = 2 6 1 5

	 ///Reorder all vectors to match the default
	 for (int v = 3; v < 5; v++) {
	   Reorder(output[v],idxvec_reordered_ddp,v);
	 }


	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = +output[1][i] + output[2][i];
	   doublecomplex temp2 = -output[3][i] - output[4][i];
	   doublecomplex m1 = ( temp1*6.0 -temp2*2.0)*std::conj(temp1)/3.0;
	   doublecomplex m2 = ( -temp1*2.0 +temp2*6.0)*std::conj(temp2)/3.0;	   
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
	 
	 ///Initial State Partons
	 Array2 vec1;
	 Array2 vec2;
	 if ( !swapPartonMom ) {
	   vec1 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	 } else {
	   vec1 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
	   vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
	 }
	 ///Final State Neutrino & Partons
	 Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
	 Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
	 Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, +1);

	 Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factorGWF, wMass, wWidth);
	 Array2 vec8 = DHELAS::fvixxx(vec2, vec7, factorGWF, 0, 0);
	 Array4 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);

	 OutputType output[NOutputP1];
	 output[1] = DHELAS::iovxxx(vec1, vec6, vec9, factorGG);//input order = 1 6 2 5 : default

	 //Array2 vec15 = DHELAS::fvixxx(vec1, vec7, factorGWF, 0, 0);
	 Array2 vec16 = DHELAS::fvoxxx(vec5, vec7, factorGWF, 0, 0);
	 Array4 vec17 = DHELAS::jioxxx(vec2, vec16, factorGG, 0, 0);
	 output[2] = DHELAS::iovxxx(vec1, vec6, vec17, factorGG);//input order = 1 6 2 5

	 Array4 vec24 = DHELAS::jioxxx(vec1, vec5, factorGG, 0, 0);
	 Array8 vec25 = DHELAS::fvixxx(vec2, vec24, factorGG, 0, 0);
	 output[3] = DHELAS::iovxxx(vec25, vec6, vec7, factorGWF);//input order = 2 1 5 6
	 output[4] = DHELAS::iovxxx(vec8, vec6, vec24, factorGG);//input order = 2 6 1 5

	 ///Reorder all vectors to match the default
	 for (int v = 3; v < 5; v++) {
	   Reorder(output[v],idxvec_reordered_uum,v);
	 }

	 for (unsigned i = 0; i < vecSize; ++i) {
	   doublecomplex temp1 = -output[1][i] - output[2][i];
	   doublecomplex temp2 = +output[3][i] + output[4][i];
	   doublecomplex m1 = ( temp1*6.0 -temp2*2.0)*std::conj(temp1)/3.0;
	   doublecomplex m2 = ( -temp1*2.0 +temp2*6.0)*std::conj(temp2)/3.0;
	   answer+= (m1+m2).real();
	 }
       }
     answer /= 72;
   }



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
    wpllisc01m_(fortranArray , &mw, &an);
    //wpllisc02m_(fortranArray , &mw, &an);
    //wpllisc03m_(fortranArray , &mw, &an);
    //wpllisc03altm_(fortranArray , &mw, &an);
  } else {
    wpllisc01m_(fortranArray , &mw, &an);
    //wmllisc02m_(fortranArray , &mw, &an);
    //wmllisc03m_(fortranArray , &mw, &an);
    //wmllisc03altm_(fortranArray , &mw, &an);
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
void WLLEventProb2Jet::setPartonTypes() const
{
  if ( initialStateConfiguration==1) {
    getMeasuredColl()->setParton1Type(kGluon);
    getMeasuredColl()->setParton2Type(kGluon);
  }

  if ( initialStateConfiguration==2) {
    if (getPartonColl()->getLepCharge() > 0) {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kUp);
    } else {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kDown);
    }
  }

  if ( initialStateConfiguration==3) {
    if ( ((getPartonColl()->getLepCharge() > 0)&&(!swapPartonMom))||
	 ((getPartonColl()->getLepCharge() < 0)&&(swapPartonMom))
	 ) 
      {
	getMeasuredColl()->setParton1Type(kDown);
	getMeasuredColl()->setParton2Type(kUp);
      } else {
	getMeasuredColl()->setParton1Type(kUp);
	getMeasuredColl()->setParton2Type(kDown);
      }
  }

}


// ------------------------------------------------------------------
void WLLEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

// ------------------------------------------------------------------


bool WLLEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
    //swapPartonMom=true; //when testing alternate functions
    initialStateConfiguration=1;
    break;
  case 1:
    swapJets(0, 1);
    break;
  case 2:
    initialStateConfiguration=2;
    break;
  case 3:
    swapJets(0, 1);
    break;
  case 4:
    initialStateConfiguration=3;
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

//////////////// Functions Used To Manipulate Ordering ////////////////////////

void WLLEventProb2Jet::AssignOrdering(int o_vec[NOutputP1][NVec], int output_vecidx, const char* order) const
//// Store the ordering pattern in o_vec
//// output_vecidx is used to select the output vector which will be reordered
{
  std::istrstream ord(order);
  for (int k = 0; k < NVec; k++) {
    ord >> o_vec[output_vecidx][k];
  }
}


void WLLEventProb2Jet::Determine_switchedlabels(int sl[NOutputP1][NVec], int o_target[NVec], int o_output[NOutputP1][NVec], int output_vecidx) const
//// sl[output_vecidx][k] vector should be switched with the k'th vector in order for o_output[output_vecidx] to match o_target
//// This function also reorders the o_output[output_vecidx] to match o[target]
{
  for (int k = 0; k < NVec; k++) {
    if ( o_target[k]!=o_output[output_vecidx][k] ) {
      for (int t=k+1; t<NVec; t++ ) {
	if ( o_output[output_vecidx][t]==o_target[k] ) {
	  sl[output_vecidx][k]=t;
	  o_output[output_vecidx][t]=o_output[output_vecidx][k];
	  o_output[output_vecidx][k]=o_target[k];
	}
      }
    } else {
      sl[output_vecidx][k]=-1;
    }
  }
}

void WLLEventProb2Jet::Determine_switchedindex(int idxvec_reordered[NOutputP1][VSize], int switchedlabel[NOutputP1][NVec], int output_vecidx) const
////Determines how to switch the components of the output vector in order to match the template.
////Individual indicies corresponding to each of the vectors of size two: idx[k]=0,1 with k=0,..,NVec.
////Vector indicies: j=0,1,...vecSize; j=2^(NVec-1)idx[0]+2^(NVec-2)idx[1]+...+idx[NVec-1].
{
  int idx[NVec];
  ///Temporary variables
  int idx_p[NVec];
  int j_temp;
  int multiplier;
  int idx_temp;
  for (int j = 0; j < VSize; j++) {
    j_temp=j;
    idxvec_reordered[output_vecidx][j]=j;
    multiplier=VSize;
    ///Extract idx[k]
    //cout << "j=" << j << endl;
    for (int k=0; k<NVec; k++) {
      multiplier=multiplier/2;
      if ( j_temp<multiplier ) {
	idx[k]=0;
      } else {
	idx[k]=1;
	j_temp=j_temp-multiplier;
      }
      idx_p[k]=idx[k];
      //       cout << " k=" << k << " multiplier=" << multiplier << " idx[k]=" << idx[k] << " j_temp=" << j_temp << endl;
    }
    ///Compute how the components will have to be reordered
    for (int k=0; k<NVec; k++) {
      if ( switchedlabel[output_vecidx][k]!=-1 ) {
	// 	 cout << "j=" << j << " : switching " << k << " and " << switchedlabel[output_vecidx][k] << endl;
	// 	 cout << "idx[0]=" << idx[0] << "  idx[1]=" << idx[1] <<  "  idx[2]=" << idx[2] <<endl;
	// 	 cout << "idx_p[0]=" << idx_p[0] << "  idx_p[1]=" << idx_p[1] <<  "  idx_p[2]=" << idx_p[2] << endl;
	// 	 cout << "idx_p[switchedlabel[output_vecidx][k]]-idx_p[k]=" <<   idx_p[switchedlabel[output_vecidx][k]]-idx_p[k] << endl;
	idxvec_reordered[output_vecidx][j]=idxvec_reordered[output_vecidx][j]+pow(2,NVec-1-switchedlabel[output_vecidx][k])*(pow(2,switchedlabel[output_vecidx][k]-k)-1)*(idx_p[switchedlabel[output_vecidx][k]]-idx_p[k]);
	idx_temp=idx_p[switchedlabel[output_vecidx][k]];
	idx_p[switchedlabel[output_vecidx][k]]=idx_p[k];
	idx_p[k]=idx_temp;
      }
    }
    // cout << "j=" << j << " : idxvec_reordered[output_vecidx]=" << idxvec_reordered[output_vecidx][j] << endl;
  }
  
}

void WLLEventProb2Jet::Reorder(OutputType& output, const int idxvec_reordered[NOutputP1][VSize], int output_vecidx) const
////Reorder the output vector to match the (previously selected) template.
{
  OutputType output_temp;
  for (int j = 0; j < VSize; j++) {
    //cout << "j=" << j << " output[j]=" << output[j] << " idxvec_reordered[output_vecidx][j]=" << idxvec_reordered[output_vecidx][j] << endl;
    output_temp[idxvec_reordered[output_vecidx][j]]=output[j];
  }

  output=output_temp;
} 

