#ifndef PARTONCOLL_HH
#define PARTONCOLL_HH

// C++ libraries
#include <set>
#include <iostream>

// ROOT libraries
#include "TLorentzVector.h"

class PartonColl
{
   public:
      PartonColl();

      friend std::ostream& operator<<(std::ostream & stream, const PartonColl & partons );

      void clear();

      TLorentzVector& getLepton() {return m_lepton;} 
      TLorentzVector& getNeutrino() {return m_neutrino;}
      TLorentzVector& getParton1() {return m_quark1;}
      TLorentzVector& getParton2() {return m_quark2;}
      const TLorentzVector& getLepton() const {return m_lepton;} 
      const TLorentzVector& getNeutrino() const {return m_neutrino;}
      const TLorentzVector& getParton1() const {return m_quark1;}
      const TLorentzVector& getParton2() const {return m_quark2;}
      int getLepCharge() const {return m_lepCharge;}
      int getDetector() const {return m_detector;}

      unsigned getNJets() const {return m_jets.size();}
      unsigned getNBtags() const {return m_nBJets;}

      int getProtonType() const {return m_quark1type;}
      int getAntiprotonType() const {return m_quark2type;}
      void setParton1Type(int input) {m_quark1type = input;}
      void setParton2Type(int input) {m_quark2type = input;}

// Jet stuff
      struct Jet
      {
	


            Jet(const TLorentzVector& ilv, float itag, bool itaggable, 
                double isumE = 0, bool islt = false, double ienergy07 = 0, double iemfraction = 0, int intracks = 0,
                double iptjet = 0, double irawe = 0 ) :
                lv(ilv), tag(itag), taggable(itaggable), sumE(isumE), slt(islt),
                energy07(ienergy07), emfraction(iemfraction),
                ntracks(intracks), ptjet(iptjet), rawe(irawe) {}	

	friend std::ostream& operator<<(std::ostream & stream, const PartonColl::Jet & fulljet );

            TLorentzVector lv;
            float tag;
            bool taggable;
            double sumE;
            bool slt;
            double energy07;
	    double emfraction;
	    int ntracks;
	    double ptjet;
	    double rawe;

            static bool jetSort(const Jet& left, const Jet& right);
      };
// These are unchecked for performance reasons
      TLorentzVector& getJet(int index) {return m_jets[index].lv;}
      const TLorentzVector& getJet(int index) const {return m_jets[index].lv;}
      Jet& getFullJet(int index) {return m_jets[index];}
      const Jet& getFullJet(int index) const {return m_jets[index];}

      void addJet(const Jet& jet);
      void swapJets(unsigned index1, unsigned index2);


      void setLepton(const TLorentzVector& input) {m_lepton = input;} 
      void setNeutrino(const TLorentzVector& input) {m_neutrino = input;}
      void setQuark1(const TLorentzVector& input) {m_quark1 = input;}
      void setQuark2(const TLorentzVector& input) {m_quark2 = input;}
      void setLepCharge(int input) {m_lepCharge = input;}
      void setDetector(int input) {m_detector = input;}

      void setMet();
      double getPhaseSpace() const;

      void addExtra(const TLorentzVector& input) {m_extra.push_back(input);}
      TLorentzVector& getExtra(unsigned index) {return m_extra.at(index);}
      const TLorentzVector& getExtra(unsigned index) const
      {return m_extra.at(index);}

      double sHat() const;
      double Q2() const;
      double sumPt() const;

   private:

      TLorentzVector m_lepton;
      TLorentzVector m_neutrino;
      TLorentzVector m_quark1;
      TLorentzVector m_quark2;

      std::vector<Jet> m_jets;
      unsigned m_nBJets;

      // Just a kludge
      std::vector<TLorentzVector> m_extra;

      int m_lepCharge;
      int m_detector;

      int m_quark1type;
      int m_quark2type;

      void m_sortJets();
      void m_calcTopWidth();

};



#endif
