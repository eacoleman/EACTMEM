#include "TAMUWW/MatrixElement/interface/EventProb3jet.hh"

#include <iostream>

#include <cmath>
#include <string>
#include <vector>

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"
#include "TAMUWW/Integrator/interface/Integrator.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctionsRoot.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

using std::string;
using std::vector;

using PeterFunctions::Math::square;

extern "C"
{
   void* myschan3jet_(double[][4], double*, const double*, double*);
   void* myschan3jet2_(double[][4], double*, const double*, double*);
   void* mytchan3jetud_(double[][4], double*, const double*, double*);
   void* mytchan3jetuxdx_(double[][4], double*, const double*, double*);
   void* mytchan3jetdxux_(double[][4], double*, const double*, double*);
   void* mytchan3jetdu_(double[][4], double*, const double*, double*);
   void* mywbb3jet_(double[][4], const double*, double*);
   void* mywbb3jet2_(double[][4], const double*, double*);
   void* mytt3jet_(double[][4], double*, const double*, double*);
   void* mytt3jet2_(double[][4], double*, const double*, double*);
}

typedef SimpleArray<DHELAS::HelArray, 1> Array1;
typedef SimpleArray<DHELAS::HelArray, 2> Array2;
typedef SimpleArray<DHELAS::HelArray, 4> Array4;

EventProb3Jet::EventProb3Jet(string name, Integrator& integrator, 
                             unsigned nVars, unsigned nLoop,
                             const TransferFunction& bTF,
                             const TransferFunction& gluonTF) :
   EventProb(name, integrator, nVars, nLoop),
   m_bTF(bTF),
   m_gluonTF(gluonTF)
{}

void EventProb3Jet::getPeaks(VecVecDouble& answer, const double bounds[]) const
{
   PartonColl temp(*getMeasuredColl());
   temp.setMet();

   double jet1E = temp.getJet(0).E();
   double jet2E = temp.getJet(1).E();
   double jet3E = temp.getJet(2).E();

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
   if (jet3E < lower[3] || jet3E > upper[3])
      return;

   TLorentzVector& lepton = temp.getLepton();
   TLorentzVector neutrino = temp.getNeutrino();

//   std::cout << "Lepton: " << lepton.Px() << " " << lepton.Py() << " "
//             << lepton.Pz() << " " << lepton.E() << std::endl;
//   std::cout << "Neutrino: " << neutrino.Px() << " " << neutrino.Py() << " "
//             << neutrino.Pz() << " " << neutrino.E() << std::endl;

   const double mW = MEConstants::wMass;


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
         double phaseSpaceCoord[4] = {*it, jet1E, jet2E, jet3E};
         vector<double> phaseSpaceVec(phaseSpaceCoord, phaseSpaceCoord + 4);
         answer.push_back(phaseSpaceVec);
         std::cerr << "Returning peak: " << phaseSpaceCoord[0] << " "
                   << phaseSpaceCoord[1] << " " << phaseSpaceCoord[2]
                   << " " << phaseSpaceCoord[2]
                   << std::endl;
      }
   }

//   exit(1);
   return;
}

void EventProb3Jet::setDynamicBounds()
{
   const float lowPercent = .01;
   const float highPercent = .02;
   double lower, upper;
   m_bTF.getBounds(getMeasuredColl()->getFullJet(0), lowPercent, highPercent,
                   lower, upper);
   std::cout << "\tSetting jet 1 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(1, lower, upper);
   m_bTF.getBounds(getMeasuredColl()->getFullJet(1), lowPercent, highPercent,
                   lower, upper);
   std::cout << "\tSetting jet 2 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(2, lower, upper);
   m_gluonTF.getBounds(getMeasuredColl()->getFullJet(2), lowPercent,
                       highPercent, lower, upper);
   std::cout << "\tSetting jet 3 bounds from " << lower << " to " << upper
             << std::endl;
   setBounds(3, lower, upper);
}

unsigned EventProb3Jet::getProbMax() const
{
   return getMeasuredColl()->getNBtags() == 2 ? 2 : 4;
}

void EventProb3Jet::changeVars(const vector<double>& parameters)
{
   TLorentzVector& jet1 = getPartonColl()->getJet(0);
   TLorentzVector& jet2 = getPartonColl()->getJet(1);
   TLorentzVector& jet3 = getPartonColl()->getJet(2);

//   std::cerr << "Jet 1: " << jet1.Vect().Mag() << " Jet 2: "
//             << jet2.Vect().Mag()
//             << " Jet 3: " << jet3.Vect().Mag() << std::endl;
//   if (jet3.Vect().Mag() == 0)
//      exit(1);
//   std::cerr << "Par 1: " << parameters[1] << " Par 2: " << parameters[2]
//             << " Par 3: " << parameters[3] << std::endl;

   jet1.SetRho(parameters[1]);
   jet1.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[1])));
   jet2.SetRho(parameters[2]);
   jet2.SetE(std::sqrt(square(MEConstants::bMass) + square(parameters[2])));
   jet3.SetRho(parameters[3]);
   jet3.SetE(parameters[3]);

   getPartonColl()->getNeutrino().SetPz(parameters[0]);
}


double EventProb3Jet::totalTF() const
{
   return m_bTF.getTF(getPartonColl()->getFullJet(0),
                      getMeasuredColl()->getFullJet(0))
      * m_bTF.getTF(getPartonColl()->getFullJet(1),
                    getMeasuredColl()->getFullJet(1))
      * m_gluonTF.getTF(getPartonColl()->getFullJet(2),
                        getMeasuredColl()->getFullJet(2));
}


bool EventProb3Jet::onSwitch()
{
   if (getLoop())
   {
      if (getMeasuredColl()->getNBtags() == 2)
      {
         if (getLoop() > 1)
            return false;
         else
            swapJets(0, 1);
      }
      else
      {
         switch (getLoop())
         {
            case 1:
               swapJets(1, 2);
               break;
               
            case 2:
               swapJets(0, 1);
               break;
               
            case 3:
               swapJets(0, 2);
               break;
         }
      }
   }
   return true;
}


void EventProb3Jet::makeFortranArray(double array[][4]) const
{
   using PeterFunctions::makeArray;

   makeArray(getPartonColl()->getParton1(), array[0]);
   makeArray(getPartonColl()->getParton2(), array[1]);
   makeArray(getPartonColl()->getLepton(), array[2]);
   makeArray(getPartonColl()->getNeutrino(), array[3]);
   makeArray(getPartonColl()->getJet(0), array[4]);
   makeArray(getPartonColl()->getJet(1), array[5]);
   makeArray(getPartonColl()->getJet(2), array[6]);
}
