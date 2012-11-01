// C++ libraries
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// ROOT libraries
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TLorentzVector.h"

// This code libraries
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::set;

//-----------------------------------------------------------------------------
// This struct is used to hold the signature of the microntuple
struct Signature
{
  int run;
  int event;

  // operator < is needed for the set ordering
  bool operator<(const Signature& rhs) const{
    if (run != rhs.run)
      return run < rhs.run;    
    return event < rhs.event;
  }//operator<

};


//-----------------------------------------------------------------------------
// This is the core of the makeMicroNtuple algorithm.
void makeMicroNtuple(TChain & chain, string output, unsigned nJets, 
		     bool doLight=false, bool doNonW= false, bool doUntag=false)
{

  // Create the base objects
  METree      * meNtuple    = new METree();
  EventNtuple * eventNtuple = new EventNtuple();
  MicroNtuple * microNtuple = new MicroNtuple(nJets);

  MicroNtuple::indexMap2 indexMap = MicroNtuple::getIndexMap();
  MicroNtuple::indexMap1 indexMapWH = indexMap[DEFS::EP::WH];
  MicroNtuple::indexMap1 indexMapHWW = indexMap[DEFS::EP::HWW];

  // and set the meNtuple to read from it
  chain.SetBranchAddress("METree", &meNtuple);

  // Create and output file and clone the tree that will be in the output
  // and set microNtuple that fills it
  TFile outputFile(output.c_str(), "RECREATE");
  TTree* outputTree = chain.CloneTree(0);
  outputTree->Branch("mnt", "MicroNtuple", &microNtuple);


  // Get the entries and report if zero
  unsigned nentries = static_cast<unsigned>(chain.GetEntries());
  cout << "\tOriginal chain has " << nentries << " entries" << endl;
  if (nentries == 0){
      cout << "\tNo entries found!  Aborting.\n";
      return;
  }

  // Holds the list of events to avoid duplication
  set<Signature> sigSet;

  //Loop over all the entries in the original chain. The idea is 
  // to copy everything to microNtuple
  for (unsigned ientry = 0; ientry < nentries; ++ientry){

    // get the entry
    //cout << "Processing Entry Number " << ientry << endl;
    chain.GetEntry(ientry);

    // clear the microNtuple
    microNtuple->clear();

    // First copy all the event probabilities
    for (int i = 0; i < meNtuple->getNProbStat(); ++i){
      microNtuple->eventProb[i]    = meNtuple->getProbStat(i)->tEventProb;
      //cout << "eventProb[i]=" << microNtuple->eventProb[i] << endl;
      microNtuple->eventMaxProb[i] = meNtuple->getProbStat(i)->tEventMaxProb;
      //cout << "eventMaxProb[i]=" << microNtuple->eventMaxProb[i] << endl;
    } 
    
    
    // Get the b-probabilites for each jet.
    for (unsigned int bb = 0; bb < eventNtuple->jBtagSSV.size(); bb++)
      microNtuple->bProb[bb] = eventNtuple->jBtagSSV[bb];
    
    // WWandWZ EPD, for 
    //      DEFS::pretag is calculated ignoring the number of tags or bProb's   
    //      DEFS::eqxTSV is calculated using the number of tags or bProb's,
    //                   and the only diff between them is the coefficients.
    microNtuple->epdPretagWWandWZ = microNtuple->calcWZEPD(DEFS::pretag);

//     microNtuple->epd0tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq0TSV);
//     microNtuple->epd1tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq1TSV);
//     microNtuple->epd2tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq2TSV);

//     // Make sure to do these last!
//     microNtuple->epd1tag      = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kCombined);
//     microNtuple->epd2tag      = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kCombined);
//     microNtuple->epd1tagSchan = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kSchan);
//     microNtuple->epd2tagSchan = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kSchan);
//     microNtuple->epd1tagTchan = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kTchan);
//     microNtuple->epd2tagTchan = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kTchan);

//     // Loop over the WH masses
//     unsigned auxI = 0;
//     for (MicroNtuple::indexMap1::const_iterator it = indexMapWH.begin(); 
// 	 it != indexMapWH.end(); it++){
//       microNtuple->epd1tagWH[auxI] = microNtuple->calcWHEPD(DEFS::eq1TSV, it->first);
//       microNtuple->epd2tagWH[auxI] = microNtuple->calcWHEPD(DEFS::eq2TSV, it->first);
//       auxI ++;
//     }//for 

//     // Loop over the H->WW masses
//     auxI = 0;
//     for (MicroNtuple::indexMap1::const_iterator it = indexMapHWW.begin(); 
// 	 it != indexMapHWW.end(); it++){
//       microNtuple->epd1tagHWW[auxI] = microNtuple->calcHWWEPD(DEFS::eq1TSV, it->first);
//       microNtuple->epd2tagHWW[auxI] = microNtuple->calcHWWEPD(DEFS::eq2TSV, it->first);
//       auxI++;
//     }//for 

    /*
    // Make sure this is not a duplicated event
    Signature temp = {eventNtuple->run, eventNtuple->event};
    if (sigSet.find(temp) != sigSet.end()) {
      cout << "\tWarning! Duplicate event found! Run " << temp.run 
	   << " Event " << temp.event << endl;
    } else {
      sigSet.insert(temp);
      outputTree->Fill();
    }
    */

    outputTree->Fill();


  }//for entries


//   // Report some results
//   TTree* flatTree;
//   TObjArray *li = tempTree->GetListOfLeaves();
//   int nent = li->GetSize();
//   cout << "nent=" << nent << endl;
//   for ( int i = 0; i<nent; i++ ) {
//     if ( li->At(i)!=0 ) {
//       cout << "i=" << i << " Name=" << li->At(i)->GetName() << endl;
//       cout << "i=" << i << " Title=" << li->At(i)->GetTitle() << endl;
//     //<< " Name=" << li->At(i)->GetName() << " Title=" << li->At(i)->GetTitle << endl;
//       TString name=li->At(i)->GetName();
//       TString title=li->At(i)->GetTitle();
//       TObject *obj=li->At(i);
//       cout << "x" << endl;
//       flatTree->Branch(name,title, &name );
//       cout << "y" << endl;
//     }
//   }

  // outputTree = tempTree;
  cout << "\tWrote " << output << " with " << outputTree->GetEntries()
       << " entries" << endl;
  outputFile.Write();
  
  delete meNtuple;
  delete microNtuple;


//   // Create the base objects
//   METree      * meNtuple    = new METree();
//   EventNtuple * eventNtuple = new EventNtuple();
//   MicroNtuple * microNtuple = new MicroNtuple(nJets);

//   MicroNtuple::indexMap2 indexMap = MicroNtuple::getIndexMap();
//   MicroNtuple::indexMap1 indexMapWH = indexMap[DEFS::EP::WH];
//   MicroNtuple::indexMap1 indexMapHWW = indexMap[DEFS::EP::HWW];

//   // and set the meNtuple to read from it
//   chain.SetBranchAddress("METree", &meNtuple);
//   chain.SetBranchAddress("EvtTree", &eventNtuple);

//   // Create and output file and clone the tree that will be in the output
//   // and set microNtuple that fills it
//   TFile outputFile(output.c_str(), "RECREATE");
//   //TTree* outputTree = new TTree("outputTree","outputTree");
//   TTree* outputTree = chain.CloneTree(0);
//   //TChain* outputTree = (TChain*)chain.Clone(0);
//   outputTree->Branch("mnt", "MicroNtuple", &microNtuple);

//   // TBranch *mnt = outputTree->Branch("mnt", &microNtuple);

//   // Get the entries and report if zero
//   unsigned nentries = static_cast<unsigned>(chain.GetEntries());
//   cout << "\tOriginal chain has " << nentries << " entries" << endl;
//   if (nentries == 0){
//       cout << "\tNo entries found!  Aborting.\n";
//       return;
//   }

//   // Holds the list of events to avoid duplication
//   set<Signature> sigSet;

//   //Loop over all the entries in the original chain. The idea is 
//   // to copy everything to microNtuple
//   for (unsigned ientry = 0; ientry < nentries; ++ientry){
//   //for (unsigned ientry = 0; ientry < 10080; ++ientry){ //gives an extra copy of METree
//   //for (Int_t ientry = 0; ientry < 10079; ++ientry){
//     // get the entry
//     chain.GetEntry(ientry);

//     // clear the microNtuple
//     microNtuple->clear();

//     // First copy all the event probabilities
//     for (int i = 0; i < meNtuple->getNProbStat(); ++i){
//       microNtuple->eventProb[i]    = meNtuple->getProbStat(i)->tEventProb;
//       microNtuple->eventMaxProb[i] = meNtuple->getProbStat(i)->tEventMaxProb;  
//     }    
    
//     // Get the b-probabilites for each jet.
//     for (unsigned int bb = 0; bb < eventNtuple->jBtag.size(); bb++)
//       microNtuple->bProb[bb] = eventNtuple->jBtag[bb];
    
//     // WWandWZ EPD, for 
//     //      DEFS::pretag is calculated ignoring the number of tags or bProb's   
//     //      DEFS::eqxTSV is calculated using the number of tags or bProb's,
//     //                   and the only diff between them is the coefficients.
//     microNtuple->epdPretagWWandWZ = microNtuple->calcWZEPD(DEFS::pretag);
//     microNtuple->epd0tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq0TSV);
//     microNtuple->epd1tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq1TSV);
//     microNtuple->epd2tagWWandWZ   = microNtuple->calcWZEPD(DEFS::eq2TSV);

//     // Make sure to do these last!
//     microNtuple->epd1tag      = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kCombined);
//     microNtuple->epd2tag      = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kCombined);
//     microNtuple->epd1tagSchan = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kSchan);
//     microNtuple->epd2tagSchan = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kSchan);
//     microNtuple->epd1tagTchan = microNtuple->calcSingleTopEPD(DEFS::eq1TSV, MicroNtuple::kTchan);
//     microNtuple->epd2tagTchan = microNtuple->calcSingleTopEPD(DEFS::eq2TSV, MicroNtuple::kTchan);

//     // Loop over the WH masses
//     unsigned auxI = 0;
//     for (MicroNtuple::indexMap1::const_iterator it = indexMapWH.begin(); 
// 	 it != indexMapWH.end(); it++){
//       microNtuple->epd1tagWH[auxI] = microNtuple->calcWHEPD(DEFS::eq1TSV, it->first);
//       microNtuple->epd2tagWH[auxI] = microNtuple->calcWHEPD(DEFS::eq2TSV, it->first);
//       auxI ++;
//     }//for 

//     // Loop over the H->WW masses
//     auxI = 0;
//     for (MicroNtuple::indexMap1::const_iterator it = indexMapHWW.begin(); 
// 	 it != indexMapHWW.end(); it++){
//       microNtuple->epd1tagHWW[auxI] = microNtuple->calcHWWEPD(DEFS::eq1TSV, it->first);
//       microNtuple->epd2tagHWW[auxI] = microNtuple->calcHWWEPD(DEFS::eq2TSV, it->first);
//       auxI++;
//     }//for 

//     /*
//     // Make sure this is not a duplicated event
//     Signature temp = {eventNtuple->run, eventNtuple->event};
//     if (sigSet.find(temp) != sigSet.end()) {
//       cout << "\tWarning! Duplicate event found! Run " << temp.run 
// 	   << " Event " << temp.event << endl;
//     } else {
//       sigSet.insert(temp);
//       outputTree->Fill();
//     }
//     */

//     //mnt->Fill();
//     outputTree->Fill();


//   }//for entries


//   // Report some results
//   // outputFile.cd();
// //   TTree* theOutputTree = outputTree->CloneTree();
// //   theOutputTree->Write("EPDTree");
//   //outputTree->Delete("all");
//   //delete outputTree;
//   outputTree->Write("EPDTree");
//   outputTree->Delete("all");
//   //delete outputTree;
//   cout << "\tWrote " << output << " with " << outputTree->GetEntries() << " entries" << endl;

//   // delete outputTree;
//   outputFile.Write();
//   //outputFile.ls();

//   delete meNtuple;
//   delete microNtuple;

}// makeMicroNtuple

//-----------------------------------------------------------------------------
//Created by RE to automatize the doing of MicroNtuples
void makeMicroNtuple(vector<string> locations, string output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag= false){

  cout<<endl;
  cout<<"  Generating MicroNtuple "<<output<<endl;

  TChain chain("METree");
  int file_count =0;
  for (unsigned e=0;e<locations.size(); e++) {
    cout<<"\tAdding "<<(locations[e] + "*.root")<<endl;
    file_count += chain.Add((locations[e] + "*.root").c_str());
  }

  if (file_count==0){
    cout << "\tNo files found!  Aborting.\n";
    return;
  }

  //cout << "s1" << endl;
  makeMicroNtuple(chain, output, nJets, doLight, doNonW, doUntag);
  //cout << "s2" << endl;
}

//-----------------------------------------------------------------------------
//Created by RE to automatize the doing of MicroNtuples
void makeMicroNtuple(string location, string output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag= false){

  vector<string> locations;
  locations.push_back(location);
  makeMicroNtuple(locations, output, nJets, doLight, doNonW, doUntag);
  
}


/*
//-----------------------------------------------------------------------------
//Created by RE to automatize the doing of MicroNtuples
//use like this:
// first do the ttbar splitting and HF removal at Event_Probs level using mergeAll.C
.X mergeAll.C+

//Then do all the microntuple by doing
root -l 
.L ../lib/slc5_ia32_gcc434/libTAMUWWMEPATNtuple.so
.L TAMUWW/Tools/makeMicroNtuple.C+
createAllMicroNtuples()

*/

//RE auxiliary struct to make all the microntuples automatically
class  MyStr{
public:
  MyStr(string d, string n, bool m, bool nw, bool nwg){
    dirs.push_back(d);
    name = n;
    mistag = m ;
    nonw = nw;
    untag = nwg;
  }
  void   AddDir(string d){ dirs.push_back(d);}
  vector<string> GetLocationVector(string path){
    vector<string> res;
    for (unsigned d=0;d<dirs.size();d++)
      res.push_back(path + dirs[d]);
    return res;
  }
  vector<string> dirs  ; // the set of directories to run over
  string         name  ; // the file name of the output w/o the ".root" extension
  bool           mistag; // whether this is a mistag or
  bool           nonw  ; // whether this is nonW or not
  bool           untag  ; // whether this is untag or not
};

// WARNING!!! is the microNtuple as bigger than 2Gs the code crashes
void createAllMicroNtuples(){

  string basePath="/uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/";

  //Input files for 2-jet bin
  string inputPath=basePath+"rootOutput/";
  unsigned nJet = 2;

  //Input files for 3-jet bin
  //string inputPath=basePath+"Event_Probs_Summer09_3jets_StndTF/";
  //unsigned nJet = 3;

  //Output files
  string outputPath=basePath+"microNtuples/";

  //Create the list of MicroNtuples
  vector<MyStr> listOfMicroNtuples;

  ////Diboson
  listOfMicroNtuples.push_back(MyStr("WW*","microWW_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("WZ*","microWZ_EPDv01",false,false,false));
  

  //// Wjets, Zjets
  listOfMicroNtuples.push_back(MyStr("WJets*","microWJets_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("ZJets*","microZJets_EPDv01",false,false,false));


  //// Top & QCD
  listOfMicroNtuples.push_back(MyStr("TTbar*","microTTbar_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("STopT_T*","microSTopT_T_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("TbarSamples/STopT_Tbar*","microSTopT_Tbar_EPDv01",false,false,false));//The _Tbar samples need to be moved to a different directory in order to avoid merging them with the _T samples
  listOfMicroNtuples.push_back(MyStr("STopS_T*","microSTopS_T_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("TbarSamples/STopS_Tbar*","microSTopS_Tbar_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("STopTW_T*","microSTopTW_T_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("TbarSamples/STopTW_Tbar*","microSTopTW_Tbar_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("QCD_Mu*","microQCD_Mu_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("QCD_El*","microQCD_El_EPDv01",false,false,false));


  //// Data
  listOfMicroNtuples.push_back(MyStr("Mu_All*","microMu_All_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("El_All*","microEl_All_EPDv01",false,false,false));


  //// Higgs 
  listOfMicroNtuples.push_back(MyStr("HWWMH150*","microHWWMH150_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH160*","microHWWMH160_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH170*","microHWWMH170_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH180*","microHWWMH180_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH190*","microHWWMH190_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH200*","microHWWMH200_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH250*","microHWWMH250_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH300*","microHWWMH300_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH350*","microHWWMH350_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH400*","microHWWMH400_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH450*","microHWWMH450_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH500*","microHWWMH500_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH550*","microHWWMH550_EPDv01",false,false,false));
  listOfMicroNtuples.push_back(MyStr("HWWMH600*","microHWWMH600_EPDv01",false,false,false));


  //
  //// Non W,  has two directories 
  //MyStr nonWMyStr("nonWdata/tag/","micro_taggable_nonW",false,true);
  //nonWMyStr.AddDir("nonWdata/untagtaggable/");
  //listOfMicroNtuples.push_back(nonWMyStr);
  //listOfMicroNtuples.push_back(MyStr("nonWdata/tag/","micro_tag_nonW",false,true));
  //listOfMicroNtuples.push_back(MyStr("nonWdata/untagtaggable/","micro_untagtaggable_nonWdata",false,true));
  //
  //
  //// Data
  //listOfMicroNtuples.push_back(MyStr("data/tag/","micro_tagged_data",false,false,false));
  //listOfMicroNtuples.push_back(MyStr("data/untagtaggable/","micro_untagtaggable_data",false,false,false));
  //listOfMicroNtuples.push_back(MyStr("data_uptop17/tag/","micro_tagged_data_uptop17",false,false,false));
  
//   //MEs for Low and High Lumi, Data
  
//   MyStr dataMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/data/tag/","micro_tagged_data",false,false,false);
//   dataMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/data/tag/");
//   listOfMicroNtuples.push_back(dataMyStr);
  
//   //MEs for Low and High Lumi, Data
//   MyStr untagdataMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/data/untagtaggable/","micro_untagtaggable_data",false,false,true);
//   untagdataMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/data/untagtaggable/");
//   listOfMicroNtuples.push_back(untagdataMyStr);
  
  
//   //// Low and High Lumi, Non W,  has four directories 
  
//   MyStr nonWMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/nonWdata/tag/","micro_taggable_nonW",false,true,false);
//   nonWMyStr.AddDir("tnt_Fall2009/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/");
//   nonWMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/nonWdata/tag/");
//   nonWMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/");
//   listOfMicroNtuples.push_back(nonWMyStr);
  
//   MyStr untagnonWMyStr("tnt_Fall2009/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/","micro_untagtaggable_nonW",false,true,false);
//   untagnonWMyStr.AddDir("tnt_Fall2009_HighLumi_pass2/Event_Probs_geq1LooseSV/nonWdata/untagtaggable/");
//   listOfMicroNtuples.push_back(untagnonWMyStr);

  //Loop over all of them
  for ( vector<MyStr>::iterator it = listOfMicroNtuples.begin();
	it != listOfMicroNtuples.end(); it++){
    
    makeMicroNtuple(it->GetLocationVector(inputPath), outputPath+ it->name + ".root", nJet, it->mistag, it->nonw, it->untag);
    
  }//for


}//createAllMicroNtuples()

void createAFlatMicroNtuple (const char* inFileName, const char* outFileName, const char* treeName) 
//// Takes specified elements of branches METree, EvtTree, mnt and writes them into a flat tree with the same (treeName) name.
{
  double pi_=TMath::Pi();
  double in_Mjj, in_DRlj1, in_DRlj2;
  double in_leptonCat;
  vector <TLorentzVector> lLV, jLV, METLV;
  double out_Mjj, out_WmT, out_lEta, out_lPt, out_lrelIso, out_DRlj1, out_DRlj2, out_j1pT, out_j2pT, out_j1Eta, out_j2Eta, out_MET, out_DEtajj, out_Ptjj, out_dPhijMET, out_j1BtagDiscriminatorSSV, out_j2BtagDiscriminatorSSV, out_j1BtagDiscriminatorTC, out_j2BtagDiscriminatorTC, out_j1QGLikelihood, out_j2QGLikelihood, out_logEPD;
  int out_leptonCat;

  TFile* infile = new TFile(inFileName,"READ");
  TTree* InTree = (TTree*)infile->Get(treeName);
  EventNtuple* InEvtTree = new EventNtuple();
  InTree->SetBranchAddress("EvtTree", &InEvtTree);
  MicroNtuple * InMNT = new MicroNtuple(2);
  InTree->SetBranchAddress("mnt", &InMNT);


  TFile *outfile = new TFile(outFileName, "RECREATE");
  TTree *OutTree = new TTree("FlatTree","FlatTree");
  OutTree->Branch("Mjj",&out_Mjj,"Mjj/D");
  OutTree->Branch("WmT",&out_WmT,"WmT/D");
  OutTree->Branch("lEta",&out_lEta,"lEta/D");
  OutTree->Branch("lPt",&out_lPt,"lPt/D");
  OutTree->Branch("lrelIso",&out_lrelIso,"lrelIso/D");
  OutTree->Branch("DRlj1",&out_DRlj1,"DRlj1/D");
  OutTree->Branch("DRlj2",&out_DRlj2,"DRlj2/D");
  OutTree->Branch("j1pT",&out_j1pT,"j1pT/D");
  OutTree->Branch("j2pT",&out_j2pT,"j2pT/D");
  OutTree->Branch("j1Eta",&out_j1Eta,"j1Eta/D");
  OutTree->Branch("j2Eta",&out_j2Eta,"j2Eta/D");
  OutTree->Branch("MET",&out_MET,"MET/D");
  OutTree->Branch("j1BtagDiscriminatorSSV",&out_j1BtagDiscriminatorSSV,"j1BtagDiscriminatorSSV/D");
  OutTree->Branch("j2BtagDiscriminatorSSV",&out_j2BtagDiscriminatorSSV,"j2BtagDiscriminatorSSV/D");
  OutTree->Branch("j1BtagDiscriminatorTC",&out_j1BtagDiscriminatorTC,"j1BtagDiscriminatorTC/D");
  OutTree->Branch("j2BtagDiscriminatorTC",&out_j2BtagDiscriminatorTC,"j2BtagDiscriminatorTC/D");
  OutTree->Branch("j1QGLikelihood",&out_j1QGLikelihood,"j1QGLikelihood/D");
  OutTree->Branch("j2QGLikelihood",&out_j2QGLikelihood,"j2QGLikelihood/D");

  OutTree->Branch("DEtajj",&out_DEtajj,"DEtajj/D");
  OutTree->Branch("Ptjj",&out_Ptjj,"Ptjj/D");
  OutTree->Branch("dPhijMET",&out_dPhijMET,"dPhijMET/D");
  OutTree->Branch("leptonCat",&out_leptonCat,"leptonCat/I");
  OutTree->Branch("logEPD",&out_logEPD,"logEPD/D");

  int nEntries;
  nEntries=InTree->GetEntries();
  cout << "nEntries=" << nEntries << endl;

  QGLikelihoodCalculator* qglikeli = new QGLikelihoodCalculator();

  for (int i=0; i<nEntries; i++) {
    //cout << "1" << endl;
    InTree->GetEntry(i);
    //cout << "Processing Entry " << i << endl;
    //cout << "2" << endl;
    out_Mjj=InEvtTree->Mjj;
    out_leptonCat=InEvtTree->leptonCat;
    lLV=InEvtTree->lLV;
    jLV=InEvtTree->jLV;
    METLV=InEvtTree->METLV;
    
    out_lEta=lLV[0].Eta();
    out_lPt=lLV[0].Pt();
    out_DRlj1=InEvtTree->DRlj1;
    out_DRlj2=InEvtTree->DRlj2;
    out_j1pT=jLV[0].Pt();
    out_j2pT=jLV[1].Pt();
    out_j1Eta=jLV[0].Eta();
    out_j2Eta=jLV[1].Eta();
    out_MET=METLV[0].Et();
    out_j1BtagDiscriminatorSSV=InEvtTree->jBtagDiscriminatorSSV[0];
    out_j2BtagDiscriminatorSSV=InEvtTree->jBtagDiscriminatorSSV[1];
    out_j1BtagDiscriminatorTC=InEvtTree->jBtagDiscriminatorTC[0];
    out_j2BtagDiscriminatorTC=InEvtTree->jBtagDiscriminatorTC[1];
    out_lrelIso=InEvtTree->lrelIso;

    /// Store the QG discriminators for the two leading jets; QG_QCD_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6_Spring11-PU_S1_START311_V1G1-v1.root should be in the directory this macro is ran in.
    out_j1QGLikelihood=InEvtTree->getQGLikelihood(0,qglikeli); 
    out_j2QGLikelihood=InEvtTree->getQGLikelihood(1,qglikeli); 


    out_WmT=sqrt((lLV[0].Et()+METLV[0].Et())*(lLV[0].Et()+METLV[0].Et())-(lLV[0].Px()+METLV[0].Px())*(lLV[0].Px()+METLV[0].Px())-(lLV[0].Py()+METLV[0].Py())*(lLV[0].Py()+METLV[0].Py()) );
    out_DEtajj=fabs(jLV[0].Eta()-jLV[1].Eta());
    out_Ptjj=sqrt((jLV[0].Px()+jLV[1].Px())*(jLV[0].Px()+jLV[1].Px())+(jLV[0].Py()+jLV[1].Py())*(jLV[0].Py()+jLV[1].Py()));

    out_dPhijMET=jLV[0].Phi()-METLV[0].Phi();
    if ( out_dPhijMET > pi_ ) {
      out_dPhijMET=2*pi_-out_dPhijMET;
    }
    if ( out_dPhijMET < -pi_ ) {
      out_dPhijMET=-out_dPhijMET-2*pi_;
    }


    out_logEPD=log(InMNT->epdPretagWWandWZ);
    //cout << "Mjj=" << out_Mjj << endl;
    //out_Mjj=in_Mjj;
    OutTree->Fill();
  }

  outfile->Write();
  outfile->Close();

  //  delete infile;
  delete InTree;
  delete InEvtTree;

}


void createMultipleFlatMicroNtuples () 
//// Converts Intput Files into flatones (to be used with RooFit) in the same directory
{
  const int NFiles=1;
  TString dirName = "/uscms_data/d3/ilyao/Spring12ME7TeV/MEResults/microNtuples/";
  TString fileName[NFiles];
  TString infileName, outfileName;

  fileName[0]="microWW_EPDv01.root";
//   fileName[1]="microWZ_EPDv01.root";
//   fileName[2]="microWJets_EPDv01.root";
//   fileName[3]="microZJets_EPDv01.root";
//   fileName[4]="microTTbar_EPDv01.root";
//   fileName[5]="microSTopT_T_EPDv01.root";
//   fileName[6]="microSTopT_Tbar_EPDv01.root";
//   fileName[7]="microSTopS_T_EPDv01.root";
//   fileName[8]="microSTopS_Tbar_EPDv01.root";
//   fileName[9]="microSTopTW_T_EPDv01.root";
//   fileName[10]="microSTopTW_Tbar_EPDv01.root";
//   fileName[11]="microQCD_Mu_EPDv01.root";
//   fileName[12]="microQCD_El_EPDv01.root";
//   fileName[13]="microMu_All_EPDv01.root";
//   fileName[14]="microEl_All_EPDv01.root";
//   fileName[15]="microHWWMH150_EPDv01.root";
//   fileName[16]="microHWWMH160_EPDv01.root";
//   fileName[17]="microHWWMH170_EPDv01.root";
//   fileName[18]="microHWWMH180_EPDv01.root";
//   fileName[19]="microHWWMH190_EPDv01.root";
//   fileName[20]="microHWWMH200_EPDv01.root";
//   fileName[21]="microHWWMH250_EPDv01.root";
//   fileName[22]="microHWWMH300_EPDv01.root";
//   fileName[23]="microHWWMH350_EPDv01.root";
//   fileName[24]="microHWWMH400_EPDv01.root";
//   fileName[25]="microHWWMH450_EPDv01.root";
//   fileName[26]="microHWWMH500_EPDv01.root";
//   fileName[27]="microHWWMH550_EPDv01.root";
//   fileName[28]="microHWWMH600_EPDv01.root";

  for (Int_t i=0; i<NFiles; i++) {
    outfileName="flat"+fileName[i];
    outfileName=dirName+outfileName;
    infileName=dirName+fileName[i];
    cout << "infileName=" << infileName << " outfileName=" << outfileName << endl;
    createAFlatMicroNtuple(infileName,outfileName,"METree");
  }
}




