#include "TAMUWW/MatrixElement/interface/sChannelEventProb3Jet.hh"

#include <iostream>

#include <cmath>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

extern "C"
{
   void* myschan3jet_(double[][4], double*, const double*, double*);
   void* myschan3jet2_(double[][4], double*, const double*, double*);
}

sChannelEventProb3Jet::sChannelEventProb3Jet(Integrator& integrator,
                                             const TransferFunction& bTF,
                                             const TransferFunction& gluonTF) :
   EventProb3Jet("s-channel", integrator, 4, 4, bTF, gluonTF)
//   EventProb3Jet("s-channel", 4, 1, bTF, lightTF)
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);

}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void sChannelEventProb3Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(mTop);

}//setTopMassAndWidth

double sChannelEventProb3Jet::matrixElement() const
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
   doublecomplex gFactor[2] = {doublecomplex(-MEConstants::sgg, 0),
                               doublecomplex(-MEConstants::sgg, 0)};

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
      Array2 vec8 = DHELAS::fvoxxx(vec4, vec7, factor, m_massTop,
                                   m_widthTop);
      Array2 vec9 = DHELAS::fvixxx(vec0, vec6, gFactor, 0, 0);
      Array2 vec10 = DHELAS::jioxxx(vec9, vec1, factor, wMass, wWidth);
      OutputType output0 = DHELAS::iovxxx(vec5, vec8, vec10, factor);

      Array2 vec11 = DHELAS::fvoxxx(vec1, vec6, gFactor, 0, 0);
      Array2 vec12 = DHELAS::jioxxx(vec0, vec11, factor, wMass, wWidth);
      OutputType output1 = DHELAS::iovxxx(vec5, vec8, vec12, factor);

      Array1 vec13 = DHELAS::jioxxx(vec0, vec1, factor, wMass, wWidth);
      Array4 vec14 = DHELAS::fvoxxx(vec4, vec6, gFactor, bMass, 0);
      Array4 vec15 = DHELAS::fvoxxx(vec14, vec7, factor, m_massTop,
                                    m_widthTop);
      OutputType output2 = DHELAS::iovxxx(vec5, vec15, vec13, factor);

      Array2 vec16 = DHELAS::fvoxxx(vec8, vec13, factor, bMass, 0);
      OutputType output3 = DHELAS::iovxxx(vec5, vec16, vec6, gFactor);

      Array4 vec17 = DHELAS::fvoxxx(vec8, vec6, gFactor, m_massTop,
                                    m_widthTop);
      OutputType output4 = DHELAS::iovxxx(vec5, vec17, vec13, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex temp = -output0[i] - output1[i];
         doublecomplex temp2 = -output2[i] - output3[i] - output4[i];
         
         answer += (std::norm(temp) + std::norm(temp2))* 12;
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

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getParton2(), 0, -1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);
      Array2 vec6 = DHELAS::vxxxxx<2>(partons->getJet(2), 0, 1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec8 = DHELAS::fvixxx(vec4, vec7, factor, m_massTop,
                                   m_widthTop);
      Array2 vec9 = DHELAS::fvixxx(vec0, vec6, gFactor, 0, 0);
      Array2 vec10 = DHELAS::jioxxx(vec9, vec1, factor, wMass, wWidth);
      OutputType output0 = DHELAS::iovxxx(vec8, vec5, vec10, factor);

      Array2 vec11 = DHELAS::fvoxxx(vec1, vec6, gFactor, 0, 0);
      Array2 vec12 = DHELAS::jioxxx(vec0, vec11, factor, wMass, wWidth);
      OutputType output1 = DHELAS::iovxxx(vec8, vec5, vec12, factor);

      Array1 vec13 = DHELAS::jioxxx(vec0, vec1, factor, wMass, wWidth);
      Array4 vec14 = DHELAS::fvixxx(vec4, vec6, gFactor, bMass, 0);
      Array4 vec15 = DHELAS::fvixxx(vec14, vec7, factor, m_massTop,
                                    m_widthTop);
      OutputType output2 = DHELAS::iovxxx(vec15, vec5, vec13, factor);
      std::swap(output2[1], output2[2]);
      std::swap(output2[5], output2[6]);

      Array2 vec16 = DHELAS::fvixxx(vec8, vec13, factor, bMass, 0);
      OutputType output3 = DHELAS::iovxxx(vec16, vec5, vec6, gFactor);

      Array4 vec17 = DHELAS::fvixxx(vec8, vec6, gFactor, m_massTop,
                                    m_widthTop);
      OutputType output4 = DHELAS::iovxxx(vec17, vec5, vec13, factor);
      std::swap(output4[1], output4[2]);
      std::swap(output4[5], output4[6]);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         doublecomplex temp = -output0[i] - output1[i];
         doublecomplex temp2 = -output2[i] - output3[i] - output4[i];
         
         answer += (std::norm(temp) + std::norm(temp2))* 12;
      }

//      DHELAS::HelVec vec[18];
//      DHELAS::ixxxxx(partons->getParton1(), 0, 1, vec[0], kOne);
//      DHELAS::oxxxxx(partons->getParton2(), 0, -1, vec[1], kOne);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[4], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), bMass, 1, vec[5], kTwo);
//      DHELAS::vxxxxx(partons->getJet(2), 0, 1, vec[6], kTwo);
//
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[7]);
//      DHELAS::fvixxx(vec[4], vec[7], factor, m_massTop,
//                     m_widthTop, vec[8]);
//      DHELAS::fvixxx(vec[0], vec[6], gFactor, 0, 0, vec[9]);
//      DHELAS::jioxxx(vec[9], vec[1], factor, wMass, wWidth, vec[10]);
//      DHELAS::iovxxx(vec[8], vec[5], vec[10], factor, output[0]);
//
//      DHELAS::fvoxxx(vec[1], vec[6], gFactor, 0, 0, vec[11]);
//      DHELAS::jioxxx(vec[0], vec[11], factor, wMass, wWidth, vec[12]);
//      DHELAS::iovxxx(vec[8], vec[5], vec[12], factor, output[1]);
//
//      DHELAS::jioxxx(vec[0], vec[1], factor, wMass, wWidth, vec[13]);
//      DHELAS::fvixxx(vec[4], vec[6], gFactor, bMass, 0, vec[14]);
//      DHELAS::fvixxx(vec[14], vec[7], factor, m_massTop,
//                     m_widthTop, vec[15]);
//      DHELAS::iovxxx(vec[15], vec[5], vec[13], factor, output[2]);
//      std::swap(output[2][1], output[2][2]);
//      std::swap(output[2][5], output[2][6]);
//
//      DHELAS::fvixxx(vec[8], vec[13], factor, bMass, 0, vec[16]);
//      DHELAS::iovxxx(vec[16], vec[5], vec[6], gFactor, output[3]);
//
//      DHELAS::fvixxx(vec[8], vec[6], gFactor, m_massTop,
//                     m_widthTop, vec[17]);
//      DHELAS::iovxxx(vec[17], vec[5], vec[13], factor, output[4]);
//      std::swap(output[4][1], output[4][2]);
//      std::swap(output[4][5], output[4][6]);
   }

//   for (unsigned i = 0; i < output[0].size(); ++i)
//   {
//      doublecomplex temp = -output[0][i] - output[1][i];
//      doublecomplex temp2 = -output[2][i] - output[3][i] - output[4][i];
//
////      for (unsigned j = 0; j < 5; ++j)
////      {
////         std::cerr << "output " << j << "  " << output[j][i] << std::endl;
////      }
//      
//      answer += (std::norm(temp) + std::norm(temp2))* 12;
//   }
   
//   std::cerr << "Answer: " << answer << std::endl;
   return answer;

//   const PartonColl* partons = getPartonColl();

   double fortranArray[7][4];
   makeFortranArray(fortranArray);

   double topMass = m_massTop;
//   const double wMass = MEConstants::wMass;

//   double answer;

//   for (int i = 0; i < 7; ++i)
//      for (int j = 0; j < 4; ++j)
//      {
//         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
//      }
////   exit(1);
//
//   std::cerr << "Top mass: " << topMass << std::endl;
//   std::cerr << "W mass: " << wMass << std::endl;
// 
//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;

   if (partons->getLepCharge() > 0)
      myschan3jet_(fortranArray, &topMass, &wMass, &answer);
   else
      myschan3jet2_(fortranArray, &topMass, &wMass, &answer);
//   std::cerr << "s-channel: " << answer << std::endl;
//   exit(1);
//
   return answer;
}

void sChannelEventProb3Jet::setPartonTypes() const
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

void sChannelEventProb3Jet::getScale(double& scale1, double&scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
//   scale1 = scale2 = m_massTop;
}
