#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"


// ----------------------------------------------------------------------------
// This method returns the full basepath of config files.
// Basically returns $CMSSW_BASE+"/src/TAMUWW/Config/Official/"
string DefaultValues::getConfigPath(){

  string basePath;
  char const* tmp = getenv("CMSSW_BASE");
  
  if(tmp != NULL)
    basePath = string(tmp);
  else {
    cout << "ERROR DefaultValues::getConfigPath() cannot find the top of the local CMSSW release" << endl;
    assert(tmp!=NULL);
  }
  
  return basePath + "/src/TAMUWW/ConfigFiles/Official/";

}// getConfigPath


// ----------------------------------------------------------------------------
// This method returns the table with the event expectation for the evt/tag category
Table DefaultValues::getNormTable(DEFS::LeptonCat evtcat, DEFS::TagCat tagcat){

  Table table("NormTable");
  string eventEstimatesFile = getConfigPath()+"EventEstimates_";

  // add the tag name and the ".txt" at the end
  eventEstimatesFile += DEFS::getEventCatString(evtcat)+"_";
  eventEstimatesFile += DEFS::getTagCatString(tagcat);
  eventEstimatesFile += ".txt";

  if(!table.parseFromFile(eventEstimatesFile))
    cout<<"ERROR  DefaultValues::getNormTable() cannot parse config file "
	<<eventEstimatesFile<<endl;

  return table;
  
}//getNormTable

// ----------------------------------------------------------------------------
// This method returns the appropriate table of file location for each jet bin
// and tag category
Table DefaultValues::getFileLocationTable(DEFS::TagCat tagcat){ 

  // The location of the table with the file locations
  string fileLocationFile = getConfigPath()+"FileLocation_";
  
  // add the tag name and the ".txt" at the end
  fileLocationFile += DEFS::getTagCatString(tagcat);
  fileLocationFile += ".txt";

  // Create the table and parse the contents of the file
  FileLocationTable table("FileLocationTable");  
  if(!table.parseFromFile(fileLocationFile,"TableCellText","Normal"))
    cout<<"ERROR  DefaultValues::getFileLocationTable() cannot parse config file "
	<<fileLocationFile<<endl;


  // make sure you add the basepath to the table
  table.addBasePath();

  return table;
  
}//getFileLocationTable



// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getProcesses(vector<DEFS::PhysicsProcessType> processName,
                                                        DEFS::JetBin jetBin, 
                                                        DEFS::TagCat tagcat,
                                                        bool forPlots,
                                                        DEFS::NtupleType ntupleType){

  // The returning vector of processes
  vector<PhysicsProcess*>  proc;

  // get the table with the expected number of 
  map<DEFS::LeptonCat, Table> normTable;
  normTable[DEFS::muon    ] = getNormTable(DEFS::muon    ,tagcat);
  normTable[DEFS::electron] = getNormTable(DEFS::electron,tagcat);

  // get the table with the files location
  Table fileTable = getFileLocationTable(tagcat);

  // Loop over all the process names
  for (unsigned int prn = 0; prn < processName.size(); prn++){

    PhysicsProcess * pr = getSingleProcess(processName[prn], jetBin, normTable, fileTable, forPlots, ntupleType);
    if (pr == 0) {
      cout<<"ERROR DefaultValues::getProcesses couldnot add process"<<endl;
      continue;
    }

    proc.push_back(pr);
    
  }//for

  return proc;

}// getProcesses


// ----------------------------------------------------------------------------
// Return a PhysicsProcess object for process "process" from the given inputs.
// For some reson it does not compile when I use the signature
// (..., const Table & normTable, const Table & fileTable, ...) 
PhysicsProcess * DefaultValues::getSingleProcess(DEFS::PhysicsProcessType process,
                                                 DEFS::JetBin jetBin,
                                                 map<DEFS::LeptonCat, Table> normTable,
                                                 Table fileTable,
                                                 bool forPlots,
                                                 DEFS::NtupleType ntupleType){

   // get the process name
   string prName = DEFS::PhysicsProcess::getTypeString(process);
   
   // get the name of the jetBin
   string jetBinName = DEFS::getJetBinString(jetBin);
   
   // find the file location for that process
   TableCellText * cellFile = (TableCellText *) fileTable.getCellRowColumn(prName,"FilePath_"+DEFS::getNtupleTypeString(ntupleType));

   // make sure we found the cell
   if (cellFile == 0){
      cout<<"ERROR DefaultValues::getSingleProcess Table "<<fileTable.getTableOrigin()
          <<" does not have row "<<prName
          <<" for column FilePath_" << DEFS::getNtupleTypeString(ntupleType) <<endl;
      cout<<" SKIPPING PROCESS "<<prName<<endl;
      return 0;
   }
   
   // Get the physics parameters
   map<DEFS::LeptonCat,double> xsec;
   xsec[DEFS::electron] = getCrossSectionAndError(prName).first;
   xsec[DEFS::muon]     = getCrossSectionAndError(prName).first;
   map<DEFS::LeptonCat,double> lumi;
   if (process==DEFS::PhysicsProcess::SingleEl_Data || process==DEFS::PhysicsProcess::SingleMu_Data ||
       process==DEFS::PhysicsProcess::QCD_ElFULL){
      lumi[DEFS::electron] = 1.0;
      lumi[DEFS::muon]     = 1.0;
   }
   else{
      lumi[DEFS::electron] = 19148;
      lumi[DEFS::muon]     = 19279;
   }
   map<DEFS::LeptonCat,double> br;
   br[DEFS::electron] = getBranchingRatio(prName);
   br[DEFS::muon]     = getBranchingRatio(prName);
   map<DEFS::LeptonCat,unsigned int> numMCEvts;
   numMCEvts[DEFS::electron] = (unsigned int)getNumMCEvts(prName);
   numMCEvts[DEFS::muon]     = (unsigned int)getNumMCEvts(prName);
   map<DEFS::LeptonCat,double> sf;
   sf[DEFS::electron] = getScaleFactor(prName);
   sf[DEFS::muon]     = getScaleFactor(prName);

   // Create the PhysicsProcess
   PhysicsProcess *  proc;
   if(forPlots)
      proc =  new PlotterPhysicsProcess(prName, getTypeTitle(process), cellFile->text, getProcessColor(process), DEFS::getTreeName(ntupleType,jetBin));
   else
      proc =  new PhysicsProcess(prName, getTypeTitle(process), cellFile->text, DEFS::getTreeName(ntupleType,jetBin));
   proc->setPhysicsParameters(xsec, lumi, br, numMCEvts, sf);
   
   // and return it.
   return proc;
   
}//getSingleProcess


// ----------------------------------------------------------------------------
string  DefaultValues::getWeightForCategory(DEFS::TagCat tagcat, DEFS::PhysicsProcessType type, int iDet){

  // The returning weights, for the moment just "1"
  return "1";

  /*
    string wei ;
  // Set the detector type
  if (iDet ==  TopLepType::TCEM)
    wei += "(h.det==1)";
  else if (iDet ==  TopLepType::TPHX)
    wei += "(h.det==2)";
  else if (iDet ==  TopLepType::TCMUP)
    wei += "(h.det==3)";
  else if (iDet ==  TopLepType::TCMX)
    wei += "(h.det==4)";
  else if (iDet == 5)
    wei += "(h.det>=5)";
  else 
    wei += "(h.det>0)";

  // Set the QCD veto. For PHX add a h.met>25 cut
  wei += "*((h.det==2)*(h.met>25)+(h.det != 2))";


  // For data require 
  if (type == DEFS::Data ){
    if (tagcat == DEFS::pretag )    
      wei += "*1";
    else if (tagcat == DEFS::eq0TSV)
      wei += "*(h.ntag==0)";
    else if (tagcat == DEFS::eq1TSV)
      wei += "*(h.ntag==1)";
    else if (tagcat == DEFS::eq2TSV)
      wei += "*(h.ntag==2)";      
    else
      cout<<"ERROR DefaultValues::getWeightForCategory (data) called with tagcat="<<tagcat<<endl;     
  }
  // For MC require 
  else {

    // I have to apply this for the 1 and the 2 tags. Do I need it for the 0 tag ??
    if (type != DEFS::WLight && type != DEFS::NonW)
      if (tagcat != DEFS::pretag &&
	  tagcat != DEFS::eq0TSV )
	wei += "*(h.ntag>0)";

    // Pretag
    //if (tag==-1) 
    //wei += "*h.wgt*(h.tagProb0*h.passQCD + h.tagProb1*h.passQCD + h.tagProb2*( (h.det==2)*h.passQCD+(h.det!=2) ) )";

    //switch on ntags
    if (tagcat == DEFS::pretag )      // pretag sample 
      wei += "*(h.wgt*h.passQCD)";
    else if (tagcat == DEFS::eq0TSV )      // untag sample 
      wei += "*(h.tagProb0*h.wgt*h.passQCD)";
    else if (tagcat == DEFS::eq1TSV) // single tag sample
      wei += "*(h.tagProb1*h.wgt*h.passQCD)";
    else if (tagcat == DEFS::eq2TSV) // double tags sample
      wei += "*(h.tagProb2*h.wgt*( (h.det==2)*h.passQCD+(h.det!=2) ))";      
    else
      cout<<"ERROR DefaultValues::getWeightForCategory (MC) called with tagcat="<<tagcat<<endl;     

  }

  // For MC only, use triggerTO for tight, but getMet2JetTurnOn_v2 for loose leptons
  if (type != DEFS::Data)
    wei += "*(((h.det<5) * h.triggerTO)+ (h.det>=5) * 0.982 /(1.0 + exp(-(h.corrVtxMet-42.78)/3.681)))";


  return wei;
  */

}//getWeightForCategory

// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getProcessesWW(DEFS::JetBin jetBin,
                                                          DEFS::TagCat tagcat, 
                                                          bool include_data,
                                                          bool forPlots,
                                                          DEFS::NtupleType ntupleType){

  vector<DEFS::PhysicsProcess::Type> procs;


  procs.push_back(DEFS::PhysicsProcess::STopS_T    );
  procs.push_back(DEFS::PhysicsProcess::STopS_Tbar );
  procs.push_back(DEFS::PhysicsProcess::STopT_T    );
  procs.push_back(DEFS::PhysicsProcess::STopT_Tbar );
  procs.push_back(DEFS::PhysicsProcess::STopTW_T   );
  procs.push_back(DEFS::PhysicsProcess::STopTW_Tbar);
  procs.push_back(DEFS::PhysicsProcess::TTbar      );
  //procs.push_back(DEFS::PhysicsProcess::TTbarLJ );
  //procs.push_back(DEFS::PhysicsProcess::TTbarDil); 
  //procs.push_back(DEFS::PhysicsProcess::Wbb     );
  //procs.push_back(DEFS::PhysicsProcess::Wcc     );
  //procs.push_back(DEFS::PhysicsProcess::WLight  );
  procs.push_back(DEFS::PhysicsProcess::WJets     ); 
  procs.push_back(DEFS::PhysicsProcess::ZJets     );
  //procs.push_back(DEFS::PhysicsProcess::Ztautau );
  //procs.push_back(DEFS::PhysicsProcess::QCDMu               );
  procs.push_back(DEFS::PhysicsProcess::WW      );
  procs.push_back(DEFS::PhysicsProcess::WZ      );
  //procs.push_back(DEFS::PhysicsProcess::ZZ    );

  if (include_data) {
    procs.push_back(DEFS::PhysicsProcess::SingleEl_Data    );
    procs.push_back(DEFS::PhysicsProcess::SingleMu_Data    );
  }

  return getProcesses(procs, jetBin, tagcat, forPlots, ntupleType);

}//getProcessesWW

// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getProcessesHiggs(DEFS::JetBin jetBin,
                                                             DEFS::TagCat tagcat, 
                                                             bool include_data,
                                                             bool forPlots,
                                                             DEFS::NtupleType ntupleType){

   vector<DEFS::PhysicsProcess::Type> procs;   
   procs.push_back(DEFS::PhysicsProcess::STopS_T);
   procs.push_back(DEFS::PhysicsProcess::STopS_Tbar);
   procs.push_back(DEFS::PhysicsProcess::STopT_T);
   procs.push_back(DEFS::PhysicsProcess::STopT_Tbar);
   procs.push_back(DEFS::PhysicsProcess::STopTW_T);
   procs.push_back(DEFS::PhysicsProcess::STopTW_Tbar);
   procs.push_back(DEFS::PhysicsProcess::TTbar);
   //procs.push_back(DEFS::PhysicsProcess::TTbarLJ );
   //procs.push_back(DEFS::PhysicsProcess::TTbarDil); 
   //procs.push_back(DEFS::PhysicsProcess::Wbb     );
   //procs.push_back(DEFS::PhysicsProcess::Wcc     );
   //procs.push_back(DEFS::PhysicsProcess::WLight  );
   procs.push_back(DEFS::PhysicsProcess::WJets); 
   procs.push_back(DEFS::PhysicsProcess::ZJets);
   //procs.push_back(DEFS::PhysicsProcess::Ztautau);
   //procs.push_back(DEFS::PhysicsProcess::QCDMu);
   //procs.push_back(DEFS::PhysicsProcess::QCD_Pt20to30_EMEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_Pt30to80_EMEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_Pt80to170_EMEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_Pt170to250_EMEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_Pt250to350_EMEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_Pt350_EMEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_ElEnriched);
   procs.push_back(DEFS::PhysicsProcess::QCD_ElFULL); // use to derive QCD scale factors
   //procs.push_back(DEFS::PhysicsProcess::QCD_MuEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD250  );
   procs.push_back(DEFS::PhysicsProcess::WW);
   procs.push_back(DEFS::PhysicsProcess::WZ);
   //procs.push_back(DEFS::PhysicsProcess::ZZ);
   procs.push_back(DEFS::PhysicsProcess::ggH125);
   procs.push_back(DEFS::PhysicsProcess::qqH125);
   procs.push_back(DEFS::PhysicsProcess::WH125);
   

   if (include_data) {
      procs.push_back(DEFS::PhysicsProcess::SingleEl_Data);
      //procs.push_back(DEFS::PhysicsProcess::SingleMu_Data);
   }
   

   return getProcesses(procs, jetBin, tagcat, forPlots, ntupleType);

}//getProcessesWW

// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getProcessesTest(DEFS::JetBin jetBin,
                                                                DEFS::TagCat tagcat, 
                                                                bool include_data,
                                                                bool forPlots){

   vector<DEFS::PhysicsProcess::Type> procs;
   
   
   //procs.push_back(DEFS::PhysicsProcess::STopS_T);
   //procs.push_back(DEFS::PhysicsProcess::STopS_Tbar);
   //procs.push_back(DEFS::PhysicsProcess::STopT_T);
   //procs.push_back(DEFS::PhysicsProcess::STopT_Tbar);
   //procs.push_back(DEFS::PhysicsProcess::STopTW_T);
   //procs.push_back(DEFS::PhysicsProcess::STopTW_Tbar);
   //procs.push_back(DEFS::PhysicsProcess::TTbar);
   //procs.push_back(DEFS::PhysicsProcess::TTbarLJ );
   //procs.push_back(DEFS::PhysicsProcess::TTbarDil); 
   //procs.push_back(DEFS::PhysicsProcess::Wbb     );
   //procs.push_back(DEFS::PhysicsProcess::Wcc     );
   //procs.push_back(DEFS::PhysicsProcess::WLight  );
   procs.push_back(DEFS::PhysicsProcess::WJets); 
   procs.push_back(DEFS::PhysicsProcess::ZJets);
   //procs.push_back(DEFS::PhysicsProcess::Ztautau);
   //procs.push_back(DEFS::PhysicsProcess::QCDMu);
   //procs.push_back(DEFS::PhysicsProcess::QCDEl_Pt30to80);
   //procs.push_back(DEFS::PhysicsProcess::QCDEl_Pt80to170);
   //procs.push_back(DEFS::PhysicsProcess::QCDEl_BCtoE30to80);
   //procs.push_back(DEFS::PhysicsProcess::QCDEl_BCtoE80to170);
   procs.push_back(DEFS::PhysicsProcess::QCD_ElEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD_MuEnriched);
   //procs.push_back(DEFS::PhysicsProcess::QCD250  );
   procs.push_back(DEFS::PhysicsProcess::WW);
   //procs.push_back(DEFS::PhysicsProcess::WZ);
   //procs.push_back(DEFS::PhysicsProcess::ZZ);
   procs.push_back(DEFS::PhysicsProcess::ggH125);
   procs.push_back(DEFS::PhysicsProcess::qqH125);
   //procs.push_back(DEFS::PhysicsProcess::WH125);
   
   if (include_data) {
      procs.push_back(DEFS::PhysicsProcess::SingleEl_Data);
      procs.push_back(DEFS::PhysicsProcess::SingleMu_Data);
   }

   return getProcesses(procs, jetBin, tagcat, forPlots, DEFS::MicroNtuple);

}//getProcessesTest

// ----------------------------------------------------------------------------
pair<double,double> DefaultValues::getCrossSectionAndError(TString channelName)
{
  Table table;
  double xsec;
  double error;

  table.parseFromFile(getConfigPath()+"CrossSections_8TeV.txt","TableCellVal");
  TableCell * cell = table.getCellRowColumn(string(channelName),"CrossSection");
  if(cell){
    xsec = ((TableCellVal*)cell)->val.value;
    error = ((TableCellVal*)cell)->val.error;
    if (xsec==0)
      cout << "WARNING::getCrossSection::The cross section for " << channelName << " is 0.0 +/- 0.0" << endl;
    return make_pair(xsec,error);
  } else{
    cout << "WARNING::getCrossSection::channelName " << channelName 
	 << " not recognized. Returning -1 for the cross section." << endl 
	 << "The events will have the same scale as the MC sample, but on a negative scale." << endl 
	 << "Please check channel names." << endl;
    return make_pair(-1.0,-1.0);
  }
}//getCrossSection

// ----------------------------------------------------------------------------
double DefaultValues::getBranchingRatio(TString channelName)
{
  Table table;
  double br;

  table.parseFromFile(getConfigPath()+"BranchingRatios_8TeV.txt","TableCellVal");
  TableCell * cell = table.getCellRowColumn(string(channelName),"BranchingRatio");
  if(cell){
    br = ((TableCellVal*)cell)->val.value;
    if (br==0)
      cout << "WARNING::getBranchingRatio::The branching ratio for " << channelName << " is 0.0 +/- 0.0" << endl;
    return br;
  } else{
    cout << "WARNING::getBranchingRatio::channelName " << channelName 
	 << " not recognized. Returning -1 for the branching ratio." << endl 
	 << "The events will have the same scale as the MC sample, but on a negative scale." << endl 
	 << "Please check channel names." << endl;
    return -1.;
  }
}//getBranchingRatio

// ----------------------------------------------------------------------------
double DefaultValues::getNumMCEvts(TString channelName)
{
  Table table;
  double value;

  table.parseFromFile(getConfigPath()+"EventsFromMC_commonPATTuple_532.txt",
		      "TableCellVal");
  TableCell * cell =table.getCellRowColumn(string(channelName),"Events_PATtuple");
  if(cell){
    value = ((TableCellVal*)cell)->val.value;
    if (value==0)
      cout << "WARNING::getNumMCEvts::There are 0 events in the " << channelName << " MC." << endl;
    return value;
  } else{
    cout << "WARNING::getNumMCEvts::channelName " << channelName 
	 << " not recognized. Returning -1 event from MC." << endl 
	 << "Please check channel names." << endl;
    return -1.;
  }
}//getNumMCEvts

// ----------------------------------------------------------------------------
double DefaultValues::getScaleFactor(TString channelName)
{
  Table table;
  double sf;

  table.parseFromFile(getConfigPath()+string("ScaleFactors_8TeV.txt"),"TableCellVal");
  TableCell * cell = table.getCellRowColumn(string(channelName),"ScaleFactor");
  if(cell){
    sf = ((TableCellVal*)cell)->val.value;
    if (sf==0)
       cout << "WARNING::getScaleFactor::The scale factor for " << channelName << " is 0.0 +/- 0.0" << endl
            << "This means the process will be killed" << endl;
    return sf;
  } else{
    cout << "WARNING::getScaleFactor::channelName " << channelName 
	 << " not recognized. Returning -1 for the scale factor." << endl 
	 << "The events will have the same scale as the MC sample, but on a negative scale." << endl 
	 << "Please check channel names." << endl;
    return -1.;
  }
}//getScaleFactor

// ----------------------------------------------------------------------------
int DefaultValues::vfind(vector<string> a, string b) {
   for (unsigned int i=0; i<a.size(); i++) {
      if (TString(a[i]).CompareTo(TString(b))==0)
         return i;
   }
   return -1;
}

// ----------------------------------------------------------------------------
int DefaultValues::vfind(vector<TString> a, TString b) {
   for (unsigned int i=0; i<a.size(); i++) {
      if (a[i].CompareTo(b)==0)
         return i;
   }
   return -1;
}

// ----------------------------------------------------------------------------
void DefaultValues::DestroyCanvases() {
   TList* loc = (TList*)gROOT->GetListOfCanvases();
   TListIter itc(loc);
   TObject *o(0);
   while ((o = itc())) delete o;
}

// ----------------------------------------------------------------------------
TObject* DefaultValues::getConfigTObject(TString objectFile, TString oname, TString newName) {
 
   TString basePath = getConfigPath() + objectFile;

   TObject * hnew;
   TString currentDir = gDirectory->GetPathStatic();

   // open the file
   TFile * ifile = TFile::Open(basePath);
   if (!ifile->IsOpen()) {
      cout << "\tERROR DefaultValues::getConfigHisto file "+basePath
           << " could not be opened." << endl;
      return 0;
   }
   
   // get the histogram from the inside
   TObject * htemp = ifile->Get(oname);

   if(htemp==0) {
      cout << "ERROR DefaultValues::getConfigTObject the object " << oname << " was not found in file " << basePath << endl;
      assert(htemp!=0);
   }
   
   gDirectory->cd(currentDir);

   // clone it, assigne it to QCDWeightFunc and close the file
   hnew = htemp->Clone(newName);
   ifile->Close();

   return hnew;

}

// ----------------------------------------------------------------------------
TH1* DefaultValues::getConfigTH1(TString histoFile, TString hname, TString newName) {

   TH1* h = (TH1*) getConfigTObject(histoFile, hname, newName);

   if(h==0) {
      cout << "ERROR DefaultValues::getConfigTH1 the object found in file " << histoFile << "cannot be cast to a TH1*" << endl;
      assert(h!=0);
   }

   return h;

}

// ----------------------------------------------------------------------------
TH2* DefaultValues::getConfigTH2(TString histoFile, TString hname, TString newName) {

   TH2* h = (TH2*) getConfigTObject(histoFile, hname, newName);

   if(h==0) {
      cout << "ERROR DefaultValues::getConfigTH2 the object found in file " << histoFile << "cannot be cast to a TH2*" << endl;
      assert(h!=0);
   }

   return h;

}
