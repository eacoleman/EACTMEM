#include "TAMUWW/MatrixElement/interface/ttEventProb2Jet.hh"

#include <iostream>

#include <cmath>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

using std::cout;
using std::endl;
using std::vector;

using PeterFunctions::Math::square;

extern "C"
{
   void* mytt_(double[][4], double*, const double*, double*);
   void* mytt2_(double[][4], double*, const double*, double*);
}


// ------------------------------------------------------------------
ttEventProb2Jet::ttEventProb2Jet(Integrator& integrator,
                                 const TransferFunction& tf) :
   EventProb2Jet("tt-bar", integrator, 6, 2, tf)
{

  // Set the top mass and width
  setTopMassAndWidth(MEConstants::topMass);

}

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void ttEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(mTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------
// This is the default one for regular ME calculation
void ttEventProb2Jet::changeVars(const vector<double>& parameters)
{
 
   EventProb2Jet::changeVars(parameters);

   double r = parameters[3];
   double phi = parameters[4];
   double theta = parameters[5];

   double x = r * cos(phi) * sin(theta);
   double y = r * sin(phi) * cos(theta);
   double z = r * sin(theta);

   m_W.SetPxPyPzE(x, y, z, r);

}//changeVars


void ttEventProb2Jet::met()
{
   getPartonColl()->setMet();
   TLorentzVector& vec = getPartonColl()->getNeutrino();
   double px = vec.Px() - m_W.Px();
   double py = vec.Py() - m_W.Py();
   double energy = std::sqrt(square(px) + square(py)
                             + square(vec.Pz()));
   vec.SetPxPyPzE(px, py, vec.Pz(), energy);
}

void ttEventProb2Jet::getTotalLV(TLorentzVector& vec) const
{
   EventProb::getTotalLV(vec);
   vec += m_W;

}

double ttEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;
   typedef SimpleArray<DHELAS::HelArray, 4> Array4;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

   double answer = 0;

   doublecomplex factor[2] = {doublecomplex(MEConstants::gwf, 0),
                              doublecomplex(0, 0)};
   doublecomplex gFactor[2] = {doublecomplex(MEConstants::gg, 0),
                               doublecomplex(MEConstants::gg, 0)};

   enum {vecSize = 24};
   typedef SimpleArray<DHELAS::HelArray, 3> Array3;
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

      Array2 vec0 = DHELAS::ixxxxx<2>(partons->getQuark1(), 0, 1);
      Array2 vec1 = DHELAS::oxxxxx<2>(partons->getQuark2(), 0, -1);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);
      Array3 vec6 = DHELAS::vxxxxx<3>(m_W, wMass, 1);

      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      Array1 vec0_0, vec0_1, vec1_0, vec1_1;
      vec0_0[0] = vec0[0];
      vec0_1[0] = vec0[1];
      vec1_0[0] = vec1[0];
      vec1_1[0] = vec1[1];
      Array1 vec7_0 = DHELAS::jioxxx(vec0_1, vec1_0, gFactor, 0, 0);
      Array1 vec7_1 = DHELAS::jioxxx(vec0_0, vec1_1, gFactor, 0, 0);
      Array2 vec7;
      vec7[0] = vec7_0[0];
      vec7[1] = vec7_1[0];

//      Array4 vec7temp = DHELAS::jioxxx(vec0, vec1, gFactor, 0, 0);
//      Array2 vec7;
//      vec7[0] = vec7temp[1];
//      vec7[1] = vec7temp[2];

//      vec[7].erase(vec[7].begin());
//      vec[7].erase(vec[7].begin() + 2);
      Array1 vec8 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec9 = DHELAS::fvoxxx(vec4, vec8, factor, m_massTop, m_widthTop);
      Array4 vec10 = DHELAS::fvoxxx(vec9, vec7, gFactor, m_massTop, m_widthTop);
      OutputType output = DHELAS::iovxxx(vec5, vec10, vec6, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 2;
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

      Array2 vec0 = DHELAS::ixxxxx<2>(partons->getQuark1(), 0, 1);
      Array2 vec1 = DHELAS::oxxxxx<2>(partons->getQuark2(), 0, -1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);
      Array3 vec6 = DHELAS::vxxxxx<3>(m_W, wMass, 1);

      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      Array1 vec0_0, vec0_1, vec1_0, vec1_1;
      vec0_0[0] = vec0[0];
      vec0_1[0] = vec0[1];
      vec1_0[0] = vec1[0];
      vec1_1[0] = vec1[1];
      Array1 vec7_0 = DHELAS::jioxxx(vec0_1, vec1_0, gFactor, 0, 0);
      Array1 vec7_1 = DHELAS::jioxxx(vec0_0, vec1_1, gFactor, 0, 0);
      Array2 vec7;
      vec7[1] = vec7_0[0];
      vec7[0] = vec7_1[0];

//      Array4 vec7temp = DHELAS::jioxxx(vec0, vec1, gFactor, 0, 0);
//      Array2 vec7;
//      vec7[0] = vec7temp[1];
//      vec7[1] = vec7temp[2];

//      vec[7].erase(vec[7].begin());
//      vec[7].erase(vec[7].begin() + 2);
      Array1 vec8 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec9 = DHELAS::fvixxx(vec4, vec8, factor, m_massTop, m_widthTop);
      Array4 vec10 = DHELAS::fvixxx(vec9, vec7, gFactor, m_massTop, m_widthTop);
      OutputType output = DHELAS::iovxxx(vec10, vec5, vec6, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 2;
      }
//      DHELAS::HelVec vec[11];
//      DHELAS::ixxxxx(partons->getQuark1(), 0, 1, vec[0], kTwo);
//      DHELAS::oxxxxx(partons->getQuark2(), 0, -1, vec[1], kTwo);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[4], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), bMass, 1, vec[5], kTwo);
//      DHELAS::vxxxxx(m_W, wMass, 1, vec[6], kThree);
//
//      DHELAS::jioxxx(vec[0], vec[1], gFactor, 0, 0, vec[7]);
//      vec[7].erase(vec[7].begin());
//      vec[7].erase(vec[7].begin() + 2);
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[8]);
//      DHELAS::fvixxx(vec[4], vec[8], factor, m_massTop,
//                     partons->getTopWidth(), vec[9]);
//      DHELAS::fvixxx(vec[9], vec[7], gFactor, m_massTop,
//                     partons->getTopWidth(), vec[10]);
//      DHELAS::iovxxx(vec[10], vec[5], vec[6], factor, output);
   }

//   for (DHELAS::CompVec::const_iterator it = output.begin();
//        it != output.end(); ++it)
//   {
//      answer += std::norm(-(*it)) * 2;
//   }
   answer /= 36;

//   std::cerr << "Answer: " << answer << std::endl;
   return answer;


//   const PartonColl* partons = getPartonColl();
//
   double fortranArray[7][4];
   makeFortranArray(fortranArray);
   PeterFunctions::makeArray(m_W, fortranArray[6]);
//
   double topMass = m_massTop;
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
      mytt_(fortranArray, &topMass, &wMass, &answer);
   else
      mytt2_(fortranArray, &topMass, &wMass, &answer);

   std::cerr << "Old answer: " << answer << std::endl;
   exit(1);
   return answer;
}

double ttEventProb2Jet::phaseSpace() const
{
   // For an explanation of why this should be so, see the parallel
   // function in the 3-jet bin
   return EventProb::phaseSpace() * m_W.E() * std::sin(m_W.Theta());
}

void ttEventProb2Jet::setQuarkIDs() const
{
   getMeasuredColl()->setProtonType(kUp);
   getMeasuredColl()->setAntiprotonType(kUp);
}

void ttEventProb2Jet::getScale(double& scale1, double& scale2) const
{
//   scale1 = scale2 = m_massTop;
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
}

bool ttEventProb2Jet::onSwitch()
{
   if (getLoop())
   {
      swapJets(0, 1);
   }
   return true;
}

