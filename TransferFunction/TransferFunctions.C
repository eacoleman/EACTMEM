/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Transfer functions for matrix element analysis
//// Created by Perloff, Alexx : aperloff@physics.tamu.edu
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Used to find the jet to genParton transfer functions necessary to run a matrix element analysis.
//// These functions will help map the to the energy of the most likely gen particle, given the
//// energy of a reco jet.

//User defined Ntuple and Table objects
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/FileLocationTable.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"

//Root includes
#include "TROOT.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TChain.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TF2.h"
#include "TLegend.h"
#include "TFile.h"
#include "TIterator.h"
#include "TObject.h"
#include "TDirectoryFile.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TString.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

//Root Math Core headers
#include <Math/SpecFuncMathCore.h>
#include <Math/PdfFuncMathCore.h>
#include <Math/ProbFuncMathCore.h>

//Standard library includes
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <utility>

using namespace std;

/*****************************************************************************/

///////////////////
///// Classes /////
///////////////////

/**
channel::
  This class keeps track of the different types of gen partons that the user wants to analyze the
  sample/jets for. The object keeps track of a name for the user, the pdgIds for the code/computer
  and has a number of counters that keep track of various statistics.

Parameters:
  _name: Each channel object must be declared with the name of the gen particle it represents.

Returns:
  N/A: This is a class.

Examples:
  From GetListOfChannels:
    channels.push_back(channel("B"));
    channels.back().pdgids.push_back(5);
*/

class channel {
public:

  channel(string _name, double _minEta, double _maxEta): name(_name),
                                                         numJets(0),
                                                         numJetsPart(0),
                                                         numJetsAntiPart(0),
                                                         numJetsFailDR(0),
                                                         minEta(_minEta),
                                                         maxEta(_maxEta){ }

  void setEtaCuts(double minEtaCut, double maxEtaCut);

  string name;
  vector<int> pdgids;
  //Counts the number of jets of a certain type
  int numJets;
  int numJetsPart;
  int numJetsAntiPart;
  int numJetsFailDR;
  //These are the min and max eta cuts. They are being set by channel so that they are not global,
  //but they can also be used throughout the program.
  double minEta;
  double maxEta;
};

void channel::setEtaCuts(double minEtaCut, double maxEtaCut)
{
  minEta = minEtaCut;
  maxEta = maxEtaCut;
}

/*****************************************************************************/

///////////////////////////
///// Global Variables/////
///////////////////////////

//Used to set the current values/event that is being analyzed from the ntuple
static TChain *chain;
static EventNtuple *ntuple;
//Keeps track of the total number of events in the ntuple.
//This cuts down on the time it takes to count the number of entries in a chain each time chain->GetEntries() is called 
int nEventNtuple;
//This is the deltaR cut being used to determine is a jet/gen parton match is probable
double maxDeltaR = 0.25;
//This keeps track of the current channel being analyzed
int currentChannelIndex = 0;
vector<channel> channels;
//The fit is used to map a E_j - E_p vs E_p fit to a E_j vs E_p fit
TF2* JEvsPEfit;

/*****************************************************************************/

///////////////////////////
///// Custom Functions/////
///////////////////////////

/*
GetListOfChannels::
  This function returns a vector that holds channel objects. This list holds all the different channels
  that the user wantes the code to run over. These channels each have a name, which described the type
  of gen particle they represent, and a list of pdgIds, which is how the code knows what type of gen
  particle the channel represents.

Parameters:
  N/A: There are no parameters to this function. Instead, this list of channels must be hard coded into
       this program by the user.

Returns:
  vector<channel>: This vector holds all of the channels, and the information for that channel, that the
                   program should loop through.

Examples:
  From TransferFunctions:
    channels = GetListOfChannels();
*/

vector<channel> GetListOfChannels()
{
  vector<channel> channels;

  //Channels with |Eta|<2.4
  //The UDS channel
  channels.push_back(channel("UDS_00_24",0,2.4));
  channels.back().pdgids.push_back(1);
  channels.back().pdgids.push_back(2);
  channels.back().pdgids.push_back(3);
  
  //The C channel
  channels.push_back(channel("C_00_24",0,2.4));
  channels.back().pdgids.push_back(4);

  //The B channel
  channels.push_back(channel("B_00_24",0,2.4));
  channels.back().pdgids.push_back(5);

  //The G channel
  channels.push_back(channel("G_00_24",0,2.4));
  channels.back().pdgids.push_back(21);

  //The Tau channel
  channels.push_back(channel("T_00_24",0,2.4));
  channels.back().pdgids.push_back(15);

  //Channels with 0<|Eta|<1.3
  //The UDS channel
  channels.push_back(channel("UDS_00_13",0,1.3));
  channels.back().pdgids.push_back(1);
  channels.back().pdgids.push_back(2);
  channels.back().pdgids.push_back(3);
  
  //The C channel
  channels.push_back(channel("C_00_13",0,1.3));
  channels.back().pdgids.push_back(4);

  //The B channel
  channels.push_back(channel("B_00_13",0,1.3));
  channels.back().pdgids.push_back(5);

  //The G channel
  channels.push_back(channel("G_00_13",0,1.3));
  channels.back().pdgids.push_back(21);

  //The Tau channel
  channels.push_back(channel("T_00_13",0,1.3));
  channels.back().pdgids.push_back(15);

  //Channels with 1.3<|Eta|<2.4
  //The UDS channel
  channels.push_back(channel("UDS_13_24",1.3,2.4));
  channels.back().pdgids.push_back(1);
  channels.back().pdgids.push_back(2);
  channels.back().pdgids.push_back(3);
  
  //The C channel
  channels.push_back(channel("C_13_24",1.3,2.4));
  channels.back().pdgids.push_back(4);

  //The B channel
  channels.push_back(channel("B_13_24",1.3,2.4));
  channels.back().pdgids.push_back(5);

  //The G channel
  channels.push_back(channel("G_13_24",1.3,2.4));
  channels.back().pdgids.push_back(21);

  //The Tau channel
  channels.push_back(channel("T_13_24",1.3,2.4));
  channels.back().pdgids.push_back(15);

  return channels;
}

/*****************************************************************************/

/**
concatString::
  Put two things together and make them a string. This function is templated and can concatenate
  any two streamable objects.

Parameters:
  obj1: This object is the first value in the return string.
  obj2: This object is printed imediately after obj1 in the return string.

Returns:
  string: The returned string is the concatenation of the two input objects into one string.

Examples:
  From HistogramsByFlavorAndEnergy:
    histoTitle = concatString(histoTitle,e+25) + "GeV";
  When the inputs are an int and a string:
    concatString(2," dogs"); //The output of this is the string "2 dogs"
*/

template <class T, class U>
std::string concatString(const T& obj1, const U& obj2)
{
   std::ostringstream output;
   output << obj1 << obj2;
   return output.str();
}

/*****************************************************************************/

/**
GetArrLength::
  This function will print out the size of an array, which unlike when using vectors is a non-trivial
  task. It is currently not necessary and it does not work as well as I would like.

Parameters:
  T: The array whose size you want found.

Returns:
  int: An integer which is the size of the array.

Examples:
  int arrSize = GetArrLength(arr);
*/

//template<class T, int size>
//int GetArrLength(T(&)[size]){return size;}

/*****************************************************************************/

/**
print_time::
  This function creates a string of the properly formated time that the function was called. The time
  used is the localtime of the computer running the code. This string can then be printed to the console
  to track when an event occured. This function can be easily modified to automatically print the time
  to the console by making this a void function and having a cout statement instead of a return line.

Parameters:
  N/A: There are no parameters to this function.

Returns:
  string: This function returns a properly formated time.

Examples:
  From TransferFunctions:
    cout << "End time: " << print_time() << endl;
*/

string print_time()
{
   time_t rawtime;
   struct tm *timeinfo;
   char buffer[30];
   //Get the current time and place it into the time_t container
   time (&rawtime);
   //Turn the current time into a local time format and save it in the struct tm
   timeinfo = localtime(&rawtime);
   //Turn the time into a string and place it into the buffer
   strftime(buffer,30,"%c",timeinfo);
   string mystring;
   //Turn the buffer of chars into a string
   mystring += buffer;
   return mystring;
}

/*****************************************************************************/

/**
filename_creator::
  The purpose of this function is to generate a filename and path for an output file which follows a specific
  patter, which is hard coded by the user. The reason this function exists, however, is to append the name
  of the file with the date and time when the file was created. Using this function, two files created at
  different times, who may otherwise have the same name, will be given unique names. In addition,
  the ability to add a changing file name, instead of a hardcoded pattern, may be added at a later time.
  Currently, the pattern is TF_<Process Name>_<Date>_<Time>.<type> or TF_<Process Name>.<type> is debug is
  set to false.

Parameters:
  process: This is a string, which will be added to the filename, which specifies what physics process
           the transfer functions are being made for. This parameter could be used to append some other
           information if necessary.

  type: This specifies the type of file for which this name will be used (ex: .txt or .root)
  debug: This specifies whether the user wants the date and time appended to the filename.

Returns:
  string: This function returns a properly formated filename and path.

Examples:
  From TransferFunctions:
    string outputFilename = filename_creator(process,".root");
*/

string filename_creator(string process,string type,bool debug)
{
  /*Sets up the filename of the .ROOT file where the graphs will be stored*/
  //Format the initial filename
  string mystring = "TF_" + process;
  //If the user wants the date and time appended to the filename
  if(debug)
    {
      time_t rawtime;
      struct tm *timeinfo;
      //Buffer to store the converted c_string
      char buffer[19];
      //Get the current time
      time (&rawtime);
      //Turn the current time into a local time format and save it in the struct
      timeinfo = localtime(&rawtime);
      //Set up the c_string command that will format the filename
      string filename_date = "%m_%d_%y_%H_%M_%S";
      //Format the date/time buffer
      strftime(buffer,19, filename_date.c_str(), timeinfo);
      //Add the date/time buffer to the filename
      mystring += "_";
      mystring += buffer;
    }
  //Add the formatted filename to the directory name
  mystring += type;
  //Return the path/filename of of the root file
  return mystring;
}

/*****************************************************************************/

/**
setStyle::
  This function was created so that the same style of histograms could be created no mater what order
  the functions in this program were run and no matter if root had been closed after setting the style
  once before.

Parameters:
  name: Tells the function whether or not the user wants the histogram name printed in the statistics
        box.

Returns:
  N/A: This function simply changes the root style for the current root session. There are no returned
       objects.

Examples:
  From TransferFunctions:
    setStyle();
*/

void setStyle(bool name)
{
  gStyle->SetFillColor(0);
  if(name) gStyle->SetOptStat("neMR");
  else gStyle->SetOptStat("eMR");
  gStyle->SetOptFit(2211);
  gStyle->SetPalette(1);
  gStyle->SetTitleColor(0,"c");
  gStyle->SetTitleFillColor(0);
}

/*****************************************************************************/

/**
makeChain::
  This function makes a chain (list of tree objects) that essentially form a list of events and
  each events constituent values (such as the jets in each event or the matchedpdgIDs, etc.). In
  order to access the events, the user must call chain->GetEntry(i), where i is an integer from 0
  to one less than the total number of entries in the chain. In this one, the use can access all
  of the events, or one specific one.

Parameters:
  inputFilepath: This is the path of the input root file that contains the dataset being used in
                 the analysis. There is a check in the TransferFunctions function that has already
                 checked to make sure the string is describing a .root file. However, this does
                 not guarantee that the file exists.

Returns:
  bool: The boolean indicates whether or not the chain was created successfully or not. If the chain
        was not created for any reason, then a message is also printed to the console. Although this
        is the only official output of the function, this function also sets the chain and ntuple
        global variables to point to a specific branch address.

Examples:
  From TransferFunctions:
    if(!makeChain(inputFilepath))
*/

bool makeChain(string inputFilepath)
{
  //Open the root file containing the dataset
  TFile *inputFile = new TFile(inputFilepath.c_str());
  //Check if the file was opened successfully
  if(inputFile->IsOpen())
    {
      //Pull the tree object EvtTree from the file and place it in the TChain* object
      chain = (TChain*)inputFile->Get("EvtTree");
      //Set up the object that will hold all of the information (define its structure)
      ntuple = new EventNtuple();
      //Set the branch address where the information is stored in the tree
      chain->SetBranchAddress("EvtNtuple", &ntuple);
      //Tell the user if the previous steps were performed successfully
      return true;
    }
  else
    {
      //If the file was not opened properly, then print a message and return a false indicator
      cout << "makeChain::File " << inputFilepath << " does not exist" << endl;
      return false;
    }
}

/*****************************************************************************/

/**
makeBreakRow::
  This function makes a table row which is mean to be a division between different values/rows in
  a table. The rows are constructed with a specific table in mind and will conform to the formating
  and size of that table.

Parameters:
  cellTitles: These are the titles of all of the table columns. These must be set in advance as the
              table structure is determined by the title of each cell. Without a title, these break
              rows would be set apart from the rest of the columns.
  titleWidth: This is the maximum width of the table column/cell. This is a user set parameter. Usually
              this value is no greater than the number of character in the largest cell. The default
              value for this parameter is 4.

Returns:
  TableRow*: The return value is a painter to a row that is properly formated for the specific table
             that it is being places into.

Examples:
  From makeNumJetsTable:
    tableRow = makeBreakRow(cellTitles,14);
*/

TableRow*  makeBreakRow(vector<string> &cellTitles, int titleWidth=4)
{
  //Sets the width of the widest cell in the table
  int cellWidth = 0;
  for(unsigned int i=0; i<cellTitles.size(); i++)
    {
      if(cellTitles[i].size()> (unsigned int)cellWidth) cellWidth = cellTitles[i].size();
    }
  //Sets the number of * to place in the row titles location
  string titleString;
  for(int i=0; i<titleWidth; i++) titleString+="*";
  //Sets the number of * to place in each cell of the row
  string cellString;
  for(int i=0; i<cellWidth; i++) cellString+="*";
  //Make the row and place the * strings
  TableRow* tableRow = new TableRow(titleString);
  for(unsigned int i=0; i<cellTitles.size(); i++)
    {
      TableCellText* tableCellText = new TableCellText(cellTitles[i]);
      tableCellText->text = cellString;
      tableRow->addCellEntry(tableCellText);
    }
  //Return the row
  return tableRow;
}

/*****************************************************************************/

/**
makeTable::
  This function is responsible for making the final output table for the 10 parameters that describe the
  transfer functions. The parameters are read in from the TMinuit* pointer and are then written to a 
  Table* object. These parameters are stored in this format so that they can be used as inputs for the
  transfer functions in the matrix element.

Parameters:
  gMinuit: This is the root object that performs the fitting algorithm and stored the fitted parameters.
           The object is used here to retrieve the fit parameters.

Returns:
  Table*: This function returns a pointer to a table object which stores the parameters of the 2D unbinned
          fit performed on the E_j - E_p vs E_p data.

Examples:
  From TransferFunctions:
    tables.push_back(makeTable(gMinuit));
*/

Table* makeTable(pair<vector<string>,pair<vector<double>,vector<double> > > parameters)
{
  Table* table = new Table("TF_Par_"+channels[currentChannelIndex].name);
  TableRow* tableRow;
  TableCellVal* tableCellVal;

  for(unsigned int i=0; i<parameters.first.size(); i++)
    {
      //cout << "Par" << i << " = " << parameters.second.first[i] << " +- " << parameters.second.second[i] << endl;
      //cout << "Name: " <<  parameters.first[i] << endl;
      tableRow = new TableRow(parameters.first[i]);
      tableCellVal = new TableCellVal("Parameters"); 
      tableCellVal->val.value = parameters.second.first[i];
      tableCellVal->val.error = parameters.second.second[i];
      tableRow->addCellEntry(tableCellVal);

      table->addRow((*tableRow));

      delete tableRow;
    }
  return table;
}

/*****************************************************************************/

/**
makeNumJetsTable::
  This function makes a table listing the number of jets used in this analysis for each flavor of
  gen parton. It also calculates the total number of jets used. In addition, the table then separates
  the number of jets further in to the number of particle vs. anti-particle gen partons. This information
  is useful for cross-checking that there really are the correct number of events being used from the
  selected sample. The user has the ability to have the number of jets which failed the deltaR cut be
  printed to the table. This portion of the code, however, must be un-commented.

Parameters:
  N/A: There are no parameters in this function.

Returns:
  Table*: This function returns a pointer to a table object which stores statistical information about
          the events being used to make the histograms and the fits.

Examples:
  From TransferFunctions:
    makeNumJetsTable()->Write();
*/

Table* makeNumJetsTable()
{
  Table* table = new Table("JetsPerChannel");
  TableRow* tableRow;
  TableCellText* tableCellText;
  //Counters to total the values in the columns
  int totalJets = 0;
  int totalPart = 0;
  int totalAntiPart = 0;
  stringstream out;
  //A hard-coded list of the column titles in the table
  vector<string> cellTitles; cellTitles.push_back("Number of Jets"); cellTitles.push_back("Particle Parton"); cellTitles.push_back("Anti-Particle Parton");

  for(unsigned int i=0; i<channels.size(); i++)
    {
      tableRow = new TableRow(channels[i].name);

      tableCellText = new TableCellText(cellTitles[0]);
      totalJets += channels[i].numJets;
      //Used in order to convert integers into strings
      out << channels[i].numJets;
      tableCellText->text = out.str();
      tableRow->addCellEntry(tableCellText);
      //Clears the stringstream
      out.str("");

      tableCellText = new TableCellText(cellTitles[1]);
      totalPart += channels[i].numJetsPart;
      out << channels[i].numJetsPart;
      tableCellText->text = out.str();
      tableRow->addCellEntry(tableCellText);
      out.str("");

      tableCellText = new TableCellText(cellTitles[2]);
      totalAntiPart += channels[i].numJetsAntiPart;
      out << channels[i].numJetsAntiPart;
      tableCellText->text = out.str();
      tableRow->addCellEntry(tableCellText);
      out.str("");

      table->addRow((*tableRow));
      delete tableRow;
    }
  //Uncomment to include information about the number of jets that fail the deltaR cut
  /*
  tableRow = new TableRow("Failed DeltaR Cut");
  tableCellText = new TableCellText("Number of Jets");
  totalJets += channels[0].numJetsFailDR
  out << channels[0].numJetsFailDR;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  out.str("");
  tableCellText = new TableCellText("Particle Parton");
  out << 0;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  tableCellText = new TableCellText("Anti-Particle Parton");
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  table->addRow((*tableRow));
  out.str("");
  delete tableRow;
  */
  tableRow = makeBreakRow(cellTitles,14);
  table->addRow((*tableRow));
  delete tableRow;

  //Lists the information about the totals in each column of the table
  tableRow = new TableRow("Total");
  tableCellText = new TableCellText(cellTitles[0]);
  out << totalJets;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  out.str("");
  tableCellText = new TableCellText(cellTitles[1]);
  out << totalPart;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  out.str("");
  tableCellText = new TableCellText(cellTitles[2]);
  out << totalAntiPart;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  table->addRow((*tableRow));
  delete tableRow;
 
 return table;
}

/*****************************************************************************/

/**
makeAllJetsTable::
  This function makes a table of the number of jets of each type (pdgId) available in the sample being
  used by this analysis. The list includes the particles and their anti-particles separately. The
  function also prints out the total number of jets available in the sample.

Parameters:
  N/A: There are no parameters in this function.

Returns:
  Table*: This function returns a pointer to a table object which stores statistical information about
          the events being used to make the histograms and the fits.

Examples:
  From TransferFunctions:
    makeAllJetsTable()->Write();
*/

Table* makeAllJetsTable()
{
  //Counts the number of jets of each flavor included in the sample
  map<int,int> allJetsMap;
  for (int i = 0; i<nEventNtuple; i++)
    { 
      chain->GetEntry(i);
      for(unsigned int j=0; j<ntuple->matchedpdgId.size(); j++)
        {
          allJetsMap[ntuple->matchedpdgId[j]]++;
        }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
    }//for (int i = 0; i<nEventNtuple; i++)

  Table* table = new Table("AllJetsBypdgId");
  TableRow* tableRow;
  TableCellText* tableCellText;
  int total = 0;
  stringstream out;
  vector<string> cellTitles(1,"Number of Jets");
  for(map<int,int>::iterator it = allJetsMap.begin() ; it != allJetsMap.end(); it++)
    {
      out << (*it).first;
      tableRow = new TableRow(out.str());
      out.str("");

      tableCellText = new TableCellText("Number of Jets");
      total += (*it).second;
      out << (*it).second;
      tableCellText->text = out.str();
      tableRow->addCellEntry(tableCellText);

      table->addRow((*tableRow));
      out.str("");
      delete tableRow;
    }

  tableRow = makeBreakRow(cellTitles,14);
  table->addRow((*tableRow));
  delete tableRow;

  tableRow = new TableRow("Total");
  tableCellText = new TableCellText("Number of Jets");
  out << total;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  
  table->addRow((*tableRow));
  delete tableRow;
  
  return table;
}

/*****************************************************************************/

/**
makeEventContentTable::
  This function makes a Tabel (a list) of the number of each flavor combination of jet events being used
  by this code. It for example, it cound the number of pairs of jets both with B gen partons or with U
  gen partons. It does this for all the different combinations in the sample, but it takes care of not
  printing permutations such as a UD and DU pair. Those are treated as the same type of event.

Parameters:
  deltaRCut: This boolean tells the function whether or not to create a table that counts the number
             of events without the deltaR cut or to make a table that counts the true number of
             events being used in the fit. deltaRCut=flase indicates that the number of events will
             be counted without using the cut. deltaRCut=true will brint out the events with the cut
             in place.

Returns:
  Table*: This function returns a pointer to a table object which stores statistical information about
          the events being used to make the histograms and the fits.

Examples:
  From TransferFunctions:
    makeEventContentTable(true)->Write();
    makeEventContentTable(false)->Write();
*/

Table* makeEventContentTable(bool deltaRCut, bool etaCut)
{
  Table* table;
  //Changes the name of the table depending upon whether of not the user wants the event content with or
  //without the deltaR cut included
  if(deltaRCut && etaCut)
    {
      table = new Table("EventContentTable_EatCut_DeltaRCut");
    }
  else if(deltaRCut && !etaCut)
    {
      table = new Table("EventContentTable_DeltaRCut");
    }
  else if(!deltaRCut && etaCut)
    {
      table = new Table("EventContentTable_EtaCut");
    }
  else
    {
      table = new Table("EventContentTable_Complete");
    }
  TableRow* tableRow;
  TableCellText* tableCellText;
  int total = 0;
  stringstream out;
  //Stores the pdgIds of the pairs of jets in each event. This map is ordered by the pdgIds of the pair of jets
  map<pair<int,int>,int> ECMap;
  pair<int,int> currentPair;
  vector<string> cellTitles(1,"Number of Events");
  //Finds the jet pairs and places them in the table if that pair flavor does not exist. If that pair flavor does
  //exist, then the counter is incremented.
  for (int i = 0; i<nEventNtuple; i++)
    { 
      chain->GetEntry(i);
      for(unsigned int j=0; j<ntuple->matchedpdgId.size(); j+=2)
        {
          if((deltaRCut && (ntuple->matchedDeltaR[j]>maxDeltaR || ntuple->matchedDeltaR[j+1]>maxDeltaR)) || (etaCut && (abs(ntuple->jLV[j].Eta())>channels[currentChannelIndex].maxEta || abs(ntuple->jLV[j].Eta())<channels[currentChannelIndex].minEta)) )
            {
              continue;
            }
          else
            {
              if(ECMap.find(make_pair(ntuple->matchedpdgId[j],ntuple->matchedpdgId[j+1]))!=ECMap.end())
                {
                  ECMap[make_pair(ntuple->matchedpdgId[j],ntuple->matchedpdgId[j+1])]++;
                }
              else if(ECMap.find(make_pair(ntuple->matchedpdgId[j+1],ntuple->matchedpdgId[j]))!=ECMap.end())
                {
                  ECMap[make_pair(ntuple->matchedpdgId[j+1],ntuple->matchedpdgId[j])]++;
                }
              else
                {
                  ECMap[make_pair(ntuple->matchedpdgId[j],ntuple->matchedpdgId[j+1])]++;
                }
            }//else
        }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
    }//for (int i = 0; i<nEventNtuple; i++)

  for(map<pair<int,int>,int>::iterator it = ECMap.begin() ; it != ECMap.end(); it++)
    {
      //Formats the row title
      out << "|(" << (*it).first.first << ")(" << (*it).first.second << ")|";
      tableRow = new TableRow(out.str());
      out.str("");
      //There is a single column in this table
      tableCellText = new TableCellText("Number of Events");
      total += (*it).second;
      out << (*it).second;
      tableCellText->text = out.str();
      tableRow->addCellEntry(tableCellText);
      
      table->addRow((*tableRow));
      out.str("");
      delete tableRow;
    }
  //Formats the break row depending upon the size of the table title
  if(deltaRCut) tableRow = makeBreakRow(cellTitles,30);
  else tableRow = makeBreakRow(cellTitles,17);
  table->addRow((*tableRow));
  delete tableRow;

  tableRow = new TableRow("Total");
  tableCellText = new TableCellText("Number of Events");
  out << total;
  tableCellText->text = out.str();
  tableRow->addCellEntry(tableCellText);
  
  table->addRow((*tableRow));
  delete tableRow;

  return table;
}

/*****************************************************************************/

/**
makeInputParameterTable::
  This function makes a table containing all of the parameters used when running the TransferFunctions
  method. This is so that when looking at an output root file, the user knows what parameters/cuts were
  used.

Parameters:
  minos: The boolean which tells the program whether or not to use the minos errors from the unbinned fit.
  process: This is the physics process that the user specified for file naming purposes.
  inputFilepath: This is the path to the dataset being run over (after selection).
  minEta: The minimum eta cut.
  maxEta: The maximum eta cut.

Returns:
  Table*: This is the table containing the parameters used to run TransferFunctions.

Examples:
  From TransferFunctions:
    makeInputParameterTable(minos,process,inputFilepath,debug)->Write();
*/

Table* makeInputParameterTable(bool minos, string process, string inputFilepath, bool debug)
{
  Table* table = new Table("Program Parameters");
  TableRow* tableRow;
  TableCellText* tableCellText;

  tableRow = new TableRow("minos");
  tableCellText = new TableCellText("Parameter Value");
  if(minos) tableCellText->text = "true";
  else tableCellText->text = "false";
  tableRow->addCellEntry(tableCellText);
  table->addRow((*tableRow));
  delete tableRow;

  tableRow = new TableRow("process");
  tableCellText = new TableCellText("Parameter Value");
  tableCellText->text = process;
  tableRow->addCellEntry(tableCellText);
  table->addRow((*tableRow));
  delete tableRow;

  tableRow = new TableRow("inputFilepath");
  tableCellText = new TableCellText("Parameter Value");
  tableCellText->text = inputFilepath;
  tableRow->addCellEntry(tableCellText);
  table->addRow((*tableRow));
  delete tableRow;

  tableRow = new TableRow("debug");
  tableCellText = new TableCellText("Parameter Value");
  if(debug) tableCellText->text = "true";
  else tableCellText->text = "false";
  tableRow->addCellEntry(tableCellText);
  table->addRow((*tableRow));
  delete tableRow;

  return table;
}

/*****************************************************************************/

/**
HistogramsByFlavor::
  The purpose of this function is to fill a series of 1D & 2D histograms. The function sorts 
  the events by pdgId such that only the gen particle/jet pairs with a certain flavor will be
  placed in the histograms. There is also a maximum deltaR cut placed on the events to weed
  out gen parton/jet pairs that have a lower probability of being a good/correct match.

Parameters:
  cha: cha is the channel (UDS, C, B, or G) of events that the user would like the histogram to be filled
       with. The channel corresponds to a specific pdgId of the gen particle. If the gen particles being
       checked here do not match any of the pdgIds of the cha, then they are skipped. This is a method
       of sorting the jets by flavor.
  histoMap1D: This is the map that will contain the filled 1D histograms. It is just a storage element.
  histoMap2D: This is the map that will contain the filled 2D histograms. It is just a storage element.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since the maps are passed
       by reference, the histogram which is created, filled, and placed in the map will persist beyond
       this function. Thus, there does not need to be a return value for the function for it to modify
       the map.

Examples:
  From TransferFunctions:
    HistogramsByFlavor(channels[i],histoMap1D,histoMap2D);
*/

void HistogramsByFlavor(channel &cha, map<string,TH1D*> &histoMap1D, map<string,TH2D*> &histoMap2D)
{
  string histoTitle = "Transfer Function (" + cha.name + ")";
  string histoName1D = "TF_Histo1D_" + cha.name;
  string histoName2D = "TF_Histo2D_" + cha.name;
  string histoName2D_2 = histoName2D + "_JEvsPE";
  histoMap1D[cha.name] = new TH1D(histoName1D.c_str(),histoTitle.c_str(),400,-1000,1000);
  histoMap2D[cha.name] = new TH2D(histoName2D.c_str(),histoTitle.c_str(),400,-1000,1000,200,0,1000);
  histoMap2D[string(cha.name+"_JEvsPE")] = new TH2D(histoName2D_2.c_str(),histoTitle.c_str(),200,0,1000,200,0,1000);
  for (int i = 0; i<nEventNtuple; i++)
    { 
      chain->GetEntry(i);
      for(unsigned int j=0; j<ntuple->matchedGenParticles.size(); j++)
        {
          if((ntuple->matchedDeltaR[j] < maxDeltaR) && (abs(ntuple->jLV[j].Eta()) >= channels[currentChannelIndex].minEta) && (abs(ntuple->jLV[j].Eta()) <= channels[currentChannelIndex].maxEta))
            {
              for(unsigned int k=0; k<cha.pdgids.size(); k++)
                {
                  if(abs(ntuple->matchedpdgId[j]) == cha.pdgids[k])
                    {
                      histoMap1D[cha.name]->Fill(ntuple->jLV[j].Energy() - ntuple->matchedGenParticles[j].E());
                      histoMap2D[cha.name]->Fill(ntuple->jLV[j].Energy() - ntuple->matchedGenParticles[j].E(), ntuple->matchedGenParticles[j].E());
                      histoMap2D[string(cha.name+"_JEvsPE")]->Fill(ntuple->jLV[j].Energy(),ntuple->matchedGenParticles[j].E());
                      cha.numJets++;
                      if(ntuple->matchedpdgId[j]>0)
                        {
                          cha.numJetsPart++;
                        }
                      else if(ntuple->matchedpdgId[j]<0)
                        {
                          cha.numJetsAntiPart++;
                        }
                    }//if(abs(ntuple->matchedpdgId[j]) == cha.pdgids[k])
                  else
                    {
                      continue;
                    }
                }//for(unsigned int l=0; l<channels[currentChannelIndex].pdgid.size(); l++)
            }//if((ntuple->matchedDeltaR[j] < maxDeltaR) && (abs(ntuple->jLV[j].Eta()) >= channels[currentChannelIndex].minEta) && (abs(ntuple->jLV[j].Eta()) <= channels[currentChannelIndex].maxEta))
          else
            {
              cha.numJetsFailDR++;
            }
        }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
    }//for (int i = 0; i<nEventNtuple; i++)
}

/*****************************************************************************/

/**
HistogramsNormalizedByPartonEnergy::
  The purpose of this function is to correctly normalize the 2D JEvsPE histograms. Instead of simply
  scaleing the histograms by 1/Integral("width"), we instead to a slice in parton energy (slices of 
  width 5 GeV due to the bin size) and then normalize that slice. The normalized values and errors are
  then set as the values of the resulting TH2D*. Thus the correct normalization of the histogram is
  accomplished as a function of E_p. This function, unline the function with the same name below, is
  user accessible from the root interface.

Parameters:
  inputHist: This is the original histogram that the user wants to normalize.
  chaName: This is the name of the channel(UDS, C, B, or G) of events that the histogram contains. This
           parameter is used only for seting the name and title of the return histogram.

Returns:
  TH2D*: The pointer that is returned points to the resulting normalized 2D histogram.

Examples:
  From HistogramsNormalizedByPartonEnergy:
    HistogramsNormalizedByPartonEnergy(histoMap2D[string(cha.name+"_JEvsPE")],cha.name);
*/

TH2D* HistogramsNormalizedByPartonEnergy(TH2D* inputHist, string chaName)
{
  string histoTitle = "Transfer Function (" + chaName + ")";
  string normHistName = "TF_Histo2D_" + chaName + "_JEvsPE_norm";
  TH2D* outputHist = new TH2D(normHistName.c_str(),histoTitle.c_str(),200,0,1000,200,0,1000);
  TH1D* projHist;
  for(int i=0; i<=inputHist->GetNbinsY()+1; i++)
    {
      projHist = inputHist->ProjectionX("_px",i,i,"e");
      if(i==0) projHist->Sumw2();
      if(projHist->Integral("width") <= 0) continue;
      projHist->Scale(1/projHist->Integral("width"));
      for(int j=0; j<=projHist->GetNbinsX()+1; j++)
        {
          outputHist->SetBinContent(j,i,projHist->GetBinContent(j));
          outputHist->SetBinError(j,i,projHist->GetBinError(j));
        }
      delete projHist;
    }
  return outputHist;
}

/*****************************************************************************/

/**
HistogramsNormalizedByPartonEnergy::
  The purpose of this function is to prvide a method for the TransferFunction function to call
  the previous HistogramsNormalizedByPartonEnergy function and to have the resulting function
  placed into the 2D histogram map without there being a true return value. This is meant only
  as an internal method to the program. The user may disreguard this function when working in
  an interactive root environment.
  
Parameters:
  cha: cha is the channel (UDS, C, B, or G) of events that the user would like the histogram to be filled
       with. The channel corresponds to a specific pdgId of the gen particle. If the gen particles being
       checked here do not match any of the pdgIds of the cha, then they are skipped. This is a method
       of sorting the jets by flavor.
  histoMap2D: This is the map that will contain the filled histogram. It is just a storage element.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since the map is passed
       by reference, the histogram which is created, filled, and placed in the map will persist beyond
       this function. Thus, there does not need to be a return value for the function for it to modify
       the map.

Examples:
  From TransferFunctions:
    HistogramsNormalizedByPartonEnergy(channels[i],histoMap2D);
*/
//Jet Energy = x
//Parton Energy = y
void HistogramsNormalizedByPartonEnergy(channel &cha, map<string,TH2D*> &histoMap2D)
{
  histoMap2D[string(cha.name+"_JEvsPE_norm")] = HistogramsNormalizedByPartonEnergy(histoMap2D[string(cha.name+"_JEvsPE")],cha.name);
}

/*****************************************************************************/

/**
HistogramsByFlavorAndEnergy::
  The purpose of this function is to fill a series of 1D histograms with E_j - E_p on the x-axis. These
  histograms are slices of the larger 2D histograms, but are divided up into E_p energy ranges. This is
  done so that a 1D version of the unbinned fit can be plotted discretely as a function of E_p. There is 
  also a maximum deltaR cut placed on the events to weed out gen parton/jet pairs that have a lower 
  probability of being a good/correct match.

Parameters:
  cha: cha is the channel (UDS, C, B, or G) of events that the user would like the histogram to be filled
       with. The channel corresponds to a specific pdgId of the gen particle. If the gen particles being
       checked here do not match any of the pdgIds of the cha, then they are skipped. This is a method
       of sorting the jets by flavor.
  histoMap1D: This is the map that will contain the filled histogram. It is just a storage element.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since the map is passed
       by reference, the histogram which is created, filled, and placed in the map will persist beyond
       this function. Thus, there does not need to be a return value for the function for it to modify
       the map.

Examples:
  From TransferFunctions:
    HistogramsByFlavorAndEnergy(channels[i],histoMap1D);
*/

void HistogramsByFlavorAndEnergy(channel &cha, map<string,TH1D*> &histoMap1D)
{
  for(unsigned int e=0; e<1000; e+=25)
    {
      string histoName = concatString("TF_Histo1D_",concatString(cha.name,concatString("_",concatString(e,concatString("GeV_",concatString(e+25,"GeV"))))));
      string histoTitle = "Transfer Function (" + cha.name + ") in Parton Energy From ";
      histoTitle = concatString(histoTitle,e) + "GeV to ";
      histoTitle = concatString(histoTitle,e+25) + "GeV";
      histoMap1D[histoName] = new TH1D(histoName.c_str(),histoTitle.c_str(),400,-1000,1000);
      string histoNameJEvsPE = concatString("TF_Histo1D_",concatString(cha.name,concatString("_",concatString("JEvsPE_",concatString(e,concatString("GeV_",concatString(e+25,"GeV")))))));
      histoMap1D[histoNameJEvsPE] = new TH1D(histoNameJEvsPE.c_str(),histoTitle.c_str(),200,0,1000);

      for (int i = 0; i<nEventNtuple; i++)
        { 
          chain->GetEntry(i);
          for(unsigned int j=0; j<ntuple->matchedGenParticles.size(); j++)
            {
              if((ntuple->matchedDeltaR[j] < maxDeltaR) && (abs(ntuple->jLV[j].Eta()) >= channels[currentChannelIndex].minEta) && (abs(ntuple->jLV[j].Eta()) <= channels[currentChannelIndex].maxEta))
                {
                  {
                    for(unsigned int k=0; k<cha.pdgids.size(); k++)
                      {
                        if(abs(ntuple->matchedpdgId[j]) == cha.pdgids[k])
                          {
                            if(ntuple->matchedGenParticles[j].E()>e && ntuple->matchedGenParticles[j].E()<=(e+25))
                              {
                                histoMap1D[histoName]->Fill(ntuple->jLV[j].Energy() - ntuple->matchedGenParticles[j].E());
                                histoMap1D[histoNameJEvsPE]->Fill(ntuple->jLV[j].Energy());
                              }
                          }//if(abs(ntuple->matchedpdgId[j]) == cha.pdgids[k])
                      }//for(unsigned int l=0; l<channels[currentChannelIndex].pdgid.size(); l++)
                  }//if((ntuple->matchedDeltaR[j] < maxDeltaR) && (abs(ntuple->jLV[j].Eta()) >= channels[currentChannelIndex].minEta) && (abs(ntuple->jLV[j].Eta()) <= channels[currentChannelIndex].maxEta))
                }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
            }//for (int i = 0; i<nEventNtuple; i++) 
        }//for (int i = 0; i<nEventNtuple; i++)
    }//for(unsigned int e=0; e<1000; e+=25)
}

/*****************************************************************************/

/**
fitTF1D::
  This is a 1D function used to fit a histogram of E_j -E_p values. E_j - E_p is on the x-axis.
  This is not the unbinned fit, but rather a function used primarily to draw the resulting 
  transfer function once the parameters have been set. It, however, has the same
  form/mathematical equation as the unbinned fit with the exception that it is an 11 parameter
  function. The 11th parameter is fixed to be the parton energy at the center of a certain energy
  range. par[10], the 11th parameter, takes the place of the parton energy degree of freedom in
  the 2D fit.

Parameters:
  x: This is the array that stores the variable values of the function. It is not directly user accessible.
  par: This is the array that stores the function parameters. It is not directly user accessible.

Returns:
  double: This function returns the numberical result of the function once it is evaluated.

Examples:
  From OverlayFunction:
    TF1* fit = new TF1("TF_1D",(*fitFunc),-1000,1000,fitPars);
*/

double fitTF1D(double *x, double *par)
{
  double fxy;
  double par0 = par[0] + par[1]*par[10];
  double par1 = par[2] + par[3]*par[10];
  double par2 = par[4] + par[5]*par[10];
  double par3 = par[6] + par[7]*par[10];
  double par4 = par[8] + par[9]*par[10];
  fxy = exp(-.5*pow(x[0]-par0,2)/(pow(par1,2))); 
  fxy = fxy+par2*exp(-.5*pow(x[0]-par3,2)/(pow(par4,2)));
  return fxy/((par1*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-par[10])-par0)/(sqrt(2)*par1)))+((par2*par4)*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-par[10])-par3)/((sqrt(2)*par4)))));
}

/*****************************************************************************/

/**
fitTF2D::
  This is a 2D function used to fit a histogram of E_j -E_p vs E_p values. E_j - E_p is on the x-axis
  and E_p is on the y-axis. This is the same function used by the unbinned fit. It is, however, also used
  to draw the resulting transfer function once the parameters have been set. The mathematical functions
  has the same form/equation as the unbinned fit. This function also uses 10 parameters.

Parameters:
  x: This is the array that stores the variable values of the function. It is not directly user accessible.
  par: This is the array that stores the function parameters. It is not directly user accessible.

Returns:
  double: This function returns the numberical result of the function once it is evaluated.

Examples:
  From makeFit:
    TF2* fit = new TF2(fitName.c_str(),fitTF2D,-1000,1000,0,1000,12);
*/

double fitTF2D(double* x, double *par)
{
  double par0 = par[0] + par[1]*x[1];
  double par1 = par[2] + par[3]*x[1];
  double par2 = par[4] + par[5]*x[1];
  double par3 = par[6] + par[7]*x[1];
  double par4 = par[8] + par[9]*x[1];

  double fxy;
  fxy = exp(-.5*pow(x[0]-par0,2)/(pow(par1,2))); 
  fxy = fxy+par2*exp(-.5*pow(x[0]-par3,2)/(pow(par4,2)));

  double N;
  if(par1==0 || par4==0)
    {
      if(par1==0 && par4==0)
        {
          if(((30-x[1])-par0)>0 && ((30-x[1])-par3)>0)
            {
              N = 0;
            }
          else if(((30-x[1])-par0)<0 && ((30-x[1])-par3)<0)
            {
              N = ((par1*sqrt(TMath::Pi()/2)*2)+((par2*par4)*sqrt(TMath::Pi()/2)*2));
            }
          else if(((30-x[1])-par0)>0 && ((30-x[1])-par3)<0)
            {
              N = ((par2*par4)*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par3)/((sqrt(2)*par4))));
            }
          else if(((30-x[1])-par0)<0 && ((30-x[1])-par3)>0)
            {
              N = (par1*sqrt(TMath::Pi()/2)*2);
            }
          else //Erfc(0/0) case
            {
              N = 0;
            }
        }
      else if(par1==0)
        {
          if(((30-x[1])-par0)>0)
            {
              N = ((par2*par4)*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par3)/((sqrt(2)*par4))));
            }
          else if(((30-x[1])-par0)<0)
            {
              N = ((par1*sqrt(TMath::Pi()/2)*2)+((par2*par4)*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par3)/((sqrt(2)*par4)))));
            }
          else //Erfc(0/0) case
            {
              N = 0;
            }
        }
      else if(par4==0) 
        {
          if(((30-x[1])-par3)>0)
            {
              N = ((par1*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par0)/(sqrt(2)*par1))));
            }
          else if(((30-x[1])-par3)<0)
            {
              N = ((par1*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par0)/(sqrt(2)*par1)))+((par2*par4)*sqrt(TMath::Pi()/2)*2));
            }
          else //Erfc(0/0) case
            {
              N = 0;
            }
        }
    }
  else
    {
      N = ((par1*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par0)/(sqrt(2)*par1)))+((par2*par4)*sqrt(TMath::Pi()/2)*TMath::Erfc(((30-x[1])-par3)/((sqrt(2)*par4)))));
    }


  if(N!=0 && !TMath::IsNaN(fxy/N))
    {
      return (fxy/N);
    }
  else
    {
      return -1;
    }  
}

/*****************************************************************************/

/**
fitTFJEvsPE::
  This is a 2D function used to fit a histogram of E_j vs E_p values. E_j is on the x-axis and E_p is on
  the y-axis. It is maping a E_j - E_p vs E_p fit to the desired fit.

Parameters:
  x: This is the array that stores the variable values of the function. It is not directly user accessible.
  par: This is the array that stores the function parameters. It is not directly user accessible.

Returns:
  double: This function returns the numberical result of the function once it is evaluated.

Examples:
  From makeJEvsPEFit:
    TF2* newfit = new TF2(fitName.c_str(),fitTFJEvsPE,0,1000,0,1000,10);
*/

double fitTFJEvsPE(double* x, double *par)
{
  return JEvsPEfit->Eval(x[0]-x[1],x[1]);
}

/*****************************************************************************/

/**
fitTFLandauGaus::
  This function was first implemented when the normalization for the current double Gaussian was fit was
  not yet operational. While this function technically provides a nice fit to our data, it relies heavily
  on PDFs and is therefore computationally expensive. It also has the added frustration of not being easily
  normalizable. There are two versions of the code available. The full version assumes that all of the
  parameters scale linearly with the parton energy. The simplified version does not make this assumption
  for the parameters of the Landau function nor its normalization.

Parameters:
  x: This is the array that stores the variable values of the function. It is not directly user accessible.
  par: This is the array that stores the function parameters. It is not directly user accessible.

Returns:
  double: This function returns the numberical result of the function once it is evaluated.

Examples:
  Possible use:
    TF2* newfit = new TF2(fitName.c_str(),fitTFLandauGaus,0,1000,0,1000,10);
*/

double fitTFLandauGaus(double* x, double *par)
{
  double fxy;
  //Full
  //fxy = exp(-.5*pow(x[0]-(par[0]+par[1]*par[9]),2)/(pow((par[2]+par[3]*par[9]),2))); 
  //fxy = fxy + ((par[4]+par[9]*par[5])*(ROOT::Math::landau_pdf((x[0]-(par[6]+par[7]*par[9]))/par[8])));

  //Simplified
  fxy = par[0]*exp(-.5*pow(x[0]-(par[1]+par[2]*par[8]),2)/(pow((par[3]+par[4]*par[8]),2))); 
  fxy = fxy + (par[5]*(ROOT::Math::landau_pdf((x[0]-par[6])/par[7])));

  return fxy;
}
/*****************************************************************************/

/**
findFitFunc::
  This function is used to determine which function the user is trying to overlay on top of a histogram.
  We want the user to be able to pass in a string telling the program which function it should use and
  then have the computer interpret this and pass back the correct function. This is a bit like a lookup
  table of function pointers. This also allows the computer to determine the number of fit parameters
  based upon which function is chosen.

Parameters:
  fitFuncName: The function name is litterally the name of the fit function that the user wants to use.
               It is initially passed as a parameter to OverlayFunction, and is then passed here to
               decide which function the returned pointer should be set to.
  fitPars: This is the number of fit parameters that the fitting function uses. It is being passed
           by reference to that it can be set without having it be the return object. Therefore,
           the value of this integer can initially be anything.

Returns:
  double (double*,double*): This function returned a pointer to a function which takes two double* as
                            parameters and returns a double. It is really returning a pointer to a
                            function used for fitting.

Examples:
  From OverlayFunction:
    fitFunc = findFitFunc(functionName,fitPars);
*/

double (*findFitFunc(string fitFuncName,unsigned int& fitPars))(double*,double*)
{
  if(fitFuncName == "fitTF2D")
    {
      fitPars = 10;
      return &fitTF2D;
    }
  else if(fitFuncName == "fitTFJEvsPE")
    {
      fitPars = 10;
      return &fitTFJEvsPE;
    }
  else if(fitFuncName == "fitTFLandauGaus")
    {
      fitPars = 9;
      return &fitTFLandauGaus;
    }
  else
    {
      if(fitFuncName != "fitTF1D")
        cout << "findFitFunc::functionName not recognized. Using the default fit (fitTF1D)." << endl;

      fitPars = 11;
      return &fitTF1D; // default if invalid operator was passed
    }
}

/*****************************************************************************/

/**
makeFit::
  This function creates a TF2 pointer out of the final parameters found by doing an unbinned fit of
  E_j - E_p vs E_p. The TMinuit object is what performes and stores the fit values. The parameters
  are being used to set a TF2* in order to be able to draw the resulting function.

Parameters:
  gMinuit: This is the object which, after the fitting process is complete, initially stores the results
       of the fit, including the final parameters.

Returns:
  TF2*: This is the function resulting from the unbinned fitting of the 10 transfer function
        parameters.

Examples:
  From TransferFunctions:
    fits.push_back(makeFit(gMinuit));
*/

TF2* makeFit(pair<vector<string>,pair<vector<double>, vector<double> > > parameters)
{
  string fitName = "TF_Fit2D_"+channels[currentChannelIndex].name;
  TF2* fit = new TF2(fitName.c_str(),fitTF2D,-1000,1000,0,1000,10);

  for(unsigned int i=0; i<parameters.first.size(); i++)
    {
      //cout << parameters.first[i] << ": " << parameters.second.first[i] << " +- " << parameters.second.first[i] << endl;
      fit->SetParameter(i,parameters.second.first[i]);
      fit->SetParError(i,parameters.second.second[i]);
    }
  return fit;
}

/*****************************************************************************/

/**
makeJEvsPEFit::
  This function is designed to take the parameters/a function of E_j - E_p vs E_p and map it to a
  function of E_j vs E_p. This is only really useful for 2D equations.

Parameters:
  fit: The initial function which was made using an unbinned fit of E_j - E_p vs E_p.

Returns:
  TF2*: This is the mapped function.

Examples:
  From OverlayFunction:
    fits.push_back(makeJEvsPEFit(fits.back()));
*/

TF2* makeJEvsPEFit(TF2* fit)
{
  JEvsPEfit = (TF2*)fit->Clone();
  string fitName = "TF_Fit2D_" + channels[currentChannelIndex].name + "_JEvsPE"; 
  TF2* newfit = new TF2(fitName.c_str(),fitTFJEvsPE,0,1000,0,1000,10);
  return newfit;
}

/*****************************************************************************/

/**
getFitPars::
  This function navigated to the text document containing the ten parameters for the transfer function
  indicated by the user. It then uses a member function of the Table class to parse the file and read
  in the values. This method is instead of relying on the loaded rootfiles to contain the correct table.
  After the 20 values (10 parameters and their errors) are loaded into a table, the values are put into
  vectors for easier access.

Parameters:
  paramFile: The path to the file where the parameters of the transfer function were printed.

Returns:
  pair<vector<double>,vector<double> >: This first object in this pair contains a vector of ten doubles,
                                        which are the parameters to the transfer function. The second
                                        object contains a vector of ten double, which are the errors
                                        associated with the aforementioned parameters.

Examples:
  From OverlayFunction:
    fitParams = getFitPars(paramFile).first;
    fitParamErrors = getFitPars(paramFile).second;
*/

pair<vector<double>,vector<double> > getFitPars(string paramFile)
{
  vector<double> fitPars;
  vector<double> fitParErrors;
  Table* table = new Table(paramFile);
  table->parseFromFile(paramFile,"TableCellVal");
  vector<TableRow> rows = table->getRows();
  vector<TableCell*> col;
  TableCellVal * param;
  for(unsigned int i=0; i<rows.size(); i++)
    {
      col = rows[i].getCellEntries();
      if(col.size()>0)
        {
          param = (TableCellVal*) col[0];
          fitPars.push_back(param->val.value);
          fitParErrors.push_back(param->val.error);
        }
    }
  return make_pair(fitPars,fitParErrors); 
}

/*****************************************************************************/

/**
setFitParams::
  This function sets the parameters for the graphical representation of the transfer function.
  The function, called fit here, will eventually be drawn to a canvas and needs these parameters
  to be drawn with the correct shape. The parameters were originally put into these vectors by
  the getFitPars function which was called by OverlayFunction.

Parameters:
  fit: This is the function that will be used later on in OverlayFunction.
  fitParams: This vector contains the ten values of the parameters that are being set for the
             fit function.
  fitParamErrors: This vector contains the ten errors on the parameters that are being set for
                  the fit function.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since the fit object,
       which will be used later in OverlayFunction, is a pointer, pointers, the TF2 that is points to
       will retain the values given to it in this function.

Examples:
  From OverlayFunction:
    setFitParams(fit, fitParams, fitParamErrors);
*/

void setFitParams(TF2* fit,vector<double> &fitParams, vector<double> &fitParamErrors)
{
  for(unsigned int i=0; i<fitParams.size(); i++)
    {
      fit->SetParameter(i,fitParams[i]);
      fit->SetParError(i,fitParamErrors[i]);
    }
}

/*****************************************************************************/

/**
setFitParams::
  This function sets the parameters for the graphical representation of the transfer function.
  The function, called fit here, will eventually be drawn to a canvas and needs these parameters
  to be drawn with the correct shape. The parameters were originally put into these vectors by
  the getFitPars function which was called by OverlayFunction.

Parameters:
  fit: This is the function that will be used later on in OverlayFunction.
  fitParams: This vector contains the ten values of the parameters that are being set for the
             fit function.
  fitParamErrors: This vector contains the ten errors on the parameters that are being set for
                  the fit function.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since the fit object,
       which will be used later in OverlayFunction, is a pointer, pointers, the TF1 that is points to
       will retain the values given to it in this function.

Examples:
  From OverlayFunction:
    setFitParams(fit, fitParams, fitParamErrors);
*/

void setFitParams(TF1* fit,vector<double> &fitParams, vector<double> &fitParamErrors)
{
    for(unsigned int i=0; i<fitParams.size(); i++)
    {
      fit->SetParameter(i,fitParams[i]);
      fit->SetParError(i,fitParamErrors[i]);
    }
}

/*****************************************************************************/

/**
makeHistFromFunc::
  This function takes a root 1D function (TF1*) and makes a histogram out of it with the same dimensions
  and bins as a model histogram (which the user is in theory trying to compare it to). The contents of
  the model histogram, which is supposedly a copy of another histogram so as to not reset the original,
  are reset to be the value of the function at the center of a bin. The errors for each bin are set to
  zero, as the function has no associated error. Since the parameters, the original histogram and
  function, are pointers, no return value is necessary.

Parameters:
  hist: This istogram is a copy of the histogram that will be compared to the function being drawn. It
        is a copy so that the original histogram is not modified. The contents of this histogram,
        however, will be changed to match the value of the function at the center of each bin.
  fit: This is the function that will be evaluated at the location of the center of each bin.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since all of the
       parameters are pointers, the objects they point to retain the values given to them in this
       function.

Examples:
  From OverlayFunction:
    makeHistFromFunc(fitHist,fit);
*/

void makeHistFromFunc(TH1D* hist, TF1* fit)
{
  for(int i=0; i<=hist->GetNbinsX()+1; i++)
    {
      hist->SetBinContent(i,fit->Eval(hist->GetXaxis()->GetBinCenter(i)));
      hist->SetBinError(i,0);
    }
}

/*****************************************************************************/

/**
makeHistFromFunc::
  This function takes a root 2D function (TF2*) and makes a histogram out of it with the same dimensions
  and bins as a model histogram (which the user is in theory trying to compare it to). The contents of
  the model histogram, which is supposedly a copy of another histogram so as to not reset the original,
  are reset to be the value of the function at the center of a bin. The errors for each bin are set to
  zero, as the function has no associated error. Since the parameters, the original histogram and
  function, are pointers, no return value is necessary.

Parameters:
  hist: This istogram is a copy of the histogram that will be compared to the function being drawn. It
        is a copy so that the original histogram is not modified. The contents of this histogram,
        however, will be changed to match the value of the function at the center of each bin.
  fit: This is the function that will be evaluated at the location of the center of each bin.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since all of the
       parameters are pointers, the objects they point to retain the formatting given to them in
       this function.

Examples:
  From OverlayFunction:
    makeHistFromFunc(fitHist,fit);
*/

void makeHistFromFunc(TH2D* hist, TF2* fit)
{
  for(int i=0; i<=hist->GetNbinsX()+1; i++)
    {
      for(int j=0; j<=hist->GetNbinsY()+1; j++)
        {
          hist->SetBinContent(i,j,fit->Eval(hist->GetXaxis()->GetBinCenter(i), hist->GetYaxis()->GetBinCenter(j)));
          hist->SetBinError(i,j,0);
        }
    }
}

/*****************************************************************************/

/**
printKSandChi2::
  This function peforms a KS and a Chi2 test, separately, between the original histogram being
  drawn and a histogram made out of the function being overlaid on the histogram. The results
  of these test are then printed onto the statistics box of the original histogram. This
  function is used to perform these test on 1D histograms.

Parameters:
  hist: This is the histogram that represents the original Monte Carlo data.
  fitHist: This is a histogram where the bin contents are set to be the value of the function
           being drawn, at the center of the bin. A histogram is needed, instead of a function,
           in order to perform some of these tests. 

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since all of the
       parameters are pointers, the objects they point to retain the formatting given to them in
       this function. In addition, the updated statistics box will persis withing the histogram
       object.

Examples:
  From OverlayFunction:
    printKSandChi2(hist,fitHist);
*/

void printKSandChi2(TH1D* hist, TH1D* fitHist)
{
  TPaveStats *st = (TPaveStats*)hist->FindObject("stats");
  double chi2;
  int NDF;
  int igood;
  st->SetName("updatedStats");
  st->AddText(Form("KolmogorovTest = %g",hist->KolmogorovTest(fitHist)));
  st->AddText(Form("Chi2Prob = %g",fitHist->Chi2TestX(hist,chi2,NDF,igood,"UW")));
  st->AddText(Form("Chi2/NDF = %g",chi2/NDF));
  hist->SetStats(0); //to avoid a new "stats" to be drawn 
 }

/*****************************************************************************/

/**
printKSandChi2::
  This function peforms a KS and a Chi2 test, separately, between the original histogram being
  drawn and a histogram made out of the function being overlaid on the histogram. The results
  of these test are then printed onto the statistics box of the original histogram. This
  function is used to perform these test on 2D histograms.

Parameters:
  hist: This is the histogram that represents the original Monte Carlo data.
  fitHist: This is a histogram where the bin contents are set to be the value of the function
           being drawn, at the center of the bin. A histogram is needed, instead of a function,
           in order to perform some of these tests. 

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since all of the
       parameters are pointers, the objects they point to retain the formatting given to them in
       this function. In addition, the updated statistics box will persis withing the histogram
       object.

Examples:
  From OverlayFunction:
    printKSandChi2(hist,fitHist);
*/

void printKSandChi2(TH2D* hist, TH2D* fitHist)
{
  TPaveStats *st = (TPaveStats*)hist->FindObject("stats");
  st->SetFillColor(0);
  st->SetX1NDC(0.70); //new x start position
  st->SetX2NDC(0.99); //new x end position
  st->SetY1NDC(0.01); //new y start position
  st->SetY2NDC(0.60); //new y end position
  st->SetName("updatedStats");
  st->AddText(Form("KolmogorovTest = %g",hist->KolmogorovTest(fitHist)));
  st->AddText(Form("Chi2Test = %g",fitHist->Chi2Test(hist,"UW")));
  hist->SetStats(0); //to avoid a new "stats" to be drawn 
 }

/*****************************************************************************/

/**
formatForDraw::
  This function is called by OverlayFunction and OverlayAll1D in order to set the drawing options
  like titles, axis labels, and font size. This function is called when the histograms and functions
  being drawn are 1D.

Parameters:
  hist: This is the original histogram which will be drawn to the canvas.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since all of the
       parameters are pointers, the objects they point to retain the formatting given to them in
       this function.

Examples:
  From OverlayFunction:
    formatForDraw(hist);
*/

void formatForDraw(TH1D* hist)
{
  hist->GetXaxis()->SetTitle("E_{Jet}-E_{P} [GeV]");
  hist->GetXaxis()->SetLabelSize(0.04);
  hist->GetXaxis()->SetTitleSize(0.04);
  hist->GetXaxis()->SetTitleOffset(1.2);
}

/*****************************************************************************/

/**
formatForDraw::
  This function is called by OverlayFunction in order to set the drawing options like titles, axis
  labels, and font size. This function is called when the histograms and functions being drawn are
  2D.

Parameters:
  can: This is the canvas where the objects are being drawn to.
  hist: This is the original histogram which will be drawn to the canvas.
  fit: This is the function that is being drawn to the canvas along with the histogram. It is getting
       its own formatting as it may be drawn in a pad of its own at some point (if the
       JEvsPE option is set to false).
  JEvsPE: This boolean comes from OverlayFunction and specified what title to put on the x-axis.
          There may be some redundancies in this portion of the code as this method was hastily
          implemented.

Returns:
  N/A: This is a void function and does not explicitly return anything. However, since all of the
       parameters are pointers, the objects they point to retain the formatting given to them in
       this function.

Examples:
  From OverlayFunction:
    formatForDraw(can, hist, fit, JEvsPE);
*/

void formatForDraw(TCanvas* can, TH2D* hist, TF2* fit, bool JEvsPE)
{
  can->SetLogz();

  fit->SetTitle((((string)hist->GetTitle())+": Fit").c_str());
  hist->SetTitle((((string)hist->GetTitle())+": MC").c_str());

  if(JEvsPE)
    {
      hist->GetXaxis()->SetTitle("E_{Jet} [GeV]");
      hist->GetYaxis()->SetTitle("E_{P} [GeV]");
      hist->GetXaxis()->SetTitleOffset(2.0);
      hist->GetXaxis()->SetLabelSize(0.03);
      hist->GetXaxis()->SetTitleSize(0.03);
      hist->GetYaxis()->SetTitleOffset(2.4);
      hist->GetYaxis()->SetLabelSize(0.03);
      hist->GetYaxis()->SetTitleSize(0.03);
      hist->GetZaxis()->SetTitle("Entries per 5x5 GeV^{2}");
      hist->GetZaxis()->SetTitleOffset(1.6);
      hist->GetZaxis()->SetLabelSize(0.03);
      hist->GetZaxis()->SetTitleSize(0.03);

      fit->GetXaxis()->SetTitle("E_{Jet} [GeV]");
      fit->GetYaxis()->SetTitle("E_{P} [GeV]");
      fit->GetXaxis()->SetTitleOffset(2.0);
      fit->GetXaxis()->SetLabelSize(0.03);
      fit->GetXaxis()->SetTitleSize(0.03);
      fit->GetYaxis()->SetTitleOffset(2.4);
      fit->GetYaxis()->SetLabelSize(0.03);
      fit->GetYaxis()->SetTitleSize(0.03);
      fit->GetZaxis()->SetTitle("Entries per 5x5 GeV^{2}");
      fit->GetZaxis()->SetTitleOffset(1.6);
      fit->GetZaxis()->SetLabelSize(0.03);
      fit->GetZaxis()->SetTitleSize(0.03);
    }
  else
    {
      hist->GetXaxis()->SetTitle("E_{Jet}-E_{P} [GeV]");
      hist->GetYaxis()->SetTitle("E_{P} [GeV]");
      hist->GetXaxis()->SetTitleOffset(2.0);
      hist->GetXaxis()->SetLabelSize(0.03);
      hist->GetXaxis()->SetTitleSize(0.03);
      hist->GetYaxis()->SetTitleOffset(2.4);
      hist->GetYaxis()->SetLabelSize(0.03);
      hist->GetYaxis()->SetTitleSize(0.03);
      hist->GetZaxis()->SetTitle("Entries per 5x5 GeV^{2}");
      hist->GetZaxis()->SetTitleOffset(1.6);
      hist->GetZaxis()->SetLabelSize(0.03);
      hist->GetZaxis()->SetTitleSize(0.03);

      fit->GetXaxis()->SetTitle("E_{Jet}-E_{P} [GeV]");
      fit->GetYaxis()->SetTitle("E_{P} [GeV]");
      fit->GetXaxis()->SetTitleOffset(2.0);
      fit->GetXaxis()->SetLabelSize(0.03);
      fit->GetXaxis()->SetTitleSize(0.03);
      fit->GetYaxis()->SetTitleOffset(2.4);
      fit->GetYaxis()->SetLabelSize(0.03);
      fit->GetYaxis()->SetTitleSize(0.03);
      fit->GetZaxis()->SetTitle("Entries per 5x5 GeV^{2}");
      fit->GetZaxis()->SetTitleOffset(1.6);
      fit->GetZaxis()->SetLabelSize(0.03);
      fit->GetZaxis()->SetTitleSize(0.03);
    }
}

/*****************************************************************************/

/*
OverlayFunction::
  The overlay function was meant to be a quick way to graphically show the relationship between the
  transfer functions and the histograms they are supposed to represent. The function also properly
  formats any axes labels and titles. It also outputs the results of a KS test between the fit and
  the histogram as well as a Chi2Test. Because there are multiple different types of functions
  and histograms, there needs to be a higher than average number of parameters in order to specify
  what the user is looking for. In order to use this function, a root file created by the
  TransferFunctions function must have already been created and loaded into root. However, as this
  function does not depend upon the titles or any specifics about a histogram or function, any
  histogram/function comfination will work from any root file loaded into memory (at least in theory).

Parameters:
  histoName: This is the name of the histogram which represents the original data. It can be either the
             name of a 1D histogram or a 2D histogram. It will be drawn alongside the fitted function.
  functionName: This is the name of the fitting function (not the root TF, but the actual function in
                code) which was used to generate the unbinned fit. The possible functions are "fitTF1D",
                "fitTF2D", or "fitTFJEvsPE".
  paramFile: This is the path to the file which contains the output table of parameters for the transfer
             function that the user is trying to have drawn. In theory, the Table object version of this
             file is also stored in the root file, but this is a more robust and file independent method
             of finding the correct fit parameters. The file should contain 10 parameters.
  fitPoints: Whenever an function is drawn by root, a specific number of "draw points" is used. For 1D
             functions, like TF1s, the default number of draw points is 100 along the x axis. For 2D objects,
             like TF2s, the default number of draw points is 30 along each of the axis (x and y). This
             parameter gives the user the option to specify how many draw points should be used. The more
             draw points used, the more accurately root can draw the figure. For functions with steep curves
             or peaks, it is particularly useful to increas the number of points. However, if the black
             mesh is drawn along with the figure, it will start to obscure the drawing, making it look like a
             solid black figure. The options for the user are "default", "max", which specifies the maximum
             allowable number of draw points for the figure (10,000,000 for TF1s and 10,000 for TF2s), and
             "mid", which specifies 400 points for the x-axis and 200 points for the y-axis this function
             has only been implemented for 2D functions as it is not necessary vor viewing 1D functions.
  partonEnergy: Since the 11th parameter in the parton energy fitting function has to be fixed to be the
                parton energy for a specific slice of the 2D histogram, but this information is only stored
                in the title, the user must specify the energy. Typically this energy is the energy halfway
                between the upper and lower bound of parton energy that the histogram represents. For example,
                if the histogram is TF_Histo1D_50GeV_to_75_GeV, then the user would input 62.5. Theoretically,
                the user could put any number and the drawn histogram would be scaled, so be careful. This
                method is instead of parsing histogram titles and converting strings into doubles.
  JEvsPE: This is a boolean that indicated to the function what is being plotted along the x & y axes for
          the 2D histograms. The y-axis is always the parton energy. However, the x-axis could either be
          jet energy minus parton energy, or it could be just jet energy. If this boolean is try, it indicates
          that the user is specifying a histogram/function where jet energy is on the x-axis and that
          the formatting options for this histogram should be chosen. The default value is set to fauls so
          that the user does not need to specify this option if they are plotting jet energy minus parton
          energy or a 1D histogram.
  fitName: This parameter is only needed when the 2D histogram being drawn has jet energy on the x-axis. Since
           the functions for JEvsPE don't store their parameters, the original function (same name without the
           JEvsPE postscript) must be specified. This function is used in order to map the x and y coordinates
           from jet energy minus parton energy, to just jet energy.

Returns::
  N/A: The function graphically outputs a TCanvas* with a histogram and a function drawn.

Examples:
  For a 2D histogram overlaid with a 2D function (E_j - E_p vs E_p): 
    OverlayFunction("TF_Histo2D_B","fitTF2D","TF_ttbar_B.txt")
  For a 2D histogram overlaid with a 2D function (E_j vs E_p):
    OverlayFunction("TF_Histo2D_B_JEvsPE_norm","fitTFJEvsPE","TF_TTbarMG_B.txt","default",0,true,"TF_Fit2D_B")
  For a 1D histogram overlaid with a 1D function:
    OverlayFunction("TF_Histo1D_50GeV_to_75_GeV","fitTF1D","TF_TTbarMG_B.txt","default",62.5)
*/

void OverlayFunction(string histoName, string functionName, string paramFile, string fitPoints="default", double partonEnergy = 0, bool JEvsPE = false, string fitName = "")
{
  setStyle(false);
  gStyle->SetOptStat("eMRi");
  //Figure out which fitting function to use and set the number of fit parameters
  unsigned int fitPars = 0;
  double (*fitFunc)(double*, double*) = NULL;
  fitFunc = findFitFunc(functionName,fitPars);
  vector<double> fitParams;
  vector<double> fitParamErrors;
  
  if(string(gDirectory->Get(histoName.c_str())->ClassName()) == "TH2D")
    {
      if(!JEvsPE)
        {
          TCanvas* can = new TCanvas("can","can",1800,800);
          can->Divide(3,1);
          can->cd(1);
          TH2D* hist = (TH2D*)gDirectory->Get(histoName.c_str());
          TF2* fit = new TF2("TF_2D",(*fitFunc),-1000,1000,0,1000,fitPars);
          formatForDraw(can, hist, fit, JEvsPE);
          
          fitParams = getFitPars(paramFile).first;
          fitParamErrors = getFitPars(paramFile).second;
          setFitParams(fit, fitParams, fitParamErrors);
          hist->Sumw2();
          hist->Scale(1/(hist->Integral("width")));
          hist->DrawClone("lego");
          can->cd(2);
          fit->DrawClone("surf1");
          can->cd(3);
          fit->SetTitle("Overlay");
          if(fitPoints!="default")
            {
              if(fitPoints=="mid") 
                {
                  fit->SetNpx(400);
                  fit->SetNpy(200);
                }
              else
                {
                  fit->SetNpx(10000); //10,000 is the naximum number of points for a 2d/3d function
                  fit->SetNpy(10000);
                }
            }
          fit->Draw("surf1");
          hist->Draw("legosames");
          can->Update();
          
          TH2D* fitHist = (TH2D*)hist->Clone((((string)hist->GetName()) + "_clone").c_str());
          makeHistFromFunc(fitHist,fit);
          printKSandChi2(hist,fitHist);
          can->Modified();
        }
      //Case where the histogram has E_jet on the x axis and E_p on the y axis
      else
        {
          TCanvas* can = new TCanvas("can","can",800,800);
          can->cd();
          TH2D* hist = (TH2D*)gDirectory->Get(histoName.c_str());
          JEvsPEfit = (TF2*)((TF2*)gDirectory->Get(fitName.c_str()))->Clone();
          TF2* fit = new TF2((fitName+"_binned").c_str(),fitTFJEvsPE,0,1000,0,1000,10);
          formatForDraw(can, hist, fit, JEvsPE);
          hist->Draw("lego2");
          hist->Fit(fit);
          fit->Draw("same surf1");
          can->Update();
          
          TH2D* fitHist = (TH2D*)hist->Clone((((string)hist->GetName()) + "_clone").c_str());
          makeHistFromFunc(fitHist,fit);
          printKSandChi2(hist,fitHist);
          can->Modified();
        }
    }
  else if(string(gDirectory->Get(histoName.c_str())->ClassName()) == "TH1D")
    {
      TCanvas* can = new TCanvas("can","can",800,800);
      can->cd();
      TH1D* hist = (TH1D*)gDirectory->Get(histoName.c_str());
      TF1* fit = new TF1("TF_1D",(*fitFunc),30-partonEnergy,1000,fitPars);
      formatForDraw(hist);
      string newHistTitle = concatString(partonEnergy-12.5,concatString(" < Ep < ",partonEnergy+12.5));
      hist->SetTitle(newHistTitle.c_str());

      fitParams = getFitPars(paramFile).first;
      fitParamErrors = getFitPars(paramFile).second;
      fitParams.push_back(partonEnergy);
      setFitParams(fit, fitParams, fitParamErrors);
      hist->Sumw2();
      hist->Scale(1/(hist->Integral("width")));  
      hist->Draw("HE");
      fit->SetNpx(hist->GetNbinsX());
      fit->Draw("sames");
      fit->SetLineColor(2);
      can->Update();

      TH1D* fitHist = (TH1D*)hist->Clone((((string)hist->GetName()) + "_clone").c_str());
      makeHistFromFunc(fitHist,fit);
      printKSandChi2(hist,fitHist);
      can->Modified();
    }
  else
    {
      cout << "OverlayFunction::Failed to determine the class of the underlying histogram. Cannot overlay the fit.";
      return;
    }
}

/*****************************************************************************/

/**
OverlayAll1D::
  This function was created to draw a range of 1D histograms to a single canvas. The histograms are slices,
  in parton energy, of the 2D E_j - E_p vs E_p histograms. The slices, which are already stored in a root
  file, are retrieved and drawn. The parameters of the transfer function for this process and flavor are
  retrieved and the 1D version of the transfer function is drawn on top of the histogram. A KS test and
  chi squared analysis are also performed on each histogram.

Parameters:
  flavor: This parameter specifies the flavor of the 1D histograms that should be drawn. When the function
          finds the histograms in the loaded root file, it looks for histograms following a specific pattern
          (currently this function cannot draw all of the JEvsPE histograms). The pattern involved needs the
          flavor of the parton for which the histograms were created.
  functionName: This is the name of the fitting function (not the root TF, but the actual function in
                code) which was used to generate the unbinned fit. The possible functions are the only 
                realistic function is "fitTF1D". The only other 1D function possible is "fitTFLandauGaus".
  paramFile: This is the path to the file which contains the output table of parameters for the transfer
             function that the user is trying to have drawn. In theory, the Table object version of this
             file is also stored in the root file, but this is a more robust and file independent method
             of finding the correct fit parameters. The file should contain 10 parameters.
  highE: This tells the upper range of parton energy with which to plot the histograms. For
         example, if 500 is specified, then the first histogram will be for partons with an
         energy of 0 GeV to 25 GeV and the last histogram drawn will be for partons with an
         energy of 475 GeV to 500 GeV. If 1000 is specified, then the last histogram drawn
         will be for an energy of 975 GeV to 1000 GeV. This then specifies the number of pads
         drawn to the canvas.

Returns:
  N/A: The function graphically outputs a TCanvas* with a histogram and a function drawn.

Examples:
  From the command line:
    OverlayAll1D("B","fitTF1D","TF_TTbarMG_B.txt",1000)
*/

void OverlayAll1D(string channel, string functionName, string paramFile, double highE)
{
  setStyle(false);
  gStyle->SetOptStat("eMRi");
  //Figure out which fitting function to use and set the number of fit parameters
  unsigned int fitPars = 0;
  double (*fitFunc)(double*, double*) = NULL;
  fitFunc = findFitFunc(functionName,fitPars);
  vector<double> fitParams;
  vector<double> fitParamErrors;

  TH1D* hist;
  map<int,TF1*> fits;
  TH1D* fitHist;

  TCanvas* can = new TCanvas("can","can",1600,1000);
  if(highE == 500) can->Divide(5,4);
  else can->Divide(8,5);
  //loop through histograms
 for(unsigned int e=0; e<highE; e+=25)
   {
     can->cd((e/25)+1);
     string histoName = concatString("TF_Histo1D_",concatString(channel,concatString("_",concatString(e,concatString("GeV_",concatString(e+25,"GeV"))))));
     string histoTitle = "Transfer Function (" + channel + ") in Parton Energy From ";
     string fitName = concatString("TF_1D_",e);
     histoTitle = concatString(histoTitle,e) + "GeV to ";
     histoTitle = concatString(histoTitle,e+25) + "GeV";
     string newHistoTitle = concatString(e,concatString(" < Ep < ",e+25));
     hist = (TH1D*)gDirectory->Get(histoName.c_str());
     fits[e] = new TF1(fitName.c_str(),(*fitFunc),30-(e+25.),1000,fitPars);
     formatForDraw(hist);
     hist->SetTitle(newHistoTitle.c_str());
     fitParams = getFitPars(paramFile).first;
     fitParamErrors = getFitPars(paramFile).second;
     fitParams.push_back(e+12.5);
     setFitParams(fits[e], fitParams, fitParamErrors);
     hist->Sumw2();
     hist->Scale(1/(hist->Integral("width")));

     hist->Draw("HE");
     fits[e]->SetNpx(hist->GetNbinsX());
     fits[e]->Draw("sames");
     fits[e]->SetLineColor(2);
     can->Update();

     fitHist = (TH1D*)hist->Clone((((string)hist->GetName()) + "_clone").c_str());
     makeHistFromFunc(fitHist,fits[e]);
     printKSandChi2(hist,fitHist);
     can->Modified();
    }
}

/*****************************************************************************/

/**
OverlayDiffTF::
  This function is meant to overlay transfer functions of different processes, eta ranges, or parton flavors
  while keeping the parton energy the same. It is a graphical representation of how the transfer functions
  change with each of these parameters.

Parameters:
  functionName: This is the name of the function that was used to create these transfer functions using the
                unbinned fit (at least the 1D version of it). Generally this will be "fitTF1D."
  paramFiles: This is a vector of the locations of the transfer function parameter files (.txt files) with
              respect to where this function is being run. This vector must be created within the program
              or via the command line and fed into this function.
  partonEnergy: This is the exact parton energy that you want all of the transfer functions to be plotted at.

Returns:
  N/A: The function graphically outputs a TCanvas* with a histogram and a function drawn.

Examples:
  From the command line:
    vector<string> files                                                               
    files.push_back("../ConfigFiles/Official/TransferFunctions/TF_TTbarMG_B_0_24.txt") 
    files.push_back("../ConfigFiles/Official/TransferFunctions/TF_TTbarMG_B_0_13.txt") 
    files.push_back("../ConfigFiles/Official/TransferFunctions/TF_TTbarMG_B_13_24.txt")
    OverlayDiffTF("fitTF1D",files,50)
*/

void OverlayDiffTF(string functionName, vector<string> paramFiles, double partonEnergy)
{
  setStyle(false);
  gStyle->SetOptStat(0);
  //Figure out which fitting function to use and set the number of fit parameters
  unsigned int fitPars = 0;
  double (*fitFunc)(double*, double*) = NULL;
  fitFunc = findFitFunc(functionName,fitPars);
  vector<vector<double> > fitParams;
  vector<vector<double> > fitParamErrors;
  vector<TF1*> fits;

  TCanvas* can = new TCanvas("can","can",800,800);
  can->cd();
  TLegend* leg = new TLegend(0.7,0.4,1.0,0.6);

  for(unsigned int i=0; i<paramFiles.size(); i++)
    {
      size_t pos1 = paramFiles[i].find("TF_");
      size_t pos2 = paramFiles[i].find(".txt");
      size_t length = pos2-pos1;
      string fitName = paramFiles[i].substr(pos1,length);
      fits.push_back(new TF1(fitName.c_str(),(*fitFunc),30-partonEnergy,1000,fitPars));

      fitParams.push_back(getFitPars(paramFiles[i]).first);
      fitParamErrors.push_back(getFitPars(paramFiles[i]).second);
      fitParams.back().push_back(partonEnergy);

      setFitParams(fits.back(), fitParams[i], fitParamErrors[i]);
      if(i==0)
        {
          string canTitle = concatString("Ep = ",partonEnergy);
          fits.back()->Draw();
          fits.back()->SetTitle(canTitle.c_str());
        }
      else
        {
          fits.back()->Draw("sames");
          fits.back()->SetLineColor(i+1);
        }
      fits.back()->SetNpx(10000);
      fits.back()->GetXaxis()->SetTitle("E_{Jet}-E_{P} [GeV]");
      fits.back()->GetXaxis()->SetLabelSize(0.04);
      fits.back()->GetXaxis()->SetTitleSize(0.04);
      fits.back()->GetXaxis()->SetTitleOffset(1.2);
      leg->AddEntry(fits[i],fitName.c_str(),"l");
    }
  leg->SetFillColor(0);
  leg->Draw();
}

/*****************************************************************************/

/**
MakeGraphErrors::
  This function is designed to take a vector of similar TF1s and the number of parameters for each and
  then plot these fits as a function of parton energy with each parameters associated error. This function
  is specialized to work with the fitLandauGaus function below, but may be adapted for other uses.

Parameters:
  fits: This is a vector of TF1s that stores the root functions which have supposedly already been fit.
  fitPars: This is the number of fit parameters in each TF1. This is so the function knows how many
           different parameters it must plot.

Returns:
  vector<TGraphErrors*>: This function returns a vector of TGraphErrors pointers which can later be drawn.

Examples:
  From fitLandauGaus:
    vector<TGraphErrors*> graphs = MakeGraphErrors(fits,fitPars);
*/

vector<TGraphErrors*> MakeGraphErrors(vector<TF1*> fits,unsigned int fitPars)
{
  //Create the returning vector of Graphs
  vector< TGraphErrors*> graphs;

  //Do a basic check before proceeding
  if (fits.size()==0) 
    return graphs;

  for (unsigned int gg = 0; gg < fitPars; gg++){
    graphs.push_back(new TGraphErrors());
    stringstream ss;
    ss <<" Param"<<gg;
    graphs.back()->SetName(ss.str().c_str());
  }
  //Add an extra one for chi2/ndf
  graphs.push_back(new TGraphErrors());
  graphs.back()->SetName("Chi2/Ndf");
    
  //Loop over all the results
  for (unsigned int ff = 0; ff < fits.size(); ff++)
    {
      //Get the Ep value
      double Ep = 12.5+25*ff;
      //cout << "Looping over point " << ff <<  " with mean parton energy of " << Ep  << endl; 

      // Loop over the parameters
      for (unsigned int gg = 0; gg < fitPars; gg++)
        {
          //cout << "Looping over parameter " << gg << endl;
          int N = graphs[gg]->GetN();

          if(!TMath::IsNaN(fits[ff]->GetParameter(gg)))
            {
              graphs[gg]->SetPoint(N,Ep,fits[ff]->GetParameter(gg));// Parameter(gg));
              if(!TMath::IsNaN(fits[ff]->GetParError(gg)))
                {
                  graphs[gg]->SetPointError(N,0,fits[ff]->GetParError(gg));//ParError(gg));
                }
            }
        } //loop over parameters
      
      // The chi2/ndf plot
      if(!TMath::IsNaN(fits[ff]->GetChisquare()/fits[ff]->GetNDF()))
        {
          graphs[fitPars]->SetPoint(graphs[fitPars]->GetN(),Ep,fits[ff]->GetChisquare()/fits[ff]->GetNDF());
          graphs[fitPars]->SetPointError(graphs[fitPars]->GetN()-1,0,0);
        }
      
    } //for all fits
  
  return graphs;
  
}//MakeGraphErrors

/*****************************************************************************/

/**
fitLandauGaus::
  This function is designed to fit a Landau+Gaussian function to slices in parton energy of the E_j vs E_p
  histograms. This function overlays these fits and then draws the dependence of each parameter on E_p.
  There are currently two options available to the user which are hard coded into the program (one is
  currently commented out). Either they can use the user defined fitTFLandauGaus function or a combination
  of predefined root functions (a Gaussian function and a Landau function).

Parameters:
  cha: This is the name of the channel that the user would like analyzed.
  drawFits: This allows the user to choose whether or not to print the 40 hitogram (in parton energy) and
            their associated fits.
  drawParameters: This allows the user to choose whether or not to print the graphs of the fit parameters.

Returns:
  N/A: There are no returned objects from this function. However, several canvases will be drawn to the
       screen.

Examples:
  fitLandauGaus("B")
*/

void fitLandauGaus(string cha, bool drawFits, bool drawParameters)
{
  //Set the root style
  setStyle(false);
  //Figure out which fitting function to use and set the number of fit parameters
  unsigned int fitPars = 0;
  double (*fitFunc)(double*, double*) = NULL;
  fitFunc = findFitFunc("fitTFLandauGaus",fitPars);

  //Storage elements for the histograms and fits in memory
  vector<TH2D*> hists;
  vector<TF1*> fits;

  //Canvas for the histograms
  TCanvas* can1;
  TCanvas* can2;
  if(drawFits)
    {
      can1 = new TCanvas("can1","can",1200,900);
      can2 = new TCanvas("can2","can",1200,900);
      can1->Divide(5,4);
      can2->Divide(5,4);
    }

  cout << endl << "fitLandauGaus::Start retrieving histograms, making the fits, and fitting the histograms." << endl << "Start time: " << print_time() << endl;
  //Loop over the different slices in parton energy
  for(unsigned int e=0; e<1000; e+=25)
    {
      //Get and draw each histogram
      string histoNameJEvsPE = concatString("TF_Histo1D_",concatString(cha,concatString("_",concatString("JEvsPE_",concatString(e,concatString("GeV_",concatString(e+25,"GeV")))))));
      hists.push_back((TH2D*)gDirectory->Get(histoNameJEvsPE.c_str()));
      if(drawFits)
        {
          if(e<500) can1->cd((e/25)+1);
          else can2->cd(((e-500)/25)+1);
          hists.back()->Draw("HE");
          hists.back()->GetXaxis()->SetTitle("E_{Jet} [GeV]");
        }
      
      if((TH2D*)gDirectory->Get(histoNameJEvsPE.c_str()))
        {
          //Make the fit function and fit it
          string fitName = concatString("TF_Fit1D_LanGaus_",concatString(cha,concatString("_",concatString(e,concatString("GeV_",concatString(e+25,"GeV"))))));
          /*
          TF1* fit = new TF1(fitName.c_str(),(*fitFunc),30,1000,fitPars);
          fit->SetParameter(6,hists.back()->GetMean());
          fit->SetParameter(7,hists.back()->GetRMS());
          fit->FixParameter(9,12.5+e);
          */
          TF1* fit = new TF1(fitName.c_str(),"gaus(0)+landau(3)",30,1000);
          fit->SetParameters(hists.back()->GetEntries()/hists.back()->GetRMS(),
                             hists.back()->GetMean(),
                             hists.back()->GetRMS(),
                             hists.back()->GetEntries()/hists.back()->GetRMS(),
                             hists.back()->GetMean(),
                             hists.back()->GetRMS());
          fits.push_back(fit);
          if(drawFits) hists.back()->Fit(fits.back(),"S+MR");
          else hists.back()->Fit(fits.back(),"S+MRN");
        }
      else
        {
          cout << "fitLandauGaus::Unable to retrieve the histogram: " << histoNameJEvsPE << endl << "The function will now terminate." << endl;
          return;
        }
    }
  cout << "End time: " << print_time() << endl;

  if(drawParameters)
    {	
      cout << endl << "fitLandauGaus::Start making the parameter graphs." << endl << "Start time: " << print_time() << endl;
      //Create the vector of GraphErrors out of the fit parameters and their Chi2/NDF
      //vector<TGraphErrors*> graphs = MakeGraphErrors(fits,fitPars);
      vector<TGraphErrors*> graphs = MakeGraphErrors(fits,6);
      cout << "End time: " << print_time() << endl;
      
      cout << endl << "fitLandauGaus::Start drawing the parameter graphs." << endl << "Start time: " << print_time() << endl;
      //Display the fit parameter GraphErrors and Chi2/NDF	
      for (unsigned int gg = 0; gg < graphs.size(); gg++)
        {
          new TCanvas(graphs[gg]->GetName());
          graphs[gg]->Draw("AL");
        }
      cout << "End time: " << print_time() << endl;
    }

}

/*****************************************************************************/

/**
makeNormalizationHist::
  This function is no longer in use. However, it is kept here as part of the code in case it is ever
  needed. The purpose of this function was to plot how the normalization of the transfer functions
  changed with parton energy, assuming the normalization was linear in parton energy. We now know
  that the normalization is not linear with respect to the parton energy. The data points are the 
  value of the normalization "line" evaluated at a given parton energy divided by the integral of
  the transfer function for a given E_p range.

Parameters:
  parameters: This object contains the values of the transfer function parameters and their errors as
              well as the parameter names. Because this is such a complicated object, this function
              is not meant to be used through the command line, but is rather an internal function.

Returns:
  TProfile*: This function will return a profile histogram of the normalization as a function of the
             parton energy.

Examples:
  From TransferFunctions:
    normHists.push_back(makeNormalizationHist(parameters));
*/

TProfile* makeNormalizationHist(pair<vector<string>,pair<vector<double>, vector<double> > > parameters)
{
  double integral = 0;
  string histTitle = "Normalization Hist (" + channels[currentChannelIndex].name + ")";
  string histName = "TF_NormHist_" + channels[currentChannelIndex].name;
  TProfile* hist = new TProfile(histName.c_str(),histTitle.c_str(),200,0,1000);

  TF2* fx = makeFit(parameters);
  TF1* fy = new TF1("normTerm","pol1",0,1000);
  fy->FixParameter(0,parameters.second.first[0]);
  fy->FixParameter(1,parameters.second.first[1]);

  //Loop over the E_p bins
  for(unsigned int i=1; i<=200; i++)
    {
      integral = fx->Integral(30,1000,(i-1)*5,i*5);
      if(integral!=0) hist->Fill((i-0.5)*5,fy->Eval((i-0.5)*5)/integral);
    }//for(unsigned int i=1; i<=200; i++)

  return hist;
}

/*****************************************************************************/

/**
getMinosParameters::
  This function will retrieve and store the parameters, their names, and their errors, taken from the
  Minuit2Minimizer after the fitting process. This function was put in place to take care of all of the
  parameter shuffling in one spot without having to deal with pointers. This version of the function
  is specific to when the user would like to use the minos errors.

Parameters:
  gMinuit: This object is what fits the data and initially stores the parameters. Since the only way to
           retrieve these parameters is by using const double* arrays, this converts the output into
           a marginally better container.

Returns:
  pair<vector<string>, pair<vector<double>,vector<double> > >: This object stores the parameter values,
                                                               their names, and their errors.

Examples:
  From TransferFunctions:
    if(minos) parameters = getMinosParameters(Minimize(returnBool));
*/

pair<vector<string>, pair<vector<double>,vector<double> > > getMinosParameters(ROOT::Minuit2::Minuit2Minimizer* gMinuit)
{
  const double* pars = gMinuit->X();
  vector<double> parameters;
  vector<double> minosErrors;
  vector<string> parameterNames;
  double errLow = 0;
  double errUp = 0;

  for(unsigned int i=0; i<gMinuit->NDim(); i++)
    {
      parameters.push_back((double)pars[i]);
      parameterNames.push_back(gMinuit->VariableName(i));
      if(gMinuit->GetMinosError(i,errLow,errUp))
        {
          if(abs(errLow) == abs(errUp) && !TMath::IsNaN(errLow) && !TMath::IsNaN(errUp))
            {
              minosErrors.push_back(abs(errUp));
            }
          else if(abs(errLow) < abs(errUp) && !TMath::IsNaN(errLow) && !TMath::IsNaN(errUp))
            {
              minosErrors.push_back(abs(errUp));
            }
          else if(abs(errLow) > abs(errUp) && !TMath::IsNaN(errLow) && !TMath::IsNaN(errUp))
            {
              minosErrors.push_back(abs(errLow));
            }
          else
            {
              minosErrors.push_back(0);
            }
        }
      else
        {
          minosErrors.push_back(0);
        }
    }
  return make_pair(parameterNames,make_pair(parameters,minosErrors));
}

/*****************************************************************************/

/**
getParameters::
  This function will retrieve and store the parameters, their names, and their errors, taken from the
  Minuit2Minimizer after the fitting process. This function was put in place to take care of all of the
  parameter shuffling in one spot without having to deal with pointers.

Parameters:
  gMinuit: This object is what fits the data and initially stores the parameters. Since the only way to
           retrieve these parameters is by using const double* arrays, this converts the output into
           a marginally better container.

Returns:
  pair<vector<string>, pair<vector<double>,vector<double> > >: This object stores the parameter values,
                                                               their names, and their errors.

Examples:
  From TransferFunctions:
    else parameters = getParameters(Minimize(returnBool));
*/

pair<vector<string>, pair<vector<double>,vector<double> > > getParameters(ROOT::Minuit2::Minuit2Minimizer* gMinuit)
{
  const double* pars = gMinuit->X();
  const double* errs = gMinuit->Errors();
  vector<double> parameters;
  vector<double> errors;
  vector<string> parameterNames;

  for(unsigned int i=0; i<gMinuit->NDim(); i++)
    {
      parameters.push_back((double)pars[i]);
      if(TMath::IsNaN(errs[i]))
        {
          errors.push_back(0);
        }
      else
        {
          errors.push_back((double)errs[i]);
        }
      parameterNames.push_back(gMinuit->VariableName(i));
    }
  return make_pair(parameterNames,make_pair(parameters,errors));
}

/*****************************************************************************/

/**
fcn::
  This is the function that is being fit by minuit to the 2D histogram of E_j - E_p vs. E_p.
  The function, being 2-dimensional, is a function of x[1] = E_j - E_p and y = E_p. It has
  10 parameters, which are being modified by minuit every time it calles this function. Since
  this is an unbinned fit, there are no histograms involved. Instead, the functional takes each
  point in x and y (the energy for every jet with its associated genParton), performs a deltaR
  cut on the jets, and makes sure that this process is being done for the specific channel
  requested by the TransferFunctions function. All the while, fcn is performing a log-likelihood
  analysis for minuit. Most of the parameters for this function of used internally by minuit
  and do not need to be specified by the user. The function being fit is a normalized double
  Gaussian. In fact, however, this is not where the calculation of the function value occurs.
  Instead, this function calles fitTF2D. This method would be more appropriately called the log-
  likelihood function. Its purpose is really to simply return the likelihood of the chosen
  parameters being correct.

Parameters:
  par: This is an array of parameter values. Each time minuit calles this function, it sets the
       values in par to be slightly different in an attempt to determin which values give the
       best fit.

Returns:
  double: This function returns the sum of the negative logs of the funtion values. The goal is to
          maximize the sum by minimizing the value of fxy.

Examples:
  N/A: This function is never called explicitly by the user. It is set as the fit function in the
       Minimize function using the line:
         ROOT::Math::Functor f(&fcn,10);
*/

double fcn(const double *par)
{ 
  chain->SetBranchAddress("EvtNtuple", &ntuple);
  double sum=0.; 
  double x,y;
  double de;
  double fxy;
  double ly;
  int h = 0;
  int neg_counter = 0;
  TF2 fit("fit",fitTF2D,-1000,1000,0,1000,10);
  for (int i = 0; i<nEventNtuple; i++)
    { 
      chain->GetEntry(i);
      for(unsigned int j=0; j<ntuple->matchedGenParticles.size(); j++)
        {
          if((ntuple->matchedDeltaR[j] < maxDeltaR) && (abs(ntuple->jLV[j].Eta()) >= channels[currentChannelIndex].minEta) && (abs(ntuple->jLV[j].Eta()) <= channels[currentChannelIndex].maxEta))
            {
              h++;
              for(unsigned int k=0; k<channels[currentChannelIndex].pdgids.size(); k++)
                {
                  if(abs(ntuple->matchedpdgId[j]) == channels[currentChannelIndex].pdgids[k])
                    {
                      x = ntuple->jLV[j].Energy(); //Jet Energy
                      y = ntuple->matchedGenParticles[j].E(); //Parton Energy

                      de = x-y;
                      ly = y;
                      
                      fit.FixParameter(0,par[0]);
                      fit.FixParameter(1,par[1]);
                      fit.FixParameter(2,par[2]);
                      fit.FixParameter(3,par[3]);
                      fit.FixParameter(4,par[4]);
                      fit.FixParameter(5,par[5]);
                      fit.FixParameter(6,par[6]);
                      fit.FixParameter(7,par[7]);
                      fit.FixParameter(8,par[8]);
                      fit.FixParameter(9,par[9]);
                      fxy = fit.Eval(de,ly);

                      if(fxy<=0 || TMath::IsNaN(fxy) || TMath::IsNaN(log(fxy)))
                      {
                         neg_counter++;
                         cout<<"**********************"<<fxy<<"  "<<h<<endl;
                         cout<<"---------------------"<<sum<<"  "<<endl;
                         cout<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<"  "<<par[3]<<"  "<<par[4]<<endl;
                         cout<<par[5]<<"  "<<par[6]<<"  "<<par[7]<<"  "<<par[8]<<"  "<<par[9]<<endl;
                      }
                      else
                        {
                          if(fxy>pow(10.0,307))
                            {
                              cout << endl << "fxy = " << fxy << endl << "Jet Energy (x) = " << x << "\tParton Energy (y) = " << ly << "\tde = " << de << endl;
                              cout<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<"  "<<par[3]<<"  "<<par[4]<<endl;
                              cout<<par[5]<<"  "<<par[6]<<"  "<<par[7]<<"  "<<par[8]<<"  "<<par[9]<<endl<<endl;
                            }
                          sum -= log(fxy);
                        }
                    }
                  else
                    {
                      continue;
                    }
                }//for(unsigned int k=0; k<channels[currentChannelIndex].pdgid.size(); k++)
            }//if((ntuple->matchedDeltaR[j] < maxDeltaR) && (abs(ntuple->jLV[j].Eta()) >= channels[currentChannelIndex].minEta) && (abs(ntuple->jLV[j].Eta()) <= channels[currentChannelIndex].maxEta))
        }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
    }//for (int i = 0; i<nEventNtuple; i++)
  
  if(neg_counter > 0) cout << "Times when fxy< 0: " << neg_counter << endl;

  return sum;
}

/*****************************************************************************/

/**
Minimize::
  Minimize does as its name suggests, it runs the minuit minimization process. Specifically,
  this function uses the Migrad minimization algorithm. The user has the option to use the Minos
  algorithm for better error estimation. This process is initiated in the TransferFunction function,
  which is where the option to use Minos is set.

Parameters:
  minos: This is the boolean passed from the TransferFunctions function that tells the minuit fitter
         if it should use minos error estimation. A value of true indicates that minos will be used.

Returns:
  TMinuit*: The function returns a pointer to a TMinuit object. This object contains all of the values
            associated with the fitting process (ex: paramaters and their associated errors, flags, etc.).

Examples:
  From TransferFunctions:
    TMinuit * gMinuit = Minimize(minos);
*/

ROOT::Minuit2::Minuit2Minimizer* Minimize(bool &returnBool)
{
  //Set up the Minuit2Minimizer object
  ROOT::Minuit2::Minuit2Minimizer* gMinuit = new ROOT::Minuit2::Minuit2Minimizer(ROOT::Minuit2::kMigrad);
  gMinuit->SetPrintLevel(3);
  gMinuit->SetStrategy(2);
  gMinuit->SetMaxFunctionCalls(5000);
  gMinuit->SetMaxIterations(5000);
  gMinuit->SetTolerance(0.1);
  gMinuit->SetErrorDef(0.5);
  gMinuit->SetValidError(true);


  //Set up the parameters and parameter step sizes
  //Correct set of initial parameteres for basic normalization.
  double parameter[10] = {0.,0.,20.,0.002,0.1,0.01,0.,0.,100.,0.1};
  //parameters from minuit
  //double parameter[10]={2.02305, -0.0567438, 6.01594e-08, 0.149738, 0.0266924, 0.0001515859, 33.6103, -0.105295, 19.9699, 0.21974};
  double step[10] = {0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001};

  //Set the function being used to find the maximum log-liklihood
  ROOT::Math::Functor f(&fcn,10); 
  gMinuit->SetFunction(f);

  // Set the free variables to be minimized!  
  gMinuit->SetVariable(0, "Par0", parameter[0], step[0]);
  gMinuit->SetVariable(1, "Par1", parameter[1], step[1]);
  gMinuit->SetLowerLimitedVariable(2, "Par2", parameter[2], step[2], 0.);
  gMinuit->SetLowerLimitedVariable(3, "Par3", parameter[3], step[3], 0.);
  gMinuit->SetLowerLimitedVariable(4, "Par4", parameter[4], step[4], 0.);
  gMinuit->SetLowerLimitedVariable(5, "Par5", parameter[5], step[5], 0.);
  gMinuit->SetVariable(6, "Par6", parameter[6], step[6]);
  gMinuit->SetVariable(7, "Par7", parameter[7], step[7]);
  gMinuit->SetLowerLimitedVariable(8, "Par8", parameter[8], step[8], 0.);
  gMinuit->SetLowerLimitedVariable(9, "Par9", parameter[9], step[9], 0.);
 
  //Perform the minimization
  if(gMinuit->Minimize())
    {
      cout << "Minimize::Minimization successful!" << endl << "Status = " << gMinuit->Status() << endl << "NCalls = " << gMinuit->NCalls() << endl << "EDM = " << gMinuit->Edm() << endl;
      gMinuit->PrintResults();
      returnBool = true;
    }
  else
    {
      cout << "Minimize::Minimization failed!" << endl << "Status = " << gMinuit->Status() << endl << "NCalls = " << gMinuit->NCalls() << endl << "EDM = " << gMinuit->Edm() << endl;
      gMinuit->PrintResults();
      returnBool = false;
    }

  return gMinuit;
}

/*****************************************************************************/

////////////////////////////
// ////////////////////// //
// // Main Subroutines // //
// ////////////////////// //
////////////////////////////

/**
TransferFunctions::
  TransferFunctions takes the path of an EventNtuple and outputs a table of jet energy transfer functions
  (one for each flavor of jet). In addition, the function will create a histogram for each jet flavor in bins
  of E_jet - E_parton (among other histograms). The transfer function parameters are saved as Table objects
  in a root file as well as in individual text files.

Parameters:
  minos: A boolean which tells the program's Minuit algorithm whether or not to use the minos error algorithm.
         Minos takes into account non-linearities which will provide the user with a better estimation of the
         errors on the fit parameters. True tells the program to use minos, while false tells it to not use
         minos.
  process: A string which tells the program which physics process the input events are comming from. This is
           really not important for the making of the transfer functions, but it designates the naming scheme
           of the output root and text files.
  inputFilepath: This string is the global path of the input rootfile that should come right after the selection
                 process. There should be an EvtTree in the file. An example of this filepath can be seen
                 below.

Returns:
  N/A: This is a void function, so there is no real return value. However, this function does create a
        number of text files, one for each channel (ex: USD, C, etc.), which containes the parameters,
        in table format, of the transfer function for that specific channel. This function also creates
        and writes to a root file that stores Tables, functions, and histograms associated with the
        transfer functions.

Examples:
  How to run the program without loading it first (this will operate the main function of this macro):
    .x TransferFunctions.C++(false,"ttbar","/uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/Selection/bin/TTBarTest_status3/SelectionTest_outfile.root")
    
  How to run the program after TransferFunctions.C has been compiled and loaded:
    TransferFunctions(false,"TTbarMG","/uscms_data/d1/ilyao/Stage2SkimResults_3_8_7/MC/TTbarMG_outfileAll.root")
    TransferFunctions(true,"TTbarMG","/uscms_data/d1/ilyao/Stage2SkimResults_3_8_7/MC/TTbarMG_outfileAll.root")
    (The second one takes a lot longer due to the presence of the minos=true parameter)
    TransferFunctions(false,"TTbarMG","/uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/Selection/bin/TTBarTest_status3/SelectionTest_outfile.root")
    (For a shorter fun time)
*/

void TransferFunctions (bool minos = false, string process = "unknown" , string inputFilepath = "", bool debug = false) 
{
  //Print some initial output messages.
  cout << endl << "*****************************************************************************" << endl;
  cout << endl << "TransferFunctions::Function started at " <<  print_time() << endl;
  
  if(inputFilepath=="" || inputFilepath.substr(inputFilepath.size()-5,5)!=".root")
    {
      cout << "TransferFunctions::No rootfile was indicated in the function parameters. The function will terminate now." << endl;
      cout << "*****************************************************************************" << endl << endl;
      return;
    }

  //Set the style parameters for the root canvases
  cout << "TransferFunctions::Setting the default root style options." << endl;
  setStyle(true);

  channels = GetListOfChannels();

  cout << endl << "TransferFunctions::Start making the TChain of events." << endl << "Start time: " << print_time() << endl;
  //Checks if the dataset was successfully located and is ready to be read in
  //Otherwise the program will terminate
  if(!makeChain(inputFilepath))
    {
      cout << "TransferFunctions::The function will now exit" << endl;
      cout << endl << "*****************************************************************************" << endl << endl;
      return;
    }
  nEventNtuple = static_cast<int>(chain->GetEntries());
  cout << "End time: " << print_time() << endl;

  //Creates the storage elements for the output tbales, functions, and histograms
  vector<Table*> tables;
  vector<TF2*> fits;
  vector<TH1D*> normHists;
  map<string,TH1D*> histoMap1D;
  map<string,TH2D*> histoMap2D;

  //Tells the user is the fitting minimization was successful
  bool returnBool = false;

  //Creates the output root file
  string outputFilename = filename_creator(process,".root",debug);
  TFile* file = TFile::Open(outputFilename.c_str(),"RECREATE");

  //Loop over all channels
  for (unsigned int i=0;i<channels.size(); i++)
    {
      //Set the current channel that the program is working on. This is a global variable that all function can use
      cout << endl << "TransferFunctions::Setting the current channel to " << channels[i].name << "." << endl << "Start time: " << print_time() << endl;
      currentChannelIndex=i;
      cout << "End time: " << print_time() << endl;

      //Analyze this channel and perform the minimization
      cout << endl << "TransferFunctions::Start fitting the recoJets and genParticles by their pdgId." << endl;
      cout << "TransferFunctions::Performing the minimization and retrieving of the fit parameters." << endl << "Start time: " << print_time() << endl;
      //ROOT::Minuit2::Minuit2Minimizer* gMinuit = Minimize(returnBool);
      pair<vector<string>, pair<vector<double>,vector<double> > > parameters;
      if(minos) parameters = getMinosParameters(Minimize(returnBool));
      else parameters = getParameters(Minimize(returnBool));
      if(!returnBool)
      {
        cout << "TransferFunctions::This channel will be skipped." << endl;
        cout << "End time: " << print_time() << endl;
        continue;
      }
      cout << "End time: " << print_time() << endl;
      
      cout << endl << "TransferFunctions::Make and print the transfer function table for the " << channels[i].name << " channel." << endl << "Start time: " << print_time() << endl;
      //Call a function that produces a Table out of the gMinuit
      tables.push_back(makeTable(parameters));
      //Print the table to the console
      tables.back()->printTable(cout);
      //Print the table to a file
      string ofname = filename_creator(string(process + "_" + channels[i].name),".txt",debug);
      tables.back()->printToFile(ofname);
      //Call a function that makes a fit out of the gMinuit
      fits.push_back(makeFit(parameters));
      fits.push_back(makeJEvsPEFit(fits.back()));
      cout << "End time: " << print_time() << endl;    

      delete gMinuit;
      
      cout << endl << "TransferFunctions::Start sorting the recoJets and genParticles by their pdgId and filling histograms." << endl << "Start time: " << print_time() << endl;
      //Make the histograms and place them into their appropriate maps
      HistogramsByFlavor(channels[i],histoMap1D,histoMap2D);
      HistogramsByFlavorAndEnergy(channels[i],histoMap1D);
      HistogramsNormalizedByPartonEnergy(channels[i],histoMap2D);
      //normHists.push_back(makeNormalizationHist(parameters)); 
      cout << "End time: " << print_time() << endl;

  }//for (unsigned int i=0;i<channels.size(); i++)

  //Save all of the collected information
  cout << endl << "Saving histograms and tables, for all channels, to a file."  << endl << "Start time: " << print_time() << endl;
  for(unsigned int i=0; i<tables.size(); i++)
    {
       tables[i]->Write();
    }
  makeNumJetsTable()->Write();
  makeAllJetsTable()->Write();
  makeEventContentTable(true,true)->Write();
  makeEventContentTable(true,false)->Write();
  makeEventContentTable(false,true)->Write();
  makeEventContentTable(false,false)->Write();
  makeInputParameterTable(minos,process,inputFilepath,debug)->Write();
  cout<< "\tTables saved" << endl;
  for(unsigned int i=0; i<fits.size(); i++)
    {
       fits[i]->Write();
    }
  cout<< "\tFits saved" << endl;
  for(unsigned int i=0; i<normHists.size(); i++)
    {
       normHists[i]->Write();
    }
  cout<< "\tNormalization histograms saved" << endl;
  for(map<string,TH1D*>::iterator it = histoMap1D.begin() ; it != histoMap1D.end(); it++)
    {
      (*it).second->Write();
    }
  cout<< "\t1D Histograms saved" << endl;
  for(map<string,TH2D*>::iterator it = histoMap2D.begin() ; it != histoMap2D.end(); it++)
    {
      (*it).second->Write();
    }
  cout<< "\t2D Histograms saved" << endl;
  cout << "End time: " << print_time() << endl;

  cout << endl << "TransferFunctions::End of Function" << endl;
  cout << "*****************************************************************************" << endl << endl;
}

/*****************************************************************************/

/**
runAllTF::
  This function automates the process of running many samples through the TransferFunctions macro/function.
  This assumes that a FileLocationTable of sample locations, as well as the associated samples/datasets, has
  already been created.

Parameters:
  minos: This boolean is passed onto the TransferFunctions function. Tells the program's Minuit algorithm
         whether or not to use the minos error algorithm. When the boolean is set to true, minos will be
         used.
  MEInputFileLocation: This string indicated the location of the Table which stores the locations of the
                       datasets that the user would like to run over. The Table will be parsed into the
                       sample name and sample location before being used as input parameters for
                       TransferFunctions (the function).

Returns:
  N/A: This is a void function whose sole purpose is to automate the process of running a repetative task.

Examples:
  This is a shortened version of the run process which will run without minos and use the default location
  for the input table (minos could also be set to true):
    runAllTF(false)
  This is an example of the function using all the parameters:
    runAllTF(true,"/uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/ConfigFiles/Official/MEInputFileLocation.txt")
*/

void runAllTF(bool minos, string MEInputFileLocation = "/uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/ConfigFiles/Official/MEInputFileLocation.txt", bool debug = false)
{
  //Print some initial output messages.
  cout << endl << endl << "*****************************************************************************" << endl;
  cout << endl << "runAllTF::Program started at " <<  print_time() << endl;

  FileLocationTable* table = new FileLocationTable("MEInputFileLocation");
  table->parseFromFile(MEInputFileLocation);
  table->addBasePath();
  vector<TableRow> rows = table->getRows();
  vector<TableCell*> row;
  string process;
  TableCellText * cell;

  for(unsigned int i=0; i<rows.size(); i++)
    {
      row = rows[i].getCellEntries();
      process = rows[i].GetName();
      cell = (TableCellText*) row[0];
      if(cell)
        {
          if(debug) cout << "runAllTF::running TransferFunctions(" << minos << "," << process << "," << cell->text << "," << "true" << ")" << endl;
          else cout << "runAllTF::running TransferFunctions(" << minos << "," << process << "," << cell->text << "," << "false" << ")" << endl;
          TransferFunctions(minos,process,cell->text,debug);
        }
    }
  cout << "runAllTF::End of Program" << endl << endl;
  cout << "*****************************************************************************" << endl << endl;
}
