/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Modified by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef EVENTFILE_HH
#define EVENTFILE_HH

#include <fstream>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "TLorentzVector.h"

class PartonColl;
class TFile;
class TTree;

class EventFile
{
   public:
      typedef std::vector<TLorentzVector> TLVec;

      EventFile(std::string filename, unsigned nEvents = 0, unsigned nSkip = 0,
                unsigned nPrescale = 1, bool doCut = false);
      virtual ~EventFile() {}

      bool fillNextEvent(PartonColl& partons);

      virtual std::pair<int, int> getRunEvent() const = 0;

      void setForceJets(unsigned value) {m_forcejets = value;}

   protected:
      virtual bool m_fillNextEvent(PartonColl& partons) = 0;
      virtual bool m_cut(const PartonColl& partons) const;
      virtual void m_report() const {};

      unsigned m_getForceJets() const {return m_forcejets;}

   private:

      std::string m_filename;

      unsigned m_nEvents;
      unsigned m_nSkip;
      unsigned m_nPrescale;

      unsigned m_counter;
      bool m_doCut;

      unsigned m_forcejets;
};

class TextEventFile : public EventFile
{
   public:
      TextEventFile(std::string filename, unsigned nEvents = 0,
                    unsigned nSkip = 0, unsigned nPrescale = 1,
                    bool doCut = false);
      virtual ~TextEventFile() {}

      virtual std::pair<int, int> getRunEvent() const;

   protected:
      virtual bool m_fillNextEvent(PartonColl& partons);
      virtual bool m_fillNextEventUnder(PartonColl& partons) = 0;

      std::ifstream& getFile() {return m_file;}

   private:
      std::ifstream m_file;

      unsigned m_counter;
};


class RootEventFile : public EventFile
{
   public:
      RootEventFile(std::string filename, std::string treename,
                    unsigned nEvents = 0, unsigned nSkip = 0,
                    unsigned nPrescale = 1, bool doCut = false);
      virtual ~RootEventFile();

      virtual std::pair<int, int> getRunEvent() const = 0;

   protected:
      virtual bool m_fillNextEvent(PartonColl& partons);

      virtual void setBranches(PartonColl& partons) = 0;
      virtual void fillBranches(PartonColl& partons) = 0;
      virtual bool passCut() {return true;}
      virtual void m_report() const {};

      const TTree* getTree() const {return m_tree;}
      TTree* getTree() {return m_tree;}

   private:
      TFile* m_file;
      TTree* m_tree;
      unsigned m_counter;
      unsigned m_size;
};


class MadEventFile : public TextEventFile
{
   public:
      MadEventFile(std::string filename, unsigned nEvents = 0,
                   unsigned nSkip = 0, unsigned nPrescale = 1,
                   bool doCut = false);
      virtual ~MadEventFile() {}

   protected:
      virtual bool m_fillNextEventUnder(PartonColl& partons);
};

class SmearedMadEventFile : public MadEventFile
{
   public:
      SmearedMadEventFile(std::string filename, unsigned nEvents = 0,
                          unsigned nSkip = 0, unsigned nPrescale = 1,
                          bool doCut = false);
      virtual ~SmearedMadEventFile() {}

   protected:
      virtual bool m_fillNextEventUnder(PartonColl& partons);
};

class RecoRootEventFile : public RootEventFile
{
   public:
      RecoRootEventFile(std::string filename, std::string treename,
                        unsigned nEvents = 0, unsigned nSkip = 0,
                        unsigned nPrescale = 1, bool doCut = false);
      virtual ~RecoRootEventFile() {}

      virtual std::pair<int, int> getRunEvent() const;

   protected:
      virtual bool m_cut(const PartonColl& partons) const;
      virtual void setBranches(PartonColl& partons);
      virtual void fillBranches(PartonColl& partons);

   private:
      TLorentzVector* m_lepton;
      TLorentzVector* m_jet1;
      TLorentzVector* m_jet2;
      TLorentzVector* m_neutrino;
      int* m_quark1type;
      int* m_quark2type;
      int* m_lepCharge;
      int* m_detector;
};


class SingleTopNtuple;

class SingleTopNtupleEventFile : public RootEventFile
{
   public:
      SingleTopNtupleEventFile(std::string filename, std::string treename,
                               int jetLevel, unsigned nEvents = 0,
                               unsigned nSkip = 0, unsigned nPrescale = 1,
                               bool doCut = false);
      virtual ~SingleTopNtupleEventFile();

      void setJetCorrLevel(int level) {m_jetCorrLevel = level;}

      virtual std::pair<int, int> getRunEvent() const;

   protected:
      virtual bool m_cut(const PartonColl& partons) const;
      virtual void setBranches(PartonColl& partons);
      virtual void fillBranches(PartonColl& partons);

   private:
      SingleTopNtuple* m_ntuple;

      int m_jetCorrLevel;
};

class UCLAnt;

class UCLAntEventFile : public RootEventFile
{
   public:
      UCLAntEventFile(std::string filename, std::string treename, int jetLevel,
                      unsigned nEvents = 0, unsigned nSkip = 0,
                      unsigned nPrescale = 1, bool doCut = false);
      virtual ~UCLAntEventFile();

      void setJetCorrLevel(int level);
      
      virtual std::pair<int, int> getRunEvent() const;

      UCLAnt* getPointer() {return m_ntuple;}

   protected:
      virtual bool m_cut(const PartonColl& partons) const;
      virtual void setBranches(PartonColl& partons);
      virtual void fillBranches(PartonColl& partons);
      virtual void m_report() const;

   private:
      UCLAnt* m_ntuple;

      int m_jetCorrLevel;
};


class EventNtuple;

class EventNtupleEventFile : public RootEventFile
{
   public:
      EventNtupleEventFile(std::string filename, std::string treename,
                      unsigned nEvents = 0, unsigned nSkip = 0,
                      unsigned nPrescale = 1, bool doCut = false);
      virtual ~EventNtupleEventFile();
      
      virtual std::pair<int, int> getRunEvent() const;

      EventNtuple* getPointer() {return m_ntuple;}

   protected:
      virtual void setBranches(PartonColl& partons);
      virtual void fillBranches(PartonColl& partons);

   private:
      EventNtuple* m_ntuple;

};

#endif
