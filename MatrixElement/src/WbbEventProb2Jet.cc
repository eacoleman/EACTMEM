#include "TAMUWW/MatrixElement/interface/WbbEventProb2Jet.hh"

#include <iostream>

#include <cmath>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

extern "C"
{
   void* mywbb_(double[][4], const double*, double*);
   void* mywbb2_(double[][4], const double*, double*);
}

WbbEventProb2Jet::WbbEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
   EventProb2Jet("Wbb-bar", integrator, 3, 1, tf)
{}

double WbbEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

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

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, 1);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

//      std::cout << "0 1 2 3 4 5 " << vec0[0].p1 << " " << vec1[0].p1 << " " << vec2[0].p1
//                << " " << vec3[0].p1 << " (" << vec4[0].p1 << ", " << vec4[1].p1 << ") (" << vec5[0].p1 << ", " << vec5[1].p1
//                << std::endl;

      Array1 vec6 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array4 vec7 = DHELAS::jioxxx(vec5, vec4, gFactor, 0, 0);
      Array1 vec8 = DHELAS::fvoxxx(vec1, vec6, factor, 0, 0);
      OutputType output0 = DHELAS::iovxxx(vec0, vec8, vec7, gFactor);

      Array1 vec9 = DHELAS::fvixxx(vec0, vec6, factor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec9, vec1, vec7, gFactor);
//      std::cout << "6 7 8 9 " << vec6[0].p1 << " (" << vec7[0].p1 << ", " << vec7[1].p1
//                << ", " << vec7[2].p1 << ", " << vec7[3].p1 << ") " << vec8[0].p1 << " " << vec9[0].p1
//                << std::endl;

//      for (unsigned i = 0; i < 4; ++i)
//      {
//         std::cout << "vec7: " << vec7[i].p1 << " " << vec7[i].p2 << " "
//                   << vec7[i].p3 << " " << vec7[i].p4 << " " << vec7[i].p5
//                   << " " << vec7[i].p6 << std::endl;
//      }
//         
//      for (unsigned i = 0; i < vecSize; ++i)
//      {
//         std::cerr << "Amp1, amp2: " << output[i] << " " << output2[i]
//                   << std::endl;
//      }

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp = -output0[i] - output1[i];
//         std::cerr << "Jamp: " << jamp << std::endl;
         answer += 2 * std::norm(jamp);
      }
//   const unsigned ii = 6;
//   for (unsigned i = 0; i < vec[ii].size(); ++i)
//   {
//      std::cerr << "Vector " << i << ": " << vec[ii][i].p1 << " " << vec[ii][i].p2 << " " << vec[ii][i].p3 << " " << vec[ii][i].p4 << " " << vec[ii][i].p5 << " " << vec[ii][i].p6 << std::endl;
//   }
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

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, 1);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);

      Array1 vec6 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array4 vec7 = DHELAS::jioxxx(vec4, vec5, gFactor, 0, 0);
      Array1 vec8 = DHELAS::fvoxxx(vec1, vec6, factor, 0, 0);
      OutputType output0 = DHELAS::iovxxx(vec0, vec8, vec7, gFactor);

      Array1 vec9 = DHELAS::fvixxx(vec0, vec6, factor, 0, 0);
      OutputType output1 = DHELAS::iovxxx(vec9, vec1, vec7, gFactor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex jamp = -output0[i] - output1[i];
         answer += 2 * std::norm(jamp);
      }

//      DHELAS::HelVec vec[10];
//      DHELAS::ixxxxx(partons->getQuark1(), 0, 1, vec[0], kOne);
//      DHELAS::oxxxxx(partons->getQuark2(), 0, -1, vec[1], kOne);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[4], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), bMass, 1, vec[5], kTwo);
//
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[6]);
//      DHELAS::jioxxx(vec[4], vec[5], gFactor, 0, 0, vec[7]);
//      DHELAS::fvoxxx(vec[1], vec[6], factor, 0, 0, vec[8]);
//      DHELAS::iovxxx(vec[0], vec[8], vec[7], gFactor, output);
//
//      DHELAS::fvixxx(vec[0], vec[6], factor, 0, 0, vec[9]);
//      DHELAS::iovxxx(vec[9], vec[1], vec[7], gFactor, output2);
   }

   // output and output2 are the same size by construction
//   for (unsigned i = 0; i < output.size(); ++i)
//   {
//      doublecomplex jamp = -output[i] - output2[i];
//      answer += 2 * std::norm(jamp);
//      if (std::norm(output[i]) || std::norm(output2[i]))
//         std::cerr << "output: " << output[i] << " output2: " << output2[i] << std::endl;
//      if (2 * std::norm(jamp))
//         std::cerr << "Answer " << 2 * std::norm(jamp) << std::endl;
//   }

   answer /= 36;

//   std::cerr << "New answer: " << answer << std::endl;
   return answer;

//   const PartonColl* partons = getPartonColl();

   double fortranArray[6][4];
   makeFortranArray(fortranArray);

//   double wMass = MEConstants::wMass;

//   double answer;

   if (partons->getLepCharge() > 0)
      mywbb_(fortranArray, &wMass, &answer);
   else
      mywbb2_(fortranArray, &wMass, &answer);

   std::cerr << "Old answer: " << answer << std::endl;
   exit(1);

//   return answer;
}


void WbbEventProb2Jet::setQuarkIDs() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setProtonType(kUp);
      getMeasuredColl()->setAntiprotonType(kDown);
   }
   else
   {
      getMeasuredColl()->setProtonType(kDown);
      getMeasuredColl()->setAntiprotonType(kUp);
   }
}

void WbbEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using PeterFunctions::Math::square;
//   scale1 = scale2 = MEConstants::wMass;
   double sumPt = getPartonColl()->sumPt();
   scale1 = scale2 = std::sqrt(square(MEConstants::wMass) + square(sumPt));
}

