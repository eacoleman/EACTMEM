#include "TAMUWW/MatrixElement/interface/WbbEventProb3Jet.hh"

#include <iostream>

#include <cmath>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

extern "C"
{
   void* mywbb3jet_(double[][4], const double*, double*);
   void* mywbb3jet2_(double[][4], const double*, double*);
}

WbbEventProb3Jet::WbbEventProb3Jet(Integrator& integrator,
                                   const TransferFunction& bTF,
                                   const TransferFunction& gluonTF) :
  EventProb3Jet(DEFS::EP::Wbb, integrator, 4, 2, bTF, gluonTF)

{}


unsigned WbbEventProb3Jet::getProbMax() const
{
   return getMeasuredColl()->getNBtags() > 1 ? 1 : 4;
}

double WbbEventProb3Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

//   const double wWidth = MEConstants::wWWidth;

   double answer = 0;

//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};
//   std::cerr << "factor[0]: " << factor[0] << std::endl;
   doublecomplex gFactor[2] = {doublecomplex(-MEConstants::sgg, 0),
                               doublecomplex(-MEConstants::sgg, 0)};
   doublecomplex singleG[2] = {doublecomplex(MEConstants::sgg, 0),
                               doublecomplex(0, 0)};

   enum {vecSize = 8};
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

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(2), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec8 = DHELAS::fvixxx(vec0, vec6, gFactor, 0, 0);
      Array1 vec9 = DHELAS::fvoxxx(vec1, vec7, factor, 0, 0);
      Array2 vec10 = DHELAS::jioxxx(vec8, vec9, gFactor, 0, 0);
      OutputType output0 = DHELAS::iovxxx(vec5, vec4, vec10, gFactor);

      Array2 vec11 = DHELAS::fvoxxx(vec1, vec6, gFactor, 0, 0);
      Array2 vec12 = DHELAS::fvoxxx(vec11, vec7, factor, 0, 0);
      Array2 vec13 = DHELAS::jioxxx(vec0, vec12, gFactor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec5, vec4, vec13, gFactor);

      Array4 vec14 = DHELAS::fvoxxx(vec4, vec6, gFactor, bMass, 0);
      Array1 vec15 = DHELAS::jioxxx(vec0, vec9, gFactor, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec5, vec14, vec15, gFactor);

      Array2 vec16 = DHELAS::fvoxxx(vec4, vec15, gFactor, bMass, 0);
      OutputType output3 = DHELAS::iovxxx(vec5, vec16, vec6, gFactor);

      Array2 vec17 = DHELAS::fvoxxx(vec9, vec6, gFactor, 0, 0);
      Array2 vec18 = DHELAS::jioxxx(vec0, vec17, gFactor, 0, 0);
      OutputType output4 = DHELAS::iovxxx(vec5, vec4, vec18, gFactor);

      Array2 vec19 = DHELAS::jvvxxx(vec6, vec15, singleG, 0, 0);
      OutputType output5 = DHELAS::iovxxx(vec5, vec4, vec19, gFactor);

      Array2 vec20 = DHELAS::fvixxx(vec8, vec7, factor, 0, 0);
      Array2 vec21 = DHELAS::jioxxx(vec20, vec1, gFactor, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec5, vec4, vec21, gFactor);

      Array1 vec22 = DHELAS::fvixxx(vec0, vec7, factor, 0, 0);
      Array2 vec23 = DHELAS::jioxxx(vec22, vec11, gFactor, 0, 0);
      OutputType output7 = DHELAS::iovxxx(vec5, vec4, vec23, gFactor);

      Array1 vec24 = DHELAS::jioxxx(vec22, vec1, gFactor, 0, 0);
      OutputType output8 = DHELAS::iovxxx(vec5, vec14, vec24, gFactor);

      Array2 vec25 = DHELAS::fvoxxx(vec4, vec24, gFactor, bMass, 0);
      OutputType output9 = DHELAS::iovxxx(vec5, vec25, vec6, gFactor);

      Array2 vec26 = DHELAS::fvixxx(vec22, vec6, gFactor, 0, 0);
      Array2 vec27 = DHELAS::jioxxx(vec26, vec1, gFactor, 0, 0);
      OutputType output10 = DHELAS::iovxxx(vec5, vec4, vec27, gFactor);

      Array2 vec28 = DHELAS::jvvxxx(vec6, vec24, singleG, 0, 0);
      OutputType output11 = DHELAS::iovxxx(vec5, vec4, vec28, gFactor);

   // output and output2 are the same size by construction
      const double data[vecSize][vecSize] = {{8, -1, 7, -2}, {-1, 8, -2, 7},
                                             {7, -2, 8, -1}, {-2, 7, -1, 8}};
      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp[vecSize] = {-output0[i] - output6[i] - output10[i],
                                        -output1[i] - output4[i] - output7[i],
                                        -output2[i] - output5[i] - output8[i]
                                        - output11[i],
                                        -output3[i] + output5[i] - output9[i]
                                        + output11[i]};

//         double temp = 0;
         for (unsigned ii = 0; ii < vecSize; ++ii)
         {
            doublecomplex ztemp;
            for (unsigned j = 0; j < vecSize; ++j)
            {
               ztemp += data[ii][j] * jamp[j];
            }
            answer += std::real(ztemp * std::conj(jamp[ii])) / 3;
         }
//         answer += temp;
      }
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

      Array1 vec0 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      Array1 vec1 = DHELAS::ixxxxx<1>(partons->getParton2(), 0, 1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(2), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec8 = DHELAS::fvoxxx(vec0, vec6, gFactor, 0, 0);
      Array1 vec9 = DHELAS::fvixxx(vec1, vec7, factor, 0, 0);
      Array2 vec10 = DHELAS::jioxxx(vec9, vec8, gFactor, 0, 0);
      OutputType output0 = DHELAS::iovxxx(vec4, vec5, vec10, gFactor);

      Array2 vec11 = DHELAS::fvixxx(vec1, vec6, gFactor, 0, 0);
      Array2 vec12 = DHELAS::fvixxx(vec11, vec7, factor, 0, 0);
      Array2 vec13 = DHELAS::jioxxx(vec12, vec0, gFactor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec4, vec5, vec13, gFactor);

      Array4 vec14 = DHELAS::fvixxx(vec4, vec6, gFactor, bMass, 0);
      Array1 vec15 = DHELAS::jioxxx(vec9, vec0, gFactor, 0, 0);
      OutputType output2 = DHELAS::iovxxx(vec14, vec5, vec15, gFactor);
      std::swap(output2[1], output2[2]);
      std::swap(output2[5], output2[6]);

      Array2 vec16 = DHELAS::fvixxx(vec4, vec15, gFactor, bMass, 0);
      OutputType output3 = DHELAS::iovxxx(vec16, vec5, vec6, gFactor);

      Array2 vec17 = DHELAS::fvixxx(vec9, vec6, gFactor, 0, 0);
      Array2 vec18 = DHELAS::jioxxx(vec17, vec0, gFactor, 0, 0);
      OutputType output4 = DHELAS::iovxxx(vec4, vec5, vec18, gFactor);

      Array2 vec19 = DHELAS::jvvxxx(vec6, vec15, singleG, 0, 0);
      OutputType output5 = DHELAS::iovxxx(vec4, vec5, vec19, gFactor);

      Array2 vec20 = DHELAS::fvoxxx(vec8, vec7, factor, 0, 0);
      Array2 vec21 = DHELAS::jioxxx(vec1, vec20, gFactor, 0, 0);
      OutputType output6 = DHELAS::iovxxx(vec4, vec5, vec21, gFactor);

      Array1 vec22 = DHELAS::fvoxxx(vec0, vec7, factor, 0, 0);
      Array2 vec23 = DHELAS::jioxxx(vec11, vec22, gFactor, 0, 0);
      OutputType output7 = DHELAS::iovxxx(vec4, vec5, vec23, gFactor);

      Array1 vec24 = DHELAS::jioxxx(vec1, vec22, gFactor, 0, 0);
      OutputType output8 = DHELAS::iovxxx(vec14, vec5, vec24, gFactor);
      std::swap(output8[1], output8[2]);
      std::swap(output8[5], output8[6]);

      Array2 vec25 = DHELAS::fvixxx(vec4, vec24, gFactor, bMass, 0);
      OutputType output9 = DHELAS::iovxxx(vec25, vec5, vec6, gFactor);

      Array2 vec26 = DHELAS::fvoxxx(vec22, vec6, gFactor, 0, 0);
      Array2 vec27 = DHELAS::jioxxx(vec1, vec26, gFactor, 0, 0);
      OutputType output10 = DHELAS::iovxxx(vec4, vec5, vec27, gFactor);

      Array2 vec28 = DHELAS::jvvxxx(vec6, vec24, singleG, 0, 0);
      OutputType output11 = DHELAS::iovxxx(vec4, vec5, vec28, gFactor);

   // output and output2 are the same size by construction
      const double data[vecSize][vecSize] = {{8, -1, 7, -2}, {-1, 8, -2, 7},
                                             {7, -2, 8, -1}, {-2, 7, -1, 8}};
      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp[vecSize] = {output0[i] + output6[i] + output10[i],
                                        output1[i] + output4[i] + output7[i],
                                        output2[i] - output5[i] + output8[i]
                                        - output11[i],
                                        output3[i] + output5[i] + output9[i]
                                        + output11[i]};

//         double temp = 0;
         for (unsigned ii = 0; ii < vecSize; ++ii)
         {
            doublecomplex ztemp;
            for (unsigned j = 0; j < vecSize; ++j)
            {
               ztemp += data[ii][j] * jamp[j];
            }
            answer += std::real(ztemp * std::conj(jamp[ii])) / 3;
         }
//         answer += temp;
      }
   }

//   std::cerr << "Answer: " << answer << std::endl;

   return answer;

//   const PartonColl* partons = getPartonColl();

   double fortranArray[7][4];
   makeFortranArray(fortranArray);

//   double wMass = MEConstants::wMass;

// KLUDGE
//   double wMass = partons->getTopMass();

//   double answer;

   if (partons->getLepCharge() > 0)
      mywbb3jet_(fortranArray, &wMass, &answer);
   else
      mywbb3jet2_(fortranArray, &wMass, &answer);

   return answer;
}

void WbbEventProb3Jet::setPartonTypes() const
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

void WbbEventProb3Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

bool WbbEventProb3Jet::onSwitch()
{
   if (getLoop())
   {
      if (getMeasuredColl()->getNBtags() > 1)
         return false;

      swapJets(1, 2);
   }
   return true;
}
