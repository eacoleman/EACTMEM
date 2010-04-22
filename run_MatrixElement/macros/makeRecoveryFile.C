#include <iostream>
#include <set>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "SingleTopNtuple/UCLAnt.hh"

using std::cerr;
using std::cout;
using std::endl;

struct Signature
{
      int run;
      int event;
      int alpgentype;
      int detector;
      float lepPt;
      bool operator<(const Signature& rhs) const
      {
         if (run != rhs.run)
            return run < rhs.run;
         else if (event != rhs.event)
            return event < rhs.event;
         else if (alpgentype != rhs.alpgentype)
            return alpgentype < rhs.alpgentype;
         else if (detector != rhs.detector)
            return detector < rhs.detector;
         else
            return lepPt < rhs.lepPt;
      }
};

void makeRecoveryFile(const char* treeName, const char* tntFiles,
                      const char* meFiles, const char* recoveryFile)
{
   UCLAnt* meNtuple = new UCLAnt();

   TChain meChain("METree");
   meChain.Add(meFiles);
   meChain.SetBranchAddress("tnt", &meNtuple);

   long meEntries = long(meChain.GetEntries());

   std::set<Signature> myEvents;

   for (long ientry = 0; ientry < meEntries; ++ientry)
   {
      meChain.GetEntry(ientry);

      Signature temp = {meNtuple->h.run, meNtuple->h.event, meNtuple->h.sample,
                        meNtuple->h.det, meNtuple->lepton[0].Pt()};

      if (myEvents.find(temp) != myEvents.end())
         cerr << "Warning!  Duplicate event found: Run " << meNtuple->h.run
              << " Event " << meNtuple->h.event << endl;

      myEvents.insert(temp);
   }
   cout << "Found " << myEvents.size() << " ME events" << endl;

   TChain tntChain(treeName);
   tntChain.Add(tntFiles);
   UCLAnt* tntNtuple = new UCLAnt();
   tntChain.SetBranchAddress("UCLAnt", &tntNtuple);

   TFile outputFile(recoveryFile, "RECREATE");

   long tntEntries = long(tntChain.GetEntries());
  
   TTree* outputTree;
   if (myEvents.size() > 0)
     outputTree = tntChain.CloneTree(0);
   else {
     outputTree = tntChain.CloneTree(tntEntries);
     outputFile.Write();
     return;
   }

   cout << "Looping over " << tntEntries << " TNT events" << endl;

   for (long ientry = 0; ientry < tntEntries; ++ientry)
   {
      tntChain.GetEntry(ientry);
      
      Signature temp = {tntNtuple->h.run, tntNtuple->h.event,
                        tntNtuple->h.sample, tntNtuple->h.det,
                        tntNtuple->lepton[0].Pt()};
      
      if (myEvents.find(temp) == myEvents.end())
      {
         outputTree->Fill();
      }
   }
   outputFile.Write();

   
   //return the number of written entries
   unsigned nEnt = (unsigned) outputTree->GetEntries();

   cout << "Wrote file " << recoveryFile << " with "
        << nEnt << " events" << endl;

   //return (int)nEnt;

}//makeRecoveryFile
