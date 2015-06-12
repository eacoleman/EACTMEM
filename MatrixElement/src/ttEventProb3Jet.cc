#include "TAMUWW/MatrixElement/interface/ttEventProb3Jet.hh"

#include <iostream>

#include <cmath>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

using std::vector;

extern "C"
{
   void* mytt3jet_(double[][4], double*, const double*, double*);
   void* mytt3jet2_(double[][4], double*, const double*, double*);
}

ttEventProb3Jet::ttEventProb3Jet(Integrator& integrator,
                                 const TransferFunction& bTF,
                                 const TransferFunction& lightTF,
                                 double tMass) :
   EventProb3Jet(DEFS::EP::TTbar, integrator, 7, 2, bTF, lightTF)
{
  // Set the top mass and width
  setTopMassAndWidth(tMass);
}


// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void ttEventProb3Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(m_massTop);

  // Save the mass in EventProb's param so it is available later for ProbStat
  setEventProbParam(m_massTop);

}//setTopMassAndWidth


// ------------------------------------------------------------------
unsigned ttEventProb3Jet::getProbMax() const
{
   return 2;
}

void ttEventProb3Jet::changeVars(const vector<double>& parameters)
{
   using std::sin;
   using std::cos;

   EventProb3Jet::changeVars(parameters);

   double r = parameters[4];
   double phi = parameters[5];
   double theta = parameters[6];

   double x = r * cos(phi) * sin(theta);
   double y = r * sin(phi) * cos(theta);
   double z = r * sin(theta);

   m_lostJet.SetPxPyPzE(x, y, z, r);
//   m_lostJet = getPartonColl()->getExtra(0);
}

void ttEventProb3Jet::met()
{
   using AuxFunctions::Math::square;

   getPartonColl()->setMet();
   TLorentzVector& vec = getPartonColl()->getNeutrino();
   double px = vec.Px() - m_lostJet.Px();
   double py = vec.Py() - m_lostJet.Py();
   double energy = std::sqrt(square(px) + square(py)
                             + square(vec.Pz()));
   vec.SetPxPyPzE(px, py, vec.Pz(), energy);
}

void ttEventProb3Jet::getTotalLV(TLorentzVector& vec) const
{
   EventProb::getTotalLV(vec);
   vec += m_lostJet;
}

double ttEventProb3Jet::matrixElement() const
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
   doublecomplex gFactor[2] = {doublecomplex(MEConstants::sgg, 0),
                               doublecomplex(MEConstants::sgg, 0)};

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

      Array2 vec0 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, 1);
      Array2 vec1 = DHELAS::oxxxxx<2>(partons->getParton2(), 0, -1);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array2 vec5 = DHELAS::ixxxxx<2>(partons->getJet(1), bMass, -1);
      Array1 vec6 = DHELAS::ixxxxx<1>(partons->getJet(2), 0, -1);
      Array1 vec7 = DHELAS::oxxxxx<1>(m_lostJet, 0, 1);

      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      Array1 vec0_0, vec0_1, vec1_0, vec1_1;
      vec0_0[0] = vec0[0];
      vec0_1[0] = vec0[1];
      vec1_0[0] = vec1[0];
      vec1_1[0] = vec1[1];
      Array1 vec8_0 = DHELAS::jioxxx(vec0_1, vec1_0, gFactor, 0, 0);
      Array1 vec8_1 = DHELAS::jioxxx(vec0_0, vec1_1, gFactor, 0, 0);
      Array2 vec8;
      vec8[1] = vec8_0[0];
      vec8[0] = vec8_1[0];

//      Array4 vec8 = DHELAS::jioxxx(vec0, vec1, gFactor, 0, 0);
//      vec[8].erase(vec[8].begin());
//      vec[8].erase(vec[8].begin() + 2);

      Array1 vec9 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec10 = DHELAS::fvoxxx(vec4, vec9, factor, m_massTop, m_widthTop);
      Array4 vec11 = DHELAS::fvoxxx(vec10, vec8, gFactor,m_massTop, m_widthTop);
      Array1 vec12 = DHELAS::jioxxx(vec6, vec7, factor, wMass, wWidth);
      OutputType output = DHELAS::iovxxx(vec5, vec11, vec12, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(output[i]) * 6;
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

      Array2 vec0 = DHELAS::ixxxxx<2>(partons->getParton1(), 0, 1);
      Array2 vec1 = DHELAS::oxxxxx<2>(partons->getParton2(), 0, -1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array2 vec5 = DHELAS::oxxxxx<2>(partons->getJet(1), bMass, 1);
      Array1 vec6 = DHELAS::oxxxxx<1>(partons->getJet(2), 0, 1);
      Array1 vec7 = DHELAS::ixxxxx<1>(m_lostJet, 0, -1);

      // Rigamarole to reduce unnecessary function calls:
      // Initial quarks must have same helicities
      Array1 vec0_0, vec0_1, vec1_0, vec1_1;
      vec0_0[0] = vec0[0];
      vec0_1[0] = vec0[1];
      vec1_0[0] = vec1[0];
      vec1_1[0] = vec1[1];
      Array1 vec8_0 = DHELAS::jioxxx(vec0_1, vec1_0, gFactor, 0, 0);
      Array1 vec8_1 = DHELAS::jioxxx(vec0_0, vec1_1, gFactor, 0, 0);
      Array2 vec8;
      vec8[1] = vec8_0[0];
      vec8[0] = vec8_1[0];

//      Array4 vec8 = DHELAS::jioxxx(vec0, vec1, gFactor, 0, 0);
//      vec[8].erase(vec[8].begin());
//      vec[8].erase(vec[8].begin() + 2);

      Array1 vec9 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec10 = DHELAS::fvixxx(vec4, vec9, factor, m_massTop, m_widthTop);
      Array4 vec11 = DHELAS::fvixxx(vec10, vec8, gFactor, m_massTop, m_widthTop);
      Array1 vec12 = DHELAS::jioxxx(vec7, vec6, factor, wMass, wWidth);
      OutputType output = DHELAS::iovxxx(vec11, vec5, vec12, factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(output[i]) * 6;
      }
//      DHELAS::HelVec vec[13];
//      DHELAS::ixxxxx(partons->getParton1(), 0, 1, vec[0], kTwo);
//      DHELAS::oxxxxx(partons->getParton2(), 0, -1, vec[1], kTwo);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[4], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), bMass, 1, vec[5], kTwo);
//      DHELAS::oxxxxx(partons->getJet(2), 0, 1, vec[6], kOne);
//      DHELAS::ixxxxx(m_lostJet, 0, -1, vec[7], kOne);
//
//      DHELAS::jioxxx(vec[0], vec[1], gFactor, 0, 0, vec[8]);
//      vec[8].erase(vec[8].begin());
//      vec[8].erase(vec[8].begin() + 2);
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[9]);
//      DHELAS::fvixxx(vec[4], vec[9], factor, m_massTop,
//                     partons->getTopWidth(), vec[10]);
//      DHELAS::fvixxx(vec[10], vec[8], gFactor, m_massTop,
//                     partons->getTopWidth(), vec[11]);
//      DHELAS::jioxxx(vec[7], vec[6], factor, wMass, wWidth, vec[12]);
//      DHELAS::iovxxx(vec[11], vec[5], vec[12], factor, output);
   }

//   for (DHELAS::CompVec::const_iterator it = output.begin();
//        it != output.end(); ++it)
//   {
//      answer += std::norm(*it) * 6;
//   }

//   std::cerr << "Answer: " << answer << std::endl;
   return answer;

//   const PartonColl* partons = getPartonColl();

   double fortranArray[8][4];
   makeFortranArray(fortranArray);
   AuxFunctionsRoot::makeArray(m_lostJet, fortranArray[7]);

   double topMass = m_massTop;
//   const double wMass = MEConstants::wMass;

//   double answer;

   if (partons->getLepCharge() > 0)
      mytt3jet_(fortranArray, &topMass, &wMass, &answer);
   else
      mytt3jet2_(fortranArray, &topMass, &wMass, &answer);

   return answer;
}

double ttEventProb3Jet::phaseSpace() const
{
   // Jacobian = square(m_lostJet.E()) * std::sin(m_lostJet.Theta())
   // phase space = EventProb::phaseSpace() / m_lostJet.E()
   // so return is EventProb::phaseSpace() * m_lostJet.E() * std::sin(m_lostJet.Theta())
   return EventProb::phaseSpace() * m_lostJet.E() * std::sin(m_lostJet.Theta());
}

void ttEventProb3Jet::setPartonTypes() const
{
   getMeasuredColl()->setParton1Type(kUp);
   getMeasuredColl()->setParton2Type(kUp);
}

void ttEventProb3Jet::getScale(double& scale1, double& scale2) const
{
   double scale = getPartonColl()->sHat();
   if (scale < 0)
      scale1 = scale2 = 0;
   else
      scale1 = scale2 = std::sqrt(scale);
//   scale1 = scale2 = m_massTop;
}

bool ttEventProb3Jet::onSwitch()
{
   if (getLoop())
   {
      swapJets(0, 1);
   }
   return true;
}
