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
// #include "TAMUWW/MatrixElement/interface/SingleTopNtuple/UCLAnt.hh"

#include <iostream>

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
   int run =
      static_cast<int>(tree->GetBranch("h")->GetLeaf("run")->GetValue());
   int event =
      static_cast<int>(tree->GetBranch("h")->GetLeaf("event")->GetValue());
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
/*
SingleTopNtupleEventFile::SingleTopNtupleEventFile(string filename,
                                                   string treename,
                                                   int jetLevel,
                                                   unsigned nEvents,
                                                   unsigned nSkip,
                                                   unsigned nPrescale,
                                                   bool doCut) :
   RootEventFile(filename, treename, nEvents, nSkip, nPrescale, doCut),
   m_jetCorrLevel(jetLevel)
{
   m_ntuple = new SingleTopNtuple();
   getTree()->SetBranchAddress("SingleTopNtuple", &m_ntuple);
}

SingleTopNtupleEventFile::~SingleTopNtupleEventFile()
{
   delete m_ntuple;
}

bool SingleTopNtupleEventFile::m_cut(const PartonColl& partons) const
{
   float met = m_ntuple->getMet(m_jetCorrLevel, SingleTopNtuple::kNoSys).Pt();

   return met > 25 && partons.getNBtags() > 0;
}

pair<int, int> SingleTopNtupleEventFile::getRunEvent() const
{
   return make_pair(m_ntuple->getRun(), m_ntuple->getEvent());
}

void SingleTopNtupleEventFile::setBranches(PartonColl&)
{}

void SingleTopNtupleEventFile::fillBranches(PartonColl& partons)
{
   int charge = m_ntuple->getLepCharge() > 0 ? 1 : -1;

//   std::cerr << "Filling btags " << m_ntuple->getJet(0)->getSecVtx()
//             << m_ntuple->getJet(1)->getSecVtx() << std::endl;

   partons.setLepton(m_ntuple->getLepton());
   unsigned max = m_getForceJets() ? m_getForceJets()
      : static_cast<unsigned>(m_ntuple->getNJets(SingleTopNtuple::kNoSys));

   for (unsigned i = 0; i < max; ++i)
   {
      const SingleTopJet* myJet = m_ntuple->getJet(i, SingleTopNtuple::kNoSys);
      PartonColl::Jet jet(myJet->getLV(m_jetCorrLevel),
                          myJet->getSecVtx() == 1, myJet->isTaggable(), 
                          myJet->getTrkJetSumPt(), false, 0, 0, 0, 0, 0);
      partons.addJet(jet);
   }
   partons.setLepCharge(charge);
   partons.setDetector(m_ntuple->getDetector());
}


UCLAntEventFile::UCLAntEventFile(string filename, string treename,
                                 int jetLevel, unsigned nEvents,
                                 unsigned nSkip, unsigned nPrescale,
                                 bool doCut) :
   RootEventFile(filename, treename, nEvents, nSkip, nPrescale, doCut)
{
   m_ntuple = new UCLAnt();
   getTree()->SetBranchAddress("UCLAnt", &m_ntuple);

   setJetCorrLevel(jetLevel);
}

UCLAntEventFile::~UCLAntEventFile()
{
   delete m_ntuple;
}

void UCLAntEventFile::setJetCorrLevel(int level)
{
   if ((level > 0 && level < 4) || (level > 7))
   {
      throw std::runtime_error("Invalid jet level in UCLAntEventFile::setJetCorrLevel");
   }
   m_jetCorrLevel = level;
}

pair<int, int> UCLAntEventFile::getRunEvent() const
{
   return make_pair(m_ntuple->h.run, m_ntuple->h.event);
}

bool UCLAntEventFile::m_cut(const PartonColl& partons) const
{
   float met = m_ntuple->h.met;

   return met > 25 && partons.getNBtags() > 0;
}

void UCLAntEventFile::setBranches(PartonColl&)
{}

void UCLAntEventFile::fillBranches(PartonColl& partons)
{
  bool useIsoTrack = false;
  if (m_ntuple->h.det == -1 && m_ntuple->h.isIso) 
    useIsoTrack = true;

   int charge = m_ntuple->h.lepQ > 0 ? 1 : -1;
   if (useIsoTrack) 
     charge = m_ntuple->h.isoQ > 0 ? 1 : -1;

   std::cout << "UCLAntEventFile now reading run " << m_ntuple->h.run << " event "
             << m_ntuple->h.event << std::endl;

//   std::cerr << "njets: " << m_ntuple->njets << std::endl;
   if (useIsoTrack) 
     partons.setLepton(m_ntuple->isotrk[0]);
   else
     partons.setLepton(m_ntuple->lepton[0]);

   unsigned max = m_getForceJets() ? m_getForceJets()
      : static_cast<unsigned>(m_ntuple->njets);

   for (unsigned i = 0; i < max; ++i)
   {
      UCLAJet& jet = m_ntuple->jets[i];

      TLorentzVector energy;
      switch (m_jetCorrLevel)
      {
         case 0:
            energy = jet.lv.raw;
            break;

         case 4:
            energy = jet.lv.l4;
            break;

         case 5:
            energy = jet.lv.l5;
            break;

         case 6:
            energy = jet.lv.l6;
            break;

         case 7:
            energy = jet.lv.l7;
            break;

         default:
            throw std::runtime_error("Invalid jet level in UCLAntEventFile::fillBranches");
      }


      // Decide whether this jet is considered tagged/taggable or not
      double jetIsTagged = 0, jetIsTaggable = 0;
      if (jet.secvTag == 1){
	jetIsTagged = 1; 
	jetIsTaggable = jet.secvTaggable;
      } else if(jet.isJPtag(0.05) == true){
	jetIsTagged = 1; 
	jetIsTaggable = jet.jpTaggable;
      }else{
	jetIsTagged = 0; 
	jetIsTaggable = jet.secvTaggable;
      }		 

      //PartonColl::Jet myJet(energy, jet.secvTag ? 1 : 0, jet.secvTaggable, 
      PartonColl::Jet myJet(energy, jetIsTagged, jetIsTaggable, 
                            jet.trkSumPt, static_cast<bool>(jet.sltm),
                            jet.cone07L5E, jet.EMFraction, jet.NTracks, jet.Ptjet, jet.RawE);

      
      // std::cerr << "*************************" << std::endl;
      // std::cerr << "Jet " << i << " energy: " << energy.E() << std::endl;
      // std::cerr << "Energy 07: " << jet.cone07L5E << std::endl;
      // std::cerr << "EMFraction: " << jet.EMFraction << std::endl;
      // std::cerr << "NTracks: " << jet.NTracks << std::endl;
      // std::cerr << "Ptjet: " << jet.Ptjet << std::endl;
      // std::cerr << "RawE: " << jet.RawE << std::endl;
      // std::cerr << "Njets: " << m_ntuple->njets << std::endl;
      
      partons.addJet(myJet);
   }
   partons.setLepCharge(charge);
   partons.setDetector(m_ntuple->h.det);

}//fillBranches

//Report what we consider important
void UCLAntEventFile::m_report() const
{  
   unsigned max = m_getForceJets() ? m_getForceJets()
      : static_cast<unsigned>(m_ntuple->njets);

   for (unsigned i = 0; i < max; ++i)
   {
      UCLAJet& jet = m_ntuple->jets[i];
      std::cout << "  Jet # " << i << " (unordered)" << " tagged = " <<
         (jet.secvTag ? "SV" : (jet.isJPtag(0.05) ? "JP" : "no")) << std::endl;
   }//for 
}
*/
