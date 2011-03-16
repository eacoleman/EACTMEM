#include "TAMUWW/MatrixElement/interface/WcEventProb2Jet.hh"

#include <iostream>

#include <cmath>
#include <string>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

using std::string;
using std::vector;

extern "C"
{
   void* mywcj_(double[][4], const double*, double*);
   void* mywcj2_(double[][4], const double*, double*);
}

typedef SimpleArray<DHELAS::HelArray, 1> Array1;
typedef SimpleArray<DHELAS::HelArray, 2> Array2;
typedef SimpleArray<DHELAS::HelArray, 4> Array4;

using std::cout;
using std::endl;

WcEventProb2Jet::WcEventProb2Jet(Integrator& integrator,
                                 const TransferFunction& lighttf,
                                 const TransferFunction& gluontf) :
  EventProb2Jet(DEFS::EP::Wc, integrator, 3, 4, lighttf),
  m_gluonTF(gluontf) {}

WcEventProb2Jet::WcEventProb2Jet(Integrator& integrator,
                                 const TransferFunction& lighttf,
                                 const TransferFunction& gluontf,
				 DEFS::EP::Type ept, unsigned iterations) :
   EventProb2Jet(ept, integrator, 3, iterations, lighttf),
   m_gluonTF(gluontf) { }

void WcEventProb2Jet::setDynamicBounds()
{
   const float lowPercent = .01;
   const float highPercent = .02;
   double lower, upper;
   getBTF().getBounds(getMeasuredColl()->getFullJet(0), lowPercent,
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

unsigned WcEventProb2Jet::getProbMax() const
{
   return getMeasuredColl()->getNBtags() == 1 ? 2 : 4;
}

void WcEventProb2Jet::changeVars(const vector<double>& parameters)
{
   using AuxFunctions::Math::square;

   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(std::sqrt(square(MEConstants::cMass) + square(parameters[1])));
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

double WcEventProb2Jet::matrixElement() const
{
   const PartonColl* partons = getPartonColl();

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   double answer = 0;

//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};
//   std::cerr << "factor[0]: " << factor[0] << std::endl;
   doublecomplex gFactor[2] = {doublecomplex(-MEConstants::gg, 0),
                               doublecomplex(-MEConstants::gg, 0)};
   doublecomplex singleG[2] = {doublecomplex(MEConstants::gg, 0),
                               doublecomplex(0, 0)};
   //if (debug == 0){
     //print the partoncollection
     //std::cout<<" Wc partons "<<(*partons);
   //}

   enum {vecSize = 4};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   if (partons->getLepCharge() > 0)
   {
      // Calculate the lepton only once per integration
      static Array1 vec2;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
         lepE = partons->getLepton().E();
      }

      Array1 vec0 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec6 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec7 = DHELAS::fvixxx(vec4, vec1, gFactor, 0, 0);
      Array2 vec8 = DHELAS::fvoxxx(vec0, vec5, gFactor, 0, 0);
      OutputType output0 = DHELAS::iovxxx(vec7, vec8, vec6, factor);

      Array4 vec9 = DHELAS::jvvxxx(vec5, vec1, singleG, 0, 0);
      Array1 vec10 = DHELAS::fvoxxx(vec0, vec6, factor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec4, vec10, vec9, gFactor);

      Array2 vec11 = DHELAS::fvixxx(vec4, vec5, gFactor, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec11, vec10, vec1, gFactor);

      Array4 vec12 = DHELAS::fvixxx(vec7, vec5, gFactor, 0, 0);
      OutputType output3 = DHELAS::iovxxx(vec12, vec0, vec6, factor);

      Array1 vec13 = DHELAS::fvixxx(vec4, vec6, factor, 0, 0);
      OutputType output4 = DHELAS::iovxxx(vec13, vec8, vec1, gFactor);

      OutputType output5 = DHELAS::iovxxx(vec13, vec0, vec9, gFactor);

      Array2 vec14 = DHELAS::fvoxxx(vec0, vec1, gFactor, 0, 0);
      Array2 vec15 = DHELAS::fvoxxx(vec14, vec6, factor, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec4, vec15, vec5, gFactor);

      OutputType output7 = DHELAS::iovxxx(vec13, vec14, vec5, gFactor);

      std::swap(output1[1], output1[2]);
      std::swap(output2[1], output2[2]);
      std::swap(output4[1], output4[2]);
      std::swap(output5[1], output5[2]);

//      for (unsigned ii = 0; ii < 4; ++ii)
//         std::cerr << "Amp " << ii << " " << output0[ii] << " " << std::endl
//                << output1[ii] << " " << std::endl
//                << output2[ii] << " " << std::endl
//                << output3[ii] << " " << std::endl
//                << output4[ii] << " " << std::endl
//                << output5[ii] << " " << std::endl
//                << output6[ii] << " " << std::endl
//                << output7[ii] << std::endl;

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp1 = -output0[i] - output1[i] - output3[i]
            - output4[i] - output5[i];
         doublecomplex jamp2 = output1[i] - output2[i] + output5[i]
            - output6[i] - output7[i];
  
//         std::cerr << "jamp: " << jamp1 << " " << jamp2 << std::endl;

         doublecomplex ztemp = 16. * jamp1 - 2. * jamp2;
         answer += std::real(ztemp * std::conj(jamp1) / 3.);
         
         doublecomplex ztemp2 = -2. * jamp1 + 16. * jamp2;
         answer += std::real(ztemp2 * std::conj(jamp2) / 3.);
      }
//      const unsigned swapSize = 4;
//      const unsigned swapVecs[swapSize] = {1, 2, 4, 5};
//      for (unsigned i = 0; i < swapSize; ++i)
//      {
//         std::swap(output[swapVecs[i]][1], output[swapVecs[i]][2]);
//      }

//      for (unsigned ii = 0; ii < 16; ++ii)
//         for (unsigned i = 0; i < vec[ii].size(); ++i)
//         {
//            if (ii == 1 || ii == 5 || ii == 9)           
//            std::cerr << "W" << ii+1 << " Vector " << i << ": " << vec[ii][i].p1 << " " << vec[ii][i].p2 << " " << vec[ii][i].p3 << " " << vec[ii][i].p4 << " " << vec[ii][i].p5 << " " << vec[ii][i].p6 << std::endl;
//         }

   // output and output2 are the same size by construction
//      for (unsigned i = 0; i < output[0].size(); ++i)
//      {
//         doublecomplex jamp1 = -output[0][i] - output[1][i] - output[3][i]
//            - output[4][i] - output[5][i];
//         doublecomplex jamp2 = output[1][i] - output[2][i] + output[5][i]
//            - output[6][i] - output[7][i];
//         
////      std::cerr << "Jamp: " << jamp1 << " " << jamp2 << std::endl;
//         
//         doublecomplex ztemp = 16. * jamp1 - 2. * jamp2;
//         answer += std::real(ztemp * std::conj(jamp1) / 3.);
//         
//         doublecomplex ztemp2 = -2. * jamp1 + 16. * jamp2;
//         answer += std::real(ztemp2 * std::conj(jamp2) / 3.);
////      std::cerr << "ztemp: " << ztemp << " " << ztemp2 << std::endl;
////      double me = std::real(ztemp * std::conj(jamp1) / 3.) + std::real(ztemp2 * std::conj(jamp2) / 3.);
////      std::cerr << "ME: " << me << std::endl;
////      for (unsigned j = 0; j < 8; ++j)
////      {
////         std::cerr << "Output " << j << ": " << output[j][i] << std::endl;
////      }
//      }
   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec2;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
         lepE = partons->getLepton().E();
      }

      Array2 vec0 = DHELAS::vxxxxx<2>(partons->getParton1(), 0, -1);
      Array1 vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, 1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, 1);
      Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec6 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array4 vec7 = DHELAS::jvvxxx(vec5, vec0, singleG, 0, 0);
      Array1 vec8 = DHELAS::fvixxx(vec1, vec6, factor, 0, 0);
      OutputType output0 = DHELAS::iovxxx(vec8, vec4, vec7, gFactor);

      Array2 vec9 = DHELAS::fvoxxx(vec4, vec0, gFactor, 0, 0);
      Array2 vec10 = DHELAS::fvixxx(vec1, vec5, gFactor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec10, vec9, vec6, factor);

      Array2 vec11 = DHELAS::fvoxxx(vec4, vec5, gFactor, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec8, vec11, vec0, gFactor);

      Array4 vec12 = DHELAS::fvoxxx(vec9, vec5, gFactor, 0, 0);
      OutputType output3 = DHELAS::iovxxx(vec1, vec12, vec6, factor);

      Array1 vec13 = DHELAS::fvoxxx(vec4, vec6, factor, 0, 0);
      OutputType output4 = DHELAS::iovxxx(vec1, vec13, vec7, gFactor);

      OutputType output5 = DHELAS::iovxxx(vec10, vec13, vec0, gFactor);

      Array2 vec14 = DHELAS::fvixxx(vec1, vec0, gFactor, 0, 0);
      Array2 vec15 = DHELAS::fvixxx(vec14, vec6, factor, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec15, vec4, vec5, gFactor);

      OutputType output7 = DHELAS::iovxxx(vec14, vec13, vec5, gFactor);

      std::swap(output0[1], output0[2]);
      std::swap(output1[1], output1[2]);
      std::swap(output2[1], output2[2]);
      std::swap(output4[1], output4[2]);
      std::swap(output5[1], output5[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp1 = output0[i] - output1[i] - output3[i]
            + output4[i] - output5[i];
         doublecomplex jamp2 = -output0[i] - output2[i] - output4[i]
            - output6[i] - output7[i];
         
         doublecomplex ztemp = 16. * jamp1 - 2. * jamp2;
         answer += std::real(ztemp * std::conj(jamp1) / 3.);
         
         doublecomplex ztemp2 = -2. * jamp1 + 16. * jamp2;
         answer += std::real(ztemp2 * std::conj(jamp2) / 3.);
      }

//      DHELAS::HelVec vec[16];
//      DHELAS::vxxxxx(partons->getParton1(), 0, -1, vec[0], kTwo);
//      DHELAS::ixxxxx(partons->getParton2(), 0, 1, vec[1], kOne);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::oxxxxx(partons->getJet(0), 0, 1, vec[4], kOne);
//      DHELAS::vxxxxx(partons->getJet(1), 0, 1, vec[5], kTwo);
//
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[6]);
//      DHELAS::jvvxxx(vec[5], vec[0], singleG, 0, 0, vec[7]);
//      DHELAS::fvixxx(vec[1], vec[6], factor, 0, 0, vec[8]);
//      DHELAS::iovxxx(vec[8], vec[4], vec[7], gFactor, output[0]);
//
//      DHELAS::fvoxxx(vec[4], vec[0], gFactor, 0, 0, vec[9]);
//      DHELAS::fvixxx(vec[1], vec[5], gFactor, 0, 0, vec[10]);
//      DHELAS::iovxxx(vec[10], vec[9], vec[6], factor, output[1]);
//
//      DHELAS::fvoxxx(vec[4], vec[5], gFactor, 0, 0, vec[11]);
//      DHELAS::iovxxx(vec[8], vec[11], vec[0], gFactor, output[2]);
//
//      DHELAS::fvoxxx(vec[9], vec[5], gFactor, 0, 0, vec[12]);
//      DHELAS::iovxxx(vec[1], vec[12], vec[6], factor, output[3]);
//
//      DHELAS::fvoxxx(vec[4], vec[6], factor, 0, 0, vec[13]);
//      DHELAS::iovxxx(vec[1], vec[13], vec[7], gFactor, output[4]);
//
//      DHELAS::iovxxx(vec[10], vec[13], vec[0], gFactor, output[5]);
//
//      DHELAS::fvixxx(vec[1], vec[0], gFactor, 0, 0, vec[14]);
//      DHELAS::fvixxx(vec[14], vec[6], factor, 0, 0, vec[15]);
//      DHELAS::iovxxx(vec[15], vec[4], vec[5], gFactor, output[6]);
//
//      DHELAS::iovxxx(vec[14], vec[13], vec[5], gFactor, output[7]);
//      const unsigned swapSize = 5;
//      const unsigned swapVecs[swapSize] = {0, 1, 2, 4, 5};
//      for (unsigned i = 0; i < swapSize; ++i)
//      {
//         std::swap(output[swapVecs[i]][1], output[swapVecs[i]][2]);
//      }
//
//   // output and output2 are the same size by construction
//      for (unsigned i = 0; i < output[0].size(); ++i)
//      {
//         doublecomplex jamp1 = output[0][i] - output[1][i] - output[3][i]
//            + output[4][i] - output[5][i];
//         doublecomplex jamp2 = -output[0][i] - output[2][i] - output[4][i]
//            - output[6][i] - output[7][i];
//         
////      std::cerr << "Jamp: " << jamp1 << " " << jamp2 << std::endl;
//         
//         doublecomplex ztemp = 16. * jamp1 - 2. * jamp2;
//         answer += std::real(ztemp * std::conj(jamp1) / 3.);
//         
//         doublecomplex ztemp2 = -2. * jamp1 + 16. * jamp2;
//         answer += std::real(ztemp2 * std::conj(jamp2) / 3.);
////      std::cerr << "ztemp: " << ztemp << " " << ztemp2 << std::endl;
////      double me = std::real(ztemp * std::conj(jamp1) / 3.) + std::real(ztemp2 * std::conj(jamp2) / 3.);
////      std::cerr << "ME: " << me << std::endl;
////      for (unsigned j = 0; j < 8; ++j)
////      {
////         std::cerr << "Output " << j << ": " << output[j][i] << std::endl;
////      }
//      }
   }


   answer /= 96;
//   std::cerr << "Answer: " << answer << std::endl;

   return answer;

//   const PartonColl* partons = getPartonColl();
//
   double fortranArray[6][4];
   makeFortranArray(fortranArray);
//
//   double wMass = MEConstants::wMass;
//
//   double answer;
//
   if (partons->getLepCharge() > 0)
      mywcj_(fortranArray, &wMass, &answer);
   else
      mywcj2_(fortranArray, &wMass, &answer);

   std::cerr << "Old Answer: " << answer << std::endl;
   exit(1);
   return answer;
}


void WcEventProb2Jet::setPartonTypes() const
{

   // Try both combinations
   if (getLoop() % 2 == 0)
   {
     getMeasuredColl()->setParton1Type(kStrange);
     getMeasuredColl()->setParton2Type(kGluon);
   }
   else
   {
     getMeasuredColl()->setParton1Type(kGluon);
     getMeasuredColl()->setParton2Type(kStrange);
   }
}

void WcEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

double WcEventProb2Jet::totalTF() const
{
  /*
  if (debug ==0){
    cout<<"  getBTF().getTF="<< getBTF().getTF(getPartonColl()->getFullJet(0),
					      getMeasuredColl()->getFullJet(0))
	<<" m_gluonTF.getTF="<< m_gluonTF.getTF(getPartonColl()->getFullJet(1),
						getMeasuredColl()->getFullJet(1))<<endl;
  }
  */

  return getBTF().getTF (getPartonColl()->getFullJet(0),
			 getMeasuredColl()->getFullJet(0))
       * m_gluonTF.getTF(getPartonColl()->getFullJet(1),
		         getMeasuredColl()->getFullJet(1));
}

bool WcEventProb2Jet::onSwitch()
{
   if (getLoop() == 2)
   {
      if (getMeasuredColl()->getNBtags() == 1)
         return false;

      swapJets(0, 1);
   }
   return true;
}

WjgEventProb2Jet::WjgEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& lighttf,
                                   const TransferFunction& gluontf) :

  // BEFORE USING THIS ME NEED TO CHANGE DEFS::EP::Wc to something else
  WcEventProb2Jet(integrator, lighttf, gluontf, DEFS::EP::Wc, 2)
{}

unsigned WjgEventProb2Jet::getProbMax() const
{
   return EventProb::getProbMax();
}

void WjgEventProb2Jet::changeVars(const vector<double>& parameters)
{
   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(parameters[1]);
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}


void WjgEventProb2Jet::setPartonTypes() const
{

   if (getPartonColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kGluon);
   }
   else
   {
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kUp);
   }
}

