// This class libraries
#include "TAMUWW/Tools/interface/MicroNtupleMaker.hh"

//______________________________________________________________________________
MicroNtupleMaker::MicroNtupleMaker() {

}//C'tor

//______________________________________________________________________________
MicroNtupleMaker::~MicroNtupleMaker() {

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

  //cout << "s1" << endl;
  makeMicroNtuple(chain, output, nJets, doLight, doNonW, doUntag);
  //cout << "s2" << endl;
}

//______________________________________________________________________________
void MicroNtupleMaker::makeMicroNtuple(TChain & chain, TString output, unsigned nJets, 
		     bool doLight, bool doNonW, bool doUntag)
{

  // Create the base objects
  METree      * meNtuple    = new METree();


  // and set the meNtuple to read from it
  chain.SetBranchAddress("METree", &meNtuple);

  EventNtuple * eventNtuple = new EventNtuple();
  chain.SetBranchAddress("EvtTree", &eventNtuple);

  // Create and output file and clone the tree that will be in the output and set microNtuple that fills it
  TFile outputFile(output, "RECREATE");
  TTree* outputTree = chain.CloneTree(0);
  MicroNtuple * microNtuple = new MicroNtuple(nJets);
  //MicroNtuple::indexMap1 indexMapWH = indexMap[DEFS::EP::WH];
  //MicroNtuple::indexMap1 indexMapHWW = indexMap[DEFS::EP::HWW];
  outputTree->Branch("mnt", "MicroNtuple", &microNtuple);

  //outputTree->Print();
  // Get the entries and report if zero
  unsigned nentries = static_cast<unsigned>(chain.GetEntries());
  cout << "\tOriginal chain has " << nentries << " entries" << endl;
  if (nentries == 0){
      cout << "\tNo entries found!  Aborting.\n";
      return;
  }

  // Holds the list of events to avoid duplication
  set<Signature> sigSet;

  //Loop over all the entries in the original chain. The idea is to copy everything to microNtuple
  for (unsigned ientry = 0; ientry < nentries; ++ientry){
    // get the entry
    chain.GetEntry(ientry);

    if (ientry%10000 == 0)
       cout << "MicroNtupleMaker::makeMicroNtuple Doing entry " << ientry << " ..." << endl;

    if (ientry == 0)
       microNtuple->indexMap = meNtuple->fillIndexMap();

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
    microNtuple->epd0tagWWandWZ = microNtuple->calcWZEPD(DEFS::eq0TSV);
    microNtuple->epd1tagWWandWZ = 0;//microNtuple->calcWZEPD(DEFS::eq1TSV);
    microNtuple->epd2tagWWandWZ = 0;//microNtuple->calcWZEPD(DEFS::eq2TSV);
    int counterHWW = 0;
    int counterWH = 0;
    int counterH = 0;
    microNtuple->epdPretagHiggs.Set(MicroNtuple::nHiggsMasses);
    for(int i = 0; i < meNtuple->getNProbStat(); ++i) {
       if(meNtuple->getProbStat(i)->tmeType == DEFS::EP::HWW) {
          microNtuple->epd1tagHWW[counterHWW] = 0;//microNtuple->calcHWWEPD(DEFS::eq1TSV,meNtuple->getProbStat(i)->tmeParam);
          microNtuple->epd2tagHWW[counterHWW++] = 0;//microNtuple->calcHWWEPD(DEFS::eq2TSV,meNtuple->getProbStat(i)->tmeParam);
       }
       if(meNtuple->getProbStat(i)->tmeType == DEFS::EP::WH) {
          microNtuple->epd1tagWH[counterWH] = 0;//microNtuple->calcWHEPD(DEFS::eq1TSV,meNtuple->getProbStat(i)->tmeParam);
          microNtuple->epd2tagWH[counterWH++] = 0;//microNtuple->calcWHEPD(DEFS::eq2TSV,meNtuple->getProbStat(i)->tmeParam);
       }
       if(meNtuple->getProbStat(i)->tmeType == DEFS::EP::HWW || meNtuple->getProbStat(i)->tmeType == DEFS::EP::WH) {
          //microNtuple->epdPretagHiggs[counterH] = microNtuple->calcHiggsEPD(DEFS::pretag,meNtuple->getProbStat(i)->tmeParam);
          microNtuple->epdPretagHiggs.AddAt(counterH,microNtuple->calcHiggsEPD(DEFS::pretag,meNtuple->getProbStat(i)->tmeParam));
          microNtuple->epd1tagHiggs[counterH] = 0;//microNtuple->calcHiggsEPD(DEFS::eq1TSV,meNtuple->getProbStat(i)->tmeParam);
          microNtuple->epd2tagHiggs[counterH++] = 0;//microNtuple->calcHiggsEPD(DEFS::eq2TSV,meNtuple->getProbStat(i)->tmeParam);
       }
    }

    microNtuple->reader = 0;

    outputTree->Fill();
  }//for entries

  // Report some results
  cout << "\tWrote " << output << " with " << outputTree->GetEntries()
       << " entries" << endl;
  outputFile.Write();
  
  delete meNtuple;
  delete microNtuple;

}// makeMicroNtuple
