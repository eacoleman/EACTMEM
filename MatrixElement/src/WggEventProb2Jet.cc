#include "TAMUWW/MatrixElement/interface/WggEventProb2Jet.hh"

#include <iostream>

#include <cmath>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

using std::vector;

extern "C"
{
   void* mywgg_(double[][4], const double*, double*);
   void* mywgg2_(double[][4], const double*, double*);
}

WggEventProb2Jet::WggEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
   EventProb2Jet("Wgg (mistag)", integrator, 3, 1, tf)
{}

void WggEventProb2Jet::changeVars(const vector<double>& parameters)
{
   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(parameters[1]);
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
//   EventProb2Jet::changeVars(parameters);
}

double WggEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

   double answer = 0;

//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};
//   std::cerr << "factor[0]: " << factor[0] << std::endl;
   doublecomplex gFactor[2] = {doublecomplex(-MEConstants::gg, 0),
                               doublecomplex(-MEConstants::gg, 0)};
   doublecomplex singleG[2] = {doublecomplex(MEConstants::gg, 0),
                               doublecomplex(0, 0)};

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

      Array1 vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factor, 0, 0);
      Array2 vec9 = DHELAS::fvoxxx(vec8, vec5, gFactor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec1, vec9, vec6, gFactor);

      Array2 vec10 = DHELAS::fvixxx(vec1, vec5, gFactor, 0, 0);
      Array2 vec11 = DHELAS::fvixxx(vec10, vec7, factor, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec11, vec2, vec6, gFactor);

      Array1 vec12 = DHELAS::jioxxx(vec1, vec8, gFactor, 0, 0);
      OutputType output3 = DHELAS::vvvxxx(vec6, vec5, vec12, singleG);

      OutputType output4 = DHELAS::iovxxx(vec10, vec8, vec6, gFactor);

      Array2 vec13 = DHELAS::fvoxxx(vec2, vec5, gFactor, 0, 0);
      Array2 vec14 = DHELAS::fvoxxx(vec13, vec7, factor, 0, 0);
      OutputType output5 = DHELAS::iovxxx(vec1, vec14, vec6, gFactor);

      Array1 vec15 = DHELAS::fvixxx(vec1, vec7, factor, 0, 0);
      Array2 vec16 = DHELAS::fvixxx(vec15, vec5, gFactor, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec16, vec2, vec6, gFactor);

      Array1 vec17 = DHELAS::jioxxx(vec15, vec2, gFactor, 0, 0);
      OutputType output7 = DHELAS::vvvxxx(vec6, vec5, vec17, singleG);

      OutputType output8 = DHELAS::iovxxx(vec15, vec13, vec6, gFactor);

      std::swap(output3[1], output3[2]);
      std::swap(output7[1], output7[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp1 = output1[i] - output3[i] + output5[i]
            - output7[i] + output8[i];
         doublecomplex jamp2 = output2[i] + output3[i] + output4[i]
            + output6[i] + output7[i];
  
         doublecomplex ztemp = 16. * jamp1 - 2. * jamp2;
         answer += std::real(ztemp * std::conj(jamp1) / 3.);
         
         doublecomplex ztemp2 = -2. * jamp1 + 16. * jamp2;
         answer += std::real(ztemp2 * std::conj(jamp2) / 3.);
      }
   }
   else
   {
      // Calculate the lepton only once per integration
      static Array1 vec3;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
         lepE = partons->getLepton().E();
      }

      Array1 vec1 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
//      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec5 = DHELAS::vxxxxx<2>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factor, 0, 0);
      Array2 vec9 = DHELAS::fvoxxx(vec8, vec5, gFactor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec1, vec9, vec6, gFactor);

      Array2 vec10 = DHELAS::fvixxx(vec1, vec5, gFactor, 0, 0);
      Array2 vec11 = DHELAS::fvixxx(vec10, vec7, factor, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec11, vec2, vec6, gFactor);

      Array1 vec12 = DHELAS::jioxxx(vec1, vec8, gFactor, 0, 0);
      OutputType output3 = DHELAS::vvvxxx(vec6, vec5, vec12, singleG);

      OutputType output4 = DHELAS::iovxxx(vec10, vec8, vec6, gFactor);

      Array2 vec13 = DHELAS::fvoxxx(vec2, vec5, gFactor, 0, 0);
      Array2 vec14 = DHELAS::fvoxxx(vec13, vec7, factor, 0, 0);
      OutputType output5 = DHELAS::iovxxx(vec1, vec14, vec6, gFactor);

      Array1 vec15 = DHELAS::fvixxx(vec1, vec7, factor, 0, 0);
      Array2 vec16 = DHELAS::fvixxx(vec15, vec5, gFactor, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec16, vec2, vec6, gFactor);

      Array1 vec17 = DHELAS::jioxxx(vec15, vec2, gFactor, 0, 0);
      OutputType output7 = DHELAS::vvvxxx(vec6, vec5, vec17, singleG);

      OutputType output8 = DHELAS::iovxxx(vec15, vec13, vec6, gFactor);

      std::swap(output3[1], output3[2]);
      std::swap(output7[1], output7[2]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp1 = -output1[i] + output3[i] - output5[i]
            + output7[i] - output8[i];
         doublecomplex jamp2 = -output2[i] - output3[i] - output4[i]
            - output6[i] - output7[i];
  
         doublecomplex ztemp = 16. * jamp1 - 2. * jamp2;
         answer += std::real(ztemp * std::conj(jamp1) / 3.);
         
         doublecomplex ztemp2 = -2. * jamp1 + 16. * jamp2;
         answer += std::real(ztemp2 * std::conj(jamp2) / 3.);
      }
   }


   answer /= 96;
//   std::cerr << "New Answer: " << answer << std::endl;

   return answer;
//   const PartonColl* partons = getPartonColl();
//
   double fortranArray[6][4];
   makeFortranArray(fortranArray);
//
//   double topMass = partons->getTopMass();
// KLUDGE
//   const double wMass = partons->getTopMass();
//   const double wMass = MEConstants::wMass;
//
//   double answer;
//
////   for (int i = 0; i < 7; ++i)
////      for (int j = 0; j < 4; ++j)
////      {
////         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
////      }
////   exit+(1);
////
////   std::cerr << "Top mass: " << topMass << std::endl;
////   std::cerr << "W mass: " << wMass << std::endl;
//   
////   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
//
   if (partons->getLepCharge() > 0)
      mywgg_(fortranArray, &wMass, &answer);
   else
      mywgg2_(fortranArray, &wMass, &answer);

//   std::cerr << "Old answer: " << answer << std::endl;
//   exit(1);
   return answer;
}

void WggEventProb2Jet::setPartonTypes() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setParton1Type(kUp);
      getMeasuredColl()->setParton2Type(kDown);
   }
   else
   {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kUp);
   }
}

void WggEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using PeterFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}
