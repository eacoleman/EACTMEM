#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <map>
// #include "TROOT.h"
// #include "TSystemDirectory.h"
#include "TSystem.h"
#include "TLorentzVector.h"


using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::string;

class madProcess {

public:
  string name;
  string initial_state;
  string final_state;
  
  string diagram;
  double xs;
  double err_xs;
  double cum_xs;

  // Get the flavor of the final state
  // 0 for light  (everything but charm or bottom)
  // 1 for charm  (charm but not bottom)
  // 2 for bottom
  int getFlavor(){
    if (final_state.find("b") != string::npos)
      return 2;
    if ( (final_state.find("c") != string::npos) && 
	 (final_state.find("b") == string::npos))
      return 1;
    
    return 0;
  }


  void Print(){
    cout<<" "<<initial_state<<" "<<final_state<<" "<<diagram<<" "<<xs<<" "<<err_xs<<" "<<cum_xs<<endl;
  }

};

//----------------------------------------------------------------------------------------
vector<string> SplitString(string str, string delim){

  vector<string> results;
  unsigned int cutAt;
  while( (cutAt = str.find_first_of(delim)) != str.npos ) {
    if(cutAt > 0) {
      results.push_back(str.substr(0,cutAt));
    }
    str = str.substr(cutAt+1);
  }
  if(str.length() > 0){
    results.push_back(str);
  }

  return results;

}//SplitString

//----------------------------------------------------------------------------------------
bool xs_comparator(madProcess m1, madProcess m2){
  if (m2.getFlavor() != m1.getFlavor())
    return m2.getFlavor() < m1.getFlavor();

  // light processes first
  return m2.xs<m1.xs;

}//xs_comparator


//----------------------------------------------------------------------------------------
vector<madProcess>  IdentifySubProcessesFile(string filename){

  vector<madProcess> mad_process;

  // Open the file and make sure it exists and its open
  ifstream file(filename.c_str(), std::ios::in);
  if (!file){
    cout << "Unable to open " << filename << " in IdentifySubProcesses"<<endl;
    return mad_process;
  }
  
  //Loop until the next line is "<event>
  
  string p_name = ""; // process name
  string is     = ""; // initial state
  string fs     = ""; // final state
  double units_xs = 0.0, units_exs = 0.0;
  string nextline;
  while (!file.eof()){

    // Get the next line 
    getline(file, nextline);
    vector<string> words = SplitString(nextline,"<>=");

    // Get the title if never gotten befoer
    if ((p_name == "") && (words.size()>4)){
      if (words[0]=="head"   && words[1]=="title" && 
	  words[3]=="/title" && words[4]=="/head"){
	
	p_name = words[2];
	vector<string> w = SplitString(words[2],"_");
	if (w.size()>1){
	  is = w[0];
	  fs = w[1];
	}

      }//if    
    }//if
  
    //for (int t=0;t<words.size();t++)
    //cout<<"   w"<<t<<"="<<words[t];
    //cout<<endl;

    // Set the units if not set already
    if (units_xs <1E-3 && words.size()>2 ){
      if (words[1]=="Cross Sect(ab)")
	units_xs = 0.1;
      if (words[1]=="Cross Sect(fb)")
	units_xs = 1;
      if (words[1]=="Cross Sect(pb)")
	units_xs = 1000;
      if (words[1]=="Cross Sect(nb)")
	units_xs = 1000000;

      //cout<<"\t UNITS XS set to "<<units_xs<<endl;
    }

    // Set the units if not set already
    if (units_exs <1E-3 && words.size()>4){
      if (words[4]=="Error(ab)")
	units_exs = 0.1;
      if (words[4]=="Error(fb)")
	units_exs = 1;
      if (words[4]=="Error(pb)")
	units_exs = 1000;
      if (words[4]=="Error(nb)")
	units_exs = 1000000;
      //cout<<"\t UNITS EXS set to "<<units_exs<<endl;
    }
   

    if (words.size()>15 && words[0] == "tr" && words[1] == "td align" && 
	words[2] == "right"){
      if ((units_xs <1E-3) || (units_exs<1E-3)){
	cout<<" ERROR IdentifySubProcessesFile Bad format in file "<<filename<<endl;
	cout<<"\t System of units xs and exs have not been set!!"<<endl;
	assert(1);
      }
      madProcess mp;
      mp.name = p_name;
      mp.initial_state = is;
      mp.final_state = fs;
      mp.diagram = words[3];
      mp.xs = units_xs * atof(words[9].c_str());
      mp.err_xs = units_exs * atof(words[15].c_str());     
      mad_process.push_back(mp);
    }
    
  }//while
  
  return mad_process;

}//IdentifySubProcessesFile



//----------------------------------------------------------------------------------------
bool IsItDirectory(const char * name){

  Long64_t size =0 ;
  Long_t id=0, flags =0, modtime = 0;

  gSystem->GetPathInfo(name, &id, & size, & flags, &modtime);
  Int_t isdir = (Int_t) flags & 2;

  if (isdir) 
    return true;
    
  return false;

}//IsItDirectory

//----------------------------------------------------------------------------------------
void PrintOutDiagrams(string title, vector<madProcess> mp){

  cout<<title<<endl;

  //Get the total cumulative cross section
  double total_xs = 0;
  for (unsigned int d=0;d<mp.size();d++)
    total_xs += mp[d].xs;

  //Report all the diagrams
  double cum_total = 0;
  for (unsigned int d=0;d<mp.size();d++){
    cum_total +=  mp[d].xs;
    mp[d].cum_xs = cum_total/total_xs; 
    mp[d].Print();
  }

  // Report the number of diagrams
  cout<<"\t Number of diagrams "<<mp.size()<<" with sum of xs="<<total_xs<<endl;
  cout<<endl;

}// PrintOutDiagrams

//----------------------------------------------------------------------------------------
void IdentifySubProcesses(string dir_name,string file_name){

  // To store the processes
  vector<madProcess> mad_processes;

  // Open the directory
  void *dirp = gSystem->OpenDirectory(dir_name.c_str());

  // Loop over its contents
  const char * nextFileName;

  while((nextFileName = gSystem->GetDirEntry(dirp))) { 
    if (IsItDirectory((dir_name+"/"+nextFileName).c_str()) && 
	(string(nextFileName) != "." ) && 
	(string(nextFileName) != ".." )){
      vector<madProcess> mps = IdentifySubProcessesFile(dir_name+"/"+nextFileName+"/"+file_name);
      mad_processes.insert(mad_processes.end(),mps.begin(),mps.end());
      cout<<" Inserted "<<mps.size()<<" elements from "<<nextFileName<<endl;
    }
  }//while
  
  //Free the resources
  gSystem->FreeDirectory(dirp);


  //Sort all Processes by cross section
  sort(mad_processes.begin(),mad_processes.end(), xs_comparator);


  // Split the processes into flavor composition
  vector<madProcess> mad_processes_light, mad_processes_charm, mad_processes_bottom;
  for (unsigned int d=0;d<mad_processes.size();d++){
    switch(mad_processes[d].getFlavor()){

    case 0:
      mad_processes_light.push_back(mad_processes[d]);
      break;

    case 1:
      mad_processes_charm.push_back(mad_processes[d]);
      break;

    case 2:
      mad_processes_bottom.push_back(mad_processes[d]);
      break;

    default:
      cout<<"ERROR IdentifySubProcesses process with flavor="<<mad_processes[d].getFlavor()<<endl;
      exit(0);
    }//switch
  }//for

  //Report all the diagrams
  // PrintOutDiagrams("All Diagrams",mad_processes);
  PrintOutDiagrams("Light  Diagrams",mad_processes_light);
  PrintOutDiagrams("Charm  Diagrams",mad_processes_charm);
  PrintOutDiagrams("Bottom Diagrams",mad_processes_bottom);


}//IdentifySubProcesses
