// Author: Ricardo Eusebi, Ilya Osipenkov Texas A&M University.
// Created : 10/18/2010
// The diagrams can be compared with madgraph using the processes 
// g u -> e+ e- u g (u g -> e+ e- u g )
// g d -> e+ e- d g (d g -> e+ e- d g )

#include "TAMUWW/MatrixElement/interface/ZLightEventProb2Jet.hh"


#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

//  C++ libraries
//#include "TLorentzVector.h"

#include <vector>
#include <iostream>
#include <cmath>

#include <strstream>

//for output on the screen
#include <iomanip>
using namespace std;

// Set this flag to test the output madgraph would give
//#define MADGRAPH_TEST

using std::vector;
using std::cout;
using std::endl;

#ifdef MADGRAPH_TEST
extern "C"
{
  void* zugm_(double[][4], const double*, const double*, double*); // lepQ>0 or lepQ<0 ( g u -> e+ e- u g  )
  void* zugaltm_(double[][4], const double*, const double*, double*); // for u g -> e+ e- u g instead
  void* zdgm_(double[][4], const double*, const double*, double*); // lepQ>0 or lepQ<0 ( g d -> e+ e- d g  )
  void* zdgaltm_(double[][4], const double*, const double*, double*); // for d g -> e+ e- d g instead

}
#endif

// ------------------------------------------------------------------
ZLightEventProb2Jet::ZLightEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
  EventProb2Jet(DEFS::EP::ZLight, integrator, 3, 8, tf), 
  swapPartonMom(false), 
  swapQuarkFlavor(false), 
  alphas_process(0.13) //Take the alphas_process value from MadGraph or use MEConstants::alphas
{

  int o_output[NOutputP1][NVec];
  AssignOrdering(o_output,1,"6 1 2 3 4 5");
  AssignOrdering(o_output,2,"2 5 1 3 4 6");
  AssignOrdering(o_output,3,"2 3 4 1 5 6");
  AssignOrdering(o_output,4,"2 3 4 5 1 6");
  AssignOrdering(o_output,5,"6 1 2 5 3 4");
  AssignOrdering(o_output,6,"2 5 3 4 1 6");
  AssignOrdering(o_output,7,"2 1 3 4 5 6");
  AssignOrdering(o_output,8,"2 1 5 3 4 6");
  
  int switchedlabel[NOutputP1][NVec];
  
  Determine_switchedlabels(switchedlabel,o_output[2],o_output,1);
  Determine_switchedindex(idxvec_reordered,switchedlabel,1);
  for (int v = 3; v < NOutputP1; v++) {
    Determine_switchedlabels(switchedlabel,o_output[2],o_output,v);
    Determine_switchedindex(idxvec_reordered,switchedlabel,v);
  }

}

// ------------------------------------------------------------------


// ------------------------------------------------------------------
void ZLightEventProb2Jet::changeVars(const vector<double>& parameters)
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
double ZLightEventProb2Jet::matrixElement() const
{

   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;
   typedef SimpleArray<DHELAS::HelArray, 8> Array8;
   typedef SimpleArray<DHELAS::HelArray, 16> Array16;
   typedef SimpleArray<DHELAS::HelArray, 32> Array32;
   typedef SimpleArray<DHELAS::HelArray, 64> Array64;

   using MEConstants::zMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zWidth;

   //   MEConstants::PrintAllConstants(alphas_process);

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factorGG[2]  = {doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0),
				 doublecomplex(MEConstants::GetAdjusted_gg(alphas_process), 0)};

   doublecomplex factorSGG[2]    = {doublecomplex(MEConstants::GetAdjusted_sgg(alphas_process), 0),
				    doublecomplex(0, 0)};

   doublecomplex zFactorU[2] = {doublecomplex(-MEConstants::gzu1, 0),
                                doublecomplex(-MEConstants::gzu2, 0)};
   doublecomplex zFactorD[2] = {doublecomplex(-MEConstants::gzd1, 0),
                                doublecomplex(-MEConstants::gzd2, 0)};
   doublecomplex zFactorL[2] = {doublecomplex(-MEConstants::gzl1, 0),
                                doublecomplex(-MEConstants::gzl2, 0)};

   //select the specific constants (based on whether we have a u or a d final state)
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



//    //a check:
//    for (int k = 0; k < NVec; k++) {
//      cout << "switchedlabel4[" << k << "]=" << switchedlabel4[k] << endl;
//      cout << "o_output4[" << k << "]=" << o_output4[k] << endl;
//    }

//    int switchedlabel1[NVec];
//    for (int k = 0; k < NVec; k++) {
//      if ( o_target[k]!=o_output1[k] ) {
//        for (int t=k+1; t<NVec; t++ ) {
// 	 if ( o_output1[t]==o_target[k] ) {
// 	   switchedlabel1[k]=t;
// 	   o_output1[t]=o_output1[k];
// 	   o_output1[k]=o_target[k];
// 	 }
//        }
//      } else {
//        switchedlabel1[k]=-1;
//      }
//    }

   if (partons->getLepCharge() > 0) {
     // Calculate the lepton only once per integration
     static Array2 vec3;
     static double lepE = 0;
     if (lepE != partons->getLepton().E()) {
       vec3 = DHELAS::ixxxxx<2>(partons->getLepton(), 0, -1);
       lepE = partons->getLepton().E();
     }

     /// The negatively charged lepton is 'missing'
     Array2 vec4 = DHELAS::oxxxxx<2>(partons->getNeutrino(), 0, 1);;

     Array2 vec1;     
     Array2 vec2;
     if ( !swapPartonMom ) {
       vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
       vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
     } else {
       vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
       vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
     }
     Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
     Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, +1);
     
     Array4 vec7 = DHELAS::jioxxx(vec3, vec4, zFactorL, zMass, zWidth);
     Array8 vec8 = DHELAS::fvixxx(vec2, vec7, zFactorJJ, 0, 0);
     Array16 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);
     OutputType output[NOutputP1];
     output[1] = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);//input order = 6 1 2 3 4 5
     
     Array4 vec10 = DHELAS::fvoxxx(vec5,vec1,factorGG,0,0);
     Array16 vec11 = DHELAS::fvoxxx(vec10,vec7,zFactorJJ,0,0);
   
     output[2] = DHELAS::iovxxx(vec2,vec11,vec6,factorGG);//input order = 2 5 1 3 4 6 : Use as the template
     
     
     Array16 vec12 = DHELAS::fvixxx(vec8,vec1,factorGG,0,0);
     output[3] = DHELAS::iovxxx(vec12,vec5,vec6,factorGG);//input order = 2 3 4 1 5 6
     output[4] = DHELAS::iovxxx(vec8,vec10,vec6,factorGG);//input order = 2 3 4 5 1 6
     
     Array8 vec13 = DHELAS::fvoxxx(vec5,vec7,zFactorJJ,0,0);
     Array16 vec14 = DHELAS::jioxxx(vec2,vec13,factorGG,0,0);
     
     output[5] = DHELAS::vvvxxx(vec6,vec1,vec14,factorSGG);//input order = 6 1 2 5 3 4
     
     Array16 vec15 = DHELAS::fvoxxx(vec13,vec1,factorGG,0,0);
     
     output[6] = DHELAS::iovxxx(vec2,vec15,vec6,factorGG);//input order = 2 5 3 4 1 6
     
     Array4 vec16 = DHELAS::fvixxx(vec2,vec1,factorGG,0,0);
     Array16 vec17 = DHELAS::fvixxx(vec16,vec7,zFactorJJ,0,0);
     
     output[7] = DHELAS::iovxxx(vec17,vec5,vec6,factorGG);//input order = 2 1 3 4 5 6
     output[8] = DHELAS::iovxxx(vec16,vec13,vec6,factorGG);//input order = 2 1 5 3 4 6 

     ///Reorder all vectors to match the second one
     Reorder(output[1],idxvec_reordered,1);
     for (int v = 3; v < NOutputP1; v++) {
       Reorder(output[v],idxvec_reordered,v);
     }
     
     for (unsigned i = 0; i < vecSize; ++i)
       {
	 doublecomplex temp1 = -output[1][i]+ output[2][i] + output[4][i] - output[5][i] + output[6][i];
	 doublecomplex temp2 =  output[1][i] + output[3][i] + output[5][i] + output[7][i] + output[8][i];
	 doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	 doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;
	 answer+= (m1+m2).real();
	 //cout << "i=" << i << " helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;
       }
     
   } else {
     
     // Calculate the lepton only once per integration
     static Array2 vec4;
     static double lepE = 0;
     if (lepE != partons->getLepton().E()) {
       vec4 = DHELAS::oxxxxx<2>(partons->getLepton(), 0, 1);
       lepE = partons->getLepton().E();
     }

     /// The positively charged lepton is 'missing'
     Array2 vec3 = DHELAS::ixxxxx<2>(partons->getNeutrino(), 0, -1);

     /// The rest of the computation is identical to the LepCharge>0 case
     Array2 vec1;     
     Array2 vec2;
     if ( !swapPartonMom ) {
       vec1 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
       vec2 = DHELAS::ixxxxx<2>(partons->getParton2(), 0, +1);
     } else {
       vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
       vec2 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, +1);
     }
     Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, +1);
     Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, +1);
     
     Array4 vec7 = DHELAS::jioxxx(vec3, vec4, zFactorL, zMass, zWidth);
     Array8 vec8 = DHELAS::fvixxx(vec2, vec7, zFactorJJ, 0, 0);
     Array16 vec9 = DHELAS::jioxxx(vec8, vec5, factorGG, 0, 0);
     OutputType output[NOutputP1];
     output[1] = DHELAS::vvvxxx(vec6, vec1, vec9, factorSGG);//input order = 6 1 2 3 4 5
     
     Array4 vec10 = DHELAS::fvoxxx(vec5,vec1,factorGG,0,0);
     Array16 vec11 = DHELAS::fvoxxx(vec10,vec7,zFactorJJ,0,0);
   
     output[2] = DHELAS::iovxxx(vec2,vec11,vec6,factorGG);//input order = 2 5 1 3 4 6 : Use as the template
     
     
     Array16 vec12 = DHELAS::fvixxx(vec8,vec1,factorGG,0,0);
     output[3] = DHELAS::iovxxx(vec12,vec5,vec6,factorGG);//input order = 2 3 4 1 5 6
     output[4] = DHELAS::iovxxx(vec8,vec10,vec6,factorGG);//input order = 2 3 4 5 1 6
     
     Array8 vec13 = DHELAS::fvoxxx(vec5,vec7,zFactorJJ,0,0);
     Array16 vec14 = DHELAS::jioxxx(vec2,vec13,factorGG,0,0);
     
     output[5] = DHELAS::vvvxxx(vec6,vec1,vec14,factorSGG);//input order = 6 1 2 5 3 4
     
     Array16 vec15 = DHELAS::fvoxxx(vec13,vec1,factorGG,0,0);
     
     output[6] = DHELAS::iovxxx(vec2,vec15,vec6,factorGG);//input order = 2 5 3 4 1 6
     
     Array4 vec16 = DHELAS::fvixxx(vec2,vec1,factorGG,0,0);
     Array16 vec17 = DHELAS::fvixxx(vec16,vec7,zFactorJJ,0,0);
     
     output[7] = DHELAS::iovxxx(vec17,vec5,vec6,factorGG);//input order = 2 1 3 4 5 6
     output[8] = DHELAS::iovxxx(vec16,vec13,vec6,factorGG);//input order = 2 1 5 3 4 6 

     ///Reorder all vectors to match the second one
     Reorder(output[1],idxvec_reordered,1);
     for (int v = 3; v < NOutputP1; v++) {
       Reorder(output[v],idxvec_reordered,v);
     }
     
     for (unsigned i = 0; i < vecSize; ++i)
       {
	 doublecomplex temp1 = -output[1][i]+ output[2][i] + output[4][i] - output[5][i] + output[6][i];
	 doublecomplex temp2 =  output[1][i] + output[3][i] + output[5][i] + output[7][i] + output[8][i];
	 doublecomplex m1 = ( temp1*16.0 -temp2*2.0)*std::conj(temp1)/3.0;
	 doublecomplex m2 = ( -temp1*2.0 +temp2*16.0)*std::conj(temp2)/3.0;
	 answer+= (m1+m2).real();
	 //cout << "i=" << i << " helicity 'amplitude'" << (m1+m2).real() << " + " << (m1+m2).imag() << "i" << endl;
       }


   }
   
   answer /= 96; //relative weight
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
   
  double fortranArray_vl[6][4];
  makeFortranArray_qqvl(fortranArray_vl);
  // Evalute the matrix element according to madgraph
  //double mhiggs = m_massHiggs; // to get rid of the const identifier
  double mw  = wMass; // to get rid of the const identifier
  double mz = zMass;
  double an = 0;
  if (partons->getLepCharge() > 0)
    zugm_(fortranArray, &mz, &mw, &an);
    //zugaltm_(fortranArray, &mz, &mw, &an);
    //zdgm_(fortranArray, &mz, &mw, &an);
    //zdgaltm_(fortranArray, &mz, &mw, &an);
  else
    zugm_(fortranArray_vl, &mz, &mw, &an);
    //zugaltm_(fortranArray_vl, &mz, &mw, &an);
    //zdgm_(fortranArray_vl, &mz, &mw, &an);
    //zdgaltm_(fortranArray_vl, &mz, &mw, &an);

  cout << "Madgraph answer= " << an << endl;
   

  // Exit right away
  exit(1);  
 

#endif

  //cout << "answer=" << answer << endl;
  return answer;

}

void ZLightEventProb2Jet::setPartonTypes() const
{
  if ( !swapQuarkFlavor ) {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kUp);
    } else {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kGluon);
    }
  } else {
    if ( !swapPartonMom ) {
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kDown);
    } else {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kGluon);
    }
  }

}


void ZLightEventProb2Jet::AssignOrdering(int o_vec[NOutputP1][NVec], int output_vecidx, const char* order) const
//// Store the ordering pattern in o_vec
//// output_vecidx is used to select the output vector which will be reordered
{
  istrstream ord(order);
  for (int k = 0; k < NVec; k++) {
    ord >> o_vec[output_vecidx][k];
  }
}


void ZLightEventProb2Jet::Determine_switchedlabels(int sl[NOutputP1][NVec], int o_target[NVec], int o_output[NOutputP1][NVec], int output_vecidx) const
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

void ZLightEventProb2Jet::Determine_switchedindex(int idxvec_reordered[NOutputP1][VSize], int switchedlabel[NOutputP1][NVec], int output_vecidx) const
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

void ZLightEventProb2Jet::Reorder(OutputType& output, const int idxvec_reordered[NOutputP1][VSize], int output_vecidx) const
////Reorder the output vector to match the (previously selected) template.
{
  OutputType output_temp;
  for (int j = 0; j < VSize; j++) {
    output_temp[idxvec_reordered[output_vecidx][j]]=output[j];
  }
  output = output_temp;
} 

void ZLightEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool ZLightEventProb2Jet::onSwitch()
{
  
  switch (getLoop()) {
  case 0:
    swapQuarkFlavor=false;
    swapPartonMom=false; 
    //swapPartonMom=true; //when testing alternate functions
    //swapQuarkFlavor=true; //when testing with d instead of u
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
