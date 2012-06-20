/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////
//// A Tool For Working with a single set of CRAB Jobs
//// Requires that directories logs, logs/detailed and cfgs exist
//// initialize the proxy: voms-proxy-init -voms cms
//// replace _test with _MC or _Data for actual runs.


///temporary fix
#include </uscms/home/ilyao/MATRIXELEMENT/CMSSW_4_2_8/src/TAMUWW/Tools/ManipulateCRABOutput.cc>

//#include <stdlib> 
#include <iostream>
#include <iomanip>
#include <strstream>
//#include <sstream>
#include <fstream>
//#include <ofstream>
#include <vector>
#include <map>
#include <string>

// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>

using namespace std;

#include <TString.h>
#include <TChain.h>


#define MAXJOBS 20000

const bool isData=false; //use when setting .cfg file parameters.
const bool isLocalDataset=true; //use to set .cfg file parameters when running on a locally stored dataset.
const TString dbs_url_name = "http://cmsdbsprod.cern.ch/cms_dbs_ph_analysis_02/servlet/DBSServlet";//Use when working with a local dataset.
//const TString dbs_url_name = "http://cmsdbsprod.cern.ch/cms_dbs_ph_analysis_01/servlet/DBSServlet";//Use when working with a local dataset.
const bool useBackupLogs=false; //a feature used to restore logs, if they were damaged or lost (set to true and run CreateJobs, StartJobs and UpdateJobStatus).

//NTuple Generation
//const TString designation="MC"; //Set to MC or Data for actual runs
//const TString designation="Data";
//const TString designation="MCEWKFall11_mu";
//const TString designation="MCEWKFall11_el";
//const TString designation="MCEWKPATSpring12_mu";
//const TString designation="MCEWKPATSpring12_el";
const TString designation="FromPAT428";

///PAT Tuple Generation
//const TString designation="MCVBFPATGen";

/// Set to true for PATTuple generation
const bool useCfguser_remote_dir = false;// true = take the user_remote_dir from the TemplateCfg or false = construct it dynamically (will be pnfsstoragedir/designation/setName_VX_RepY_PtZ, should not be present in the TemplateCfg)

void ManipulateSingleCRABJobSet () {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////    Helper Functions  ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool getIntLine(const char* inLogFileName, int rep, int pt, TString charstr, int& parameter, bool replace) {
//// Finds a line of the form charstr = parameter and reads it's value
//// replace=true to replace the parameter, =false to read the parameter.
  char logline[19000];
  char I_char[5];
  TString label, token, Command;
  ifstream inLogFile(inLogFileName);
  ofstream outLogFile;
  outLogFile.open("logs/templog.log",ios::out);
  TString in_rep, in_pt, l_rep, l_pt;
  int par;
  bool found=false;

  sprintf(I_char,"%i",rep);
  in_rep=I_char;
  in_rep = "Rep"+in_rep;
  sprintf(I_char,"%i",pt);
  in_pt=I_char;
  in_pt="Pt"+in_pt;
  //cout << "in_rep=" << in_rep << " in_pt=" << in_pt << endl;
  while ( inLogFile.good() ) {
    inLogFile.getline(logline,19000);
    istrstream str(logline);
    str >> l_rep >> l_pt >> label >> token >> par;
    //cout << "l_rep=" << l_rep << " l_pt=" << l_pt << " label=" << label << endl;
    if ( (label==charstr)&&(l_rep==in_rep)&&(l_pt==in_pt) ) {
      //cout << "found : " << label << " par=" << par << endl;
      found=true;
      if ( !replace ) {
	parameter=par;
	outLogFile << logline << endl;
      } else {
	//cout << "Replacing:" << endl;
	outLogFile << label << " " << token << " " << parameter << endl;
      }
    } else {
      //cout << "par=" << par << "  logline=" << logline << endl;
      outLogFile << logline << endl;
    }
  }
  Command = inLogFileName;
  Command = "rm " + Command;
  system(Command);
  Command = inLogFileName;
  Command = "mv logs/templog.log " + Command;
  system(Command);
  return found;
}

bool getStrLine(const char* inLogFileName, int rep, int pt, TString charstr, TString& parameter, bool replace) {
//// Finds a line of the form charstr = parameter and reads it's value
//// replace=true to replace the parameter, =false to read the parameter.
  char logline[19000];
  char I_char[5];
  TString label, token, temppar, Command;
  ifstream inLogFile(inLogFileName);
  ofstream outLogFile;
  outLogFile.open("logs/templog.log",ios::out);
  TString in_rep, in_pt, l_rep, l_pt;
  TString par;
  bool found=false;
  bool first=true;

  sprintf(I_char,"%i",rep);
  in_rep=I_char;
  in_rep="Rep"+in_rep;
  sprintf(I_char,"%i",pt);
  in_pt=I_char;
  in_pt="Pt"+in_pt;
  while ( inLogFile.good() ) {
    inLogFile.getline(logline,19000);
    istrstream str(logline);
    str >> l_rep >> l_pt >> label >> token >> temppar;
    //cout << "l_rep=" << l_rep << " l_pt=" << l_pt << " label=" << label << " token=" << token << " temppar=" << temppar << endl;
    par="";
    first=true;
    while ( temppar!="" ) {
      if ( !first ) {
	par=par+" ";
      } else {
	first=false;
      }
      par=par+temppar;
      str >> temppar;
    }
    if ( (label==charstr)&&(l_rep==in_rep)&&(l_pt==in_pt) ) {
      found=true;
      if ( !replace ) {
	parameter=par;
	outLogFile << logline << endl;
      } else {
// 	cout << "REPLACING with: " << endl;
// 	cout << l_rep << " " << l_pt << " " << label << " " << token << " " << parameter << " " << endl;
	outLogFile << l_rep << " " << l_pt << " " << label << " " << token << " " << parameter << " " << endl;
      }
    } else {
      outLogFile << logline << endl;
    }
  }
  Command = inLogFileName;
  Command = "rm " + Command;
  //cout << "Command=" << Command << endl;
  system(Command);
  Command = inLogFileName;
  Command = "mv logs/templog.log " + Command;
  //cout << "Command=" << Command << endl;
  system(Command);
  return found;
}

TString getOutputFile(const char* configFileName)
//// Extracts (&adjusts for input) the name for the .root output files
{
  char logline[2000];
  int len;
  TString label, token1, token2, filename, outFile;
  ifstream cfgFile(configFileName);
  
  while ( cfgFile.good() ) {
    cfgFile.getline(logline,2000);
    istrstream str(logline);
    str >> label >> token1 >> token2;
    if ( label=="output_file" ) {
      outFile=token2;
    }
  }

  ///filename will have an extra .root at the end and will be missing an _
  len = outFile.Length();
  //cout << "len=" << len << endl;
  filename="";
  for (Int_t i=0; i<(len-5) ;i++) {
    filename=filename+outFile[i];
  }
  filename=filename+"_";
  return filename;
}

void removeBlanks(const char* inLogFileName) {
//// removes all of the blank lines from the log file
  char logline[500];
  TString token, Command;
  ifstream inLogFile(inLogFileName);
  ofstream outLogFile;
  outLogFile.open("logs/templog.log",ios::out);
  while ( inLogFile.good() ) {
    inLogFile.getline(logline,500);
    istrstream str(logline);
    str >> token;
    if ( token!="" ) {
      outLogFile << logline << endl;
    }
  }
  Command = inLogFileName;
  Command = "rm " + Command;
  //cout << "Command=" << Command << endl;
  system(Command);
  Command = inLogFileName;
  Command = "mv logs/templog.log " + Command;
  //cout << "Command=" << Command << endl;
  system(Command);
}


void editJobString (bool newJobs[MAXJOBS], TString& jobString, bool add, int beg, int end)
/// add =true if we're adding the newJobs and =false if we're removing them
/// jobString is of the form j1 , j2 , j3 , ... , jn
{
  bool currentJobs[MAXJOBS];
  bool endloop=false;
  int jobN, ri;
  char tempCnt[3];
  TString token;
  for (Int_t i=beg; i<(end+1);i++) {
    currentJobs[i]=false;
  }

  istrstream str(jobString);
  if ( jobString=="" ) {
    endloop=true;
  }
  while( !endloop ) {
    str >> jobN >> token;
    //cout << "jobN=" << jobN << "  token=" << token << endl;
    currentJobs[jobN]=true;
    if ( token!="," ) { endloop=true; }
  }
  
  jobString="";
  token="";
  for (Int_t i=beg; i<(end+1);i++) {
    ri=end-(i-beg);
    if ( add&&(currentJobs[ri]||newJobs[ri]) ){
      if ( jobString != "" ) {
	jobString= " , " + jobString;
      }
      sprintf(tempCnt,"%i",ri);
      jobString=tempCnt+jobString;
      //cout << "ri=" << ri << " : jobString=" << jobString << endl;
    }
    if ( (!add)&&currentJobs[ri]&&(!newJobs[ri]) ) {
      if ( jobString != "" ) {
	jobString= " , " + jobString;
      }
      sprintf(tempCnt,"%i",ri);
      jobString=tempCnt+jobString;
      //cout << "ri=" << ri << " : jobString=" << jobString << endl;
    }
  }
}

void addLogEntry (const char* _setName, TString baseDir, const char* inLogFileName, bool firstJobCreation, int rep, int pt, int StartJob, int EndJob) 
/// Puts an additional entry at the end of the log file logFileName
{
  char logline[19000];
  ifstream inLogFile(inLogFileName);
  ofstream outLog;
  outLog.open("logs/temporarylog.log",ios::out);
  int NVersion;
  bool writeline;
  TString Command, fulldir, strUnfinished, strResubmit, strExtract, IsComplete, token1, token2, in_rep, in_pt;
  char I_char[5];
  sprintf(I_char,"%i",rep);
  in_rep=I_char;
  in_rep="Rep"+in_rep;
  sprintf(I_char,"%i",pt);
  in_pt=I_char;
  in_pt="Pt"+in_pt;
  //cout << "testing 123" << endl;
  //outLog << "testing 123" << endl;

  //get (or initialize) the values
  strResubmit="";
  strExtract="";
  IsComplete="False";
  if ( firstJobCreation ) {
    //cout << "firstJobCreation" << endl;
    NVersion=1;
    strUnfinished="All";
  } else {
    //cout << "NOT firstJobCreation" << endl;
    getIntLine(inLogFileName,rep,pt,"Version",NVersion,false);
    NVersion=NVersion+1;
    getStrLine(inLogFileName,rep,pt,"JobsUnfinished",strUnfinished,false);
    if ( strUnfinished=="" ) {
      IsComplete="True";
    }
    int startJ, endJ;
    getIntLine(inLogFileName,rep,pt,"StartJob",startJ,false);
    getIntLine(inLogFileName,rep,pt,"EndJob",endJ,false);
    if ( (startJ!=StartJob)||(endJ!=EndJob) ) {
      cerr << "ERROR  :  For Rep" << rep << "Pt=" << pt << " set Start/EndJob=" << StartJob << "/" << EndJob << ", while expected (from the log)=" << startJ << "/" << endJ << endl;
    }
  }
  //cout << "NVersion=" << NVersion << endl;


  sprintf(I_char,"%i",pt);
  fulldir=I_char;
  fulldir="_Pt"+fulldir;
  //  cout << "fulldir=" << fulldir << endl;
  sprintf(I_char,"%i",rep);
  fulldir=I_char+fulldir;
  fulldir="_Rep"+fulldir;
  sprintf(I_char,"%i",NVersion);
  fulldir=I_char+fulldir;
  fulldir="_V"+fulldir;
  //  cout << "fulldir=" << fulldir << endl;
  fulldir=_setName+fulldir;
  fulldir="Unprocessed/"+fulldir;
//   getStrLine(inLogFileName,0,0,"BaseDir",baseDir,false);
//  baseDir="/uscms_data/d1/ilyao/Summer11/test/MC/";
  fulldir=baseDir+fulldir;
  //  cout << "fulldir=" << fulldir << endl;

  ///rewrite the nonempty lines:
  writeline=true;
  //cout << "1" << endl;
  while ( inLogFile.good() ) {
    //cout << "2" << endl;
    inLogFile.getline(logline,19000);
    istrstream str(logline);
    str >> token1 >> token2 ;
    if ( token1!="" ) {
      //cout << "3" << endl;
      if ( (token1==in_rep)&&(token2==in_pt)&&(!firstJobCreation) ) {
	///add the lines, provided they haven't been added already
	if ( writeline==true ) {
	  //cout << "Replacing the lines for " << in_rep << " " << in_pt << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " Version = " << NVersion << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " FullDir = " << fulldir << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " StartJob = " << StartJob << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " EndJob = " << EndJob << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " JobsUnfinished = " << strUnfinished << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " JobstoResubmit = " << strResubmit << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " JobstoExtract = " << strExtract << endl;
	  outLog << "Rep" << rep << " Pt" << pt << " IsComplete = " << IsComplete << endl;
	}
	writeline=false;
      } else {
	writeline=true;
      }
      if ( writeline ) { 
	//cout << "line=" << logline << endl;
	outLog << logline << endl;
      }
    }
  }

  //outLog << "testing 456" << endl;

  if ( firstJobCreation ) {
    ///add the new lines
    outLog << "-----------------------------------------------------------------------" << endl;
    outLog << "Rep" << rep << " Pt" << pt << " Version = " << NVersion << endl;
    outLog << "Rep" << rep << " Pt" << pt << " FullDir = " << fulldir << endl;
    outLog << "Rep" << rep << " Pt" << pt << " StartJob = " << StartJob << endl;
    outLog << "Rep" << rep << " Pt" << pt << " EndJob = " << EndJob << endl;
    outLog << "Rep" << rep << " Pt" << pt << " JobsUnfinished = " << strUnfinished << endl;
    outLog << "Rep" << rep << " Pt" << pt << " JobstoResubmit = " << strResubmit << endl;
    outLog << "Rep" << rep << " Pt" << pt << " JobstoExtract = " << strExtract << endl;
    outLog << "Rep" << rep << " Pt" << pt << " IsComplete = " << IsComplete << endl;
    outLog << "-----------------------------------------------------------------------" << endl;
  }

  ///replace the file
//   Command = "more logs/temporarylog.log";
//   cout << "Command=" << Command << endl;
//   system(Command);
  Command=inLogFileName;
  Command="logs/temporarylog.log "+Command;
  Command="mv "+Command;
  //cout << "Command=" << Command << endl;
  system(Command);

}

TString makeJobInputString(TString jobString)
///make an input string with spaces removed
{
  TString inputString, token, jobN;
  bool endloop=false;
  istrstream str(jobString);
  inputString="";
  if ( jobString=="" ) {
    endloop=true;
    inputString = "None";
  }
  while( !endloop ) {
    str >> jobN >> token;
    inputString=inputString+jobN;
    if ( token!="," ) { 
      endloop=true;
    } else {
      inputString=inputString+",";
    }
  }
  return inputString;
}


bool returnNJobs(const char* statusLogName, int& NJobs, bool isStatusLog) {
//// Reads the number of jobs from a statusLogFile
  bool found=false;
  char logline[19000];
  TString label, token1, token2, token3;
  ifstream inStatusLogFile(statusLogName);
  while ( inStatusLogFile.good() ) {
    inStatusLogFile.getline(logline,19000);
    istrstream str(logline);
    str >> label >> token1 >> token2 >> token3;
    if ( (label=="crab:")&&(token2=="Total")&&(token3=="Jobs")&&isStatusLog ) {
      NJobs = atoi(token1);
      found=true;
    }
    if ( (label=="crab:")&&(token1=="Total")&&(token2=="of")&&(!isStatusLog) ) {
      NJobs = atoi(token3);
      found=true;
    }
  }
  return found;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    Main Functions  ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CreateJobs (const char* _setName, int _nEvtsOrLumisPerJob, int _nEvtsOrJobsTotal, const char* _baseDir, const char* _dataset, const char* _lumimask, const char* _runselection, int _nReps, int _nJobsPerPart, const char* _pset, const char* _publishDataName, bool _createAtFNAL)
///Creates the config cfgs/_setName.cfg and a log in logs/_setName.log
///Each set of jobs can have X Repetitions (i.e. the number of times you launch this set of jobs), Y Parts and version Z (i.e. if your resubmission fails enough time you can recreate).
///Log lines are of the form RepX PtY Label = Value
///For MC/Data events_per_job/lumis_per_job = _nEvtsOrLumisPerJob, total_number_of_events/number_of_jobs = _nEvtsOrJobsTotal
{
  TString Command, tempStr, tempName, processeddir, logFileName, cfgFileName, workingdir, remotedir, remotedir_base, outputdir, joblog, token, runselection, pset, publishDataName;
  int NVersion, NJobsTot, NJobsProcessed, NParts, StartJob, EndJob;
  bool firstJobCreation=false;
  char line[500];
  char I_char[5];
  //cout << "1" << endl;
  tempName="Template.cfg";
//   tempName=".cfg";
//   tempName="FNAL"+tempName;
//   tempName="Template"+tempName;
  tempName=designation+tempName;
  tempName="crab_"+tempName;
  // cout << "2" << endl;
  ifstream inTemplateFile(tempName);
  //  cout << "TemplateName = " << tempName << endl;
  if ( !inTemplateFile.good() ) {
    cerr << "ERROR: Unable to find the template file" << endl;
    cout << "Exiting" << endl;
    exit(1); 
  }

  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  // cout << "3" << endl;
  ifstream inLogFile(logFileName);
  //cout << "3.1" << endl;
  ofstream outLogFile;
  //cout << "3.2" << endl;
  outLogFile.open("logs/tempLog.txt",ios::out);
  cfgFileName=".cfg";
  cfgFileName=designation+cfgFileName;
  cfgFileName="_"+cfgFileName;
  cfgFileName=_setName+cfgFileName;
  cfgFileName="cfgs/"+cfgFileName;
  //cout << "3.6" << endl;
  ofstream outCfgFile;
  outCfgFile.open(cfgFileName,ios::out);

  //cout << "4" << endl;
  if ( !inLogFile.good() ) {
    firstJobCreation=true;
    NVersion=1;
    NJobsProcessed=-1;
    //// Make the processed dir if it doesn't already exist (should be of the form _baseDir/Processed/_setName)
    processeddir="Processed/";
    processeddir=_baseDir+processeddir;
    Command=processeddir + " >& logs/tempErr.txt";
    Command="ls " + Command;
    //cout << "Command = " << Command << endl;
    system(Command);
    ifstream tempin1("logs/tempErr.txt");
    tempin1.getline(line,500);
    tempStr = line;
    if ( tempStr.Contains("No such file or directory") ) {
      Command="mkdir " + processeddir;
      //cout << "Command = " << Command << endl;
      system(Command);
    }

    processeddir="/";
    processeddir=_setName+processeddir;
    processeddir="Processed/"+processeddir;
    processeddir=_baseDir+processeddir;
    Command=processeddir + " >& logs/tempErr.txt";
    Command="ls " + Command;
    //cout << "Command = " << Command << endl;
    system(Command);
    ifstream tempin2("logs/tempErr.txt");
    tempin2.getline(line,500);
    tempStr = line;
    //    cout << "tempStr=" << tempStr << endl;
    if ( tempStr.Contains("No such file or directory") ) {
      Command="mkdir " + processeddir;
      //cout << "Command = " << Command << endl;
      system(Command);
    }
  } else {
    getIntLine(logFileName,1,1,"Version",NVersion,false);
    NVersion=NVersion+1;
    getIntLine(logFileName,0,0,"NJobsProcessed",NJobsProcessed,false);
    getStrLine(logFileName,0,0,"ProcessedDir",processeddir,false);
  }



  //// Make the config file
  inTemplateFile.getline(line,500);
  tempStr = line;
  bool writeline=true;//use to overwrite a line if creating at FNAL
  while ( (!tempStr.Contains("BEGINJOBSPECIFIC"))&&(inTemplateFile.good()) ) {
    istrstream str(line);
    str >> token;
    if (_createAtFNAL) {
      if ( token=="use_server" ) {
	outCfgFile << "## " << line << endl;
	writeline=false;
      }
      if ( token=="scheduler" ) {
	outCfgFile << "scheduler = condor" << endl;
	writeline=false;
      }
    }
    if ( writeline ) {
      outCfgFile << line << endl;
    }
    writeline=true;
    inTemplateFile.getline(line,500);
    tempStr = line;
  }

  sprintf(I_char,"%i",NVersion);
  workingdir=I_char;
  workingdir="_V"+workingdir;
  workingdir=_setName+workingdir;
  workingdir="Unprocessed/"+workingdir;
  workingdir=_baseDir+workingdir;
  remotedir_base=I_char;
  remotedir_base="_V"+remotedir_base;
  remotedir_base=_setName+remotedir_base;
  remotedir_base="/"+remotedir_base;
  remotedir_base=designation+remotedir_base;
  outCfgFile << "### Start Job Specific Settings ###" << endl;
  pset=_pset;
  if ( pset!="NOTJOBSPECIFIC" ) {
    outCfgFile << "pset = " << pset << endl;
  }
  if ( isData ) {
    outCfgFile << "lumis_per_job = " << _nEvtsOrLumisPerJob << endl;
    outCfgFile << "number_of_jobs = " << _nEvtsOrJobsTotal << endl;
    outCfgFile << "lumi_mask = " << _lumimask << endl;
    runselection=_runselection;
    if ( runselection!="0" ) {
      outCfgFile << "runselection = " << _runselection << endl;
    }
  } else {
    outCfgFile << "events_per_job = " << _nEvtsOrLumisPerJob << endl;
    outCfgFile << "total_number_of_events = " << _nEvtsOrJobsTotal << endl;
  }
  if ( isLocalDataset ) {
    outCfgFile << "dbs_url = " << dbs_url_name << endl;
  }
  outCfgFile << "datasetpath = " << _dataset << endl;
  outCfgFile << "[USER]" << endl;
  outCfgFile << "##ui_working_dir = " << workingdir << endl;
  if ( !useCfguser_remote_dir ) {
    outCfgFile << "##user_remote_dir = " << remotedir_base << endl;
  }
  publishDataName=_publishDataName;
  if ( publishDataName=="NONE" ) {
    outCfgFile << "publish_data = 0" << endl;
    outCfgFile << "publish_data_name = NONE" << endl;
  } else {
    outCfgFile << "publish_data = 1" << endl;
    outCfgFile << "publish_data_name = " << _publishDataName << endl;
  }
  outCfgFile << "### End Job Specific Settings ###" << endl;


  while ( (!tempStr.Contains("ENDJOBSPECIFIC"))&&(inTemplateFile.good())  ) {
    inTemplateFile.getline(line,500);
    tempStr = line;
  }

  while ( inTemplateFile.good() ) {
    inTemplateFile.getline(line,500);
    istrstream str(line);
    str >> token;
    if (_createAtFNAL) {
      if ( token=="se_black_list" ) {
	outCfgFile << "## " << line << endl;
	outCfgFile << "ce_white_list = cmssrm.fnal.gov" << endl;
	writeline=false;
      }
    }
    if ( writeline ) {
      outCfgFile << line << endl;
    }
    writeline=true;
  }


  ///Create the jobs for Rep1Pt1
  if ( !useBackupLogs ) {
    outputdir=workingdir+"_Rep1_Pt1";
    remotedir=remotedir_base+"_Rep1_Pt1";
    Command =".cfg >& logs/jobLog.txt";
    Command=designation+Command;
    Command="_"+Command;
    Command =_setName+Command;
    Command=" -create -cfg cfgs/"+Command;
    Command=outputdir+Command;
    Command = " -USER.ui_working_dir "+Command;
    if ( !useCfguser_remote_dir ) {
      Command=remotedir+Command;
      Command = "-USER.user_remote_dir "+Command;
    }
    Command = "crab "+Command;
    cout << "Command = " << Command << endl;
    system(Command);
    Command = "_Rep1_Pt1.txt";
    Command = _setName+Command;
    Command = "cp logs/jobLog.txt logs/detailed/creationLog_"+Command;
    system(Command);
  }
  //// Get the number of jobs
  if ( !useBackupLogs ) {
    joblog = "logs/jobLog.txt";
  } else {
    joblog = "_Rep1_Pt1.txt";
    joblog = _setName + joblog;
    joblog = "logs/detailed/creationLog_" + joblog;
  }
  returnNJobs(joblog,NJobsTot,false);
  double njt(NJobsTot);
  double njpp(_nJobsPerPart);
  NParts=ceil(njt/njpp);
  cout<< "NJobsTot=" << NJobsTot << endl;
  //// Make the log file
  if ( firstJobCreation ) {
    cout << "Creating a new log file for process : " << _setName << endl;
    ////make the header
    outLogFile << "Rep0 Pt0 " << "Name = " << _setName << endl;
    outLogFile << "Rep0 Pt0 " << "BaseDir = " << _baseDir << endl;
    outLogFile << "Rep0 Pt0 " << "ProcessedDir = " << processeddir << endl;
    outLogFile << "Rep0 Pt0 " << "EvtsPerJob = " << _nEvtsOrLumisPerJob << endl;
    outLogFile << "Rep0 Pt0 " << "EvtsTot = " << _nEvtsOrJobsTotal << endl;
    outLogFile << "Rep0 Pt0 " << "NJobsTot = " << NJobsTot << endl;
    outLogFile << "Rep0 Pt0 " << "NJobsProcessed = " << NJobsProcessed << endl;
    outLogFile << "Rep0 Pt0 " << "NParts = " << NParts << endl;
    outLogFile << "Rep0 Pt0 " << "NRepetitions = " << _nReps << endl;
    outLogFile << "-----------------------------------------------------------------------" << endl;
    Command=logFileName;
    Command="logs/tempLog.txt "+Command;
    Command="mv "+Command;
    system(Command);
  }

  ///add the log entry for each part
  for (Int_t rep=1; rep<(_nReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      StartJob=(pt-1)*_nJobsPerPart+1;
      EndJob=pt*_nJobsPerPart;
      ///make sure that the jobs on the last part don't exceed the total number of jobs
      if ( EndJob>NJobsTot ) { EndJob=NJobsTot; }
      cout << "rep=" << rep << ", pt=" << pt << endl;
      addLogEntry(_setName,_baseDir,logFileName,firstJobCreation,rep,pt,StartJob,EndJob);
      //cout << "added a log entry" << endl;
      ///Create the jobs the remaining parts and repetitions
      if ( !useBackupLogs ) {
	if ( (rep!=1)||(pt!=1) ) {
	  sprintf(I_char,"%i",pt);
	  outputdir=I_char;
	  outputdir="_Pt"+outputdir;
	  sprintf(I_char,"%i",rep);
	  outputdir=I_char+outputdir;
	  outputdir="_Rep"+outputdir;
	  remotedir=remotedir_base+outputdir;
	  outputdir=workingdir+outputdir;
	  Command =".cfg >& logs/jobLog.txt";
	  Command =designation+Command;
	  Command ="_"+Command;
	  Command =_setName+Command;
	  Command=" -create -cfg cfgs/"+Command;
	  Command=outputdir+Command;
	  Command = " -USER.ui_working_dir "+Command;
	  if ( !useCfguser_remote_dir ) {
	    Command=remotedir+Command;
	    Command = "-USER.user_remote_dir "+Command;
	  }
	  Command = "crab "+Command;
	  cout << "Command = " << Command << endl;
	  system(Command);
	  Command = ".txt";
	  sprintf(I_char,"%i",pt);
	  Command = I_char+Command;
	  Command="_Pt"+Command;
	  sprintf(I_char,"%i",rep);
	  Command = I_char+Command;
	  Command="_Rep"+Command;
	  Command = _setName+Command;
	  Command = "cp logs/jobLog.txt logs/detailed/creationLog_"+Command;
	  cout << "Command = " << Command << endl;
	  system(Command);
	}
      }
    }
  }

}

void StartJobs (const char* _setName)
///Submits the crab jobs & updates the log file
{
  bool newJobs[MAXJOBS];
  char I_char[5];
  int StartJob, EndJob;
  TString Command, FullDir, jobList, strUnfinished, tempstr, logFileName;
  int NReps,NParts,nJob;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;

  ///Determine which jobs need to be submitted
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      cout << "rep=" << rep << " pt=" << pt << endl;
      getIntLine(logFileName,rep,pt,"StartJob",StartJob,false);
      getIntLine(logFileName,rep,pt,"EndJob",EndJob,false);
      getStrLine(logFileName,rep,pt,"JobsUnfinished",strUnfinished,false);
      //cout << "log file strUnfinished=" << strUnfinished << endl;
      if ( strUnfinished=="All" ) {
	strUnfinished="";
	for (Int_t i=StartJob; i<(EndJob+1);i++) {
	  newJobs[i]=true;
	}
	editJobString(newJobs,strUnfinished,true,StartJob,EndJob);
	//cout << "Edited strUnfinished=" << strUnfinished << endl;
	getStrLine(logFileName,rep,pt,"JobsUnfinished",strUnfinished,true);
	jobList=makeJobInputString(strUnfinished);
	//	jobList="";
      } else {
	jobList=makeJobInputString(strUnfinished);
      }
      if ( jobList!="None" ) {
	tempstr="";
	getStrLine(logFileName,rep,pt,"JobstoResubmit",tempstr,true);
	getStrLine(logFileName,rep,pt,"JobstoExtract",tempstr,true);
	//If there's only one job on the jobList, add an extra one after it - necessary due to CRAB interpreting the job number as the number of jobs to be submitted (e.g. crab -submit 1001 ... would submit first 1001 jobs).
	if ( !jobList.Contains(",") ) {
	  nJob=atoi(jobList);
	  nJob++;
	  sprintf(I_char,"%i",nJob);
	  jobList=jobList + "," + I_char;
	}
	//Submit the jobs
	if ( !useBackupLogs ) {
	  Command = " >& logs/jobLog.txt";
	  getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
	  Command = FullDir + Command;
	  Command = " -c " + Command;
	  Command = jobList + Command;
	  Command = "crab -submit " + Command;
	  cout << "Command = " << Command << endl;
	  system(Command);
	  Command = ".txt";
	  sprintf(I_char,"%i",pt);
	  Command = I_char+Command;
	  Command="_Pt"+Command;
	  sprintf(I_char,"%i",rep);
	  Command = I_char+Command;
	  Command="_Rep"+Command;
	  Command = _setName+Command;
	  Command = "cp logs/jobLog.txt logs/detailed/submissionLog_"+Command;
	  //	cout << "Command = " << Command << endl;
	  system(Command);
	}
      }
    }
  }

}

void UpdateJobStatus(const char* _setName, bool printJobStatus=true)
///Checks the Status and records/updates it in log/processName_log.log
{
  cout << "UPDATING JOB STATUS" << endl;
  bool jobResubmit[MAXJOBS];
  bool jobCompleted[MAXJOBS];
  bool jobCompletedNotExtracted[MAXJOBS];
  char logline[1000];
  char I_char[5];
  TString strID, strEND, strSTATUS, strACTION, strExeExitCode, strJobExitCode;
  TString Command, FullDir, strUnfinished, strResubmit, strExtract, strCheckJobs, strE_HOST, logFileName;
  int intJobExitCode, NParts, NReps, StartJob, EndJob, NJobsTot;
  int iterator;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;

  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  getIntLine(logFileName,0,0,"NJobsTot",NJobsTot,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getIntLine(logFileName,rep,pt,"StartJob",StartJob,false);
      getIntLine(logFileName,rep,pt,"EndJob",EndJob,false);
      sprintf(I_char,"%i",EndJob);
      strCheckJobs=I_char;
      strCheckJobs="-"+strCheckJobs;
      sprintf(I_char,"%i",StartJob);
      strCheckJobs=I_char+strCheckJobs;
      Command =" >& logs/jobLog.txt";
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      Command = FullDir + Command;
      Command = " -c " + Command;
      Command = strCheckJobs + Command;
      Command = "crab -status " + Command;
      cout << "Command = " << Command << endl;
      system(Command);
      //save the status log
      Command = ".txt";
      sprintf(I_char,"%i",pt);
      Command = I_char+Command;
      Command="_Pt"+Command;
      sprintf(I_char,"%i",rep);
      Command = I_char+Command;
      Command="_Rep"+Command;
      Command = _setName+Command;
      Command = "cp logs/jobLog.txt logs/detailed/statusLog_"+Command;
      cout << "Command = " << Command << endl;
      system(Command);

      ///Record which jobs may need to be extracted (exit code 0) and which may need to be resubmitted (done with a different exit code)
      ifstream inJobLogFile("logs/jobLog.txt");
      while ( inJobLogFile.good() ) {
	inJobLogFile.getline(logline,1000);
	istrstream str(logline);
	str >> strID >> strEND >> strSTATUS >> strACTION >> strExeExitCode >> strJobExitCode;
	if ( (strID=="ID")&&(strEND=="END")&&(strSTATUS=="STATUS")&&(strACTION=="ACTION")&&(strExeExitCode=="ExeExitCode")&&(strJobExitCode=="JobExitCode") ) {
	  ///Begin parsing the relevant portion of the output
	  inJobLogFile.getline(logline,1000);//skip the first line: "----..."
	  iterator=0;
	  while ( iterator < NJobsTot ) {
	    iterator++;
	    //cout << "iterator=" << iterator << endl;
	    inJobLogFile.getline(logline,1000);
	    istrstream str2(logline);
	    str2 >> strID >> strEND >> strSTATUS >> strACTION >> strExeExitCode >> intJobExitCode >> strE_HOST;
	    //cout << " intJobExitCode = " << intJobExitCode << endl;
	    if ( strID=="--------------------------------------------------------------------------------" ) {
	      /// skip this line
	      iterator=iterator-1;
	    } else {
	      jobResubmit[iterator]=false;
	      jobCompleted[iterator]=false;
	      jobCompletedNotExtracted[iterator]=false;
	      //cout << "strSTATUS=" << strSTATUS << " , StartJob=" << StartJob << " , EndJob=" << EndJob << endl;
	      if ( ((strSTATUS=="Done")||(strSTATUS=="Cleared")||(strSTATUS=="Retrieved"))&&
		   (iterator>=StartJob)&&
		   (iterator<=EndJob)
		   ) {
		//cout << "Updating the log file" << endl;
		if ( intJobExitCode==0 ) {
		  //cout << "mark the job as completed" << endl;
		  jobCompleted[iterator]=true;
		  if ( strSTATUS!="Retrieved" ) {
		    jobCompletedNotExtracted[iterator]=true;
		  }
		} else {
		  //cout << "strID=" << strID << " strJobExitCode=" << strJobExitCode << endl;
		  if ( strE_HOST.Length()!=0 ) {
		    //cout << "mark the job for resubmission" << endl;
		  jobResubmit[iterator]=true;
		  }
		}
	      }
	    }
	  } 
	}
      }
      ///Compare with the log file and update
      getStrLine(logFileName,rep,pt,"JobsUnfinished",strUnfinished,false);
      getStrLine(logFileName,rep,pt,"JobstoResubmit",strResubmit,false);
      getStrLine(logFileName,rep,pt,"JobstoExtract",strExtract,false);
      getIntLine(logFileName,rep,pt,"StartJob",StartJob,false);
      getIntLine(logFileName,rep,pt,"EndJob",EndJob,false);
      //cout << "unedited strUnfinished = " << strUnfinished << endl;
      editJobString(jobCompleted,strUnfinished,false,StartJob,EndJob);//remove the completed jobs
      //cout << "edited strUnfinished = " << strUnfinished << endl;
      editJobString(jobCompletedNotExtracted,strExtract,true,StartJob,EndJob);//add the unextracted jobs
      editJobString(jobResubmit,strResubmit,true,StartJob,EndJob);//add the jobs to resubmit
      getStrLine(logFileName,rep,pt,"JobsUnfinished",strUnfinished,true);
      getStrLine(logFileName,rep,pt,"JobstoResubmit",strResubmit,true);
      getStrLine(logFileName,rep,pt,"JobstoExtract",strExtract,true);
      if ( printJobStatus ) {
	cout << "Jobs Which Haven't Been Extracted (e.g. Starting, Running, etc.): " << strUnfinished << endl;
	cout << "Jobs To Resubmit: " << strResubmit << endl;
	cout << "Jobs To Extract: " << strExtract << endl;
      }
    }
  }

}


void PreResubmissionCleanup(const char* _setName)
/// Compensates for bugs in CRAB
/// Removes FullDir + "/res/" + CMSSW_N.stderr, CMSSW_N.stdout, crab_fjr_N.xml; where N=each job to resubmit
/// Can be used after resubmission (but before extraction/reextraction)
{
  TString tempstr, jobN, Command, FullDir, resDir, strResubmit, jobList, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps;
  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      resDir="/res/";
      resDir=FullDir+resDir;
      getStrLine(logFileName,rep,pt,"JobstoResubmit",strResubmit,false);
      jobList = makeJobInputString(strResubmit);
      //cout << "jobList=" << jobList << endl;
      cout << "Cleaning up Following Jobs: " << jobList << endl;
      if ( jobList!="None" ) {
	istrstream jobstr(strResubmit);
	tempstr=",";
	while ( tempstr=="," ) {
	  jobstr >> jobN >> tempstr;
	  Command = ".*";
	  Command = jobN+Command;
	  Command = "*_"+Command;
	  Command = resDir+Command;
	  Command = "rm "+Command;
	  //cout << "Command = " << Command << endl;
	  system(Command);
	}
      }
    }
  }

}



void ResubmitJobs(const char* _setName, bool resubmitAllUnfinished, bool doforceResubmit=false)
/// Resubmits the jobs already scheduled as in need of resubmisssion or all unfinished jobs
{
  cout << "RESUBMITTING JOBS" << endl;
  TString tempstr, Command, FullDir, strResubmit, jobList, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps;
  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      if ( resubmitAllUnfinished ) {
	getStrLine(logFileName,rep,pt,"JobsUnfinished",strResubmit,false);
      } else {
	getStrLine(logFileName,rep,pt,"JobstoResubmit",strResubmit,false);
      }
      jobList = makeJobInputString(strResubmit);
      //cout << "jobList=" << jobList << endl;
      if ( jobList!="None" ) {
	tempstr = FullDir;
	tempstr = " -c " + tempstr;
	tempstr = jobList + tempstr;
	//To avoid issues with CRAB, first get the output for the jobs you're about to resubmit
	Command = "crab -getoutput " + tempstr;
	cout << "Command = " << Command << endl;
	system(Command);
	if ( doforceResubmit ) {
	  Command = "crab -forceResubmit " + tempstr;
	} else {
	  Command = "crab -resubmit " + tempstr;
	}
	cout << "Command = " << Command << endl;
	system(Command);
	strResubmit="";
	getStrLine(logFileName,rep,pt,"JobstoResubmit",strResubmit,true);
      }
    }
  }

}

void ExtractJobs (const char* _setName, bool extractEverything)
//// Extracts the information of the jobs (.root files are still kept at the FNAL dcache directory) to a /res directory
//// The JobStatus should be updated first
{
  cout << "EXTRACTING JOBS" << endl;
  TString Command, FullDir, strExtract, jobList, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps;

  ///Get the .root files
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      getStrLine(logFileName,rep,pt,"JobstoExtract",strExtract,false);
      if ( !extractEverything ) {
	jobList = makeJobInputString(strExtract);
      } else {
	jobList="all";
      }
      if ( jobList!="None" ) {
	Command = FullDir;
	Command=" -c " + Command;
	Command=jobList + Command;
	Command = "crab -getoutput " + Command;
	cout << "Command = " << Command << endl;
	system(Command);
	strExtract="";
	getStrLine(logFileName,rep,pt,"JobstoExtract",strExtract,true);
      }
    }
  }
}

void PublishJobs(const char* _setName)
/// Publishes all completed jobs
{
  cout << "PUBLISHING ALL COMPLETED JOBS" << endl;
  bool Overwrite_dbs_url_for_publication=false;
  TString Alternate_dbs_url_for_publication="\"https://cmsdbsprod.cern.ch:8443/cms_dbs_ph_analysis_01_writer/servlet/DBSServlet\"";

  TString Command, FullDir, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps;
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      Command = " -c " + FullDir;
      if ( Overwrite_dbs_url_for_publication ) {
	Command = Alternate_dbs_url_for_publication + Command;
	Command = " -USER.dbs_url_for_publication=" + Command;
      }
      Command = "crab -publish" + Command;
      cout << "Command = " << Command << endl;
      system(Command);
    }
  }
}


void CopyJobs(const char* _setName)
/// Copies the results to the /res directory
{
  cout << "COPYING ALL EXTRACTED JOBS" << endl;
  TString Command, FullDir, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps;
  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      Command = "crab -copyData all -c " + FullDir;
      cout << "Command = " << Command << endl;
      system(Command);
    }
  }
}

//////////////////////////////////
/////// Generic Copy Function
void DCCPDirToDir_All(const char* dirIn, const char* dirOut)
/// Copies (via dccp) all files from dirIn to dirOut
{
  TString Command, remotename;
  char cpline[500];
  Command=" >& dccpls.txt";
  Command=dirIn+Command;
  Command = "ls "+Command;
  cout << "Command = " << Command << endl;
  system(Command);
  // Copy - has to be done line by line:
  ifstream inCopyFile("dccpls.txt");
  inCopyFile.getline(cpline,500);
  istrstream strinit(cpline);
  strinit >> remotename;
  while ( inCopyFile.good() ) {
    remotename="/"+remotename;
    remotename=dirIn+remotename;
    Command=dirOut;
    Command = " "+Command;
    Command = remotename+Command;
    Command = "/opt/d-cache/dcap/bin/dccp "+Command;
    cout << "Command = " << Command << endl;
    system(Command);
    inCopyFile.getline(cpline,500);
    istrstream str(cpline);
    str >> remotename;
  }
}
/////////////////////////////////


void DirectCopy(const char* _setName)
/// Copies the results to the /res directory
{
  cout << "DIRECTLY COPYING ALL JOBS" << endl;
  TString Command, FullDir, remotedir, remotedir_base, remotename, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps, NVersion;
  char I_char[5];

  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  remotedir_base="_V";
  remotedir_base=_setName+remotedir_base;
  remotedir_base="/"+remotedir_base;
  remotedir_base=designation+remotedir_base;
  remotedir_base="/pnfs/cms/WAX/11/store/user/ilyao/"+remotedir_base;

  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      //Construct the remote directory: remotedir
      sprintf(I_char,"%i",pt);
      remotedir=I_char;
      remotedir="_Pt"+remotedir;
      sprintf(I_char,"%i",rep);
      remotedir=I_char+remotedir;
      remotedir="_Rep"+remotedir;
      getIntLine(logFileName,rep,pt,"Version",NVersion,false);
      sprintf(I_char,"%i",NVersion);
      remotedir=I_char+remotedir;
      remotedir=remotedir_base+remotedir;
      //Get the name of the directory to copy files to
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      FullDir=FullDir+"/res/";
      //DirectCopy:
      DCCPDirToDir_All(remotedir,FullDir);
    }
  }
}


void DirectRemove(const char* _setName)
/// Directly (i.e. without relying on CRAB) removes the files in the user_remote_dir
{
  cout << "DIRECTLY ERASING ALL JOBS" << endl;
  TString Command, logFileName, FullDir, remotedir, remotedir_base;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps, NVersion;
  char I_char[5];

  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  remotedir_base="_V"+remotedir_base;
  remotedir_base=_setName+remotedir_base;
  remotedir_base="/"+remotedir_base;
  remotedir_base=designation+remotedir_base;
  remotedir_base="/pnfs/cms/WAX/11/store/user/ilyao/"+remotedir_base;

  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      //List the files in a remote directory
      sprintf(I_char,"%i",pt);
      remotedir=I_char;
      remotedir="_Pt"+remotedir;
      sprintf(I_char,"%i",rep);
      remotedir=I_char+remotedir;
      remotedir="_Rep"+remotedir;
      getIntLine(logFileName,rep,pt,"Version",NVersion,false);
      sprintf(I_char,"%i",NVersion);
      remotedir=I_char+remotedir;
      remotedir=remotedir_base+remotedir;
      Command=remotedir+"/*";
      Command="rm "+Command;
      cout << "Command = " << Command << endl;
      system(Command);
    }
  }
}

void MoveJobs(const char* _setName)
/// Moves the files from res to ProcessedDir
{
  cout << "MOVING JOBS" << endl;
  TString FullDir,  resDir, procDir, outFile, fileName, logFileName, cfgFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  cfgFileName=".cfg";
  cfgFileName=designation+cfgFileName;
  cfgFileName="_"+cfgFileName;
  cfgFileName=_setName+cfgFileName;
  cfgFileName="cfgs/"+cfgFileName;
  int NParts, NReps, StartJob, EndJob;

  fileName=getOutputFile(cfgFileName);
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      resDir="/res/";
      resDir=FullDir+resDir;
      getIntLine(logFileName,rep,pt,"StartJob",StartJob,false);
      getIntLine(logFileName,rep,pt,"EndJob",EndJob,false);
      getStrLine(logFileName,0,0,"ProcessedDir",procDir,false);
      MoveCRABOutput(resDir,procDir,fileName,StartJob,EndJob);
    }
  }

}

void CheckProcessedJobs(const char* _setName)
/// Checks the files in ProcessedDir and overwrites JobsUnfinished string in the log file
{
  cout << "CHECKING PROCESSED JOBS" << endl;
  TString FullDir, resDir, procDir, outFile, fileName, missingfiles, logFileName, cfgFileName, allMissingFiles;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  cfgFileName=".cfg";
  cfgFileName=designation+cfgFileName;
  cfgFileName="_"+cfgFileName;
  cfgFileName=_setName+cfgFileName;
  cfgFileName="cfgs/"+cfgFileName;
  int NParts, NReps, StartJob, EndJob;

  getStrLine(logFileName,0,0,"ProcessedDir",procDir,false);
  fileName=getOutputFile(cfgFileName);
  fileName=procDir+fileName;
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);

  for (Int_t rep=1; rep<(NReps+1);rep++) {
    allMissingFiles="";
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getIntLine(logFileName,rep,pt,"StartJob",StartJob,false);
      getIntLine(logFileName,rep,pt,"EndJob",EndJob,false);
      CheckCRABOutput(fileName,missingfiles,StartJob,EndJob,true);
      getStrLine(logFileName,rep,pt,"JobsUnfinished",missingfiles,true);
      allMissingFiles=allMissingFiles+",";
      allMissingFiles=allMissingFiles+missingfiles;
    }
  }
  cout << "The following files have not been processed: " << allMissingFiles << endl;
}

void RunStandardSequence(const char* _setName, bool resubmitAllUnfinished, bool withExtraction)
//// Updates the status, resubmits jobs and then has an option of following up by extracting, copying & moving the output.
{
  UpdateJobStatus(_setName);
  if ( withExtraction ) {
    ExtractJobs(_setName,false);
    //CopyJobs(_setName);
    DirectCopy(_setName);
    DirectRemove(_setName);
    MoveJobs(_setName);
    CheckProcessedJobs(_setName);
  }
  ResubmitJobs(_setName, resubmitAllUnfinished);
  TString logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  removeBlanks(logFileName);
}

void KillJobs(const char* _setName)
/// Kills all jobs for the set
{
  cout << "KILLING ALL JOBS" << endl;
  TString Command, FullDir, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;

  int NParts, NReps;
  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      Command = "crab -kill all -c " + FullDir;
      cout << "Command = " << Command << endl;
      system(Command);
    }
  }
}

void CleanUpJobs(const char* _setName, bool removeUnpDirs, bool removeLogs)
/// Cleans up all jobs for the set, and can also remove all of the directories (except Processed) and logs
{
  cout << "CLEANING UP JOBS" << endl;
  TString Command, FullDir, logFileName;
  logFileName=".log";
  logFileName=designation+logFileName;
  logFileName="_"+logFileName;
  logFileName=_setName+logFileName;
  logFileName="logs/"+logFileName;
  int NParts, NReps;
  ///Check the status & store it in the jobLog.txt file
  getIntLine(logFileName,0,0,"NParts",NParts,false);
  getIntLine(logFileName,0,0,"NRepetitions",NReps,false);
  for (Int_t rep=1; rep<(NReps+1);rep++) {
    for (Int_t pt=1; pt<(NParts+1);pt++) {
      getStrLine(logFileName,rep,pt,"FullDir",FullDir,false);
      Command = "crab -cleanCache -c " + FullDir;
      cout << "Command = " << Command << endl;
      system(Command);
      //remove the Storage dirs
      TString remotedir_base, remotedir;
      char I_char[5];
      int NVersion;
      remotedir_base="_V"+remotedir_base;
      remotedir_base=_setName+remotedir_base;
      remotedir_base="/"+remotedir_base;
      remotedir_base=designation+remotedir_base;
      remotedir_base="/pnfs/cms/WAX/11/store/user/ilyao/"+remotedir_base;
      sprintf(I_char,"%i",pt);
      remotedir=I_char;
      remotedir="_Pt"+remotedir;
      sprintf(I_char,"%i",rep);
      remotedir=I_char+remotedir;
      remotedir="_Rep"+remotedir;
      getIntLine(logFileName,rep,pt,"Version",NVersion,false);
      sprintf(I_char,"%i",NVersion);
      remotedir=I_char+remotedir;
      Command=remotedir_base+remotedir;
      Command="rm -rf "+Command;
      cout << "Command = " << Command << endl;
      system(Command);
      if ( removeUnpDirs ) {
	//remove the Unprocessed dirs
	Command = "rm -rf " + FullDir;
	cout << "Command = " << Command << endl;
	system(Command);
      }
    }
  }
  //delete the log file
  if ( removeLogs ) {
    Command="rm " + logFileName;
    cout << "Command = " << Command << endl;
    system(Command);
  }

}
