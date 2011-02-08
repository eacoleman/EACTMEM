#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"

#include <iostream>
// #include "TAMUWW/SpecialTools/interface/TopLepType.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

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
    if (tagcat == DEFS::PreTag )    
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
      if (tagcat != DEFS::PreTag &&
	  tagcat != DEFS::eq0TSV )
	wei += "*(h.ntag>0)";

    // Pretag
    //if (tag==-1) 
    //wei += "*h.wgt*(h.tagProb0*h.passQCD + h.tagProb1*h.passQCD + h.tagProb2*( (h.det==2)*h.passQCD+(h.det!=2) ) )";

    //switch on ntags
    if (tagcat == DEFS::PreTag )      // pretag sample 
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
// We should have N number of tables, where N is the number of lepton categories
// multiplied by the number of tagging categories. Basically one table for 
// "0 tags electron", "1 tag electron", etc.
// Each table has:
//  - columns with the number of jets from 0 to 5.
//  - rows with the different physical processes as defined in the Defs.hh
// This method returns the total event expectation and its error 
// for the  given DEFS::PhysicsProcessType, jetBin, tagcat and detector
// as they appear in the appropriate place in the apropriate table. 

Value DefaultValues::getNorm(DEFS::PhysicsProcessType pType, DEFS::TagCat tagcat, 
			     DEFS::JetBin jetBin, int detector){
  
  // From the number of tags and the detector type find the proper table. 
  // The table might depend on the physics type as we might want to have, for 
  // example, the WW cross section depending whether the Higgs is included or not. 
  Table * table = getM2Table(pType, tagcat, detector);

  // Get the number and the error at the proper row and column names
  Value val = table->getValueAtRowColumnStrings(DEFS::getProcessTypeString(pType),
						DEFS::getJetBinString(jetBin));

  // Clean up
  delete table;

  // Return the value and its error
  return val;

}// DefaultValues::getNorm

// ----------------------------------------------------------------------------
// This method returns the appropriate table
Table * DefaultValues::getM2Table(DEFS::PhysicsProcessType processType, DEFS::TagCat tagcat, int detector){

  Table * table = new Table("M2table");
  
  table->parseFromFile("tableExample.txt");

  return table;
  
}//getMapOfM2Tables

// ----------------------------------------------------------------------------
// This method returns a map of files in which each process is map to a root file
map < DEFS::PhysicsProcessType, string > 
DefaultValues::getMapOfFiles(DEFS::JetBin jetBin){ // njets

  // BE CAREFUL THE ORDER THIS MAP STORES THE PROCESSES 
  // IS DEFINED BY THE DEFINITION OF PhysicsProcessType

  map<DEFS::PhysicsProcessType, string> processesMap;

  // Set the base-path where the files are located
  string path;
  if (jetBin == DEFS::jets2)
    path = "/cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008_pass2/Event_Probs_Summer09_newNNTF_atleastoneTightSecVtx/MicroNtuples_v4/";
  else if (jetBin == DEFS::jets3)   
    path = "/cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008_pass2/Event_Probs_Summer09_3jets_StndTF/MicroNtuples_v4/";
  else {
    cout<<" ERROR in DefaultValues::getMapOfFiles called with jetBin=="<<jetBin<<endl;
    return processesMap;
  }

  processesMap[DEFS::WH100  ] = path + "/" +"*WH100*root";
  processesMap[DEFS::WH105  ] = path + "/" +"*WH105*root";
  processesMap[DEFS::WH110  ] = path + "/" +"*WH110*root";
  processesMap[DEFS::WH115  ] = path + "/" +"*WH115*root";
  processesMap[DEFS::WH120  ] = path + "/" +"*WH120*root";
  processesMap[DEFS::WH125  ] = path + "/" +"*WH125*root";
  processesMap[DEFS::WH130  ] = path + "/" +"*WH130*root";
  processesMap[DEFS::WH135  ] = path + "/" +"*WH135*root";
  processesMap[DEFS::WH140  ] = path + "/" +"*WH140*root";
  processesMap[DEFS::WH145  ] = path + "/" +"*WH145*root";
  processesMap[DEFS::WH150  ] = path + "/" +"*WH150*root";
  processesMap[DEFS::STopS  ] = path + "/" +"*schan*root";
  processesMap[DEFS::STopT  ] = path + "/" +"*tchan*root";
  processesMap[DEFS::Wbb    ] = path + "/" +"*_bottom*root";
  processesMap[DEFS::Wcc    ] = path + "/" +"*_charm*root";
  processesMap[DEFS::WLight ] = path + "/" +"*_light*root";
  processesMap[DEFS::NonW   ] = path + "/" +"*nonW*root";
  processesMap[DEFS::WW     ] = path + "/" +"*WW*root";
  processesMap[DEFS::WZ     ] = path + "/" +"*WZ*root";
  processesMap[DEFS::ZZ     ] = path + "/" +"*ZZ*root";
  processesMap[DEFS::TopLJ  ] = path + "/" +"*nondilepton*root";
  processesMap[DEFS::TopDil ] = path + "/" +"*_dilepton*root";
  processesMap[DEFS::Zjets  ] = path + "/" +"*Zjets*root";
  processesMap[DEFS::Data   ] = path + "/" +"*data*root";
  
  return processesMap;

}//getMapOfFiles


// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getAllProcesses(DEFS::JetBin jetBin, DEFS::TagCat tagcat){

  // The returning vector of processes
  vector<PhysicsProcess*>  proc;

  // Get the map of files
  map<DEFS::PhysicsProcessType, string> processesMap = getMapOfFiles(jetBin);

  // Loop over the files
  for (map<DEFS::PhysicsProcessType,string>::iterator it = processesMap.begin(); it != processesMap.end(); it++){

      // Get the single process
      PhysicsProcess * thisProc = getSingleProcess(jetBin, tagcat, it->first);

      // Copy the object to the vector
      proc.push_back(thisProc);    
   
  }//for 
  
  return proc;

}//getAllProcesses

// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getProcesses(DEFS::JetBin jetBin, DEFS::TagCat tagcat, 
							DEFS::PhysicsProcessType HiggsType){

  // The returning vector of processes
  vector<PhysicsProcess*>  proc;

  // Get the map of files
  map<DEFS::PhysicsProcessType, string> processesMap = getMapOfFiles(jetBin);

  // Loop over the files
  for (map<DEFS::PhysicsProcessType,string>::iterator it = processesMap.begin(); it != processesMap.end(); it++){

    // Skip Data in this method
    if (it->first == DEFS::Data ) continue;

    // Add only if it is not Higgs, or if it is the type we want
    if (!DEFS::isHiggs(it->first) || it->first == HiggsType){

      // Get the single process
      PhysicsProcess * thisProc = getSingleProcess(jetBin, tagcat, it->first);

      // Copy the object to the vector
      proc.push_back(thisProc);    
      
    }//if

  }//for 

  return proc;

}//getProcesses

// ----------------------------------------------------------------------------
vector < PhysicsProcess * > DefaultValues::getProcesses(DEFS::JetBin jetBin, DEFS::TagCat tagcat, int HiggsMassIndex){
  return getProcesses(jetBin, tagcat, DEFS::getHiggsType(HiggsMassIndex));
}

// ----------------------------------------------------------------------------
PhysicsProcess * DefaultValues::getSingleProcess(DEFS::JetBin jetBin, DEFS::TagCat tagcat, 
						 DEFS::PhysicsProcessType processType){
  
  // The returning process
  PhysicsProcess*  proc = 0;

  // Get the map of files
  map<DEFS::PhysicsProcessType, string> processesMap = getMapOfFiles(jetBin);

  // Get the process from the map
  map<DEFS::PhysicsProcessType,string>::iterator it = processesMap.find(processType);

  // If the process is in the list
  if (it != processesMap.end()) {
    
    //Report
    cout<<" Process "<< DEFS::getProcessTypeString(it->first) 
	<<" from file "<< it->second <<endl;    
    
    // Create the chain    
    TChain * chain ;
    chain  = new TChain("METree");
    chain->Add(it->second.c_str());
    
    // Create the process
    proc = new PhysicsProcess(it->first, chain ,1,0);
    
    // Set the weight
    proc->setWeight(getWeightForCategory(tagcat, it->first, 0));
    
    // Set the category 
    proc->setCategory("h.det*(h.det<5)+ 5*(h.det>=5)");
    
    // Set the normalization for each category
    for (int iDet = 0 ; iDet < 5 ; iDet++) {
      Value norm = getNorm(it->first, tagcat, jetBin, iDet);
      proc->setCategoryNorm(iDet+1,norm.value,norm.error);
    }

  }else{
    cout<<"ERROR  DefaultValues::getSingleProcess was called withprocessType="<<processType
	<<"\n but the process is not in the map of files!"<<endl;
    return 0;
  }
  
  return proc;

}//getSingleProcess

