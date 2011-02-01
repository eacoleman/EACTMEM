#include "TAMUWW/MatrixElement/interface/EventProb2jet.hh"

#include <iostream>

#include <cmath>
#include <string>
#include <vector>

#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctionsRoot.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

using std::string;
using std::vector;

using PeterFunctions::Math::square;

EventProb2Jet::EventProb2Jet(string name, Integrator& integrator,
                             unsigned nVars, unsigned nLoop,
                             const TransferFunction& tf) :
   EventProb(name, integrator, nVars, nLoop),
   m_tf(tf)
{}

void EventProb2Jet::getPeaks(VecVecDouble& answer, const double bounds[]) const
{
//   std::cerr << "Calling getPeaks\n";

   PartonColl temp(*getMeasuredColl());
   temp.setMet();

   double jet1E = temp.getJet(0).E();
   double jet2E = temp.getJet(1).E();

   vector<double> lower, upper;
   for (unsigned i = 0; i < getDimension(); ++i)
   {
      lower.push_back(bounds[2 * i]);
      upper.push_back(bounds[2 * i + 1]);
   }
   adjustBounds(lower);
   adjustBounds(upper);

   if (jet1E < lower[1] || jet1E > upper[1])
      return;
   if (jet2E < lower[2] || jet2E > upper[2])
      return;

   TLorentzVector& lepton = temp.getLepton();
   TLorentzVector neutrino = temp.getNeutrino();

//   std::cout << "Lepton: " << lepton.Px() << " " << lepton.Py() << " "
//             << lepton.Pz() << " " << lepton.E() << std::endl;
//   std::cout << "Neutrino: " << neutrino.Px() << " " << neutrino.Py() << " "
//             << neutrino.Pz() << " " << neutrino.E() << std::endl;

   const double mW = MEConstants::wMass;

// Let's try brute force to avoid algebra errors
//   for (double i = lower[0]; i < upper[0]; ++i)
//   {
//      neutrino.SetPz(i);
//      TLorentzVector total = lepton + neutrino;
////      std::cout << total.M2() << " " << square(mW) << std::endl;
//      if (std::abs(total.M2() - square(mW)) < 100)
//      {
//         double phaseSpaceCoord[3] = {i, jet1E, jet2E};
//         vector<double> phaseSpaceVec(phaseSpaceCoord, phaseSpaceCoord + 3);
//         answer.push_back(phaseSpaceVec);
//         std::cerr << "Returning peak: " << phaseSpaceCoord[0] << " "
//                   << phaseSpaceCoord[1] << " " << phaseSpaceCoord[2]
//                   << std::endl;
//      }
//   }



   double a1 = lepton.Px() * neutrino.Px() + lepton.Py() * neutrino.Py();
   double b1 = square(mW) / 2;
   double c1 = lepton.Pz();
   double d1 = lepton.E();
   double e1 = square(lepton.Px()) + square(lepton.Py());

   double a = square(d1) - square(c1);
   double b = -2 * c1 * (a1 + b1);
   double c = e1 * square(d1) - square(a1) - square(b1) - 2 * a1 * b1;


   double discriminant = square(b) - 4 * a * c;

   vector<double> pz;
   if (discriminant < 0)
   {
      pz.push_back(-b / (2 * a));
   }
   else
   {
      pz.push_back((-b + sqrt(discriminant)) / (2 * a));
      pz.push_back((-b - sqrt(discriminant)) / (2 * a));
   }

   for (vector<double>::const_iterator it = pz.begin(); it != pz.end(); ++it)
   {
      if (*it > lower[0] && *it < upper[0])
      {
         double phaseSpaceCoord[3] = {*it, jet1E, jet2E};
         vector<double> phaseSpaceVec(phaseSpaceCoord, phaseSpaceCoord + 3);
         answer.push_back(phaseSpaceVec);
         std::cerr << "Returning peak: " << phaseSpaceCoord[0] << " "
                   << phaseSpaceCoord[1] << " " << phaseSpaceCoord[2]
                   << std::endl;
      }
   }

//   exit(1);
   return;
}

void EventProb2Jet::setDynamicBounds()
{
   const float lowPercent = .01;
   const float highPercent = .02;
   double lower, upper;
   m_tf.getBounds(getMeasuredColl()->getFullJet(0), lowPercent, highPercent,
                   lower, upper);
   std::cout << "\tSetting jet 1 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(1, lower, upper);
   m_tf.getBounds(getMeasuredColl()->getFullJet(1), lowPercent, highPercent,
                   lower, upper);
   std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(2, lower, upper);
}

void EventProb2Jet::changeVars(const vector<double>& parameters)
{
  std::cout << "ERROR: EventProb2Jet::changeVars is implemented (should be defined in the specific matrix element instead)." << std::endl;
//    TLorentzVector& jet1 = getPartonColl()->getJet(0);
//    TLorentzVector& jet2 = getPartonColl()->getJet(1);
// //   std::cerr << jet2.Px() << " " << jet2.Py() << " " << jet2.Pz() << " " << jet2.E() << std::endl;
//    jet1.SetRho(parameters[1]);
//    jet1.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[1])));
//    jet2.SetRho(parameters[2]);
//    jet2.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[2])));
// //
//    getPartonColl()->getNeutrino().SetPz(parameters[0]);
// // KLUDGE!
// //   getPartonColl()->getNeutrino().SetPz(196 / 5 * (getPartonColl()->getTopMass() - 200) + 980);
// //   std::cerr << "Setting neutrino to " << getPartonColl()->getNeutrino().Pz() << std::endl;
}


double EventProb2Jet::totalTF() const
{
   return m_tf.getTF(getPartonColl()->getFullJet(0),
                     getMeasuredColl()->getFullJet(0))
      * m_tf.getTF(getPartonColl()->getFullJet(1),
                   getMeasuredColl()->getFullJet(1));
}


void EventProb2Jet::makeFortranArray(double array[][4]) const
{
   using PeterFunctions::makeArray;

   makeArray(getPartonColl()->getParton1(), array[0]);
   makeArray(getPartonColl()->getParton2(), array[1]);
   makeArray(getPartonColl()->getLepton(), array[2]);
   makeArray(getPartonColl()->getNeutrino(), array[3]);
   makeArray(getPartonColl()->getJet(0), array[4]);
   makeArray(getPartonColl()->getJet(1), array[5]);
}

void EventProb2Jet::makeFortranArray_qqvl(double array[][4]) const
{
   using PeterFunctions::makeArray;

   makeArray(getPartonColl()->getParton1(), array[0]);
   makeArray(getPartonColl()->getParton2(), array[1]);
   makeArray(getPartonColl()->getNeutrino(), array[2]);
   makeArray(getPartonColl()->getLepton(), array[3]);
   makeArray(getPartonColl()->getJet(0), array[4]);
   makeArray(getPartonColl()->getJet(1), array[5]);
}

//added to for consistency with sChannel & tChannel.
void EventProb2Jet::makeFortranArray_qlvq(double array[][4]) const
{
   using PeterFunctions::makeArray;

   makeArray(getPartonColl()->getParton1(), array[0]);
   makeArray(getPartonColl()->getParton2(), array[1]);
   makeArray(getPartonColl()->getJet(0), array[2]);
   makeArray(getPartonColl()->getLepton(), array[3]);
   makeArray(getPartonColl()->getNeutrino(), array[4]);
   makeArray(getPartonColl()->getJet(1), array[5]);
}

//added to for consistency with ttChannel.
void EventProb2Jet::makeFortranArray_qlvWq(double array[][4],TLorentzVector m_W) const
{
   using PeterFunctions::makeArray;

   makeArray(getPartonColl()->getParton1(), array[0]);
   makeArray(getPartonColl()->getParton2(), array[1]);
   makeArray(getPartonColl()->getJet(0), array[2]);
   makeArray(getPartonColl()->getLepton(), array[3]);
   makeArray(getPartonColl()->getNeutrino(), array[4]);
   makeArray(m_W, array[5]);
   makeArray(getPartonColl()->getJet(1), array[6]);
}

// ------------------------------------------------------------------
// The maximum number of permutations with two jets is just two 
// regardless of the tagging information.Fill onSwitch in this class
// so it affects the default behaviour of all l+2jets ME's.
bool EventProb2Jet::onSwitch(){

  if (getLoop())
    swapJets(0,1);

  return true;

}//onSwitch
