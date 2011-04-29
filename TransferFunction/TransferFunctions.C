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

  channel(string _name): name(_name),
                         numJets(0),
                         numJetsPart(0),
                         numJetsAntiPart(0),
                         numJetsFailDR(0){ }

  string name;
  vector<int> pdgids;
  //Counts the number of jets of a certain type
  int numJets;
  int numJetsPart;
  int numJetsAntiPart;
  int numJetsFailDR;
};

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

  //The UDS channel
  channels.push_back(channel("UDS"));
  channels.back().pdgids.push_back(1);
  channels.back().pdgids.push_back(2);
  channels.back().pdgids.push_back(3);
  
  //The C channel
  channels.push_back(channel("C"));
  channels.back().pdgids.push_back(4);

  //The B channel
  channels.push_back(channel("B"));
  channels.back().pdgids.push_back(5);

  //The G channel
  channels.push_back(channel("G"));
  channels.back().pdgids.push_back(21);

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
  This function is currently not being used. Its purpose is to generate a filename and path for an output
  file which follows a specific patter. The reason this function exists, however, is to append the name
  of the file with the date that the file was created on. Using this function, two files created on
  different days, who may otherwise have the same name, will be given unique names. This does not protect
  files which are created on the same day. An implementation of this protection may come later. In addition,
  the ability to add a changing file name, instead of a hardcoded patter, may be added.

Parameters:
  N/A: There are no parameters to this function.

Returns:
  string: This function returns a properly formated filename and path.

Examples:
  N/A: This function is currently not being used in this program.
  Possible Uses: string myfilename = filename_creator();
*/

string filename_creator()
{
   /*Sets up the filename of the .ROOT file where the graphs will be stored*/
   time_t rawtime;
   struct tm *timeinfo;
   //Buffer to store the converted c_string
   char buffer[35];
   //Get the current time
   time (&rawtime);
   //Turn the current time into a local time format and save it in the struct
   timeinfo = localtime(&rawtime);
   //Set up the c_string command that will format the filename
   string filename_template = "%Y-%b-%d-TransferFunctions";
   filename_template += ".root";
   strftime(buffer,30, filename_template.c_str(), timeinfo);
   //Change the first letter of the month to lowercase
   buffer[3] =  buffer[3] + 32;
   //Make sure the file is printed to the correct directory
   string mystring = "~aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/TransferFunction/";
   //Add the formatted filename to the directory name
   mystring += buffer;
   //Return the path/filename of of the root file
   return mystring;
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
      tableRow->addCellEntries(tableCellText);
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

Table* makeTable(TMinuit* gMinuit)
{
  Table* table = new Table("TF_Par_"+channels[currentChannelIndex].name);
  TableRow* tableRow;
  TableCellVal* tableCellVal;
  int  i1;
  double pars[4];
  TString chnam;

  for(int i=0; i<gMinuit->GetNumPars(); i++)
    {
      //Reads out the information from the TMinuit object and places it infomation in the pars array
      //mnpout provides the user with information concerning the current status of the fit
      gMinuit->mnpout(i,chnam,pars[0],pars[1],pars[2],pars[3],i1);
      tableRow = new TableRow(string(chnam));

      tableCellVal = new TableCellVal("Parameters");
      tableCellVal->val.value = pars[0];
      tableCellVal->val.error = pars[1];
      tableRow->addCellEntries(tableCellVal);

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
  selected sample.

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
      tableRow->addCellEntries(tableCellText);
      //Clears the stringstream
      out.str("");

      tableCellText = new TableCellText(cellTitles[1]);
      totalPart += channels[i].numJetsPart;
      out << channels[i].numJetsPart;
      tableCellText->text = out.str();
      tableRow->addCellEntries(tableCellText);
      out.str("");

      tableCellText = new TableCellText(cellTitles[2]);
      totalAntiPart += channels[i].numJetsAntiPart;
      out << channels[i].numJetsAntiPart;
      tableCellText->text = out.str();
      tableRow->addCellEntries(tableCellText);
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
  tableRow->addCellEntries(tableCellText);
  out.str("");
  tableCellText = new TableCellText("Particle Parton");
  out << 0;
  tableCellText->text = out.str();
  tableRow->addCellEntries(tableCellText);
  tableCellText = new TableCellText("Anti-Particle Parton");
  tableCellText->text = out.str();
  tableRow->addCellEntries(tableCellText);
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
  tableRow->addCellEntries(tableCellText);
  out.str("");
  tableCellText = new TableCellText(cellTitles[1]);
  out << totalPart;
  tableCellText->text = out.str();
  tableRow->addCellEntries(tableCellText);
  out.str("");
  tableCellText = new TableCellText(cellTitles[2]);
  out << totalAntiPart;
  tableCellText->text = out.str();
  tableRow->addCellEntries(tableCellText);
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
      tableRow->addCellEntries(tableCellText);

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
  tableRow->addCellEntries(tableCellText);
  
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

Table* makeEventContentTable(bool deltaRCut)
{
  Table* table;
  //Changes the name of the table depending upon whether of not the user wants the event content with or
  //without the deltaR cut included
  if(deltaRCut)
    {
      table = new Table("EventContentTableWithDeltaRCut");
    }
  else
    {
      table = new Table("EventContentTable");
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
          if(deltaRCut && ( ntuple->matchedDeltaR[j]>maxDeltaR || ntuple->matchedDeltaR[j+1]>maxDeltaR) )
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
      tableRow->addCellEntries(tableCellText);
      
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
  tableRow->addCellEntries(tableCellText);
  
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
  histoMap1D[cha.name] = new TH1D(histoName1D.c_str(),histoTitle.c_str(),200,-500,500);
  histoMap2D[cha.name] = new TH2D(histoName2D.c_str(),histoTitle.c_str(),200,-500,500,100,0,500);
  histoMap2D[string(cha.name+"_JEvsPE")] = new TH2D(histoName2D_2.c_str(),histoTitle.c_str(),100,0,500,100,0,500);
  for (int i = 0; i<nEventNtuple; i++)
    { 
      chain->GetEntry(i);
      for(unsigned int j=0; j<ntuple->matchedGenParticles.size(); j++)
        {
          if(ntuple->matchedDeltaR[j] < maxDeltaR)
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
            }//if(ntuple->matchedDeltaR[j] < maxDeltaR)
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
  TH2D* outputHist = new TH2D(normHistName.c_str(),histoTitle.c_str(),100,0,500,100,0,500);
  TH1D* projHist;
  for(int i=0; i<=inputHist->GetNbinsY()+1; i++)
    {
      projHist = inputHist->ProjectionX("_px",i,i,"e");
      projHist->Sumw2();
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
  for(unsigned int e=0; e<500; e+=25)
    {
      string histoName = concatString("TF_Histo1D_",concatString(cha.name,concatString("_",concatString(e,concatString("GeV_",concatString(e+25,"GeV"))))));
      string histoTitle = "Transfer Function (" + cha.name + ") in Parton Energy From ";
      histoTitle = concatString(histoTitle,e) + "GeV to ";
      histoTitle = concatString(histoTitle,e+25) + "GeV";
      histoMap1D[histoName] = new TH1D(histoName.c_str(),histoTitle.c_str(),200,-500,500);
      for (int i = 0; i<nEventNtuple; i++)
        { 
          chain->GetEntry(i);
          for(unsigned int j=0; j<ntuple->matchedGenParticles.size(); j++)
            {
              if(ntuple->matchedDeltaR[j] < maxDeltaR)
                {
                  {
                    for(unsigned int k=0; k<cha.pdgids.size(); k++)
                      {
                        if(abs(ntuple->matchedpdgId[j]) == cha.pdgids[k])
                          {
                            if(ntuple->matchedGenParticles[j].E()>e && ntuple->matchedGenParticles[j].E()<=(e+25))
                              {
                                histoMap1D[histoName]->Fill(ntuple->jLV[j].Energy() - ntuple->matchedGenParticles[j].E());
                              }
                          }//if(abs(ntuple->matchedpdgId[j]) == cha.pdgids[k])
                      }//for(unsigned int l=0; l<channels[currentChannelIndex].pdgid.size(); l++)
                  }//if(ntuple->matchedDeltaR[j] < maxDeltaR)
                }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
            }//for (int i = 0; i<nEventNtuple; i++) 
        }//for (int i = 0; i<nEventNtuple; i++)
    }//for(unsigned int e=0; e<500; e+=25)
}

/*****************************************************************************/

/**
fitTF2D::
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
    TF1* fit = new TF1("TF_1D",(*fitFunc),-500,500,fitPars);
*/

double fitTF1D(double *x, double *par)
{
  double fxy;
  fxy = exp(-.5*pow(x[0]-(par[0]+par[1]*par[10]),2)/(pow((par[2]+par[3]*par[10]),2))); 
  fxy = fxy+(par[4]+par[10]*par[5])*exp(-.5*pow(x[0]-(par[6]+par[7]*par[10]),2)/(pow((par[8]+par[9]*par[10]),2)));
  fxy = fxy/((par[2]+par[3]*par[10])+(par[4]+par[5]*par[10])*(par[8]+par[9]*par[10]));
  fxy = fxy/(sqrt(2.*TMath::Pi()));
  return fxy;
}

/*****************************************************************************/

/**
fitTF2D::
  This is a 2D function used to fit a histogram of E_j -E_p vs E_p values. E_j - E_p is on the x-axis
  and E_p is on the y-axis. This is not the unbinned fit, but rather a function used primarily to
  draw the resulting transfer function once the parameters have been set. It, however, has the same
  form/mathematical equation as the unbinned fit. This function also uses 10 parameters

Parameters:
  x: This is the array that stores the variable values of the function. It is not directly user accessible.
  par: This is the array that stores the function parameters. It is not directly user accessible.

Returns:
  double: This function returns the numberical result of the function once it is evaluated.

Examples:
  From makeFit:
    TF2* fit = new TF2(fitName.c_str(),fitTF2D,-500,500,0,500,10);
*/

double fitTF2D(double *x, double *par)
{
  double fxy;
  fxy = exp(-.5*pow(x[0]-(par[0]+par[1]*x[1]),2)/(pow((par[2]+par[3]*x[1]),2))); 
  fxy = fxy+(par[4]+x[1]*par[5])*exp(-.5*pow(x[0]-(par[6]+par[7]*x[1]),2)/(pow((par[8]+par[9]*x[1]),2)));
  fxy = fxy/((par[2]+par[3]*x[1])+(par[4]+par[5]*x[1])*(par[8]+par[9]*x[1]));
  fxy = fxy/(sqrt(2.*TMath::Pi()));  
  return fxy;
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
    TF2* newfit = new TF2(fitName.c_str(),fitTFJEvsPE,0,500,0,500,10);
*/

double fitTFJEvsPE(double* x, double *par)
{
  return JEvsPEfit->Eval(x[0]-x[1],x[1]);
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
  if(fitFuncName == "fitTFJEvsPE")
    {
      fitPars = 10;
      return &fitTFJEvsPE;
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

TF2* makeFit(TMinuit* gMinuit)
{
  int  i1;
  double pars[4];
  TString chnam;
  string fitName = "TF_Fit2D_"+channels[currentChannelIndex].name;
  TF2* fit = new TF2(fitName.c_str(),fitTF2D,-500,500,0,500,10);

  for(int i=0; i<gMinuit->GetNumPars(); i++)
    {
       //mnpout provides the user with information concerning the current status of the fit
      gMinuit->mnpout(i,chnam,pars[0],pars[1],pars[2],pars[3],i1);
      fit->SetParameter(i,pars[0]);
      fit->SetParError(i,pars[1]);      
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
  TF2* newfit = new TF2(fitName.c_str(),fitTFJEvsPE,0,500,0,500,10);
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
  st->SetName("updatedStats");
  st->AddText(Form("KolmogorovTest = %g",hist->KolmogorovTest(fitHist)));
  st->AddText(Form("Chi2Test = %g",fitHist->Chi2Test(hist,"UW")));
  hist->SetStats(0); //to avoid a new "stats" to be drawn 
 }

/*****************************************************************************/

/**
formatForDraw::
  This function is called by OverlayFunction in order to set the drawing options like titles, axis
  labels, and font size. This function is called when the histograms and functions being drawn are
  1D.

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
          TF2* fit = new TF2("TF_2D",(*fitFunc),-500,500,0,500,fitPars);
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
          TF2* fit = new TF2((fitName+"_binned").c_str(),fitTFJEvsPE,0,500,0,500,10);
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
      TF1* fit = new TF1("TF_1D",(*fitFunc),-500,500,fitPars);
      formatForDraw(hist);

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
fcn::
  This is the function that is being fit by minuit to the 2D histogram of E_j - E_p vs. E_p.
  The function, being 2-dimensional, is a function of x[1] = E_j - E_p and y = E_p. It has
  10 parameters, which are being modified by minuit every time it calles this function. Since
  this is an unbinned fit, there are no histograms involved. Instead, the functional takes each
  point in x and y (the energy for every jet with its associated genParton), performs a deltaR
  cut on the jets, and makes sure that this process is being done for the specific channel
  requested by the TransferFunctions function. All the while, fcn is performing a log-likelyhood
  analysis for minuit. Most of the parameters for this function of used internally by minuit
  and do not need to be specified by the user. The function being fit is a normalized double
  Gaussian.

Parameters:
  f: Stores the running sum of the negative log of the value of the function (a positive value
     since the value of the normalized double Gaussian should be less than one). This value is
     only ever read internally by minuit and does not need to be returned explicitly by the user.
  par: This is an array of parameter values. Each time minuit calles this function, it sets the
       values in par to be slightly different in an attempt to determin which values give the
       best fit.

Returns:
  N/A: There are no returns for this function and all of the paramters are handled and returned
       internally by minuit.

Examples:
  N/A: This function is never called explicitly by the user. It is set as the fit function in the
       Minimize function using the line:
         gMinuit->SetFCN(fcn);
       Internal parameter names:
         void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
*/

void fcn(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t)
{ 
  chain->SetBranchAddress("EvtNtuple", &ntuple);
  f=0.; 
  double sum=0.; 
  double x,y;
  double de;
  double fxy;
  double ly;
  int h = 0;
  for (int i = 0; i<nEventNtuple; i++)
    { 
      chain->GetEntry(i);
      for(unsigned int j=0; j<ntuple->matchedGenParticles.size(); j++)
        {
          if(ntuple->matchedDeltaR[j] < maxDeltaR)
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
                      
                      fxy = exp(-.5*pow(de-(par[0]+par[1]*ly),2)/(pow((par[2]+par[3]*ly),2))); 
                      fxy = fxy+(par[4]+ly*par[5])*exp(-.5*pow(de-(par[6]+par[7]*ly),2)/(pow((par[8]+par[9]*ly),2)));
                      fxy = fxy/((par[2]+par[3]*ly)+(par[4]+par[5]*ly)*(par[8]+par[9]*ly));
                      fxy = fxy/(sqrt(2.*3.141592654));    
                      
                      if(fxy<0){
                        cout<<"**********************"<<fxy<<"  "<<h<<endl;
                        cout<<"---------------------"<<sum<<"  "<<endl;
                        cout<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<"  "<<par[3]<<"  "<<par[4]<<endl;
                        cout<<par[5]<<"  "<<par[6]<<"  "<<par[7]<<"  "<<par[8]<<"  "<<par[9]<<endl;
                      }
                      
                      sum = sum - log(fxy);
                    }
                  else
                    {
                      continue;
                    }
                }//for(unsigned int l=0; l<channels[currentChannelIndex].pdgid.size(); l++)
            }//if(ntuple->matchedDeltaR[j] < maxDeltaR)
        }//for(unsigned int j=0; j<ntuple->matchedGenParticles.Size(); j++)
    }//for (int i = 0; i<nEventNtuple; i++)
  
  f = sum;  
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

//Minimization
TMinuit* Minimize(bool minos)
{
  int ierflg = 0;
  //char chnam[100];
  TString chnam;
  
  TMinuit *gMinuit = new TMinuit(10);       //initialize TMinuit 10 is the maximun number of parameters used by Minuit
  gMinuit->mninit(5,6,7);                   //It initializes some constants
  gMinuit->SetFCN(fcn);                     //fcn store as a pointer, SetFCN used to define it.

  //double parameter[10]={18.854,-0.243698,3.5811,0.197366,0,0.012156,18.5608,-0.1316,8.68633, 0.031137};
  double parameter[10]={2.02305, -0.0567438, 6.01594e-08, 0.149738, 0.0266924, 0.0001515859, 33.6103, -0.105295, 19.9699, 0.21974};

  double a0 = 3.;
  gMinuit->mnexcm("SET PRI", &a0,1, ierflg);  
  
  gMinuit->mnparm(0, "Par0", parameter[0], .001, 0.,0.,ierflg);   
  gMinuit->mnparm(1, "Par1", parameter[1], .001, 0.,0.,ierflg);   //Implements one parameter definition
  gMinuit->mnparm(2, "Par2", parameter[2], .001, 0.,100.,ierflg);   //p[0] starting value, .001 starting step size 0, 0 no limits
  gMinuit->mnparm(3, "Par3", parameter[3], .001, 0.,100.,ierflg); 
  gMinuit->mnparm(4, "Par4", parameter[4], .001, 0.,10.,ierflg); //These 2 parameters need to be bound so Minuit won't use
  gMinuit->mnparm(5, "Par5", parameter[5], .001, 0.,10.,ierflg); // negative values. lowlimit 0., upperlimit 10.
  gMinuit->mnparm(6, "Par6", parameter[6], .001, 0.,0.,ierflg);
  gMinuit->mnparm(7, "Par7", parameter[7], .001, 0.,0.,ierflg);
  gMinuit->mnparm(8, "Par8", parameter[8], .001, 0.,100.,ierflg);
  gMinuit->mnparm(9, "Par9", parameter[9], .001, 0.,100.,ierflg);
 

  double a1 = .5;
  gMinuit->mnexcm("SET ERR", &a1 ,1, ierflg);    

  double a2 = 1000; //number of iterations
  gMinuit->mnexcm("MIGRAD", &a2 ,1, ierflg);
  if(minos)
    {
      gMinuit->mnexcm("MINOS", &a2,1, ierflg);   // add another minimization comand.
      // It is slower but more precise
    }

  cout << "flag = " << ierflg << endl;
 
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
*/

void TransferFunctions (bool minos = false, string process = "unknown" , string inputFilepath = "") 
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
  //gStyle->SetFillColor(0);
  gStyle->SetOptStat("neMR");
  gStyle->SetOptFit(2211);
  gStyle->SetPalette(1);
  gStyle->SetTitleColor(0,"c");
  gStyle->SetTitleFillColor(0);

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
  map<string,TH1D*> histoMap1D;
  map<string,TH2D*> histoMap2D;

  //Creates the output root file
  string outputFilename = "TF_" + process + ".root";
  TFile* file = TFile::Open(outputFilename.c_str(),"RECREATE");

  //Loop over all channels
  for (unsigned int i=0;i<channels.size(); i++)
    {
      cout << endl << "TransferFunctions::Start fitting the recoJets and genParticles by their pdgId." << endl << "Start time: " << print_time() << endl;
      //Set the current channel that the program is working on. This is a global variable that all function can use
      currentChannelIndex=i;
      //Analyze this channel and perform the minimization
      TMinuit * gMinuit = Minimize(minos);
      cout << "End time: " << print_time() << endl;
      
      cout << endl << "TransferFunctions::Make and print the transfer function table for the " << channels[currentChannelIndex].name << " channel." << endl << "Start time: " << print_time() << endl;
      //Call a function that produces a Table out of the gMinuit
      tables.push_back(makeTable(gMinuit));
      //print the table to the console
      tables.back()->printTable(cout);
      //print the table to a file
      string ofname = "TF_" + process + "_" + channels[i].name + ".txt"; 
      tables.back()->printToFile(ofname);
      //Call a function that makes a fit out of the gMinuit
      fits.push_back(makeFit(gMinuit));
      fits.push_back(makeJEvsPEFit(fits.back()));
      cout << "End time: " << print_time() << endl;    

      delete gMinuit;
      
      cout << endl << "TransferFunctions::Start sorting the recoJets and genParticles by their pdgId and filling histograms." << endl << "Start time: " << print_time() << endl;
      //Make the histograms and place them into their appropriate maps
      HistogramsByFlavor(channels[i],histoMap1D,histoMap2D);
      HistogramsByFlavorAndEnergy(channels[i],histoMap1D);
      HistogramsNormalizedByPartonEnergy(channels[i],histoMap2D);
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
  makeEventContentTable(true)->Write();
  makeEventContentTable(false)->Write();
  cout<< "\tTables saved" << endl;
  for(unsigned int i=0; i<fits.size(); i++)
    {
       fits[i]->Write();
    }
  cout<< "\tFits saved" << endl;
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
  This assumes that a Table of sample locations, as well as the associated samples/datasets, has already
  been created.

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

void runAllTF(bool minos, string MEInputFileLocation = "/uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/ConfigFiles/Official/MEInputFileLocation.txt")
{
  //Print some initial output messages.
  cout << endl << endl << "*****************************************************************************" << endl;
  cout << endl << "runAllTF::Program started at " <<  print_time() << endl;

  FileLocationTable* table = new FileLocationTable("MEInputFileLocation");
  //Table* table = new Table("MEInputFileLocation");
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
          cout << "runAllTF::running TransferFunctions(" << minos << "," << process << "," << cell->text << ")" << endl;
          TransferFunctions(minos,process,cell->text);
        }
    }
  cout << "runAllTF::End of Program" << endl << endl;
  cout << "*****************************************************************************" << endl << endl;
}
