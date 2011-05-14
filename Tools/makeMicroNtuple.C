#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TTree.h"

#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

using std::string;
using std::vector;
using std::cout;
using std::endl;

struct Signature
{
  int run;
  int event;
  int alpgentype;
  int detector;
  double weight;
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
    else if (weight != rhs.weight)
      return weight < rhs.weight;
    else
      return lepPt < rhs.lepPt;
    //            return lepPt - rhs.lepPt < -.01;
  }
};

void fillBProb(double knntag[], double bProb[], unsigned nJets)
{
 
  for (unsigned int i = 0; i < nJets; ++i)
    {
      if (knntag[i] >= -1)
	bProb[i] = MicroNtuple::getKNNTagProb(knntag[i]);
      else
	bProb[i] = -999;

      // USE JUST FOR TESTING without KIT 
      //bProb[i] = 0.5;
    }  

  std::sort(bProb, bProb + nJets, std::greater<double>());
}

void makeMicroNtuple(TChain & chain, string output, unsigned nJets, bool doLight=false, bool doNonW= false, bool doUntag=false)
{

  EventNtuple* eventNtuple = new EventNtuple();
  METree* meNtuple = new METree();

  //  chain.SetBranchAddress("EvtTree", &eventNtuple);
  //cout << "t1" << endl;
  chain.SetBranchAddress("METree.", &meNtuple);
  //cout << "t2" << endl;

  MicroNtuple* microNtuple = new MicroNtuple(nJets);


  TFile outputFile(output.c_str(), "RECREATE");
  //   TTree outputTree("mnt", "Micro-ntuples for discriminant");
  //cout << "t2.5" << endl;
  TTree* outputTree = chain.CloneTree(0);
  //cout << "t2.6" << endl;
  outputTree->Branch("mnt", "MicroNtuple", &microNtuple);

  //cout << "t3" << endl;


  //   TFile histFile("KaNN_templates_final_nnBtagSys.root", "READ");
//   TFile histFile("KaNN_templates_final.root", "READ");
//   TH1* lightTemp = dynamic_cast<TH1*>(histFile.Get("h14"));
//   TH1* kann_template_b = dynamic_cast<TH1*>(histFile.Get("h10"));
//   TH1* kann_template_c = dynamic_cast<TH1*>(histFile.Get("h12"));
//   TH1* nonwTemp = new TH1F("kann_template_nonW","kann_template_nonW",100,-1,1);
//   //nonwTemp->Add(lightTemp,kann_template_b,.15/lightTemp->Integral(),.45/kann_template_b->Integral());
//   nonwTemp->Add(lightTemp      , 0.15/lightTemp->Integral());
//   nonwTemp->Add(kann_template_b, 0.45/kann_template_b->Integral());
//   nonwTemp->Add(kann_template_c,.4/kann_template_c->Integral());

//   if (!lightTemp || !nonwTemp)
//     {
//       throw std::runtime_error("Unable to find template!");
//     }

  gRandom = new TRandom3(7);

  unsigned nentries = static_cast<unsigned>(chain.GetEntries());

  //cout << "t4" << endl;

  if (nentries == 0)
    {
      std::cerr << "\tNo entries found!  Aborting.\n";
      return;
    }

  std::cout << "\tOriginal chain has " << nentries << " entries" << std::endl;

  std::set<Signature> sigSet;

  //cout << "t5" << endl;

  int not_taggable_events =0 ;
  //Loop over all the events
  for (unsigned ientry = 0; ientry < nentries; ++ientry){

    chain.GetEntry(ientry);
    
    microNtuple->clear();

//     if (!uclaNtuple->h.uniqueJet){
//       std::cerr << "Reject event for HFR  i="<<ientry<<" (h.det="<<uclaNtuple->h.det<<") Skipping.\n";
//       not_taggable_events ++;
//       continue;
//     }
   for (int i = 0; i < meNtuple->getNProbStat(); ++i)
      {
	microNtuple->eventProb[i]    = meNtuple->getProbStat(i)->tEventProb;
	microNtuple->eventMaxProb[i] = meNtuple->getProbStat(i)->tEventMaxProb;
	
      }

  //cout << "t6" << endl;

//     unsigned nTaggable = 0;
//     for (unsigned i = 0; i < nJets; ++i)
//       {
// 	microNtuple->knntag[i] = uclaNtuple->jets[i].kNN;
// 	microNtuple->tag[i]    = uclaNtuple->jets[i].secvTag;
	  
// 	if (uclaNtuple->jets[i].secvTaggable)
// 	  ++nTaggable;
//       }
      


//     if (nTaggable == 0)
//       {
// 	std::cerr << "Event has no taggable jets! i="<<ientry<<" (h.det="<<uclaNtuple->h.det<<") Skipping.\n";
// 	not_taggable_events ++;
// 	continue;
//       }
      
//    if(uclaNtuple->h.det<0) continue;
//    if (uclaNtuple->h.det>4&&uclaNtuple->h.passMetJetL2==false) continue;
   //if( doLight==false && doNonW==false && doUntag==false )continue;
       //&& !(uclaNtuple->h.ntag>0))continue;
     
    
  //int secvtxtag  = uclaNtuple->h.ntag;
  int secvtxtag  = 0;

    // ----------------------------------
    //  DO THE CALCULATION OF THE BPROBS |
    // ----------------------------------
    /////////////////////////////////////////////////////////////////////////////
    // Ricardo, Kike and Barbara, August 7th, 2009			       //
    // We changed the bProb definition. We don't sort the bProb		       //
    // values as we did before we are not using the fillBProb function.        //
    //                                                                         //
    // If we have 2 jets no secvtx tagged we will get the bProb value randomly //
    // not from the bigger one as we did before.                               //
    //                                                                         //
    // bProb1[1] is always -999 for all the proccesses.			       //
    // 									       //
    /////////////////////////////////////////////////////////////////////////////
    

    // Calculate bProb0 for the EPD loose tags
    microNtuple->bProb0[0] = 0.5;
    microNtuple->bProb0[1] = 0.5;

    // Calculate bProb for 0-tag events. NonW and light only, the rest of the proccesses 
    // have at least one secvtx tag.
//     if ( secvtxtag == 0 && (doLight || doNonW)) {

//       // Get a KIT from random value from the light or nonW template
//       double kit_temp = doLight ? lightTemp->GetRandom() : nonwTemp->GetRandom();
//       microNtuple->bProb1[0] = MicroNtuple::getKNNTagProb(kit_temp);
//       microNtuple->bProb1[1] = -999.;//we don't use the bProb1[1]

//       // for bprob2 we get two random values 
//       double kit_temp1 = doLight ? lightTemp->GetRandom() : nonwTemp->GetRandom();
//       double kit_temp2 = doLight ? lightTemp->GetRandom() : nonwTemp->GetRandom();
//       microNtuple->bProb2[0] = MicroNtuple::getKNNTagProb(kit_temp1);
//       microNtuple->bProb2[1] = MicroNtuple::getKNNTagProb(kit_temp2);

//     } // if 0-tagged events
  
   
//     // Calculate bProb for 1-tag events
//     if ( secvtxtag == 1) {

//       // find the kit of the tagged jet and put in kit_temp1
//       double kit_temp1 = -999.;
//       for(unsigned int i = 0; i< nJets; i++){
//         if(uclaNtuple->jets[i].kNN >= -1) kit_temp1 = uclaNtuple->jets[i].kNN;
//       }
//       microNtuple->bProb1[0] = MicroNtuple::getKNNTagProb(kit_temp1);
//       microNtuple->bProb1[1] = -999.;//we don't use the bProb1[1]

//       // get another random kit depending on the sample
//       double kit_temp2 = -999.;
//       if (doLight || doNonW) 
// 	kit_temp2 = doLight ? lightTemp->GetRandom() : nonwTemp->GetRandom();
//       else 
// 	kit_temp2 = lightTemp->GetRandom();

//       // Now fill bProb2[0] and bProb2[1] with kit_temp1 and kit_temp2
//       // but the order is random (which goes with which is random) 
//       // Pick a random number between 0 and 1, and half the time switch the order
//       if(gRandom->Rndm()<0.5) {
//         microNtuple->bProb2[0] = MicroNtuple::getKNNTagProb(kit_temp2);
// 	microNtuple->bProb2[1] = MicroNtuple::getKNNTagProb(kit_temp1);
//       } else {
// 	microNtuple->bProb2[0] = MicroNtuple::getKNNTagProb(kit_temp1);
// 	microNtuple->bProb2[1] = MicroNtuple::getKNNTagProb(kit_temp2);
//       }

//     }
 
//     // Calculate bProb for >= 2-tag events
//     if ( secvtxtag >= 2) {
//       //make a vector list of the kit for all tagged jets
//      vector<double> kitVec;
//      for(unsigned int i = 0; i< nJets; i++){
//         if(uclaNtuple->jets[i].kNN >= -1) kitVec.push_back(uclaNtuple->jets[i].kNN);        
//       }
//       // Pick one from the list randomly and assign that one for bProb1 
//       int index1 = (int)gRandom->Rndm()*kitVec.size();
//       microNtuple->bProb1[0] = MicroNtuple::getKNNTagProb(kitVec[index1]);
//       microNtuple->bProb1[1] = -999.;//we don't use the bProb1[1]

//       // Pick one from the list randomly and assign that one for bProb2       
//       int index2 = (int)gRandom->Rndm()*kitVec.size();
//       microNtuple->bProb2[0] = MicroNtuple::getKNNTagProb(kitVec[index2]);

//       // Pick another random number different from index2`
//       kitVec.erase(kitVec.begin()+index2); // remove index2 from the list
//       int index3 = (int)gRandom->Rndm()*kitVec.size();
//       microNtuple->bProb2[1] = MicroNtuple::getKNNTagProb(kitVec[index3]);         
//     }
//     // --------------------------------
//     // END CALCULATION OF THE BPROBS
//     // --------------------------------
 

    /*
    //Calculate bProb1 for EPD1tag
    double knn_1tag[nJets];
    double knn_2tag[nJets];      
    int secvtxtag  = uclaNtuple->h.ntag;

    for(unsigned int i = 0; i< nJets; i++){
    knn_1tag[i]=  uclaNtuple->jets[i].kNN;

    //Randomize the knntag for light and nonW samples for NO single and double tag events
    if (doLight || doNonW ){
	  
    if ( secvtxtag == 0 && knn_1tag[i]< -1 &&  uclaNtuple->jets[i].secvTaggable){
    knn_1tag[i] = doLight ? lightTemp->GetRandom() : 
    nonwTemp->GetRandom();
    microNtuple->knntag[i] = knn_1tag[i];
    }
	
    }//light and nonW

    }//for jets

    fillBProb(knn_1tag,microNtuple->bProb1, nJets); //fillBProb function

    //Calculate bProb2 for EPD2tag different from the bProb1 in the single tag case.
      
    for(unsigned int i = 0; i< nJets; i++){

    knn_2tag[i]=  uclaNtuple->jets[i].kNN;

    //Randomize the knntag for light, nonW samples and single tag secvtx events
    if ( knn_2tag[i]< -1  && uclaNtuple->jets[i].secvTaggable){

    if (doLight || doNonW){

    knn_2tag[i] = doLight ? lightTemp->GetRandom() : nonwTemp->GetRandom();
    microNtuple->knntag[i] = knn_2tag[i];

    }
    else if (secvtxtag == 1){
    knn_2tag[i] = lightTemp->GetRandom();
    microNtuple->knntag[i] = knn_2tag[i];
    }
    }//

    }//for jets


    fillBProb(knn_2tag, microNtuple->bProb2, nJets);//fillBProb function
    */

     
//     //JET PROB (JP): including JP in the microNtuples
//     microNtuple->nTagExt     = uclaNtuple->NtagExt();
//     microNtuple->tagProbExt0 = uclaNtuple->getTagProbExt(0,0);
//     microNtuple->tagProbExt1 = uclaNtuple->getTagProbExt(1,0);
//     microNtuple->tagProbExt2 = uclaNtuple->getTagProbExt(2,0);
   
    //Including Loose SecVtx tagProbs in the microNtuples
    microNtuple->nTagExtLooseSV     = 0;//uclaNtuple->NtagExtLooseSV();
    microNtuple->tagProbExtLooseSV0 = 0;//uclaNtuple->getTagProbExtLooseSV(0,0);
    microNtuple->tagProbExtLooseSV1 = 0;//uclaNtuple->getTagProbExtLooseSV(1,0);
    microNtuple->tagProbExtLooseSV2 = 0;//uclaNtuple->getTagProbExtLooseSV(2,0);

//     if (uclaNtuple->h.triggerTO <= 0)
//       throw std::runtime_error("Trigger turn-on of zero!");
//     microNtuple->weight = uclaNtuple->h.wgt * uclaNtuple->h.triggerTO;

//     microNtuple->h.det = uclaNtuple->h.det;
//     microNtuple->h.ntag = uclaNtuple->h.ntag;
//     microNtuple->h.run = uclaNtuple->h.run;
//     microNtuple->h.event = uclaNtuple->h.event;


    // Make sure to do these last!
    microNtuple->epd1tag = microNtuple->calcEPD(1, MicroNtuple::kCombined);
    microNtuple->epd2tag = microNtuple->calcEPD(2, MicroNtuple::kCombined);
    microNtuple->epd1tagSchan = microNtuple->calcEPD(1, MicroNtuple::kSchan);
    microNtuple->epd2tagSchan = microNtuple->calcEPD(2, MicroNtuple::kSchan);
    microNtuple->epd1tagTchan = microNtuple->calcEPD(1, MicroNtuple::kTchan);
    microNtuple->epd2tagTchan = microNtuple->calcEPD(2, MicroNtuple::kTchan);

    //cout << "t7" << endl;

    //WWandWZ EPD
    microNtuple->epd0tagWWandWZ = microNtuple->calcWZEPD(0, secvtxtag);
    microNtuple->epd1tagWWandWZ = microNtuple->calcWZEPD(1, secvtxtag);
    microNtuple->epd2tagWWandWZ = microNtuple->calcWZEPD(2, secvtxtag);

//     for (unsigned imass = 0; imass < MicroNtuple::nWHmasses; ++imass)
//       {
// 	const double masses[MicroNtuple::nWHmasses] = {100, 105, 110, 115, 120, 125, 130,
// 						       135, 140, 145, 150};

// 	// REGULAR EPD
// 	microNtuple->epdNoKITWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsEPD(0, masses[imass]);
// 	microNtuple->epd1tagWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsEPD(1, masses[imass]);
// 	microNtuple->epd2tagWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsEPD(2, masses[imass]);
	 
// 	// MAX PROB EPD
// 	microNtuple->MPepd1tagWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsMaxProbEPD(1,masses[imass]);
// 	microNtuple->MPepd2tagWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsMaxProbEPD(2,masses[imass]);

// 	// SUPER EPD, regular and maxProb all together
// 	microNtuple->MAPTIPepd1tagWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsSuperEPD(1,masses[imass]);
// 	microNtuple->MAPTIPepd2tagWH[MicroNtuple::getHiggsMassIndex(masses[imass])] 
// 	  = microNtuple->calcHiggsSuperEPD(2,masses[imass]);

//       }// for Higgs masses
     
    //      std::cerr << "Tag: " << microNtuple->knntag[0] << " " << microNtuple->knntag[1] << " Bprob: "
    //                << microNtuple->bProb[0] << " " << microNtuple->bProb[1] << " " << " EPD: " 
    //                << microNtuple->epd1tag << " " << microNtuple->epd2tag << std::endl;
    //      exit(1);

//     Signature temp = {uclaNtuple->h.run, uclaNtuple->h.event,
// 		      uclaNtuple->h.sample, uclaNtuple->h.det,
// 		      uclaNtuple->h.wgt, uclaNtuple->lepton[0].Pt()};

//     Signature temp = {eventNtuple->run, eventNtuple->event,0,0,1,eventNtuple->lLV[0].Pt()};
// //     //      if (false)
//     if (sigSet.find(temp) != sigSet.end())
//       {
// 	std::cerr << "\tWarning! Duplicate event found! Run " << temp.run 
// 		  << " Event " << temp.event << " Detector " << temp.detector
// 		  << std::endl;
//       }
//     else
//       {
// 	sigSet.insert(temp);
// 	outputTree->GetCurrentFile()->cd();
 	outputTree->Fill();
	//      }


 
  }//for entries
  std::cout << " Number of events with no taggable jets ="<<not_taggable_events <<std::endl;
  std::cout << "\tWrote " << output << " with " << outputTree->GetEntries()
	    << " entries" << std::endl;
  outputTree->GetCurrentFile()->Write();
  //   outputFile.Write();


  //  delete nonwTemp;
//  delete uclaNtuple;
  delete meNtuple;
  delete microNtuple;

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
    std::cerr << "\tNo files found!  Aborting.\n";
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
.L makeMicroNtuple.C+
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

  string basePath="/uscms/home/ilyao/nobackup/MEResults/387PF2PAT/";
  //string basePath="/uscms/home/ilyao/nobackup/MEResults/PAT356FullScan/";
  // string basePath="./";

  //Input files for 2-jet bin
  string inputPath=basePath+"";
  unsigned nJet = 2;

  //Input files for 3-jet bin
  //string inputPath=basePath+"Event_Probs_Summer09_3jets_StndTF/";
  //unsigned nJet = 3;

  //Output files
  //string outputPath=inputPath+"MicroNtuples/";
  string outputPath="/uscms/home/ilyao/nobackup/MEResults/387PF2PAT/microNtuples/";
  //string outputPath="./";

  //Create the list of MicroNtuples
  vector<MyStr> listOfMicroNtuples;

  //// Diboson
  //  listOfMicroNtuples.push_back(MyStr("WW3300Evt*","micro_WW3300Evt",false,false,false));

//   listOfMicroNtuples.push_back(MyStr("WW2200Evt*","micro_WW2200Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WZ1500Evt*","micro_WZ1500Evt",false,false,false));

//   //// W+jets and Z+jets
//   listOfMicroNtuples.push_back(MyStr("WpJ26000Evt*","micro_WpJ24000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("ZpJ680Evt*","micro_ZpJ680Evt",false,false,false));

//   //// ttbar
//   listOfMicroNtuples.push_back(MyStr("TTbar12000Evt*","micro_TTbar12000Evt",false,false,false));

  //// Single top
  listOfMicroNtuples.push_back(MyStr("STopT12000Evt*","micro_STopT24000Evt",false,false,false));
  listOfMicroNtuples.push_back(MyStr("STopS12000Evt*","micro_STopS24000Evt",false,false,false));

  //// QCD
//    listOfMicroNtuples.push_back(MyStr("QCDElData400Evt*","micro_QCDElData400Evt",false,false,false));
//    listOfMicroNtuples.push_back(MyStr("QCDMuData7600Evt*","micro_QCDMuData7600Evt",false,false,false));
//  listOfMicroNtuples.push_back(MyStr("QCDOnlyElData85Evt*","micro_QCDOnlyElData85Evt",false,false,false));


  //// Data
  //  listOfMicroNtuples.push_back(MyStr("DataEl1900Evt*","micro_DataEl1900Evt",false,false,false));
  listOfMicroNtuples.push_back(MyStr("DataMu2300Evt*","micro_DataMu2300Evt",false,false,false));
  listOfMicroNtuples.push_back(MyStr("Data*","micro_DataEl1900AndMu2300Evt",false,false,false));


//   //// Higgs 
//   listOfMicroNtuples.push_back(MyStr("HWWMH120_600Evt*","micro_HWWMH120_600Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH130_800Evt*","micro_HWWMH130_800Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH140_1100Evt*","micro_HWWMH140_1100Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH150_1600Evt*","micro_HWWMH150_1600Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("HWWMH160_2700Evt*","micro_HWWMH160_2700Evt",false,false,false));

//   listOfMicroNtuples.push_back(MyStr("WHMH115_3000Evt*","micro_WHMH115_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH120_3000Evt*","micro_WHMH120_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH125_3000Evt*","micro_WHMH125_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH130_3000Evt*","micro_WHMH130_3000Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("WHMH135_3000Evt*","micro_WHMH135_3000Evt",false,false,false));



//   //// W+jets and Z+jets
//   listOfMicroNtuples.push_back(MyStr("WpJ35700Evt*","micro_WpJ35700Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("ZpJ2000Evt*","micro_ZpJ2000Evt",false,false,false));

//   //// ttbar
//   listOfMicroNtuples.push_back(MyStr("TTbar30600Evt*","micro_TTbar30600Evt",false,false,false));

//   //// Single top
//   listOfMicroNtuples.push_back(MyStr("STopT42500Evt*","micro_STopT42500Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("STopS42500Evt*","micro_STopS42500Evt",false,false,false));

//   //// QCD
//   listOfMicroNtuples.push_back(MyStr("QCD_HT100to250NoIso92Evt*","micro_QCD_HT100to250NoIso92Evt",false,false,false));
//   listOfMicroNtuples.push_back(MyStr("QCD_HT250to500NoIso472Evt*","micro_QCD_HT250to500NoIso472Evt",false,false,false));


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

