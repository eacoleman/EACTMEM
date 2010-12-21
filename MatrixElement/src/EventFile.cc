/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Modified by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "TAMUWW/MatrixElement/interface/EventFile.hh"

#include <cmath>
#include <cstdlib>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>


#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"
// #include "TAMUWW/MatrixElement/interface/SingleTopNtuple/SingleTopNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

#include "TLorentzVector.h"
#include <iostream>
using namespace std;

using std::make_pair;
using std::pair;
using std::runtime_error;
using std::string;
using std::vector;

EventFile::EventFile(string filename, unsigned nEvents, unsigned nSkip,
                     unsigned nPrescale, bool doCut) :
   m_filename(filename),
   m_nEvents(nEvents),
   m_nSkip(nSkip),
   m_nPrescale(nPrescale),
   m_counter(0),
   m_doCut(doCut),
   m_forcejets(0)
{}

bool EventFile::fillNextEvent(PartonColl& partons)
{
   do {
      if (m_nEvents != 0 && m_counter >= m_nEvents * m_nPrescale + m_nSkip)
         return false;

      do {
         partons.clear();
         if (!m_fillNextEvent(partons))
            return false;

      } while (m_doCut && !m_cut(partons));

      ++m_counter;
   } while (m_counter - 1 < m_nSkip || (m_counter - 1) % m_nPrescale != 0);

   // report what you want
   m_report();
   
   return true;
}

bool EventFile::m_cut(const PartonColl& partons) const
{
   return partons.getNBtags() > 0 && partons.getNeutrino().Pt() > 25;
}

TextEventFile::TextEventFile(string filename, unsigned nEvents, unsigned nSkip,
                             unsigned nPrescale, bool doCut) :
   EventFile(filename, nEvents, nSkip, nPrescale, doCut),
   m_file(filename.c_str(), std::ios::in)
{
   if (!m_file)
   {
      std::ostringstream error;
      error << "Unable to open " << filename << " in TextEventFile!";
      throw runtime_error(error.str().c_str());
   }
}

pair<int, int> TextEventFile::getRunEvent() const
{
   return make_pair((int)1, m_counter);
}

bool TextEventFile::m_fillNextEvent(PartonColl& partons)
{
   bool response = m_fillNextEventUnder(partons);

   if (response)
      ++m_counter;

   return response;
}

RootEventFile::RootEventFile(string filename, string treename,
                             unsigned nEvents, unsigned nSkip,
                             unsigned nPrescale, bool doCut) :
   EventFile(filename, nEvents, nSkip, nPrescale, doCut),
   m_file(TFile::Open(filename.c_str())),
   m_tree(0),
   m_counter(0)
{
   if (!m_file)
   {
      std::ostringstream error;
      error << "Unable to open " << filename << " in RootEventFile!";
      throw runtime_error(error.str().c_str());
   }

   m_file->GetObject(treename.c_str(), m_tree);
   if (!m_tree)
   {
      std::ostringstream error;
      error << "Unable to find tree " << treename << " in " << filename << "!";
      throw runtime_error(error.str().c_str());
   }

   m_size = static_cast<unsigned>(m_tree->GetEntries());
}

RootEventFile::~RootEventFile()
{
   delete m_tree;
   delete m_file;
}

bool RootEventFile::m_fillNextEvent(PartonColl& partons)
{
   if (m_counter >= m_size)
      return false;

   setBranches(partons);
   m_tree->GetEntry(m_counter++);
   fillBranches(partons);
   

   return true;
}

MadEventFile::MadEventFile(string filename, unsigned nEvents, unsigned nSkip,
                           unsigned nPrescale, bool doCut) :
   TextEventFile(filename, nEvents, nSkip, nPrescale, doCut)
{}

bool MadEventFile::m_fillNextEventUnder(PartonColl& partons)
{
   using std::getline;
   using std::istringstream;

   istream& file = getFile();

   if (file.eof())
      return false;

   string nextline;
   while (file && nextline != "<event>")
   {
      if (!file)
         return false;

      getline(file, nextline);
   }

   getline(file, nextline);

   unsigned nParticles;
   istringstream str(nextline);
   str >> nParticles;

   for (unsigned ipart = 0; ipart < nParticles; ++ipart)
   {
      getline(file, nextline);
      istringstream partStr(nextline);

//      std::cerr << partStr.str() << std::endl;

      int id, status, junk;
      float px, py, pz, e, mass;
      partStr >> id >> status >> junk >> junk >> junk >> junk >> px >> py >> pz
              >> e >> mass >> mass;
      
      if (!partStr)
//         throw std::runtime_error("Bad MadEvent file format!");
         return false;

      if (std::abs(status) != 1 )
         continue;

      TLorentzVector vec(px, py, pz, e);
      unsigned absid = std::abs(id);

      if (status == -1) 
      {
         if (id == 2)
         {
            partons.setQuark1(vec);
         }
         else
         {
            partons.setQuark2(vec);
         }
      }
      else if (status == 1)
      {
         if (absid == 11 || absid == 13 || absid == 15)
         {
            partons.setLepton(vec);         
            partons.setLepCharge(id > 0 ? -1 : 1);
         }
         else if (absid == 12 || absid == 14 || absid == 16)
         {
            partons.setNeutrino(vec);
         }
         else if ((absid > 0 && absid <= 6) || absid == 21)
         {
            //KLUDGE
            if (partons.getNJets() >= 3)
            {
//               std::cerr << "Adding extra, energy " << vec.E() << std::endl;
               partons.addExtra(vec);
            }
            else
            {
               bool bjet = (absid == 5);
	       PartonColl::Jet jet(vec, bjet ? 1 : 0, bjet, 0, false, 0, 0, 0, 0, 0);
	       partons.addJet(jet);
            }
         }
         else
         {
            std::cout << "Adding extra particle, id " << id << std::endl;
            partons.addExtra(vec);
         }
      }
   }

   return true;
}

SmearedMadEventFile::SmearedMadEventFile(string filename, unsigned nEvents,
                                         unsigned nSkip, unsigned nPrescale,
                                         bool doCut) :
   MadEventFile(filename, nEvents, nSkip, nPrescale, doCut)
{}

bool SmearedMadEventFile::m_fillNextEventUnder(PartonColl& partons)
{
   using PeterFunctions::Math::square;

   if (!MadEventFile::m_fillNextEvent(partons))
      return false;

   static bool initialized = false;

   if (!initialized)
   {
      std::srand(1);
      initialized = true;
   }

   for (unsigned i = 0; i < partons.getNJets(); ++i)
   {
      bool repeat = true;

      TLorentzVector& jet = partons.getJet(i);

      while (repeat)
      {
         double v1 = static_cast<double>(std::rand()) / RAND_MAX * 400;
         double v2 = static_cast<double>(std::rand()) / RAND_MAX * .5;
         
         double x = v1;
         double y = jet.E();
         bool isB = i >= partons.getNBtags();
         double smear = isB ? .15 : .2;
         double fxy = std::exp(-.5 * square((x - y) / y / smear));

         if (fxy < v2)
            continue;
            
         double newP = isB ?
            std::sqrt(square(v1) - square(MEConstants::bMass)) : v1;

//         if (jet.E() < newP)
//            newP = 0;

         std::cerr << "Old jet energy: " << jet.E();

         jet *= newP / jet.E();

         std::cerr << "  New jet energy: " << jet.E() << std::endl;

         repeat = false;
      }

   }

//   exit(1);
   return true;
}
//

RecoRootEventFile::RecoRootEventFile(string filename, string treename,
                                     unsigned nEvents, unsigned nSkip,
                                     unsigned nPrescale, bool doCut) :
   RootEventFile(filename, treename, nEvents, nSkip, nPrescale, doCut),
   m_lepton(0),
   m_jet1(0),
   m_jet2(0)
{}

pair<int, int> RecoRootEventFile::getRunEvent() const
{
   TTree* tree = const_cast<TTree*>(getTree());
   int run = static_cast<int>(tree->GetBranch("h")->GetLeaf("run")->GetValue());
   int event = static_cast<int>(tree->GetBranch("h")->GetLeaf("event")->GetValue());
   return make_pair(run, event);
}

bool RecoRootEventFile::m_cut(const PartonColl& partons) const
{
   TTree* tree = const_cast<TTree*>(getTree());
   float met = 
      static_cast<int>(tree->GetBranch("h")->GetLeaf("met")->GetValue());

   return met > 25 && partons.getNBtags() > 0;
}

void RecoRootEventFile::setBranches(PartonColl& partons)
{
   TTree* tree = getTree();
//
   delete m_lepton;
   delete m_jet1;
   delete m_jet2;

   m_lepton = new TLorentzVector();
   m_jet1 = new TLorentzVector();
   m_jet2 = new TLorentzVector();

   tree->SetBranchAddress("j1LV_L5.", &m_jet1);
   tree->SetBranchAddress("j2LV_L5.", &m_jet2);
   tree->SetBranchAddress("lLV.", &m_lepton);


//   m_jet1 = partons.getJet1Address();
//   m_jet2 = partons.getJet2Address();
//   m_lepton = partons.getLeptonAddress();
//   m_neutrino = partons.getNeutrinoAddress();
//
//   tree->SetBranchAddress("j1LV_L5.", &m_jet1);
//   tree->SetBranchAddress("j2LV_L5.", &m_jet2);
//   tree->SetBranchAddress("lLV.", &m_lepton);
//   tree->SetBranchAddress("nLV_L5.", &m_neutrino);
}

void RecoRootEventFile::fillBranches(PartonColl& partons)
{
   TTree* tree = getTree();

   partons.setLepton(*m_lepton);
   partons.setNeutrino(*m_neutrino);
   PartonColl::Jet jet1(*m_jet1, static_cast<int>(tree->GetBranch("h")
                                      ->GetLeaf("secvtxTag")->GetValue(0)),
                        true, 0, false, 0, 0, 0, 0, 0);
   PartonColl::Jet jet2(*m_jet2, static_cast<int>(tree->GetBranch("h")
                                      ->GetLeaf("secvtxTag")->GetValue(1)),
                        true, 0, false, 0, 0, 0, 0, 0);


   partons.addJet(jet1);
   partons.addJet(jet2);

   partons.setLepCharge(static_cast<int>(tree->GetBranch("h")
                                         ->GetLeaf("lQ")->GetValue()));
   partons.setDetector(static_cast<int>(tree->GetBranch("h")
                                        ->GetLeaf("detector")->GetValue()));

//   partons.setBtag1();
//   partons.setBtag2(static_cast<int>(tree->GetBranch("h")
//                                     ->GetLeaf("secvtxTag")->GetValue(1)));
}







EventNtupleEventFile::EventNtupleEventFile(string filename, string treename, 
				       unsigned nEvents, unsigned nSkip, 
				       unsigned nPrescale, bool doCut) :
  RootEventFile(filename, treename, nEvents, nSkip, nPrescale, doCut)
{
  m_ntuple = new EventNtuple();
  getTree()->SetBranchAddress("EvtNtuple", &m_ntuple);
}

EventNtupleEventFile::~EventNtupleEventFile()
{
   delete m_ntuple;
}

pair<int, int> EventNtupleEventFile::getRunEvent() const
{
  return make_pair(m_ntuple->run, m_ntuple->event);
}


void EventNtupleEventFile::setBranches(PartonColl&)
{}

void EventNtupleEventFile::fillBranches(PartonColl& partons)
{

  partons.setLepton(m_ntuple->lLV[0]);
  partons.setNeutrino(m_ntuple->METLV[0]);

  PartonColl::Jet jet1(m_ntuple->jLV[0], m_ntuple->jBtag[0],
		       true, 0, false, 0, 0, 0, 0, 0);
  PartonColl::Jet jet2(m_ntuple->jLV[1], m_ntuple->jBtag[1],
		       true, 0, false, 0, 0, 0, 0, 0);
  
  partons.addJet(jet1);
  partons.addJet(jet2);
  
  partons.setLepCharge(m_ntuple->lQ);
  partons.setDetector(m_ntuple->ldetComp);

}//fillBranches
