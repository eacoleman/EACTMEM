#include "TAMUWW/MatrixElement/interface/EventProb.hh"

//#define PRINT
//#define MEONLY
//#define BENCHMARK
#include <iostream>
#include <limits>

#include <cmath>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>

#include <TMath.h>

#include "TAMUWW/Integrator/interface/Integrator.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/DataFile.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctionsRoot.hh"

#ifdef BENCHMARK
#include "TAMUWW/MatrixElement/interface/TStopwatch.h"
TStopwatch tPrelim, tChangeVars, tMet, tQuarks, tME, tPhi4, tTF, tPDF, tTotal;
#endif

extern "C"
{
   void* setctq5_(int&);
   double ctq5pdf_(int&, double&, double&);
   void* setctq6_(int&);
   double ctq6pdf_(int&, double&, double&);
}

using AuxFunctions::Math::square;
using std::cout;
using std::endl;
using std::string;
using std::vector;

PartonColl EventProb::m_partonColl;
PartonColl EventProb::m_measuredColl;


EventProb::EventProb(DEFS::EP::Type ept, Integrator& integrator, unsigned nVars,
                     unsigned nLoop) :
   m_epType(ept),
   m_epParam(0),
   m_integrator(integrator),
   m_nLoop(nLoop),
   m_nVars(nVars),
   m_loop(-1),
   m_bounds(nVars),
   m_volume(1)
{
  m_name = DEFS::EP::getTypeString(m_epType);
   debug= 0;
   int pdfset = 3;
   setctq6_(pdfset);
}

void EventProb::setBounds(unsigned param, double lower, double upper)
{   
   if (param >= m_bounds.size())
      throw std::runtime_error
         ("Parameter subscript out of range in EventProb::setBounds()");

   // Make sure the lower bound !=0
   if ( lower<1.0 ) {
     lower=1.0;
   }
   // correct volume if a value exists already
   if (m_bounds[param].second)
   {
      m_volume /= m_bounds[param].second;
   }

   m_bounds[param] = std::make_pair(lower, upper - lower);
   m_volume *= upper - lower;
}


// This is the loop mechanism to recalculate the ME with the 
// different permutations. It will keep on looping  until either 
// the permutation reaching the given m_nLoop number or the 
// virtual onSwitch method return false;
bool EventProb::incrementLoop()
{
   ++m_loop;
   if (!onSwitch())
   {
      m_loop = -1;
      //cout << "OnSwitch set to return false" << endl;
      return false;
   }

   if (m_loop == static_cast<int>(m_nLoop))
   {
      m_loop = -1;
      //cout << " Maximum reached, m_nLoop=" << m_nLoop << endl;
      return false;
   }
   return true;
  
}

void EventProb::prepareForIntegral()
{

   setPartonTypes();

   setupIntegral();

}

//This is the method called every time during the integration. 
// The integrator parameter go between zero and one.
double EventProb::execute(const double parameters[])
{

#ifdef BENCHMARK
   static unsigned counter = 0;
   tPrelim.Start(kFALSE);
   tTotal.Start(kFALSE);
#endif

   using MEConstants::beamEnergy;

   // This can be removed simply if virtual changeVars were 
   // using the m_measuredColl instead of the m_partonColl. 
   // But this is not so easy as then I would have to know what 
   // to do when swapping jets.
   m_partonColl = m_measuredColl;

//     std::cerr << "Starting execute\n";

#ifdef BENCHMARK
   tPrelim.Stop();
   tChangeVars.Start(kFALSE);
#endif

   // change the variables according to the parameters and their bounds
   m_changeVars(parameters);

//   return (parameters[0] + parameters[1] + parameters[2]) * m_volume;
//   return (m_partonColl.getJet(0).E() + m_partonColl.getJet(1).E() + m_partonColl.getNeutrino().Pz());

#ifdef PRINT
   std::cerr << "partonColl "<<partons;
#endif

#ifdef BENCHMARK
   tChangeVars.Stop();
   tMet.Start(kFALSE);
#endif

   // Set the neutrino components E, Px and Py for the given 
   // jet energies and the neutrino Pz
   met();

#ifdef BENCHMARK
   tMet.Stop();
   tQuarks.Start(kFALSE);
#endif
   if (!isPossible()){
     /*
     if (debug < 2 ) {
      TLorentzVector total;
      getTotalLV(total);
      
      cout<<" mereg is not possible Pz="<<std::abs(total.Pz())
	  <<" E="<<total.E()<<endl;
	  }*/

     return 0;
   }

   //Compute the energies of the incident partons given the sum of 
   //the Lorentz vectors of all final state partons
   double energy1, energy2;
   if (!getPartonEnergies(energy1, energy2))
      return 0;
   
   double x1 = energy1 / beamEnergy;
   double x2 = energy2 / beamEnergy;
   
//   std::cerr << "x1: " << x1 << " x2: " << x2 << std::endl;

   m_partonColl.setQuark1(TLorentzVector(0, 0, energy1, energy1));
   m_partonColl.setQuark2(TLorentzVector(0, 0, -energy2, energy2));

#ifdef BENCHMARK
   tQuarks.Stop();
   tME.Start(kFALSE);
#endif

   //This is the actual calculation of the Matrix Element
   double me = matrixElement();

#ifdef BENCHMARK
   tME.Stop();
   tPhi4.Start(kFALSE);
#endif
   double f4 = phaseSpace();
#ifdef BENCHMARK
   tPhi4.Stop();
   tPDF.Start(kFALSE);
#endif
   double pd = pdf(x1, x2);
#ifdef BENCHMARK
   tPDF.Stop();
   tTF.Start(kFALSE);
#endif
   double tf = totalTF();
#ifdef BENCHMARK
   tTF.Stop();
#endif


   double result = me * f4 * pd * tf;

//   std::cerr << "Matrix element: " << matrixElement() << " Phi4: " << phi4() 
//             << " PDF: " << pdf(x1, x2) << " TF: " << totalTF()
//             << std::endl;
#ifdef PRINT
   std::cerr << "Matrix element: " << me << " Phi4: " << f4
             << " PDF: " << pd << " TF: " << tf
             << std::endl;
   std::cerr << "total: " << result << std::endl;
//   static int counter = 0;
//   if (counter++ == 3)


//      exit(1);
//   std::clog << counter++ << std::endl;
#endif
//   std::cerr << result << std::endl;
//   result = std::numeric_limits<double>::quiet_NaN();
   if (!(result <= 0 || result >= 0))
   {
      result = 0;
   }

#ifdef BENCHMARK
   tTotal.Stop();
   if (counter++ % 10000 == 0)
   {
      std::cout << "Prelim: " << tPrelim.CpuTime()
                << "\nChangeVars: " << tChangeVars.CpuTime()
                << "\nMet: " << tMet.CpuTime()
                << "\nQuarks: " << tQuarks.CpuTime()
                << "\nME: " << tME.CpuTime()
                << "\nPhi4: " << tPhi4.CpuTime()
                << "\nPDF: " << tPDF.CpuTime()
                << "\nTF: " << tTF.CpuTime()
                << "\nTotal: " << tTotal.CpuTime() << std::endl;
   }
#endif

   if (result * m_volume >  m_maxProbNumber)
     m_maxProbNumber = result * m_volume;
   /*
   if (debug <1){
     std::cout<<" p0 ="<<parameters[0]
	      <<" p1 ="<<parameters[1]
	      <<" p2 ="<<parameters[2]
	      <<" res="<<result*m_volume<<std::endl;
     std::cout<<" me ="<<me<<" f4="<<f4<<" pd="<<pd
	      <<" tf ="<<tf<<" vol="<<m_volume<<std::endl;
     std::cout<<" id1P="<<m_partonColl.getProtonType()
	      <<" id2P="<<m_partonColl.getAntiprotonType()<<std::endl;
     std::cout<<" id1M="<<m_measuredColl.getProtonType()
	      <<" id2M="<<m_measuredColl.getAntiprotonType()<<std::endl;
     std::cout<<"   x1="<<x1<<" x2="<<x2<<std::endl;
   }
   */
   /*
  if (debug < 2) {
    cout<<" d="<<debug<<" \tme="<<me<<" tf="<<tf
	<<" pdf(x1="<<x1<<", x2="<<x2<<")="<<pd
	<<" m_volume="<<m_volume<<endl; 
  }
   debug++;
   */

#ifdef MEONLY
   return me;
#else
   return result * m_volume ;
#endif
}


void EventProb::met()
{
   m_partonColl.setMet();
}


bool EventProb::isPossible() const
{
   TLorentzVector total;
   getTotalLV(total);

   if (std::abs(total.Pz()) > MEConstants::beamEnergy)
      return false;

   return total.E() <= 2 * MEConstants::beamEnergy;
}


bool EventProb::getPartonEnergies(double& energy1, double& energy2) const
{
   using MEConstants::beamEnergy;

   TLorentzVector total;
   getTotalLV(total);

   double partial = std::sqrt(square(total.Pz()) + square(total.M()));
   energy1 = (partial + total.Pz()) / 2;
   energy2 = (partial - total.Pz()) / 2;
   
   if (energy1 < 0)
      return false;
   if (energy1 > beamEnergy)
      return false;
   if (energy2 < 0)
      return false;
   if (energy2 > beamEnergy)
      return false;

   return true;
}

void EventProb::getTotalLV(TLorentzVector& vec) const
{
   vec = m_partonColl.getLepton() + m_partonColl.getNeutrino();
   for (unsigned i = 0; i < m_partonColl.getNJets(); ++i)
   {
      vec += m_partonColl.getJet(i);
   }
}


double EventProb::phaseSpace() const
{
   return m_partonColl.getPhaseSpace();
}


// Now, I did a trick here; for the quarkIDs I change the pdf to look at the 
// measured collection instead of the parton collection. 
// Although it may seem more proper to look at the parton here,
// there is no difference for the regular case, but there is a difference
// for the correlated EPD case. Ricardo
double EventProb::pdf(double x1, double x2) const
{
   double scale1, scale2;
   getScale(scale1, scale2);

// This is a requirement of the CTEQ6 PDF table.  Why?  I don't know!
   const double Qlimit = .226;
   if (scale1 < Qlimit || scale2 < Qlimit)
      return 0;

   double quark1E = m_partonColl.getParton1().E();
   double quark2E = m_partonColl.getParton2().E();

   if (quark1E && quark2E)
   {
      int id1 = m_measuredColl.getProtonType();
      int id2 = m_measuredColl.getAntiprotonType();
//   std::cerr << id1 << " " << x1 << " " << scale1 << std::endl;
      double pdf1 = x1 * ctq6pdf_(id1, x1, scale1) / quark1E;
      double pdf2 = x2 * ctq6pdf_(id2, x2, scale2) / quark2E;

      return pdf1 * pdf2;
   }
   else
   {
      return 0;
   }
}

void EventProb::swapJets(unsigned jet1, unsigned jet2)
{
   getPartonColl()->swapJets(jet1, jet2);
   getMeasuredColl()->swapJets(jet1, jet2);
}

void EventProb::adjustBounds(vector<double>& vec) const
{
   for (unsigned i = 0; i < vec.size(); ++i)
   {
      vec[i] = m_bounds[i].first + m_bounds[i].second * vec[i];
   }
}

void EventProb::m_changeVars(const double parameters[])
{

#ifndef MEONLY

   vector<double> trueParams(parameters, parameters + m_nVars);
   adjustBounds(trueParams);
   changeVars(trueParams);

#endif

}//m_changeVars


// This method is designed to work with the calculation of the 
// correlated EPD as calculated in the class EPDEventProb2Jet.cc
// This returns the ME * PDF * TF looped over all the combinations 
// that are pertinent to this ME. Also, if integration over extra
// variables is required it is done also.
double EventProb::matrixElementCorrelated(const vector<double> & trueParams){

  using std::endl;
  using std::cout;
  using MEConstants::beamEnergy;
  
  // Change the variables that might be needed
  changeVars(trueParams);

  // Set the neutrino components Px and Py for the given 
  // jet energies and the neutrino Pz. This uses only the 
  // lepton and jets Px and Py components, which is independent of 
  // the ME's
  met();
  
  // The incoming parton energies will be set inside the 
  // method matrixElementCorrelated() of each Event Prob.
  // This is because it depends whether we assume the partons
  if (!isPossible())    {
    /*
    if (debug < 2 ) {

      TLorentzVector total;
      getTotalLV(total);
      
      cout<<" mecorr is not possible Pz="<<std::abs(total.Pz())
	  <<" E="<<total.E()<<endl;
    }
    */
    return 0;
  }

  //Compute the energies of the incident partons given the sum of 
  //the Lorentz vectors of all final state partons
  double energy1, energy2;
  if (!getPartonEnergies(energy1, energy2)) return 0;
  getPartonColl()->setQuark1(TLorentzVector(0, 0, energy1, energy1));
  getPartonColl()->setQuark2(TLorentzVector(0, 0, -energy2, energy2));

  int counter = 0;

  double x1 = energy1 / beamEnergy; 
  double x2 = energy2 / beamEnergy; 

  // the only thing that depends on the permutations are 
  // the pdf's
  double me_pdf = 0;

  //Loop over all the permutations of one ME probability
  while (incrementLoop()){

    // set the quark Id's,
    // (i.e. initial partons are supposed to be quarks, gluons, etc)
    setPartonTypes();
    me_pdf += matrixElement()*pdf(x1,x2);
    cout << "me_pdf=" << me_pdf << endl;
    counter ++;
  } // while permutations

  double tf = totalTF();
  /*
  if (debug < 2 ) {
    cout<<" mecorr d="<<debug<<" \tme="<<matrixElement()<<" tf="<<tf
	<<" pdf(x1="<<x1<<", x2="<<x2<<")="<<pdf(x1,x2)
	<<" par[4]="<<trueParams[4]<<endl; 
  }
  debug++
  */
  return me_pdf * tf * phaseSpace() / counter;

}// matrixElementCorrelated
