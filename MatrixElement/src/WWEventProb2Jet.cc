#include "TAMUWW/MatrixElement/interface/WWEventProb2Jet.hh"

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
   void* myww_(double[][4], const double*, const double*, double*);
   void* myww2_(double[][4], const double*, const double*, double*);   
}

WWEventProb2Jet::WWEventProb2Jet(Integrator& integrator,
                                   const TransferFunction& tf) :
   EventProb2Jet("WW", integrator, 3, 1, tf)
{}

void WWEventProb2Jet::changeVars(const vector<double>& parameters)
{
   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(parameters[1]);
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

double WWEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::zMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;
   using MEConstants::zWidth;

   const PartonColl* partons = getPartonColl();

   double answer = 0;

//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};
//   std::cerr << "factor[0]: " << factor[0] << std::endl;
   doublecomplex zFactor[2] = {doublecomplex(MEConstants::gzu1, 0),
                               doublecomplex(MEConstants::gzu2, 0)};
   doublecomplex singleZ[2] = {doublecomplex(MEConstants::gwwz, 0),
                               doublecomplex(0, 0)};
//   std::cerr << "singleZ: " << singleZ[0] << std::endl;
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

      Array2 vec1 = DHELAS::ixxxxx<2>(partons->getQuark1(), 0, 1);
      Array2 vec2 = DHELAS::oxxxxx<2>(partons->getQuark2(), 0, -1);
//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(0), 0, -1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1);

      Array1 vec1W, vec2W;
      vec1W[0] = vec1[0];
      vec2W[0] = vec2[1];
      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::fvixxx(vec1W, vec7, factor, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec8, vec2W, factor, wMass, wWidth);

      SimpleArray<doublecomplex, 1> output = DHELAS::iovxxx(vec5, vec6, vec9,
                                                            factor);
      
      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      Array1 vec1_0, vec1_1, vec2_0, vec2_1;
      vec1_0[0] = vec1[0];
      vec1_1[0] = vec1[1];
      vec2_0[0] = vec2[0];
      vec2_1[0] = vec2[1];
      Array1 vec10_0 = DHELAS::jioxxx(vec1_0, vec2_1, zFactor, zMass, zWidth);
      Array1 vec10_1 = DHELAS::jioxxx(vec1_1, vec2_0, zFactor, zMass, zWidth);
      Array2 vec10;
      vec10[0] = vec10_0[0];
      vec10[1] = vec10_1[0];

      Array2 vec11 = DHELAS::jvvxxx(vec7, vec10, singleZ, wMass, wWidth);

      OutputType output2 = DHELAS::iovxxx(vec5, vec6, vec11, factor);

//      std::cerr << "output " << output[0] << std::endl;
//      for (unsigned i = 0; i < 2; ++i)
//      {
//         std::cerr << "output2 " << output2[i] << std::endl;
//      }

      answer += std::norm(output[0] + output2[0]);
      answer += std::norm(output2[1]);
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

      Array2 vec1 = DHELAS::ixxxxx<2>(partons->getQuark1(), 0, 1);
      Array2 vec2 = DHELAS::oxxxxx<2>(partons->getQuark2(), 0, -1);
//      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(0), 0, 1);
      Array1 vec6 = DHELAS::ixxxxx<1>(partons->getJet(1), 0, -1);

      Array1 vec1W, vec2W;
      vec1W[0] = vec1[0];
      vec2W[0] = vec2[1];
      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::fvoxxx(vec2W, vec7, factor, 0, 0);
      Array1 vec9 = DHELAS::jioxxx(vec1W, vec8, factor, wMass, wWidth);

      SimpleArray<doublecomplex, 1> output = DHELAS::iovxxx(vec6, vec5, vec9,
                                                            factor);
      
      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      Array1 vec1_0, vec1_1, vec2_0, vec2_1;
      vec1_0[0] = vec1[0];
      vec1_1[0] = vec1[1];
      vec2_0[0] = vec2[0];
      vec2_1[0] = vec2[1];
      Array1 vec10_0 = DHELAS::jioxxx(vec1_0, vec2_1, zFactor, zMass, zWidth);
      Array1 vec10_1 = DHELAS::jioxxx(vec1_1, vec2_0, zFactor, zMass, zWidth);
      Array2 vec10;
      vec10[0] = vec10_0[0];
      vec10[1] = vec10_1[0];

      Array2 vec11 = DHELAS::jvvxxx(vec10, vec7, singleZ, wMass, wWidth);

      OutputType output2 = DHELAS::iovxxx(vec6, vec5, vec11, factor);

//      std::cerr << "output " << output[0] << std::endl;
//      for (unsigned i = 0; i < 2; ++i)
//      {
//         std::cerr << "output2 " << output2[i] << std::endl;
//      }

      answer += std::norm(output[0] + output2[0]);
      answer += std::norm(output2[1]);
   }


   answer /= 36;
//   std::cerr << "New Answer: " << answer << std::endl;
   return answer;
//   const PartonColl* partons = getPartonColl();
//
   double fortranArray[6][4];
   makeFortranArray(fortranArray);

// KLUDGE
//   const double wMass = partons->getTopMass();
//   double zMass = partons->getTopMass();
//   const double wMass = MEConstants::wMass;
//   double zMass = MEConstants::zMass;

//   double answer;

//   for (int i = 0; i < 7; ++i)
//      for (int j = 0; j < 4; ++j)
//      {
//         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
//      }
//   exit+(1);
//
//   std::cerr << "Top mass: " << topMass << std::endl;
//   std::cerr << "W mass: " << wMass << std::endl;
   
//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;

   if (partons->getLepCharge() > 0)
      myww_(fortranArray, &zMass, &wMass, &answer);
   else
      myww2_(fortranArray, &zMass, &wMass, &answer);

   std::cerr << "Old answer: " << answer << std::endl;
   exit(1);
   return answer;
}

void WWEventProb2Jet::setQuarkIDs() const
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

void WWEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}
