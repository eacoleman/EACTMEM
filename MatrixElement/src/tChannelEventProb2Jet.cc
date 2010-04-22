#include "TAMUWW/MatrixElement/interface/tChannelEventProb2Jet.hh"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

using std::string;
using std::vector;

extern "C"
{
   void* mytchanud_(double[][4], double*, const double*, double*);
   void* mytchanuxdx_(double[][4], double*, const double*, double*);
   void* mytchandxux_(double[][4], double*, const double*, double*);
   void* mytchandu_(double[][4], double*, const double*, double*);
}

// ------------------------------------------------------------------
tChannelEventProb2Jet::tChannelEventProb2Jet(Integrator& integrator,
                                             const TransferFunction& btf,
                                             const TransferFunction& lighttf,
                                             bool rh) :
   EventProb2Jet(rh ? "t-channel right-handed" : "t-channel", integrator, 3,
                 2, btf),
   m_lightTF(lighttf),
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

} // C'tor 

// ------------------------------------------------------------------
tChannelEventProb2Jet::tChannelEventProb2Jet(Integrator& integrator,
                                             const TransferFunction& btf,
                                             const TransferFunction& lighttf,
                                             const string& name, bool rh) :
   EventProb2Jet(rh ? name + " right-handed" : name, integrator, 3, 2, btf),
   m_lightTF(lighttf),
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

} // Protected C'tor 

// ------------------------------------------------------------------
// This method sets the Top mass and the Top width
void tChannelEventProb2Jet::setTopMassAndWidth(double mTop) {

  // Set the mass
  m_massTop = mTop;

  // Use the theoretical Top width for the given mass 
  m_widthTop =  calcTopWidth(mTop);

}//setTopMassAndWidth

// ------------------------------------------------------------------
void tChannelEventProb2Jet::setDynamicBounds()
{
   const float lowPercent = .01;
   const float highPercent = .02;
   double lower, upper;
   getBTF().getBounds(getMeasuredColl()->getFullJet(0), lowPercent,
                      highPercent, lower, upper);
   std::cout << "\tSetting jet 1 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(1, lower, upper);
   m_lightTF.getBounds(getMeasuredColl()->getFullJet(1), lowPercent,
                       highPercent, lower, upper);
   std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(2, lower, upper);
}

// ------------------------------------------------------------------
unsigned tChannelEventProb2Jet::getProbMax() const
{
   return getMeasuredColl()->getNBtags() == 1 ? 1 : 2;
}

// ------------------------------------------------------------------
void tChannelEventProb2Jet::changeVars(const vector<double>& parameters)
{
   using PeterFunctions::Math::square;

   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);

   jet1.SetRho(parameters[1]);
   jet1.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[1])));
   jet2.SetRho(parameters[2]);
   jet2.SetE(parameters[2]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}

// ------------------------------------------------------------------
double tChannelEventProb2Jet::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

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
      Array2 vec1 = DHELAS::ixxxxx<2>(partons->getQuark2(), bMass, 1);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1, m_rh);

      Array1 vec6 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec7 = DHELAS::fvoxxx(vec4, vec6, factor, m_massTop, m_widthTop);
      Array1 vec8 = DHELAS::jioxxx(vec0, vec5, m_factor, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec1, vec7, vec8, m_factor);

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

      Array2 vec0 = DHELAS::oxxxxx<2>(partons->getQuark1(), bMass, -1);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1, m_rh);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(1), 0, -1, m_rh);

      Array1 vec6 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec7 = DHELAS::fvixxx(vec4, vec6, factor, m_massTop, m_widthTop);
      Array1 vec8 = DHELAS::jioxxx(vec5, vec1, m_factor, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec7, vec0, vec8, m_factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 9;
      }

//      DHELAS::HelVec vec[9];
//      DHELAS::oxxxxx(partons->getQuark1(), bMass, -1, vec[0], kTwo);
//      DHELAS::oxxxxx(partons->getQuark2(), 0, -1, vec[1], kOne);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[4], kTwo);
//      DHELAS::ixxxxx(partons->getJet(1), 0, -1, vec[5], kOne);
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[6]);
//      DHELAS::fvixxx(vec[4], vec[6], factor, m_massTop,
//                     partons->getTopWidth(), vec[7]);
//      DHELAS::jioxxx(vec[5], vec[1], factor, wMass, wWidth, vec[8]);
//      DHELAS::iovxxx(vec[7], vec[0], vec[8], factor, output);
   }

   answer /= 36;

   return answer;

////   const PartonColl* partons = getPartonColl();
//
//   double fortranArray[6][4];
//   makeFortranArray(fortranArray);
//
//   double topMass = m_massTop ;
////   const double wMass = MEConstants::wMass;
//
////   for (int i = 0; i < 6; ++i)
////      for (int j = 0; j < 4; ++j)
////      {
////         std::cerr << "Input " << i << " TLV " << j << ": " << fortranArray[i][j] << std::endl;
////      }
//////   exit(1);
////
////   std::cerr << "Top mass: " << topMass << std::endl;
////   std::cerr << "W mass: " << wMass << std::endl;
//
//   double answer2;
//
//   if (partons->getLepCharge() > 0)
//   {
//      mytchanud_(fortranArray, &topMass, &wMass, &answer);
////      answer2 = 0;
//      mytchandxux_(fortranArray, &topMass, &wMass, &answer2);
//   }
//   else
//   {
//      mytchanuxdx_(fortranArray, &topMass, &wMass, &answer);
//      mytchandu_(fortranArray, &topMass, &wMass, &answer2);
//   }
//
////   static double max = 0;
////   if (answer > max)
////   {
////      std::cout << "New maximum probability: " << answer 
////                << "   Pz value of: " << partons->getNeutrino().Pz() << std::endl;
////      max = answer;
////   }
//
////   std::cerr << "t-channel: " << answer << " " << answer2 << std::endl;
//
////   exit(1);
//   return answer + answer2;
}


// ------------------------------------------------------------------
void tChannelEventProb2Jet::setQuarkIDs() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setProtonType(kUp);
      getMeasuredColl()->setAntiprotonType(kBottom);
   }
   else
   {
      getMeasuredColl()->setProtonType(kBottom);
      getMeasuredColl()->setAntiprotonType(kUp);
   }
}

// ------------------------------------------------------------------
void tChannelEventProb2Jet::getScale(double& scale1, double& scale2) const
{
   using PeterFunctions::Math::square;
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
//   std::cerr << "t-channel scales: " << scale1 << " " << scale2 << std::endl;
}

// ------------------------------------------------------------------
double tChannelEventProb2Jet::totalTF() const
{
   return getBTF().getTF(getPartonColl()->getFullJet(0),
                         getMeasuredColl()->getFullJet(0))
      * m_lightTF.getTF(getPartonColl()->getFullJet(1),
                        getMeasuredColl()->getFullJet(1));
}

// ------------------------------------------------------------------
bool tChannelEventProb2Jet::onSwitch()
{
   if (getLoop())
   {
      if (getMeasuredColl()->getNBtags() == 1)
         return false;

      swapJets(0, 1);
   }
   return true;
}

// ------------------------------------------------------------------
void tChannelEventProb2Jet::setupIntegral(){

  std::cout<<"\tTop mass= "<< m_massTop<<" width= "<<m_widthTop<<std::endl;

}


// ------------------------------------------------------------------
tChannelEventProb2JetAlt::tChannelEventProb2JetAlt(Integrator& integrator,
                                                   const TransferFunction& btf,
                                                   const TransferFunction& ltf,
                                                   bool rh) :
   tChannelEventProb2Jet(integrator, btf, ltf, "t-channel alternate", rh)
{ }

// ------------------------------------------------------------------
double tChannelEventProb2JetAlt::matrixElement() const
{
   typedef SimpleArray<DHELAS::HelArray, 1> Array1;
   typedef SimpleArray<DHELAS::HelArray, 2> Array2;

   using MEConstants::bMass;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const PartonColl* partons = getPartonColl();

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

      Array2 vec0 = DHELAS::ixxxxx<2>(partons->getQuark1(), bMass, 1);
      Array1 vec1 = DHELAS::oxxxxx<1>(partons->getQuark2(), 0, -1, m_rh);
//      Array1 vec2 = DHELAS::ixxxxx<1>(partons->getLepton(), 0, -1);
      Array1 vec3 = DHELAS::oxxxxx<1>(partons->getNeutrino(), 0, 1);
      Array2 vec4 = DHELAS::oxxxxx<2>(partons->getJet(0), bMass, 1);
      Array1 vec5 = DHELAS::ixxxxx<1>(partons->getJet(1), 0, -1, m_rh);

      Array1 vec6 = DHELAS::jioxxx(vec2, vec3, factor, wMass, wWidth);
      Array2 vec7 = DHELAS::fvoxxx(vec4, vec6, factor, m_massTop, m_widthTop);

      Array1 vec8 = DHELAS::jioxxx(vec5, vec1, m_factor, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec0, vec7, vec8, m_factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 9;
      }

//      DHELAS::HelVec vec[9];
//      DHELAS::ixxxxx(partons->getQuark1(), bMass, 1, vec[0], kTwo);
//      DHELAS::oxxxxx(partons->getQuark2(), 0, -1, vec[1], kOne);
//      DHELAS::ixxxxx(partons->getLepton(), 0, -1, vec[2], kOne);
//      DHELAS::oxxxxx(partons->getNeutrino(), 0, 1, vec[3], kOne);
//      DHELAS::oxxxxx(partons->getJet(0), bMass, 1, vec[4], kTwo);
//      DHELAS::ixxxxx(partons->getJet(1), 0, -1, vec[5], kOne);
//      DHELAS::jioxxx(vec[2], vec[3], factor, wMass, wWidth, vec[6]);
//      DHELAS::fvoxxx(vec[4], vec[6], factor, m_massTop, m_widthTop, vec[7]);
//      DHELAS::jioxxx(vec[5], vec[1], factor, wMass, wWidth, vec[8]);
//      DHELAS::iovxxx(vec[0], vec[7], vec[8], factor, output2);
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
      Array2 vec1 = DHELAS::oxxxxx<2>(partons->getQuark2(), bMass, -1);
//      Array1 vec2 = DHELAS::oxxxxx<1>(partons->getLepton(), 0, 1);
      Array1 vec3 = DHELAS::ixxxxx<1>(partons->getNeutrino(), 0, -1);
      Array2 vec4 = DHELAS::ixxxxx<2>(partons->getJet(0), bMass, -1);
      Array1 vec5 = DHELAS::oxxxxx<1>(partons->getJet(1), 0, 1, m_rh);

      Array1 vec6 = DHELAS::jioxxx(vec3, vec2, factor, wMass, wWidth);
      Array2 vec7 = DHELAS::fvixxx(vec4, vec6, factor, m_massTop, m_widthTop);
      Array1 vec8 = DHELAS::jioxxx(vec0, vec5, m_factor, wMass, wWidth);

      OutputType output = DHELAS::iovxxx(vec7, vec1, vec8, m_factor);

      for (unsigned i = 0; i < vecSize; ++i)
      {
         answer += std::norm(-output[i]) * 9;
      }

//      DHELAS::HelVec vec[9];
//      DHELAS::ixxxxx(partons->getQuark1(), 0, 1, vec[0], kOne);
//      DHELAS::oxxxxx(partons->getQuark2(), bMass, -1, vec[1], kTwo);
//      DHELAS::oxxxxx(partons->getLepton(), 0, 1, vec[2], kOne);
//      DHELAS::ixxxxx(partons->getNeutrino(), 0, -1, vec[3], kOne);
//      DHELAS::ixxxxx(partons->getJet(0), bMass, -1, vec[4], kTwo);
//      DHELAS::oxxxxx(partons->getJet(1), 0, 1, vec[5], kOne);
//      DHELAS::jioxxx(vec[3], vec[2], factor, wMass, wWidth, vec[6]);
//      DHELAS::fvixxx(vec[4], vec[6], factor, m_massTop, m_widthTop, vec[7]);
//      DHELAS::jioxxx(vec[0], vec[5], factor, wMass, wWidth, vec[8]);
//      DHELAS::iovxxx(vec[7], vec[1], vec[8], factor, output2);
   }

   answer /= 36;

   return answer;
}

// ------------------------------------------------------------------
void tChannelEventProb2JetAlt::setQuarkIDs() const
{
   if (getMeasuredColl()->getLepCharge() > 0)
   {
      getMeasuredColl()->setProtonType(kBottom);
      getMeasuredColl()->setAntiprotonType(kDown);
   }
   else
   {
      getMeasuredColl()->setProtonType(kDown);
      getMeasuredColl()->setAntiprotonType(kBottom);
   }
}



// ------------------------------------------------------------------
void tChannelEventProb2JetAlt::setupIntegral(){

  std::cout<<"\tTop mass= "<< m_massTop<<" width= "<<m_widthTop<<std::endl;

}
