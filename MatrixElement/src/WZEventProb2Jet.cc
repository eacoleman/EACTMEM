#include "TAMUWW/MatrixElement/interface/WZEventProb2Jet.hh"

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
   void* mywz_(double[][4], const double*, const double*, double*);
   void* mywz2_(double[][4], const double*, const double*, double*);
}

WZEventProb2Jet::WZEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
   EventProb2Jet("WZ", integrator, 3, 1, tf)
{}

void WZEventProb2Jet::changeVars(const vector<double>& parameters)
{
   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(parameters[1]);
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

double WZEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zMass;
   using MEConstants::zWidth;

   const PartonColl* partons = getPartonColl();

   double answer = 0;

//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};
//   std::cerr << "factor[0]: " << factor[0] << std::endl;
   doublecomplex zFactorU[2] = {doublecomplex(-MEConstants::gzu1, 0),
                                doublecomplex(-MEConstants::gzu2, 0)};
   doublecomplex zFactorD[2] = {doublecomplex(-MEConstants::gzd1, 0),
                                doublecomplex(-MEConstants::gzd2, 0)};
   doublecomplex singleZ[2] = {doublecomplex(MEConstants::gwwz, 0),
                               doublecomplex(0, 0)};

   enum {vecSize = 2};
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

      Array1 vec1 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, 1);
      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1);
//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), 0, 1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), 0, -1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factor, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec1, vec8, zFactorU, zMass, zWidth);

      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      typedef SimpleArray<doublecomplex, 1> Array1D;

      Array1 vec5_0, vec5_1, vec6_0, vec6_1;
      vec5_0[0] = vec5[0];
      vec5_1[0] = vec5[1];
      vec6_0[0] = vec6[0];
      vec6_1[0] = vec6[1];
      Array1D out1_0 = DHELAS::iovxxx(vec6_0, vec5_1, vec9, zFactorU);
      Array1D out1_1 = DHELAS::iovxxx(vec6_1, vec5_0, vec9, zFactorU);
      OutputType output1;
      output1[0] = out1_0[0];
      output1[1] = out1_1[0];

      Array1 vec10 = DHELAS::fvixxx(vec1, vec7, factor, 0, 0);
      Array1 vec11 = DHELAS::jioxxx(vec10, vec2, zFactorD, zMass, zWidth);

      Array1D out2_0 = DHELAS::iovxxx(vec6_0, vec5_1, vec11, zFactorU);
      Array1D out2_1 = DHELAS::iovxxx(vec6_1, vec5_0, vec11, zFactorU);
      OutputType output2;
      output2[0] = out2_0[0];
      output2[1] = out2_1[0];

      Array1 vec12 = DHELAS::jioxxx(vec1, vec2, factor, wMass, wWidth);
      Array1 vec13 = DHELAS::jvvxxx(vec12, vec7, singleZ, zMass, zWidth);

      Array1D out3_0 = DHELAS::iovxxx(vec6_0, vec5_1, vec13, zFactorU);
      Array1D out3_1 = DHELAS::iovxxx(vec6_1, vec5_0, vec13, zFactorU);
      OutputType output3;
      output3[0] = out3_0[0];
      output3[1] = out3_1[0];
//      for (unsigned i = 0; i < vecSize; ++i)
//      {
//         std::cerr << "output1 " << output1[i] << std::endl;
//         std::cerr << "output2 " << output2[i] << std::endl;
//         std::cerr << "output3 " << output3[i] << std::endl;
//      }

      for (unsigned i = 0; i < vecSize; ++i)
      {
         // The plus sign here disagrees with the FORTRAN function,
         // but I can't figure out why.
         // I say it's FORTRAN's fault.
         answer += std::norm(-output1[i] - output2[i] + output3[i]);
//         std::cerr << "answer " << std::norm(-output1[i] - output2[i] + output3[i]) << std::endl;
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

      Array1 vec1 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, 1);
      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1);
//      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(0), 0, -1);
      Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2, vec7, factor, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec1, vec8, zFactorD, zMass, zWidth);

      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      typedef SimpleArray<doublecomplex, 1> Array1D;

      Array1 vec5_0, vec5_1, vec6_0, vec6_1;
      vec5_0[0] = vec5[0];
      vec5_1[0] = vec5[1];
      vec6_0[0] = vec6[0];
      vec6_1[0] = vec6[1];
      Array1D out1_0 = DHELAS::iovxxx(vec5_0, vec6_1, vec9, zFactorU);
      Array1D out1_1 = DHELAS::iovxxx(vec5_1, vec6_0, vec9, zFactorU);
      OutputType output1;
      output1[0] = out1_0[0];
      output1[1] = out1_1[0];

      Array1 vec10 = DHELAS::fvixxx(vec1, vec7, factor, 0, 0);
      Array1 vec11 = DHELAS::jioxxx(vec10, vec2, zFactorU, zMass, zWidth);

      Array1D out2_0 = DHELAS::iovxxx(vec5_0, vec6_1, vec11, zFactorU);
      Array1D out2_1 = DHELAS::iovxxx(vec5_1, vec6_0, vec11, zFactorU);
      OutputType output2;
      output2[0] = out2_0[0];
      output2[1] = out2_1[0];

      Array1 vec12 = DHELAS::jioxxx(vec1, vec2, factor, wMass, wWidth);
      Array1 vec13 = DHELAS::jvvxxx(vec7, vec12, singleZ, zMass, zWidth);

      Array1D out3_0 = DHELAS::iovxxx(vec5_0, vec6_1, vec13, zFactorU);
      Array1D out3_1 = DHELAS::iovxxx(vec5_1, vec6_0, vec13, zFactorU);
      OutputType output3;
      output3[0] = out3_0[0];
      output3[1] = out3_1[0];

      for (unsigned i = 0; i < vecSize; ++i)
      {
         // The plus sign here disagrees with the FORTRAN function,
         // but I can't figure out why.
         // I say it's FORTRAN's fault.
         answer += std::norm(-output1[i] - output2[i] + output3[i]);
      }
   }


   answer /= 36;
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
//   double zMass = partons->getTopMass();
//   double zMass = MEConstants::zMass;
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
      mywz_(fortranArray, &zMass, &wMass, &answer);
   else
      mywz2_(fortranArray, &zMass, &wMass, &answer);

   std::cerr << "Old answer: " << answer << std::endl;
   exit(1);
   return answer;
}

void WZEventProb2Jet::setQuarkIDs() const
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

void WZEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}
