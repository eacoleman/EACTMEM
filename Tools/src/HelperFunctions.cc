#include "TAMUWW/Tools/interface/HelperFunctions.hh"

////////////////////////////////////////////////////////////////////////////////
//  Implement Helper Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
pair<double,double> HelperFunctions::getCrossSectionAndError(TString channelName)
{
  Table table;
  double xsec;
  double error;

  string basePath;
  char const* tmp = getenv("BASEPATH");
  if(tmp != NULL)
     basePath = string(tmp);
  else
     basePath = gSystem->pwd();
  basePath = basePath.substr(0,basePath.find("TAMUWW"));
  table.parseFromFile(basePath+string("/TAMUWW/ConfigFiles/Official/CrossSections_8TeV.txt"),"TableCellVal");
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

//______________________________________________________________________________
double HelperFunctions::getBranchingRatio(TString channelName)
{
  Table table;
  double xsec;

  string basePath;
  char const* tmp = getenv("BASEPATH");
  if(tmp != NULL)
     basePath = string(tmp);
  else
     basePath = gSystem->pwd();
  basePath = basePath.substr(0,basePath.find("TAMUWW"));
  table.parseFromFile(basePath+string("/TAMUWW/ConfigFiles/Official/BranchingRatios_8TeV.txt"),"TableCellVal");
  TableCell * cell = table.getCellRowColumn(string(channelName),"BranchingRatio");
  if(cell){
    xsec = ((TableCellVal*)cell)->val.value;
    if (xsec==0)
      cout << "WARNING::getBranchingRatio::The branching ratio for " << channelName << " is 0.0 +/- 0.0" << endl;
    return xsec;
  } else{
    cout << "WARNING::getBranchingRatio::channelName " << channelName 
	 << " not recognized. Returning -1 for the branching ratio." << endl 
	 << "The events will have the same scale as the MC sample, but on a negative scale." << endl 
	 << "Please check channel names." << endl;
    return -1.;
  }
}//getBranchingRatio

//______________________________________________________________________________
double HelperFunctions::getNumMCEvts(TString channelName)
{
  Table table;
  double value;

  string basePath;
  char const* tmp = getenv("BASEPATH");
  if(tmp != NULL)
     basePath = string(tmp);
  else
    basePath = gSystem->pwd();
  basePath = basePath.substr(0,basePath.find("TAMUWW"));
  table.parseFromFile(basePath+string("/TAMUWW/ConfigFiles/Official/EventsFromMC_commonPATTuple_532.txt"),
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

//______________________________________________________________________________
int HelperFunctions::vfind(vector<string> a, string b) {
   for (unsigned int i=0; i<a.size(); i++) {
      if (TString(a[i]).CompareTo(TString(b))==0)
         return i;
   }
   return -1;
}
