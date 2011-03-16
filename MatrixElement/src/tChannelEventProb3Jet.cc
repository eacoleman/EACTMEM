#include "TAMUWW/MatrixElement/interface/tChannelEventProb3Jet.hh"

#include <iostream>

#include <cmath>
#include <string>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

using std::string;

extern "C"
{
   void* mytchan3jetud_(double[][4], double*, const double*, double*);
   void* mytchan3jetuxdx_(double[][4], double*, const double*, double*);
   void* mytchan3jetdxux_(double[][4], double*, const double*, double*);
   void* mytchan3jetdu_(double[][4], double*, const double*, double*);
}

tChannelEventProb3Jet::tChannelEventProb3Jet(Integrator& integrator,
                                             const TransferFunction& bTF,
                                             const TransferFunction& lightTF) :
  EventProb3Jet(DEFS::EP::TopT, integrator, 4, 4, bTF, lightTF)
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);
}

tChannelEventProb3Jet::tChannelEventProb3Jet(Integrator& integrator,
                                             const TransferFunction& bTF,
                                             const TransferFunction& lightTF,
                                             const DEFS::EP::Type & ept) :
   EventProb3Jet(ept, integrator, 4, 4, bTF, lightTF)
{
  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);
}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void tChannelEventProb3Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);

  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);

}//setTopMassAndWidth


double tChannelEventProb3Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

//   std::cout << "Width: " << m_widthTop << std::endl;

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
      static Array1 vec3;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
         lepE = partons->getLepton().E();
      }

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getJet(2), 0, 1);
//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec0, vec2, factor, wMass, wWidth);
      Array4 vec9 = DHELAS::fvixxx(vec6, vec1, gFactor, bMass, 0);
      Array4 vec10 = DHELAS::fvixxx(vec9, vec8, factor, m_massTop,
                                    m_widthTop);

      OutputType output = DHELAS::iovxxx(vec10, vec5, vec7, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(output[i]) * 12;
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

      Array1 vec0 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getJet(2), 0, -1);
//      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec2, vec0, factor, wMass, wWidth);
      Array4 vec9 = DHELAS::fvoxxx(vec6, vec1, gFactor, bMass, 0);
      Array4 vec10 = DHELAS::fvoxxx(vec9, vec8, factor, m_massTop,
                                    m_widthTop);

      OutputType output = DHELAS::iovxxx(vec5, vec10, vec7, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(output[i]) * 12;
      }

//      DHELAS::HelVec vec[11];
//      DHELAS::oxxxxx(partons->getParton1(), 0, -1, vec[0], kOne);
//      DHELAS::vxxxxx(partons->getParton2(), 0, -1, vec[1], kTwo);
//      DHELAS::ixxxxx(partons->getJet(2), 0, -1, vec[2], kOne);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[4], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[5], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), bMass, 1, vec[6], kTwo);
//
//      DHELAS::jioxxx(vec[4], vec[3], factor, wMass, wWidth, vec[7]);
//      DHELAS::jioxxx(vec[2], vec[0], factor, wMass, wWidth, vec[8]);
//      DHELAS::fvoxxx(vec[6], vec[1], gFactor, bMass, 0, vec[9]);
//      DHELAS::fvoxxx(vec[9], vec[8], factor, m_massTop,
//                     m_widthTop, vec[10]);
//      DHELAS::iovxxx(vec[5], vec[10], vec[7], factor, output);
   }

   return answer;

//   const PartonColl* partons = getPartonColl();

//   double fortranArray[7][4];
//   makeFortranArray(fortranArray);
//
//   double topMass = m_massTop;
//   const double wMass = MEConstants::wMass;

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

//   double answer = 0, answer2 = 0;

//   if (partons->getLepCharge() > 0)
//   {
//      mytchan3jetud_(fortranArray, &topMass, &wMass, &answer);
//      mytchan3jetdxux_(fortranArray, &topMass, &wMass, &answer2);
//   }
//   else
//   {
//      mytchan3jetuxdx_(fortranArray, &topMass, &wMass, &answer);
//      mytchan3jetdu_(fortranArray, &topMass, &wMass, &answer2);
//   }
//
//   return answer + answer2;
}

void tChannelEventProb3Jet::setPartonTypes() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
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

void tChannelEventProb3Jet::getScale(double& scale1, double& scale2) const
{
   using AuxFunctions::Math::square;
//   scale1 = MEConstants::wMass / 2;
//   scale2 = MEConstants::wMass / 2 + m_massTop;
   double lightLine = std::sqrt(getPartonColl()->Q2());
   double topLine = std::sqrt(getPartonColl()->Q2()
                              + square(m_massTop));

   if (getMeasuredColl()->getLepCharge() > 0)
   {
      scale1 = lightLine;
      scale2 = topLine;
   }
   else
   {
      scale1 = topLine;
      scale2 = lightLine;
   }
}

void tChannelEventProb3Jet::makeFortranArray(double array[][4]) const
{
   using AuxFunctionsRoot::makeArray;

   makeArray(getPartonColl()->getParton1(), array[0]);
   makeArray(getPartonColl()->getParton2(), array[1]);
   makeArray(getPartonColl()->getJet(2), array[2]);
   makeArray(getPartonColl()->getLepton(), array[3]);
   makeArray(getPartonColl()->getNeutrino(), array[4]);
   makeArray(getPartonColl()->getJet(0), array[5]);
   makeArray(getPartonColl()->getJet(1), array[6]);
}

tChannelEventProb3JetAlt::tChannelEventProb3JetAlt(Integrator& integrator,
                                                   const TransferFunction& bTF,
                                                   const TransferFunction& lTF)
  : tChannelEventProb3Jet(integrator, bTF, lTF, DEFS::EP::TopTAlt)
{}

double tChannelEventProb3JetAlt::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

//   std::cout << "Width: " << m_widthTop << std::endl;

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
      static Array1 vec3;
      static double lepE = 0;
      if (lepE != partons->getLepton().E())
      {
         vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
         lepE = partons->getLepton().E();
      }

      Array1 vec0 = DHELAS::oxxxxx<1>(partons->getParton1(), 0, -1);
      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getJet(2), 0, -1);
//      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec4 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec6 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec7 = DHELAS::jioxxx(vec3, vec4, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec2, vec0, factor, wMass, wWidth);
      Array4 vec9 = DHELAS::fvixxx(vec6, vec1, gFactor, bMass, 0);
      Array4 vec10 = DHELAS::fvixxx(vec9, vec8, factor, m_massTop,
                                    m_widthTop);

      OutputType output = DHELAS::iovxxx(vec10, vec5, vec7, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(output[i]) * 12;
      }

//      DHELAS::HelVec vec[11];
//      DHELAS::oxxxxx(partons->getParton1(), 0, -1, vec[0], kOne);
//      DHELAS::vxxxxx(partons->getParton2(), 0, -1, vec[1], kTwo);
//      DHELAS::ixxxxx(partons->getJet(2), 0, -1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getLepton(), 0, -1, vec[3], kOne);
//      DHELAS::oxxxxx(partons->getNeutrino(), 0, 1, vec[4], kOne);
//      DHELAS::oxxxxx(partons->getJet(0), bMass, 1, vec[5], kTwo);
//      DHELAS::ixxxxx(partons->getJet(1), bMass, -1, vec[6], kTwo);
//
//      DHELAS::jioxxx(vec[3], vec[4], factor, wMass, wWidth, vec[7]);
//      DHELAS::jioxxx(vec[2], vec[0], factor, wMass, wWidth, vec[8]);
//      DHELAS::fvixxx(vec[6], vec[1], gFactor, bMass, 0, vec[9]);
//      DHELAS::fvixxx(vec[9], vec[8], factor, m_massTop,
//                     m_widthTop, vec[10]);
//      DHELAS::iovxxx(vec[10], vec[5], vec[7], factor, output2);
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

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getParton1(), 0, 1);
      Array2 vec1 = DHELAS::vxxxxx<2>(partons->getParton2(), 0, -1);
      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getJet(2), 0, 1);
//      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec4 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec6 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);

      Array1 vec7 = DHELAS::jioxxx(vec4, vec3, factor, wMass, wWidth);
      Array1 vec8 = DHELAS::jioxxx(vec0, vec2, factor, wMass, wWidth);
      Array4 vec9 = DHELAS::fvoxxx(vec6, vec1, gFactor, bMass, 0);
      Array4 vec10 = DHELAS::fvoxxx(vec9, vec8, factor, m_massTop,
                                    m_widthTop);

      OutputType output = DHELAS::iovxxx(vec5, vec10, vec7, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(output[i]) * 12;
      }

//      DHELAS::HelVec vec[11];
//      DHELAS::ixxxxx(partons->getParton1(), 0, 1, vec[0], kOne);
//      DHELAS::vxxxxx(partons->getParton2(), 0, -1, vec[1], kTwo);
//      DHELAS::oxxxxx(partons->getJet(2), 0, 1, vec[2], kOne);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[4], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[5], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), bMass, 1, vec[6], kTwo);
//
//      DHELAS::jioxxx(vec[4], vec[3], factor, wMass, wWidth, vec[7]);
//      DHELAS::jioxxx(vec[0], vec[2], factor, wMass, wWidth, vec[8]);
//      DHELAS::fvoxxx(vec[6], vec[1], gFactor, bMass, 0, vec[9]);
//      DHELAS::fvoxxx(vec[9], vec[8], factor, m_massTop,
//                     m_widthTop, vec[10]);
//      DHELAS::iovxxx(vec[5], vec[10], vec[7], factor, output2);
   }

//   for (DHELAS::CompVec::const_iterator it = output2.begin();
//        it != output2.end(); ++it)
//   {
////      std::cerr << "Output " << *it << std::endl;
//      answer2 += std::norm(*it) * 12;
//   }

//   std::cerr << "answer2: " << answer2 << std::endl;

   return answer;

//   const PartonColl* partons = getPartonColl();

//   double fortranArray[7][4];
//   makeFortranArray(fortranArray);
//
//   double topMass = m_massTop;
////   const double wMass = MEConstants::wMass;
//
////   for (int i = 0; i < 7; ++i)
////      for (int j = 0; j < 4; ++j)
////      {
////         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
////      }
//////   exit(1);
////
////   std::cerr << "Top mass: " << topMass << std::endl;
////   std::cerr << "W mass: " << wMass << std::endl;
//// 
////   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
//
////   double answer = 0, answer2 = 0;
//
//   if (partons->getLepCharge() > 0)
//   {
//      mytchan3jetud_(fortranArray, &topMass, &wMass, &answer);
//      mytchan3jetdxux_(fortranArray, &topMass, &wMass, &answer2);
//   }
//   else
//   {
//      mytchan3jetuxdx_(fortranArray, &topMass, &wMass, &answer);
//      mytchan3jetdu_(fortranArray, &topMass, &wMass, &answer2);
//   }
//
//   return answer + answer2;
}


void tChannelEventProb3JetAlt::setPartonTypes() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setParton1Type(kGluon);
      getMeasuredColl()->setParton2Type(kDown);
   }
   else
   {
      getMeasuredColl()->setParton1Type(kDown);
      getMeasuredColl()->setParton2Type(kGluon);
   }
}
