#include "TAMUWW/MatrixElement/interface/sChannelEventProb2Jet.hh"

#include <iostream>

#include <cmath>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

extern "C"
{
   void* myschan_(double[][4], double*, const double*, double*);
   void* myschan2_(double[][4], double*, const double*, double*);
}

sChannelEventProb2Jet::sChannelEventProb2Jet(Integrator& integrator,
                                             const TransferFunction& tf,
                                             bool rh) :
   EventProb2Jet(rh ? "s-channel right-handed" : "s-channel", integrator, 3,
                 2, tf),
   m_rh(rh)
{
   if (rh)
   {
      m_factor[0] = doublecomplex(0, 0);
      m_factor[1] = doublecomplex(MEConstants::gwf, 0);
   }
   else
   {
      m_factor[0] = doublecomplex(MEConstants::gwf, 0);
      m_factor[1] = doublecomplex(0, 0);
   }

  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);

}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void sChannelEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(mTop);

}//setTopMassAndWidth


double sChannelEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

//   std::cout << "Width: " << partons->getTopWidth() << std::endl;

   double answer = 0;

//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;
   enum {vecSize = 4};
   typedef SimpleArray<doublecomplex, vecSize> OutputType;

   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};

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
      
      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, 1, m_rh);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1, m_rh);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);

      Array1 vec6 = DHELAS::jioxxx(vec0, vec1, m_factor, wMass, wWidth);
      Array1 vec7 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec8 = DHELAS::fvoxxx(vec4, vec7, factor, m_massTop, m_widthTop);

//      std::cout << "vec6: " << vec6[0] << std::endl;

      OutputType output = DHELAS::iovxxx(vec5, vec8, vec6, m_factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 9;
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

      Array1 vec0 = DHELAS::ixxxxx<1>(partons->getQuark1(), 0, 1, m_rh);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1, m_rh);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);

      Array1 vec6 = DHELAS::jioxxx(vec0, vec1, m_factor, wMass, wWidth);
      Array1 vec7 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec8 = DHELAS::fvixxx(vec4, vec7, factor, m_massTop, m_widthTop);
      OutputType output = DHELAS::iovxxx(vec8, vec5, vec6, m_factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 9;
      }
   }

   answer /= 36;


//   const unsigned ii = 8;
//   for (unsigned i = 0; i < vec[ii].size(); ++i)
//   {
//      std::cerr << "Vector " << i << vec[ii][i].p1 << " " << vec[ii][i].p2 << " " << vec[ii][i].p3 << " " << vec[ii][i].p4 << " " << vec[ii][i].p5 << " " << vec[ii][i].p6 << std::endl;
//   }
//   for (DHELAS::CompVec::const_iterator it = output.begin(); it != output.end(); ++it)
//   {
//      std::cerr << "Output: " << *it << std::endl;
//   }
//   for (int i = 0; i < 6; ++i)
//      for (int j = 0; j < 4; ++j)
//      {
//         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
//      }
////   exit(1);
//
//   std::cerr << "Top mass: " << topMass << std::endl;
//   std::cerr << "W mass: " << wMass << std::endl;
   
//   std::cerr << "Lepton charge: " << partons->getLepCharge() << std::endl;

//   double answer;

   //For debugging
   //Fortran array for slow method
   /*
   std::cerr << "New answer: " << answer << std::endl;
   double fortranArray[6][4];
   makeFortranArray(fortranArray);

   double topMass = m_massTop;

   if (partons->getLepCharge() > 0)
      myschan_(fortranArray, &topMass, &wMass, &answer);
   else
      myschan2_(fortranArray, &topMass, &wMass, &answer);

   std::cerr << "s-channel matrix element: " << answer << std::endl;
   exit(1);
   */
   
   return answer;
}


void sChannelEventProb2Jet::setQuarkIDs() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setProtonType(kUp);
//      std::cerr << "Setting proton type to " << getMeasuredColl()->getProtonType() << std::endl;
      getMeasuredColl()->setAntiprotonType(kDown);
   }
   else
   {
      getMeasuredColl()->setProtonType(kDown);
      getMeasuredColl()->setAntiprotonType(kUp);
   }
}

void sChannelEventProb2Jet::getScale(double& scale1, double& scale2) const
{
//   scale1 = scale2 = m_massTop;
   // Q = sqrt(sHat)
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool sChannelEventProb2Jet::onSwitch()
{
   if (getLoop())
   {
      swapJets(0, 1);
   }
   return true;
//   if (getLoop() == 2)
//   {
//      getPartonColl()->swapJets(0, 1);
//      getMeasuredColl()->swapJets(0, 1);
//   }
//   return true;   
}

