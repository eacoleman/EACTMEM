#include "TCut.h"
#include "TSystem.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"

#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TLorentzVector.h"

#include <vector>

#ifdef __CINT__
#pragma link C++ class std::vector<TLorentzVector>+;
#endif

void TMVAtest(){
  //gSystem->Load("../lib/slc5_amd64_gcc462/libTAMUWWMEPATNtuple.so");
  gSystem->Load("libPhysics");
  //gSystem->Load("EvtTreeForAlexx_h.so");
  gSystem->Load("libTMVA.1");
  gSystem->Load("AutoDict_vector_TLorentzVector__cxx.so");
  TMVA::Tools::Instance();
  TFile* outputFile = TFile::Open("TMVA1.root", "RECREATE");
  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification",outputFile,"V=true:Color:DrawProgressBar");// ":Transformations=I;D;P;G,D" );
  TFile* signal = TFile::Open("/uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWW_EPDv01.root");
  TFile* bkg = TFile::Open("/uscms_data/d2/aperloff/Spring12ME7TeV/MEResults/microNtuples_oldStructure/microWJets_EPDv01.root");

  TTree* stree = (TTree*)signal->Get("METree");
  TTree* btree = (TTree*)bkg->Get("METree");
  factory->AddSignalTree(stree,1.0);
  factory->AddBackgroundTree(btree,1.0);


  factory->SetSignalWeightExpression("1.0");
  factory->SetBackgroundWeightExpression("1.0");
  factory->AddVariable("tEventProb[0]");
  factory->AddVariable("tEventProb[1]");
  factory->AddVariable("tEventProb[2]");

  //factory->AddVariable("tEventProb0 := tEventProb[0]",'F');
  //factory->AddVariable("tEventProb1 := tEventProb[1]",'F');
  //factory->AddVariable("tEventProb2 := tEventProb[2]",'F');
  TCut test("Entry$>-2 && jLV[1].Pt()>30");
  TCut mycuts (test);
  factory->PrepareTrainingAndTestTree(mycuts,mycuts,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:NormMode=None:V=true:VerboseLevel=DEBUG");
  factory->BookMethod( TMVA::Types::kBDT, "BDT","!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  outputFile->Close(); 

}


