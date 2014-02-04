// This class libraries
#include "TAMUWW/Tools/interface/MicroNtupleMaker.hh"

//______________________________________________________________________________
MicroNtupleMaker::MicroNtupleMaker() {

}//C'tor

//______________________________________________________________________________
MicroNtupleMaker::~MicroNtupleMaker() {

   if(mergeChain)
      delete mergeChain;
   if(index)
      delete index;
   if(mergeEventNtuple)
      delete mergeEventNtuple;

}//D'tor

//______________________________________________________________________________
void MicroNtupleMaker::createMicroNtuple(TString inputPath, TString outputPath, int largeProcessCase, TString smallProcessLabel){
//// Large processes need to be split up into parts 'by hand'
//// largeProcessCase=0 corresponds to small processes
//// The files with similar names, but which need to be ran on separately should be put into separate directories (e.g. STopT_T and STopT_Tbar)

   //string basePath="/uscms_data/d3/ilyao/Winter12to13ME8TeV/MEResults/";

   //Input files for 2-jet bin
//   string inputPath=basePath+"rootOutput/";
//   string inputPathBeta="/eos/uscms/store/user/eusebi/Winter12to13ME8TeV/rootOutput/";
   unsigned nJet = 2;

   //Input files for 3-jet bin
   //string inputPath=basePath+"Event_Probs_Summer09_3jets_StndTF/";
   //unsigned nJet = 3;

   //Output files
   //  string outputPath=basePath+"microNtuples/";

   //Create the list of MicroNtuples
   vector<MyStr> listOfMicroNtuples;

   /// Test Ntuples:
//   inputPath=basePath+"test_rootOutput/";
//   outputPath=basePath+"test_microNtuples/";
//   listOfMicroNtuples.push_back(MyStr("WZ*","testmicroWZ",false,false,false));
   TString microOutputSuffix="_BaseCuts"; //Set a suffix entire microNtuple production
   TString microOutputName;

   switch (largeProcessCase) {
      case 0:
      {
         ///Small Process:
         TString inputName="*";
         inputName=smallProcessLabel+inputName;
         microOutputName=microOutputSuffix;
         microOutputName=smallProcessLabel+microOutputName;
         microOutputName="micro"+microOutputName;
         listOfMicroNtuples.push_back(MyStr(inputName,microOutputName,false,false,false));
      }
      break;
      case -1:
         ///Multiple Small Processes
         break;
         ///W+3Jets
      case 1001:
      {
         microOutputName="microW3Jets_pt1"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W3Jets1*",microOutputName,false,false,false));
      }
      break;
      case 1002:
      {
         microOutputName="microW3Jets_pt2"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W3Jets2*",microOutputName,false,false,false));
      }
      break;
      case 1003:
      {
         microOutputName="microW3Jets_pt3"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W3Jets3*",microOutputName,false,false,false));
      }
      break;
      case 1004:
      {
         microOutputName="microW3Jets_pt4"+microOutputSuffix;
         MyStr fileLocations_W3Jets_pt4("W3Jets4*",microOutputName,false,false,false);
         fileLocations_W3Jets_pt4.AddDir("W3Jets5*");
         fileLocations_W3Jets_pt4.AddDir("W3Jets6*");
         fileLocations_W3Jets_pt4.AddDir("W3Jets7*");
         fileLocations_W3Jets_pt4.AddDir("W3Jets8*");
         fileLocations_W3Jets_pt4.AddDir("W3Jets9*");
         fileLocations_W3Jets_pt4.AddDir("W3Jets0*");
         listOfMicroNtuples.push_back(fileLocations_W3Jets_pt4);
      }
      break;

      ///W+4Jets
      case 1005:
      {
         microOutputName="microW4Jets_pt1"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W4Jets1*",microOutputName,false,false,false));
      }
      break;
      case 1006:
      {
         microOutputName="microW4Jets_pt2"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W4Jets2*",microOutputName,false,false,false));
      }
      break;
      case 1007:
      {
         microOutputName="microW4Jets_pt3"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W4Jets3*",microOutputName,false,false,false));
      }
      break;
      case 1008:
      {
         microOutputName="microW4Jets_pt4"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("W4Jets4*",microOutputName,false,false,false));
      }
      break;
      case 1009:
      {
         microOutputName="microW4Jets_pt5"+microOutputSuffix;
         MyStr fileLocations_W4Jets_pt5("W4Jets5*",microOutputName,false,false,false);
         fileLocations_W4Jets_pt5.AddDir("W4Jets6*");
         fileLocations_W4Jets_pt5.AddDir("W4Jets7*");
         fileLocations_W4Jets_pt5.AddDir("W4Jets8*");
         fileLocations_W4Jets_pt5.AddDir("W4Jets9*");
         fileLocations_W4Jets_pt5.AddDir("W4Jets0*");
         listOfMicroNtuples.push_back(fileLocations_W4Jets_pt5);
      }
      break;

      ///Data: Muon
      case 1011:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt1"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb10*",microOutputName,false,false,false));
      }
      break;
      case 1012:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt2"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb2*",microOutputName,false,false,false));
      }
      break;
      case 1013:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt3"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb3*",microOutputName,false,false,false));
      }
      break;
      case 1014:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt4"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb4*",microOutputName,false,false,false));
      }
      break;
      case 1015:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt5"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb5*",microOutputName,false,false,false));
      }
      break;
      case 1016:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt6"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb6*",microOutputName,false,false,false));
      }
      break;
      case 1017:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt7"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb7*",microOutputName,false,false,false));
      }
      break;
      case 1018:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt8"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb8*",microOutputName,false,false,false));
      }
      break;
      case 1019:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt9"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb9*",microOutputName,false,false,false));
      }
      break;
      case 1020:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt10"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleMu_Data_19p279fb11*",microOutputName,false,false,false));
      }
      break;
      case 1021:
      {
         microOutputName="microSingleMu_Data_19p279fb_pt11"+microOutputSuffix;
         MyStr fileLocations_SingleMu_Data_19p279fb_pt11("SingleMu_Data_19p279fb12*",microOutputName,false,false,false);
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb13*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb14*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb15*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb16*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb17*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb18*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb19*");
         fileLocations_SingleMu_Data_19p279fb_pt11.AddDir("SingleMu_Data_19p279fb0*");
         listOfMicroNtuples.push_back(fileLocations_SingleMu_Data_19p279fb_pt11);
      }
      break;

      ///Data: Electron
      case 1031:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt1"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb1*",microOutputName,false,false,false));
      }
      break;
      case 1032:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt2"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb2*",microOutputName,false,false,false));
      }
      break;
      case 1033:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt3"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb3*",microOutputName,false,false,false));
      }
      break;
      case 1034:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt4"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb4*",microOutputName,false,false,false));
      }
      break;
      case 1035:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt5"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb5*",microOutputName,false,false,false));
      }
      break;
      case 1036:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt6"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb6*",microOutputName,false,false,false));
      }
      break;
      case 1037:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt7"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb7*",microOutputName,false,false,false));
      }
      break;
      case 1038:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt8"+microOutputSuffix;
         listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb8*",microOutputName,false,false,false));
      }
      break;
      case 1039:
      {
         microOutputName="microSingleEl_Data_19p148fb_pt9"+microOutputSuffix;
         MyStr fileLocations_SingleEl_Data_19p148fb_pt9("SingleEl_Data_19p148fb9*",microOutputName,false,false,false);
         fileLocations_SingleEl_Data_19p148fb_pt9.AddDir("SingleEl_Data_19p148fb0*");
         listOfMicroNtuples.push_back(fileLocations_SingleEl_Data_19p148fb_pt9);
      }
      break;

      default:
      {
         cout << "Input the correct largeProcessCase number" << endl;
      }
      break;
   }

//     //listOfMicroNtuples.push_back(MyStr("SingleEl_Data_19p148fb*","microSingleEl_Data_19p148fb_BaseCuts",false,false,false));

// //     //// Higgs 
// //     listOfMicroNtuples.push_back(MyStr("ggH110*","microggH110_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH120*","microggH120_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH125*","microggH125_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH130*","microggH130_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH140*","microggH140_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH150*","microggH150_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH160*","microggH160_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH170*","microggH170_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH180*","microggH180_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH190*","microggH190_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH200*","microggH200_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH250*","microggH250_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH300*","microggH300_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH350*","microggH350_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH400*","microggH400_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH450*","microggH450_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH500*","microggH500_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH550*","microggH550_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH600*","microggH600_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH700*","microggH700_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH800*","microggH800_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH900*","microggH900_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("ggH1000*","microggH1000_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("WH125*","microWH125_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH125*","microqqH125_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH170*","microqqH170_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH180*","microqqH180_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH190*","microqqH190_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH200*","microqqH200_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH250*","microqqH250_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH300*","microqqH300_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH350*","microqqH350_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH400*","microqqH400_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH450*","microqqH450_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH500*","microqqH500_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH550*","microqqH550_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH600*","microqqH600_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH700*","microqqH700_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH800*","microqqH800_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH900*","microqqH900_BaseCuts",false,false,false));
// //     listOfMicroNtuples.push_back(MyStr("qqH1000*","microqqH1000_BaseCuts",false,false,false));

//   }

   //Loop over all of them
   for ( vector<MyStr>::iterator it = listOfMicroNtuples.begin();
         it != listOfMicroNtuples.end(); it++){
    
      makeMicroNtuple(it->GetLocationVector(inputPath), outputPath+ it->name + ".root", nJet, it->mistag, it->nonw, it->untag);
    
   }//for


}//createAllMicroNtuples()

//______________________________________________________________________________
void MicroNtupleMaker::makeMicroNtuple(TString location, TString output, unsigned nJets, bool doLight, bool doNonW, bool doUntag){

   vector<TString> locations;
   locations.push_back(location);
   makeMicroNtuple(locations, output, nJets, doLight, doNonW, doUntag);
  
}

//______________________________________________________________________________
void MicroNtupleMaker::makeMicroNtuple(vector<TString> locations, TString output, unsigned nJets, bool doLight, bool doNonW, bool doUntag){

   cout<<endl;
   cout<<"  Generating MicroNtuple "<<output<<endl;

   TChain chain("METree");
   int file_count =0;
   for (unsigned e=0;e<locations.size(); e++) {
      cout<<"\tAdding "<<(locations[e] + "*.root")<<endl;
      file_count += chain.Add((locations[e] + "*.root"));
   }

   if (file_count==0){
      cout << "\tNo files found!  Aborting.\n";
      return;
   }

   if (mergeNewEventNtuple.CompareTo("")!=0) {
      mergeChain = new TChain("jets2p","Output tree for matrix element");
      if(currentProcess=="ZJets") currentProcess="DYJets";
      cout << "\tAdding " << mergeNewEventNtuple << currentProcess << ".root/PS/jets2p" << endl;
      mergeChain->Add(mergeNewEventNtuple+currentProcess+".root/PS/jets2p");
      mergeChain->LoadTree(0);
      mergeTree = mergeChain->GetTree();
   }

   chain.SetBranchStatus("*",0);
   chain.SetBranchStatus("m_event",1);
   chain.LoadTree(0);
   Int_t treeEntries = chain.GetTree()->GetEntries();
   TTree* tmpTree;
   index = new TTreeIndex();
   if(mergeNewEventNtuple.CompareTo("")!=0) {
      for(Int_t t=0; t<chain.GetNtrees(); t++) {
      //for(Int_t t=0; t<2; t++) {
         chain.LoadTree((t*treeEntries)+1);
         cout << "MicroNtupleMaker::makeMicroNtuple Building index for tree " << chain.GetTreeNumber() << " ... ";
         tmpTree = chain.GetTree();
         //tmpTree->SetBranchStatus("*",0);
         //tmpTree->SetBranchStatus("m_event",1);
         tmpTree->BuildIndex("m_event");
         index->Append((TTreeIndex*)tmpTree->GetTreeIndex());
         cout << "DONE" << endl;
      }
   }
   chain.SetBranchStatus("*",1);

   cout << "\t\tmakeMicroNtuple(TChain& chain, TString output, unsigned nJets, bool doLight, bool doNonW, bool doUntag) ... " << endl;
   //makeMicroNtuple(chain.GetTree(), output, nJets, doLight, doNonW, doUntag);
   makeMicroNtuple(chain, output, nJets, doLight, doNonW, doUntag);

}

//______________________________________________________________________________
//void MicroNtupleMaker::makeMicroNtuple(TChain & chain, TString output, unsigned nJets, 
void MicroNtupleMaker::makeMicroNtuple(TChain& chain, TString output, unsigned nJets, 
                                       bool doLight, bool doNonW, bool doUntag)
{
   cout << "\t\tMicroNtupleMaker::makeMicroNtuple Make ntuples ... ";
   // Create the base objects
   EventNtuple * eventNtuple      = new EventNtuple();
   METree      * meNtuple         = new METree();
   METree      * meNtupleBlank    = new METree();
   MicroNtuple * microNtuple      = new MicroNtuple(nJets);
   MicroNtuple * microNtupleBlank = new MicroNtuple(nJets);
   cout << "DONE" << endl;

   cout << "\t\tMicroNtupleMaker::makeMicroNtuple Set METree and EvtTree branches ... ";
   // and set the meNtuple to read from it
   chain.SetBranchAddress("METree", &meNtuple);
   chain.SetBranchAddress("EvtTree", &eventNtuple);
   cout << "DONE" << endl;

   //sort chains by event number
   if(mergeNewEventNtuple.CompareTo("")!=0) {
      
      cout << "\t\tMicroNtupleMaker::makeMicroNtuple Set merge EvtNtuple branch ... ";
      mergeEventNtuple = new EventNtuple();
      mergeChain->GetFile()->cd();
      mergeTree->SetBranchAddress("EvtNtuple", &mergeEventNtuple);
      cout << "DONE" << endl;

      //chain->BuildIndex("m_event");
      //index = (TTreeIndex*)chain->GetTreeIndex();

      Long64_t local = -1;
      chain.SetBranchStatus("*",0);
      chain.SetBranchStatus("event*",1);
      int nIndex = index->GetN();      
      cout << "\t\tMicroNtupleMaker::makeMicroNtuple Making event index map for " << nIndex << " entries ... " << endl;
      cout.flush();
      for( int i = 0; i < nIndex ; ++i ) {
      //for( int i = 0; i < 2 ; ++i ) {
         //cout << i << " out of " << index->GetN() << endl;
         //if(i%10000 == 0) {
         //   cout << ".";
         //   cout.flush();
         //}
         //if(fmod(double(i)/index->GetN(),0.01)==0) {
         //   cout << "\t\t\t" << (double(i)/index->GetN())*100 << "%" << endl;
         //   cout.flush();
         //}
         loadbar(i+1,nIndex);
         //loadBar(i+1,(int)index->GetN(),50,50);
         local = index->GetIndex()[i];
         //chain.GetEntry(local,0);
         //eventIndex[eventNtuple->event] = local;
         eventIndex[index->GetIndexValues()[i]>>31] = local;
      }
      chain.SetBranchStatus("*",1);
      cout << endl <<"\t\tDONE" << endl;
   }

   cout << "\t\tMicroNtupleMaker::makeMicroNtuple Create output file and setup output tree ... ";
   // Create and output file and clone the tree that will be in the output and set microNtuple that fills it
   TFile outputFile(output, "RECREATE");
   //TTree* outputTree = chain->CloneTree(0);
   TTree* outputTree = new TTree(chain.GetName(),chain.GetTitle());
   //MicroNtuple::indexMap1 indexMapWH = indexMap[DEFS::EP::WH];
   //MicroNtuple::indexMap1 indexMapHWW = indexMap[DEFS::EP::HWW];
   outputTree->Branch("METree", "METree", &meNtuple);
   outputTree->Branch("EvtTree", "EventNtuple", &mergeEventNtuple);
   outputTree->Branch("mnt", "MicroNtuple", &microNtuple);
   cout << "DONE" << endl;

   // Get the entries and report if zero
   mergeNewEventNtuple.CompareTo("")!=0 ? nentries = static_cast<unsigned>(mergeTree->GetEntries()) : nentries = static_cast<unsigned>(chain.GetEntries());
   cout << "\t\tOriginal chain has " << nentries << " entries" << endl;
   if (nentries == 0){
      cout << "\t\tNo entries found!  Aborting.\n";
      return;
   }

   // Holds the list of events to avoid duplication
   set<Signature> sigSet;

   // The index map has not been filled yet
   imFilled = false;

   //Loop over all the entries in the original chain. The idea is to copy everything to microNtuple
   cout << "\t\tMicroNtupleMaker::makeMicroNtuple Filling MicroNtuple ..." << endl;
   if(missingEventsFlag){
      mergeTree->SetBranchStatus("*",0);
      mergeTree->SetBranchStatus("event",1);
   }
   for (unsigned ientry = 0; ientry < nentries; ++ientry){
   //for (unsigned ientry = 0; ientry < 2; ++ientry){

      //if (ientry%10000 == 0) {
         //cout << "\t\tMicroNtupleMaker::makeMicroNtuple Doing entry " << ientry << " ..." << endl;
      //}
      loadbar2(ientry+1,nentries);

      // get the entry
      if (mergeNewEventNtuple.CompareTo("")!=0) {
         mergeTree->GetEntry(ientry);
         if(eventIndex.find(mergeEventNtuple->event)!=eventIndex.end()) {
            //cout << "Not missing entry " << ientry << endl;
            if(!missingEventsFlag)
               chain.GetEntry(eventIndex[mergeEventNtuple->event]);
            else
               continue;
         }
         else {
            //cout << "Missing entry " << ientry << endl;
            missingME[ientry] = mergeEventNtuple->event;
            if(!missingEventsFlag) {
               microNtuple->clear();
               meNtuple->clear();
               outputTree->Fill();
            }
            continue;
         }
      }
      else {
         chain.GetEntry(ientry);
      }

      if (!imFilled) {
         microNtuple->indexMap = meNtuple->fillIndexMap();
         imFilled = true;
      }

      // clear the microNtuple
      microNtuple->clear();

      // First copy all the event probabilities
      for (int i = 0; i < meNtuple->getNProbStat(); ++i){
         microNtuple->eventProb[i]    = meNtuple->getProbStat(i)->tEventProb;
         microNtuple->eventMaxProb[i] = meNtuple->getProbStat(i)->tEventMaxProb;  
         //cout << "i=" << i << " eventProb=" << microNtuple->eventProb[i] << endl;
      }   

      microNtuple->size = meNtuple->getNProbStat();
      microNtuple->run = meNtuple->getRun();
      microNtuple->event = meNtuple->getEvent();

      microNtuple->epdPretagWWandWZ = microNtuple->calcWZEPD(DEFS::pretag);
      microNtuple->epd0tagWWandWZ = microNtuple->calcWZEPD(DEFS::eq0tag);
      microNtuple->epd1tagWWandWZ = microNtuple->calcWZEPD(DEFS::eq1tag);
      microNtuple->epd2tagWWandWZ = microNtuple->calcWZEPD(DEFS::eq2tag);
      int counterHWW = 0;
      int counterWH = 0;
      int counterH = 0;
      //If using TArrayD
      //microNtuple->epdPretagHiggs.Set(MicroNtuple::nHiggsMasses);
      for(int i = 0; i < meNtuple->getNProbStat(); ++i) {
         if(meNtuple->getProbStat(i)->tmeType == DEFS::EP::HWW) {
            microNtuple->epd1tagHWW[counterHWW] = microNtuple->calcHWWEPD(DEFS::eq1tag,meNtuple->getProbStat(i)->tmeParam);
            microNtuple->epd2tagHWW[counterHWW++] = microNtuple->calcHWWEPD(DEFS::eq2tag,meNtuple->getProbStat(i)->tmeParam);
         }
         if(meNtuple->getProbStat(i)->tmeType == DEFS::EP::WH) {
            microNtuple->epd1tagWH[counterWH] = microNtuple->calcWHEPD(DEFS::eq1tag,meNtuple->getProbStat(i)->tmeParam);
            microNtuple->epd2tagWH[counterWH++] = microNtuple->calcWHEPD(DEFS::eq2tag,meNtuple->getProbStat(i)->tmeParam);
         }
         if(meNtuple->getProbStat(i)->tmeType == DEFS::EP::HWW || meNtuple->getProbStat(i)->tmeType == DEFS::EP::WH) {
            //Used to absorb the error of the missing first entry
            microNtuple->absorbError[counterH] = -1;
            //If using TArrayD
            //microNtuple->epdPretagHiggs.AddAt(counterH,microNtuple->calcHiggsEPD(DEFS::pretag,meNtuple->getProbStat(i)->tmeParam));
            microNtuple->epdPretagHiggs[counterH] = microNtuple->calcHiggsEPD(DEFS::pretag,meNtuple->getProbStat(i)->tmeParam);
            microNtuple->epd1tagHiggs[counterH] = microNtuple->calcHiggsEPD(DEFS::eq1tag,meNtuple->getProbStat(i)->tmeParam);
            microNtuple->epd2tagHiggs[counterH++] = microNtuple->calcHiggsEPD(DEFS::eq2tag,meNtuple->getProbStat(i)->tmeParam);
         }
      }

      microNtuple->reader = 0;
      
      outputTree->Fill();

      microNtuple->clear();
      meNtuple->clear();


   }//for entries

   // Report some results
   cout << endl << "\t\tWrote " << output << " with " << outputTree->GetEntries() << " entries" << endl;
   outputFile.Write();
  
   delete meNtuple;
   delete microNtuple;
   delete eventNtuple;    
   delete meNtupleBlank;
   delete microNtupleBlank;

   if(mergeNewEventNtuple.CompareTo("")!=0 && !missingEventsFlag) {
      cout << "\t\tMissing Information ..." << endl;
      stringstream missingIndex;
      stringstream missingEvents;
      map<int,int>::iterator finalIter = missingME.end();
      if(missingME.size()>0)
         --finalIter;
      for (map<int,int>::iterator it=missingME.begin(); it!=missingME.end(); ++it) {
         if(it!=finalIter) {
            missingIndex << it->first << ",";
            missingEvents << it->second << ",";
         }
         else {
            missingIndex << it->first << endl;
            missingEvents << it->second << endl;
         }
      }
      cout << "\t\tThe indecies for the entries missing ME (" << missingME.size() << " total):" << endl << endl << missingIndex.str() << endl
           << "\t\tThe event numbers for the entries missing ME (" << missingME.size() << " total):" << endl << endl << missingEvents.str();
   }
   else if(mergeNewEventNtuple.CompareTo("")!=0 && missingEventsFlag) {
      writeMissingEventsFile(missingME);
   }

}// makeMicroNtuple

void MicroNtupleMaker::writeMissingEventsFile(map<int,int> &missingME) {
   string filename = string(outputPath+"micro"+currentProcess+"_missingEvents.txt");
   cout << "\t\tWriting missing events file to " << filename << " ... ";
   fstream ofile;
   ofile.open(filename,std::fstream::out | std::fstream::trunc);
   int len = getIntLength(missingME.size());

   if (ofile.is_open()) {
      ofile << setw(34) << "Missing Index In New EventNtuple (" << setw(len) << missingME.size() << setw(7) << " total)" << setw(10) << " " << setw(23) << "Missing Event Numbers (" <<  setw(len) << missingME.size() << setw(7) << " total)" << endl;
      ofile << setfill('=') << setw(34+len+7) << "" << setfill(' ') << setw(10) << " " << setfill('=') << setw(23+len+7) << ""  << setfill(' ') << endl;

      for (map<int,int>::iterator it=missingME.begin(); it!=missingME.end(); ++it) {
         ofile << setw(34+len+7) << it->first << setw(10) << " " << setw(23+len+7) << it->second << endl; 
      }

      ofile.close();
      cout << "DONE" << endl;
   }
   else {
      cout << endl << "\t\tERROR::Could not open file " << filename << endl;
   }
}//writeMissingEventsFile

int MicroNtupleMaker::getIntLength(int i) {
   int power = -1;
   int result = -1;
   while (result!=0) {
      power++;
      result = i/int(pow(10,power));
   }
   return power;
}//getIntLength