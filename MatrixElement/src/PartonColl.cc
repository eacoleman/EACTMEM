#include "TAMUWW/MatrixElement/interface/PartonColl.hh"

#include <iostream>

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include <TLorentzVector.h>

#include "TAMUWW/MatrixElement/interface/MEConstants.hh"

//needed for overload of operator<< for TLorentzVectors
#include "TAMUWW/MatrixElement/interface/PeterFunctionsRoot.hh" 

using std::vector;

PartonColl::PartonColl()
{
   clear();
}

void PartonColl::clear()
{
   m_lepton.SetPxPyPzE(0, 0, 0, 0);
   m_neutrino.SetPxPyPzE(0, 0, 0, 0);
   m_quark1.SetPxPyPzE(0, 0, 0, 0);
   m_quark2.SetPxPyPzE(0, 0, 0, 0);

   m_jets.clear();
   m_nBJets = 0;
   m_extra.clear();
   m_lepCharge = 0;
   m_detector = 0;
   m_quark1type = 0;
   m_quark2type = 0;
 }

void PartonColl::addJet(const Jet& jet)
{
   Jet myJet(jet);

   if (jet.tag > 0)
      ++m_nBJets;

   m_jets.push_back(myJet);
   m_sortJets();
}

void PartonColl::swapJets(unsigned index1, unsigned index2)
{
   std::swap(m_jets.at(index1), m_jets.at(index2));
}

void PartonColl::setMet()
{
   using PeterFunctions::Math::square;

   double jetPx = 0, jetPy = 0;
   for (vector<Jet>::const_iterator it = m_jets.begin(); it != m_jets.end();
        ++it)
   {
      jetPx += it->lv.Px();
      jetPy += it->lv.Py();
   }

   double px = -(m_lepton.Px() + jetPx);
   double py = -(m_lepton.Py() + jetPy);
   double e = std::sqrt(square(px) + square(py) + square(m_neutrino.Pz()));
   m_neutrino.SetPxPyPzE(px, py, m_neutrino.Pz(), e);
}

double PartonColl::getPhaseSpace() const
{
   using PeterFunctions::Math::square;

   double jetP = 1, jetE = 1;
   for (vector<Jet>::const_iterator it = m_jets.begin(); it != m_jets.end();
        ++it)
   {
      jetP *= it->lv.P();
      jetE *= it->lv.E();
   }

   double denom = m_lepton.E() * m_neutrino.E() * jetE;
   if (denom)
      return square(jetP) / denom;
   else
      return 0;
}

double PartonColl::sHat() const
{
   return (m_quark1 + m_quark2).Mag2();
}

double PartonColl::Q2() const
{
   // This chooses the valence quark of the two input quarks
   const TLorentzVector& quark = (m_quark1type == 1 || m_quark1type == 2)
      ? m_quark1 : m_quark2;
   // Q2 = -q2 = -t
   return -(quark - m_jets.back().lv).Mag2();
}

double PartonColl::sumPt() const
{
   double answer = 0;
   for (vector<Jet>::const_iterator it = m_jets.begin(); it != m_jets.end();
        ++it)
   {
      answer += it->lv.Pt();
   }
   return answer;
}

void PartonColl::m_sortJets()
{
   std::sort(m_jets.begin(), m_jets.end(), Jet::jetSort);
}


// This method orders jets such that after sorting all of the 
// conditions below are satified.
// -- Tagged jets are first
// -- Taggable jets are second
// -- Higher energy jets are third 
bool PartonColl::Jet::jetSort(const Jet& left, const Jet& right)
{
   using PeterFunctions::Math::square;

   if (left.tag != right.tag)
      return left.tag > right.tag;
   else if (left.taggable != right.taggable)
      return left.taggable;
   else
   {
      double l = square(left.lv.Pt()) + square(left.lv.Mt());
      double r = square(right.lv.Pt()) + square(right.lv.Mt());
      return l > r;
   }
}

// Overload the stream operator to report to the output
std::ostream& operator<<(std::ostream & str , const PartonColl & partons ){

   //Report the input values
   str << std::endl
       << "\tLepton= " << partons.getLepton() << " Charge= "
       << partons.getLepCharge() << std::endl;
   for (unsigned ijet = 0; ijet < partons.getNJets(); ++ijet){
     PartonColl::Jet fj = partons.getFullJet(ijet);

     str <<"\tJet  " << ijet << "= " << partons.getJet(ijet)
	 <<" tagged= "<< (fj.tag ==1 ? "yes" : "no") <<std::endl;  

     // write the details of the jet
     str<<fj;

   }

  return str;

}//operator<<

// Overload the stream operator to report to the output
std::ostream& operator<<(std::ostream & str , const PartonColl::Jet & fj ){

  str<<" \t\t tag="<<fj.tag<<" taggable="<<fj.taggable<<" sumE="<<fj.sumE<<std::endl
     <<" \t\t slt="<<fj.slt<<" energy07="<<fj.energy07<<" emfraction="<<fj.emfraction<<std::endl
     <<" \t\t ntracks="<<fj.ntracks<<" ptjet="<<fj.ptjet<<" rawe="<<fj.rawe<<std::endl;
  
  return str;

}//operator<<
