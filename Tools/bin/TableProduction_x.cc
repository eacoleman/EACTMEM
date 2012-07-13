//ReadTable_x -crossSectionFilePath /uscms/home/travlamb/CMSSW_5_2_5/src/TAMUWW/ConfigFiles/Official/CrossSections_8TeV.txt -rootFoldersFileLocation /uscms_data/d3/travlamb/PS_outfiles_20120621_NTUPLES/ -latexTemplateFilePath /uscms_data/d3/travlamb/Tables/TEMPLATE.tex -latexOutputFilePath /uscms_data/d3/travlamb/Tables/Tables.tex

//TEMP FOR FERMILAB
//TableProduction_x -crossSectionFilePath /uscms/home/travlamb/CMSSW_5_2_5/src/TAMUWW/ConfigFiles/Official/CrossSections_8TeV.txt -rootFoldersFileLocation /uscms_data/d3/travlamb/PS_outfiles_20120621_NTUPLES/ -latexTemplateFilePath /uscms_data/d3/travlamb/Tables/TEMPLATE.tex -latexOutputFilePath /uscms_data/d3/travlamb/Tables/Tables.tex -muonLuminosity 3575.666 -electronLuminosity 3568.051

//FOR ALEXX'S HIGGS SAMPLES
//TableProduction_x -crossSectionFilePath /uscms/home/travlamb/CMSSW_5_2_5/src/TAMUWW/ConfigFiles/Official/CrossSections_8TeV.txt -latexTemplateFilePath /uscms_data/d3/travlamb/Tables/TEMPLATE.tex -latexOutputFilePath /uscms_data/d3/travlamb/Tables/Tables.tex -muonLuminosity 3575.666 -electronLuminosity 3568.051 -rootFoldersFileLocation /uscms_data/d3/travlamb/PS_outfiles_20120709_MC_20/ -processNames WJets ggH125

//This file contains the functions to read several input files,
//and display yield tables, tables with added FracPass and EventsPerInvpb rows ("Enhanced Tables"),
//and FracPass and EventsPerInvpb tables
//
//Made by Travis Lamb
//
//TODO: If I am given the time, more code cleanup. Seperate TableProduction into multiple classes.

//CMSSW Libraries
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"

#include "TAMUWW/Tools/interface/PDFCreator.hh"

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

//Root Libraries
#include "TROOT.h"
#include "TFile.h"

//C++ Libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

//****************************************************************************************************//

//Used by TableProduction to produce Enhances Tables
class EnhancedTableProduction
{
public:
   EnhancedTableProduction();
   static Table produce(Table &input, double crossSect, double eventsFromMC);
private:
   static void addFracPassRow(Table &enhancedTable, double eventsFromMC);
   static void addEvtsPerInvpbRow(Table &enhancedTable, double crossSect);
};

//****************************************************************************************************//

//creates an enhanced table from a table passed into it
Table EnhancedTableProduction::produce(Table &input, double crossSect, double eventsFromMC = 0)
{
   //If we weren't given an events value, we take it from the top row of the table
   if (eventsFromMC == 0)
      eventsFromMC = ((TableCellVal*)input.getCellRowColumn("NPATtupleEvts", "jets0"))->val.value;
   
   //Copy the input table to our new table
   Table enhancedTable = input;
   
   //add the required rows
   addFracPassRow(enhancedTable, eventsFromMC);
   addEvtsPerInvpbRow(enhancedTable, crossSect);
   
   return enhancedTable;
}

void EnhancedTableProduction::addFracPassRow(Table &enhancedTable, double eventsFromMC)
{
   //this is the temporary row we will add to the table
   TableRow* tempRow = new TableRow;
   
   //for all columns 0 through 4
   for(int i = 0; i <= 4; i++)
   {
      //Some more temporary data structures
      Value* tempValue = new Value;
      TableCellVal* tempCell = new TableCellVal;
      double mete = 0;
      
      //grabs the mete from the table. Renames the cell.
      if(i != 1)
      {
         mete = ((TableCellVal*)enhancedTable.getCellRowColumn("c6:METE", "jets"+to_string(i)))->val.value;
         tempCell->SetName(("jets"+to_string(i)).c_str());
      }
      else
      {
         mete = ((TableCellVal*)enhancedTable.getCellRowColumn("c6:METE", "jet"+to_string(i)))->val.value;
         tempCell->SetName(("jet"+to_string(i)).c_str());
      }
      
      //FracPass is MissingTransverseEnergy times EventsFromMC. The error is the binomial error.
      double acceptance = mete / eventsFromMC;
      tempValue->value = acceptance;
      tempValue->error = sqrt((acceptance*(1.0 - acceptance)) / eventsFromMC);
      
      //store value in cell. Store cell in row
      tempCell->val=(*tempValue);
      tempRow->addCellEntries(tempCell);
      
      //cleanup
      //TODO: FIX: if we uncomment the lines below, then it segfaults
      //tempValue->~Value();
      //tempCell->~TableCellVal();
      //delete tempValue;
      //delete tempCell;
   }
   //name the row and add it to table
   tempRow->SetName("FracPass");
   enhancedTable.addRow(*tempRow);
   
   tempRow->~TableRow();
   delete tempRow;
}

void EnhancedTableProduction::addEvtsPerInvpbRow(Table &enhancedTable, double crossSect)
{
   //this is the temporary row we will add to the table
   TableRow* tempRow = new TableRow;
   
   //for all columns 0 through 4
   for(int i = 0; i <= 4; i++)
   {
      //Some more temporary data structures
      Value tempValue;
      TableCellVal* tempCell = new TableCellVal;
      
      //grabs the mete from the table. Renames the cell.
      if(i != 1)
      {
         tempValue = ((TableCellVal*)enhancedTable.getCellRowColumn("FracPass", "jets"+to_string(i)))->val;
         tempCell->SetName(("jets"+to_string(i)).c_str());
      }
      else
      {
         tempValue = ((TableCellVal*)enhancedTable.getCellRowColumn("FracPass", "jet"+to_string(i)))->val;
         tempCell->SetName(("jet"+to_string(i)).c_str());
      }
      
      //FracPass is MissingTransverseEnergy times EventsFromMC
      tempValue *= crossSect;
      
      //store value in cell. Store cell in row
      tempCell->val=(tempValue);
      tempRow->addCellEntries(tempCell);
      
      //cleanup
      //TODO: FIX: if we uncomment the lines below, then it segfaults
      //tempValue->~Value();
      //tempCell->~TableCellVal();
      //delete tempValue;
      //delete tempCell;
   }
   //name the row and add it to table
   tempRow->SetName("EvtsPerInvpb");
   enhancedTable.addRow(*tempRow);
   
   tempRow->~TableRow();
   delete tempRow;
}

//****************************************************************************************************//

//This class is used to read in files and output multiple tables
//NOTE: Currently assumes the following:
//1) That CrossSections are stored together in a text file
//2) That input tables are grouped by process into a root file that is stored in seperate folders
//   E.G. WW/PS.root stores MuonTable, ElectronTable, and LeptonTable
//3) That EventsFromMC are stored in the top row of the input tables
class TableProduction
{

private:
   //Stores file locations
   string crossSectionFilePath;
   string rootFoldersFileLocation;
   string latexTemplatePath;
   string writeRootPath;
   string writeFilePath;
   string writeLatexPath;
   
   //Stores luminosity values
   double muonLuminosity;
   double electronLuminosity;
   //Stores all the process names that are used as keys
   vector<string> processNames;
   
   //These are the input files
   //Maps store data with the processName as key
   map<string, double> crossSections;
   map<string, double> crossSectionErrors;
   map<string, double> eventsFromMC;
   map<string, double> eventsFromMCError;
   map<string, Table> muonTables;
   map<string, Table> electronTables;
   map<string, Table> leptonTables;
   
   //The following are output tables
   //Store Tables by process name
   map<string, Table> muonAcceptanceTables;
   map<string, Table> electronAcceptanceTables;
   map<string, Table> leptonAcceptanceTables;
   map<string, Table> muonInvpbTables;
   map<string, Table> electronInvpbTables;
   map<string, Table> leptonInvpbTables;
   map<string, Table> muonYieldTables;
   map<string, Table> electronYieldTables;
   map<string, Table> leptonYieldTables;
   map<string, Table> muonEnhancedTables;
   map<string, Table> electronEnhancedTables;
   map<string, Table> leptonEnhancedTables;
   //The following maps store the particle as the key
   //I.E. "Muon", "Electron", "Lepton"
   map<string, Table> fracPassTables;
   map<string, Table> eventsPerInvpbTables;
   //store table as cut names
   //I.E. "c5:NotLooseEl", "c6:METE"
   map<string, Table> muonCutYieldTables;
   map<string, Table> electronCutYieldTables;
   //Used to display our input data
   Table crossSectionsTable;
   Table eventsFromMCTable;
   
public:
   TableProduction();
   TableProduction(
      string crossSectionFilePathTEMP,
      string rootFoldersFileLocationTEMP,
      string textOutputFilePathTEMP,
      string rootOutputFilePathTEMP,
      string latexOutputFilePathTEMP,
      string latexTemplateFilePathTEMP,
      vector<string> &processNamesTEMP,
      double muonLuminosityTEMP,
      double ElectronLuminosityTEMP);
   
   //TODO This function
   ~TableProduction();
   
   //Basic set functions
   void setCrossSectionFilePath(string filePath);
   void setRootFoldersLocation(string fileLoc);
   void setWriteRootPath(string filePath);
   void setWriteFilePath(string filePath);
   void setLatexTemplatePath(string filePath);
   void setWriteLatexPath(string filePath);
   
   void setMuonLuminosity(double lum);
   void setElectronLuminosity(double lum);
   
   //sets the processNames vector to the given vector
   void setProcesses(vector<string> &processNamesTEMP);
   
   //adds a process to the process vector. Inputs will need to be reread
   void addProcess(string proc);
   
   //Stores CrossSections, input tables, and EventsFromMC
   void readAllInputs();
   
   //This creates all tables internally to prepare for printing and writing
   void formAllTables();
   
   //Writes to writeRootPath
   void writeAllTablesToRoot(string rootLoc);
   
   //Writes to writeLatexPath
   void writeAllTablesToLatex(string title, string author, string email, string templateLoc, string writeLoc);
   
private:
   //Opens and stores all values of CrossSections
   void readCrossSections();
   
   //Reads ALL input tables from all process folders
   void readInputTables();
   
   //Gets EventsFromMC from top rows of our input tables
   void readEventsFromMC();
   
   //Creates acceptance tables internally
   void formAcceptanceTables();

   //calculates errors for acceptance tables
   void calculateAndApplyError(map<string, Table> &tables);
   
   //Creates Invpb tables internally
   void formInvpbTables();
   
   //Creates yield tables internally
   void formYieldTables();
   
   //Creates enhanced tables internally
   void formEnhancedTables();
   
   //Creates FracPass tables internally
   void formFracPassTables();
   
   //creates a single fracPass Table
   Table formOneFracPassTable(map<string, Table> &enhancedTables);
   
   //creates EventsPerInvpb tables internally
   void formEventsPerpbTables();
   
   //this creates a single eventsPerInvpbTable
   Table formOneEventsTable(map<string, Table> &enhancedTables);
   
   //Creates cutYieldTables internally
   void formCutYieldTables();
   
   //Creates a single cut yield table
   Table formOneCutYieldTable(map<string, Table> &yieldTables, unsigned int row);
   
   //Adds a 2+ Jet(s) column at the end of every output table
   void add2PlusJetsColumnToAllTables();
   
   //Adds a column at the end of the table that is 2+ Jets
   void add2PlusJetsColumnToTable(Table &table);
   
   //prints a single map of tables
   void outputTableMap(map<string, Table> &tableMap, ostream &os = cout, string style = "Normal");
   
   //prints a single map of tables to root
   void outputTableMapToRoot(map<string, Table> &tableMap);
};

//****************************************************************************************************//

TableProduction::TableProduction()
{
   //Default read and write locations
   setCrossSectionFilePath("CrossSections_8TeV.txt");
   setRootFoldersLocation("");
   setWriteRootPath("PS.root");
   setWriteFilePath("Tables.txt");
   setWriteLatexPath("Tables.tex");
   setLatexTemplatePath("TEMPLATE.tex");
   
   //Default luminosity in pb^(-1)
   setMuonLuminosity(1606);
   setElectronLuminosity(1599);
   
   //Default processes
   addProcess("DYJets");
   addProcess("QCD_MuEnriched");
   addProcess("SingleEl_Data");
   addProcess("SingleMu_Data");
   addProcess("STopS_Tbar");
   addProcess("STopT_T");
   addProcess("STopTW_T");
   addProcess("STopTW_Tbar");
   addProcess("WJets");
   addProcess("WW");
   addProcess("WZ");
}

TableProduction::TableProduction(
   string crossSectionFilePathTEMP,
   string rootFoldersFileLocationTEMP,
   string textOutputFilePathTEMP,
   string rootOutputFilePathTEMP,
   string latexOutputFilePathTEMP,
   string latexTemplateFilePathTEMP,
   vector<string> &processNamesTEMP,
   double muonLuminosityTEMP,
   double ElectronLuminosityTEMP)
{
   setCrossSectionFilePath(crossSectionFilePathTEMP);
   setRootFoldersLocation(rootFoldersFileLocationTEMP);
   setWriteRootPath(rootOutputFilePathTEMP);
   setWriteFilePath(textOutputFilePathTEMP);
   setWriteLatexPath(latexOutputFilePathTEMP);
   setLatexTemplatePath(latexTemplateFilePathTEMP);
   
   setMuonLuminosity(muonLuminosityTEMP);
   setElectronLuminosity(ElectronLuminosityTEMP);
   
   setProcesses(processNamesTEMP);
}

TableProduction::~TableProduction()
{
   //TODO
}

void TableProduction::setCrossSectionFilePath(string filePath)
{
   crossSectionFilePath = filePath;
}

void TableProduction::setRootFoldersLocation(string fileLoc)
{
   rootFoldersFileLocation = fileLoc;
}

void TableProduction::setWriteRootPath(string filePath)
{
   writeRootPath = filePath;
}

void TableProduction::setWriteFilePath(string filePath)
{
   writeFilePath = filePath;
}

void TableProduction::setWriteLatexPath(string filePath)
{
   writeLatexPath = filePath;
}

void TableProduction::setLatexTemplatePath(string filePath)
{
   latexTemplatePath = filePath;
}

void TableProduction::setMuonLuminosity(double lum)
{
   muonLuminosity = lum;
}

void TableProduction::setElectronLuminosity(double lum)
{
   electronLuminosity = lum;
}

void TableProduction::setProcesses(vector<string> &processNamesTEMP)
{
   processNames = processNamesTEMP;
}

//Input Tables will need to be reread after adding a process
void TableProduction::addProcess(string proc)
{
   processNames.push_back(proc);
}

void TableProduction::readAllInputs()
{
   readCrossSections();
   readInputTables();
   readEventsFromMC();
}

void TableProduction::readCrossSections()
{
   //This fills the crossSections Table
   //First we store the entire table
   crossSectionsTable.parseFromFile(crossSectionFilePath, "TableCellVal");
   //tempRows are the current rows in the table, newRows will only store the processes that we want
   vector<TableRow> tempRows = crossSectionsTable.getRows();
   vector<TableRow> newRows;
   //iterates through all processes
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      //iterates through every row
      for(unsigned int j = 0; j < tempRows.size(); j++)
      {
         //we will store the row if it is a process we want
         if(process == tempRows[j].GetName())
         {
            newRows.push_back(tempRows[j]);
         }
      }
   }
   //we save the rows of the processes we want
   crossSectionsTable.setRows(newRows);
   
   //This fills the crossSections map
   double tempVal = 0.0;
   double tempErr = 0.0;
   //We iterate trough every string in our processNames vector
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      //We store the double value associated with that process name into tempVal
      tempVal = ((TableCellVal*)crossSectionsTable.getCellRowColumn(processNames[i], "CrossSection"))->val.value;
      tempErr = ((TableCellVal*)crossSectionsTable.getCellRowColumn(processNames[i], "CrossSection"))->val.error;
      //Place tempVal into our map of CrossSections
      crossSections[processNames[i]] = tempVal;
      crossSectionErrors[processNames[i]] = tempErr;
   }
}

void TableProduction::readInputTables()
{
   //We iterate across the processNames vector
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      //Open root folder and cd to directory containing folders
      TFile* rootFile = new TFile((rootFoldersFileLocation + processNames[i] + "/PS/PS.root").c_str());
      gROOT->ProcessLine("PS->cd()");
      
      //Save input tables to their respective maps
      muonTables[processNames[i]] = *(Table*)gDirectory->Get("MuonTable");
      electronTables[processNames[i]] = *(Table*)gDirectory->Get("ElectronTable");
      leptonTables[processNames[i]] = *(Table*)gDirectory->Get("LeptonTable");
      
      //Cleanup
      rootFile->~TFile();
      delete rootFile;
   }
}

void TableProduction::readEventsFromMC()
{
   double tempVal = 0.0;
   double tempErr = 0.0;
   eventsFromMCTable.SetName("Process");
   
   //We iterate through the processNames vector
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      //store the NPATtuplesEvts into tempVal
      tempVal = ((TableCellVal*)(muonTables[processNames[i]]).getCellRowColumn("NPATtupleEvts", "jets0"))->val.value;
      tempErr = ((TableCellVal*)(muonTables[processNames[i]]).getCellRowColumn("NPATtupleEvts", "jets0"))->val.error;
      
      //Stores tempVal into the eventsFromMC map
      eventsFromMC[processNames[i]] = tempVal;
      eventsFromMCError[processNames[i]] = tempErr;
      
      //This is to construct the eventsFromMC Table
      //we construct a temporary cell and value
      TableCellVal* tempCellVal = new TableCellVal;
      tempCellVal->SetName("EventsFromMC");
      tempCellVal->val.value = tempVal;
      tempCellVal->val.error = tempErr;
      
      //create the row we are going to add using out temporary cell
      TableRow tempRow;
      tempRow.SetName(processNames[i].c_str());
      tempRow.addCellEntries(tempCellVal);
      
      //add the row for this process
      eventsFromMCTable.addRow(tempRow);
   }
}

void TableProduction::formAllTables()
{
   formAcceptanceTables();
   formInvpbTables();
   formYieldTables();
   formEnhancedTables();
   formFracPassTables();
   formEventsPerpbTables();
   formCutYieldTables();
   add2PlusJetsColumnToAllTables();
}

void TableProduction::formAcceptanceTables()
{
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      //Copies Yield Tables
      muonAcceptanceTables[process] = muonTables[process];
      electronAcceptanceTables[process] = electronTables[process];
      leptonAcceptanceTables[process] = leptonTables[process];
      
      //multiplies Acceptance tables by required value
      muonAcceptanceTables[process] /= eventsFromMC[process];
      electronAcceptanceTables[process] /= eventsFromMC[process];
      leptonAcceptanceTables[process] /= eventsFromMC[process];
      
      //Names new yield tables
      muonAcceptanceTables[process].SetName(("MuAcceptance_" + process).c_str());
      electronAcceptanceTables[process].SetName(("ElAcceptance_" + process).c_str());
      leptonAcceptanceTables[process].SetName(("LeptonAcceptance_" + process).c_str());
   }
   
   calculateAndApplyError(muonAcceptanceTables);
   calculateAndApplyError(electronAcceptanceTables);
   calculateAndApplyError(leptonAcceptanceTables);
}

void TableProduction::calculateAndApplyError(map<string, Table> &tables)
{
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      vector<TableRow> tempRowVect = tables[process].getRows();
      for(unsigned int i = 0; i < tempRowVect.size(); i++)
      {
         string rowName = tempRowVect[i].GetName();
         for(int i = 0; i <= 4; i++)
         {
            double acceptance;
            if(i != 1)
            {
               acceptance = ((TableCellVal*)(tables[process]).getCellRowColumn(rowName, "jets" + to_string(i)))->val.value;
               
               ((TableCellVal*)(tables[process]).getCellRowColumn(rowName, "jets" + to_string(i)))->val.error = sqrt((acceptance*(1 - acceptance)) / eventsFromMC[process]);
            }
            else
            {
               acceptance = ((TableCellVal*)(tables[process]).getCellRowColumn(rowName, "jet" + to_string(i)))->val.value;
               
               ((TableCellVal*)(tables[process]).getCellRowColumn(rowName, "jet" + to_string(i)))->val.error = sqrt((acceptance*(1 - acceptance)) / eventsFromMC[process]);
            }
         }
      }
   }
}

void TableProduction::formInvpbTables()
{
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      //Copies Yield Tables
      muonInvpbTables[process] = muonAcceptanceTables[process];
      electronInvpbTables[process] = electronAcceptanceTables[process];
      leptonInvpbTables[process] = leptonAcceptanceTables[process];
      
      //multiplies Acceptance tables by required value
      muonInvpbTables[process] *= (((TableCellVal*)crossSectionsTable.getCellRowColumn(processNames[i], "CrossSection"))->val);
      electronInvpbTables[process] *= (((TableCellVal*)crossSectionsTable.getCellRowColumn(processNames[i], "CrossSection"))->val);
      leptonInvpbTables[process] *= (((TableCellVal*)crossSectionsTable.getCellRowColumn(processNames[i], "CrossSection"))->val);
      
      //Names new yield tables
      muonInvpbTables[process].SetName(("MuInvpb_" + process).c_str());
      electronInvpbTables[process].SetName(("ElInvpb_" + process).c_str());
      leptonInvpbTables[process].SetName(("LeptonInvpb_" + process).c_str());
   }
}

void TableProduction::formYieldTables()
{
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      //Copies Yield Tables
      muonYieldTables[process] = muonInvpbTables[process];
      electronYieldTables[process] = electronInvpbTables[process];
      leptonYieldTables[process] = leptonInvpbTables[process];
      
      //multiplies Acceptance tables by required value
      muonYieldTables[process] *= muonLuminosity;
      electronYieldTables[process] *= electronLuminosity;
      leptonYieldTables[process] *= 1600;
      
      //Names new yield tables
      muonYieldTables[process].SetName(("MuYield_" + process).c_str());
      electronYieldTables[process].SetName(("ElYield_" + process).c_str());
      leptonYieldTables[process].SetName(("LeptonYield_" + process).c_str());
   }
}

void TableProduction::formEnhancedTables()
{
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      //Copies enhanced tables from original tables
      muonEnhancedTables[process] = EnhancedTableProduction::produce(muonTables[process], crossSections[process]);
      electronEnhancedTables[process] = EnhancedTableProduction::produce(electronTables[process], crossSections[process]);
      leptonEnhancedTables[process] = EnhancedTableProduction::produce(leptonTables[process], crossSections[process]);
      
      //Names enhanced tables
      muonEnhancedTables[process].SetName(("Mu_" + process).c_str());
      electronEnhancedTables[process].SetName(("El_" + process).c_str());
      leptonEnhancedTables[process].SetName(("Lepton_" + process).c_str());
   }
}

void TableProduction::formFracPassTables()
{
      //constructs the MuonEvtsPerInvpbTable
   Table tempTable = formOneFracPassTable(muonEnhancedTables);
   tempTable.SetName("MuFracPass");
   
   fracPassTables["Muon"] = tempTable;
   
   //constructs the ElectronEvtsPerInvpbTable
   tempTable = formOneFracPassTable(electronEnhancedTables);
   tempTable.SetName("ElFracPass");
   
   fracPassTables["Electron"] = tempTable;
   
   //constructs the LeptonEvtsPerInvpbTable
   tempTable = formOneFracPassTable(leptonEnhancedTables);
   tempTable.SetName("LeptonFracPass");
   
   fracPassTables["Lepton"] = tempTable;
}

Table TableProduction::formOneFracPassTable(map<string, Table> &enhancedTables)
{
   //create table
   Table fracPassTable;
   
   //get FracPassRow from every enhanced Table
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      vector<TableRow> rows = enhancedTables[process].getRows();
      TableRow tempRow;
      for(unsigned int j = 0; j < rows.size(); j++)
      {
         
         string rowName = rows[j].GetName();
         if(rowName == "FracPass")
            tempRow = rows[j];
      }
      tempRow.SetName(process.c_str());
      
      fracPassTable.addRow(tempRow);
   }
   
   return fracPassTable;
}

void TableProduction::formEventsPerpbTables()
{
   //constructs the MuonEvtsPerInvpbTable
   Table tempTable = formOneEventsTable(muonEnhancedTables);
   tempTable.SetName("MuEvtsPerInvpb");
   
   eventsPerInvpbTables["Muon"] = tempTable;
   
   //constructs the ElectronEvtsPerInvpbTable
   tempTable = formOneEventsTable(electronEnhancedTables);
   tempTable.SetName("ElEvtsPerInvpb");
   
   eventsPerInvpbTables["Electron"] = tempTable;
   
   //constructs the LeptonEvtsPerInvpbTable
   tempTable = formOneEventsTable(leptonEnhancedTables);
   tempTable.SetName("LeptonEvtsPerInvpb");
   
   eventsPerInvpbTables["Lepton"] = tempTable;
}

Table TableProduction::formOneEventsTable(map<string, Table> &enhancedTables)
{
   //create table
   Table eventsTable;
   
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      vector<TableRow> rows = enhancedTables[process].getRows();
      TableRow tempRow;
      for(unsigned int j = 0; j < rows.size(); j++)
      {
         string rowName = rows[j].GetName();
         if(rowName == "EvtsPerInvpb")
            tempRow = rows[j];
      }
      tempRow.SetName(process.c_str());
      
      eventsTable.addRow(tempRow);
   }
   
   return eventsTable;
}

void TableProduction::formCutYieldTables()
{
   int c5RowNum, c6RowNum;
   vector<TableRow> tempRows = muonYieldTables[processNames[0]].getRows();
   for(unsigned int i = 0; i < tempRows.size(); i++)
   {
      string rowName = tempRows[i].GetName();
      if(rowName == "c5:NotLooseEl")
         c5RowNum = i;
      if(rowName == "c6:METE")
         c6RowNum = i;
   }
   
   muonCutYieldTables["c5:NotLooseEl"] = formOneCutYieldTable(muonYieldTables, c5RowNum);
   muonCutYieldTables["c5:NotLooseEl"].SetName("Mu_C5:NotLooseElYields");
   
   muonCutYieldTables["c6:METE"] = formOneCutYieldTable(muonYieldTables, c6RowNum);
   muonCutYieldTables["c6:METE"].SetName("Mu_C6:METEYields");
   
   electronCutYieldTables["c5:NotLooseEl"] = formOneCutYieldTable(electronYieldTables, c5RowNum);
   electronCutYieldTables["c5:NotLooseEl"].SetName("El_C5:NotLooseElYields");
   
   electronCutYieldTables["c6:METE"] = formOneCutYieldTable(electronYieldTables, c6RowNum);
   electronCutYieldTables["c6:METE"].SetName("El_C6:METEYields");
}

Table TableProduction::formOneCutYieldTable(map<string, Table> &yieldTables, unsigned int rowNum)
{
   //create table
   Table cutYieldTable;
   
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];

      vector<TableRow> rows = yieldTables[process].getRows();
      TableRow tempRow = rows[rowNum];
      tempRow.SetName(process.c_str());
      
      cutYieldTable.addRow(tempRow);
   }
   cutYieldTable.SetName("CutYield");
   
   return cutYieldTable;
}

void TableProduction::add2PlusJetsColumnToAllTables()
{
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string process = processNames[i];
      
      add2PlusJetsColumnToTable(muonAcceptanceTables[process]);
      add2PlusJetsColumnToTable(electronAcceptanceTables[process]);
      add2PlusJetsColumnToTable(leptonAcceptanceTables[process]);
      
      add2PlusJetsColumnToTable(muonInvpbTables[process]);
      add2PlusJetsColumnToTable(electronInvpbTables[process]);
      add2PlusJetsColumnToTable(leptonInvpbTables[process]);
      
      add2PlusJetsColumnToTable(muonYieldTables[process]);
      add2PlusJetsColumnToTable(electronYieldTables[process]);
      add2PlusJetsColumnToTable(leptonYieldTables[process]);
      
      add2PlusJetsColumnToTable(muonEnhancedTables[process]);
      add2PlusJetsColumnToTable(electronEnhancedTables[process]);
      add2PlusJetsColumnToTable(leptonEnhancedTables[process]);
   }
   
   add2PlusJetsColumnToTable(fracPassTables["Muon"]);
   add2PlusJetsColumnToTable(fracPassTables["Electron"]);
   add2PlusJetsColumnToTable(fracPassTables["Lepton"]);
   
   add2PlusJetsColumnToTable(eventsPerInvpbTables["Muon"]);
   add2PlusJetsColumnToTable(eventsPerInvpbTables["Electron"]);
   add2PlusJetsColumnToTable(eventsPerInvpbTables["Lepton"]);
   
   add2PlusJetsColumnToTable(muonCutYieldTables["c5:NotLooseEl"]);
   add2PlusJetsColumnToTable(muonCutYieldTables["c6:METE"]);
   
   add2PlusJetsColumnToTable(electronCutYieldTables["c5:NotLooseEl"]);
   add2PlusJetsColumnToTable(electronCutYieldTables["c6:METE"]);
}

void TableProduction::add2PlusJetsColumnToTable(Table &table)
{
   //This was the code used for the sync with Fermilab
   vector<TableRow> oldRows = table.getRows();
   vector<TableRow> newRows = oldRows;
   for(unsigned int i = 0; i < oldRows.size(); i++)
   {
      string rowName = oldRows[i].GetName();
      Value tempValue;
      TableCellVal* tempCell = new TableCellVal;
      tempValue = ((TableCellVal*)table.getCellRowColumn(rowName, "jets2"))->val;
      if(rowName != "NPATtupleEvts")
      {
         tempValue += ((TableCellVal*)table.getCellRowColumn(rowName, "jets3"))->val;
         tempValue += ((TableCellVal*)table.getCellRowColumn(rowName, "jets4"))->val;
      }
      
      tempCell->val=(tempValue);
      tempCell->SetName("jets2+");
      
      newRows[i].addCellEntries(tempCell);
      
      // For Fermilab syncing
      Value tempValue2;
      TableCellVal* tempCell2 = new TableCellVal;
      tempValue2 = ((TableCellVal*)table.getCellRowColumn(rowName, "jets2"))->val;
      if(rowName != "NPATtupleEvts")
      {
         tempValue2 += ((TableCellVal*)table.getCellRowColumn(rowName, "jets3"))->val;
      }
      
      tempCell2->val=(tempValue2);
      tempCell2->SetName("jets2+3");
      
      newRows[i].addCellEntries(tempCell2);
      
   }
   table.setRows(newRows);
}

void TableProduction::writeAllTablesToLatex(
   string title,
   string author,
   string email,
   string templateLoc = "",
   string writeLoc = "")
{
   if(templateLoc == "")
      templateLoc = latexTemplatePath;
   if(writeLoc == "")
      writeLoc = writeLatexPath;
   
   PDFCreator pCreate(latexTemplatePath, writeLatexPath);
   
   pCreate.beginDocument(title, author, email);
   
   pCreate.writeIntroduction(muonLuminosity,
                             electronLuminosity,
                             crossSectionsTable,
                             eventsFromMCTable);
   
   // These are the summary tables.
   pCreate.newSection("Acceptance Tables Summary");
   pCreate.newSubSection("Summary Tables");
   
   pCreate.startFrame("AcceptanceTables");
   pCreate.writeTable(fracPassTables["Muon"]);
   pCreate.writeTable(fracPassTables["Electron"]);
   pCreate.endFrame();
   
   pCreate.startFrame("Yield Tables Per $pb^{-1}$");
   pCreate.writeTable(eventsPerInvpbTables["Muon"]);
   pCreate.writeTable(eventsPerInvpbTables["Electron"]);
   pCreate.endFrame();
   
   pCreate.newSubSection("Specific Cut Yields");
   
   pCreate.startFrame("Muon Cut Yields");
   pCreate.writeTable(muonCutYieldTables["c5:NotLooseEl"]);
   pCreate.writeTable(muonCutYieldTables["c6:METE"]);
   pCreate.endFrame();
   
   pCreate.startFrame("ELectron Cut Yields");
   pCreate.writeTable(electronCutYieldTables["c5:NotLooseEl"]);
   pCreate.writeTable(electronCutYieldTables["c5:NotLooseEl"]);
   pCreate.endFrame();
   
   // These are the full tables
   pCreate.newSection("Full Tables");
   pCreate.newSubSection("Acceptances");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Acceptances: " + procName);
      if(procName != "QCD")//TEMP
         pCreate.writeTable(muonAcceptanceTables[procName]);
      pCreate.writeTable(electronAcceptanceTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.newSubSection("Yields per Inv pb");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Yields per Inv pb: " + procName);
      if(procName != "QCD")//TEMP      
         pCreate.writeTable(muonInvpbTables[procName]);
      pCreate.writeTable(electronInvpbTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.newSubSection("Total Yields");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Yields for Total Luminosity: " + procName);
      if(procName != "QCD")//TEMP
         pCreate.writeTable(muonYieldTables[procName]);
      pCreate.writeTable(electronYieldTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.newSubSection("Event Tables");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Number of Events in MC: " + procName);
      if(procName != "QCD")//TEMP
         pCreate.writeTable(muonEnhancedTables[procName]);
      pCreate.writeTable(electronEnhancedTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.endDocument();
   
   /*
   if(templateLoc == "")
      templateLoc = latexTemplatePath;
   if(writeLoc == "")
      writeLoc = writeLatexPath;
   
   PDFCreator pCreate(latexTemplatePath, writeLatexPath);
   
   pCreate.beginDocument(title, author, email);
   
   pCreate.writeIntroduction(muonLuminosity,
                             electronLuminosity,
                             crossSectionsTable,
                             eventsFromMCTable);
   
   // These are the summary tables.
   pCreate.newSection("Acceptance Tables Summary");
   pCreate.newSubSection("Summary Tables");
   
   pCreate.startFrame("AcceptanceTables");
   pCreate.writeTable(fracPassTables["Muon"]);
   pCreate.writeTable(fracPassTables["Electron"]);
   pCreate.endFrame();
   
   pCreate.startFrame("Yield Tables Per $pb^{-1}$");
   pCreate.writeTable(eventsPerInvpbTables["Muon"]);
   pCreate.writeTable(eventsPerInvpbTables["Electron"]);
   pCreate.endFrame();
   
   pCreate.newSubSection("Specific Cut Yields");
   
   pCreate.startFrame("Muon Cut Yields");
   pCreate.writeTable(muonCutYieldTables["c5:NotLooseEl"]);
   pCreate.writeTable(muonCutYieldTables["c6:METE"]);
   pCreate.endFrame();
   
   pCreate.startFrame("ELectron Cut Yields");
   pCreate.writeTable(electronCutYieldTables["c5:NotLooseEl"]);
   pCreate.writeTable(electronCutYieldTables["c5:NotLooseEl"]);
   pCreate.endFrame();
   
   // These are the full tables
   pCreate.newSection("Full Tables");
   pCreate.newSubSection("Acceptances");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Acceptances: " + procName);
      pCreate.writeTable(muonAcceptanceTables[procName]);
      pCreate.writeTable(electronAcceptanceTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.newSubSection("Yields per Inv pb");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Yields per Inv pb: " + procName);
      pCreate.writeTable(muonInvpbTables[procName]);
      pCreate.writeTable(electronInvpbTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.newSubSection("Total Yields");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Yields for Total Luminosity: " + procName);
      pCreate.writeTable(muonYieldTables[procName]);
      pCreate.writeTable(electronYieldTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.newSubSection("Event Tables");
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      string procName = processNames[i];
      
      pCreate.startFrame("Number of Events in MC: " + procName);
      pCreate.writeTable(muonEnhancedTables[procName]);
      pCreate.writeTable(electronEnhancedTables[procName]);
      pCreate.endFrame();
   }
   
   pCreate.endDocument();*/
}

void TableProduction::outputTableMap(map<string, Table> &tableMap, ostream &os, string style)
{
   string process = "";
   
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      process = processNames[i];
      tableMap[process].printTable(os, style);
   }
}

void TableProduction::writeAllTablesToRoot(string rootLoc = "")
{
   if(rootLoc == "")
      rootLoc = writeRootPath;
   
   string process = "";
   
   TFile* tempRootFile = new TFile(rootLoc.c_str(), "RECREATE");
   
   outputTableMapToRoot(muonAcceptanceTables);
   outputTableMapToRoot(electronAcceptanceTables);
   outputTableMapToRoot(leptonAcceptanceTables);
   
   outputTableMapToRoot(muonInvpbTables);
   outputTableMapToRoot(electronInvpbTables);
   outputTableMapToRoot(leptonInvpbTables);
   
   outputTableMapToRoot(muonYieldTables);
   outputTableMapToRoot(electronYieldTables);
   outputTableMapToRoot(leptonYieldTables);
   
   outputTableMapToRoot(muonEnhancedTables);
   outputTableMapToRoot(electronEnhancedTables);
   outputTableMapToRoot(leptonEnhancedTables);
   
   fracPassTables["Muon"].Write();
   fracPassTables["Electron"].Write();
   fracPassTables["Lepton"].Write();
   
   eventsPerInvpbTables["Muon"].Write();
   eventsPerInvpbTables["Electron"].Write();
   eventsPerInvpbTables["Lepton"].Write();
   
   muonCutYieldTables["c5:NotLooseEl"].Write();
   muonCutYieldTables["c6:METE"].Write();
   
   electronCutYieldTables["c5:NotLooseEl"].Write();
   electronCutYieldTables["c6:METE"].Write();
}

void TableProduction::outputTableMapToRoot(map<string, Table> &tableMap)
{
   string process = "";
   
   for(unsigned int i = 0; i < processNames.size(); i++)
   {
      process = processNames[i];
      tableMap[process].Write();
   }
}

//****************************************************************************************************//

int main(int argc, char** argv)
{
   CommandLine cl;
   if(!cl.parse(argc, argv))
      return 0;
   
   string crossSectionFilePathCL    = cl.getValue<string>  ("crossSectionFilePath", "CrossSections_8TeV.txt");
   string rootFoldersFileLocationCL = cl.getValue<string>  ("rootFoldersFileLocation", "");
   string textOutputFilePathCL      = cl.getValue<string>  ("textOutputFilePath", "Tables.txt");
   string rootOutputFilePathCL      = cl.getValue<string>  ("rootOutputFilePath", "PS.root");
   string latexTemplateFilePathCL   = cl.getValue<string>  ("latexTemplateFilePath", "TEMPLATE.tex");
   string latexOutputFilePathCL     = cl.getValue<string>  ("latexOutputFilePath", "Tables.tex");
   string latexTitleCL              = cl.getValue<string>  ("latexTitle", "Monte Carlo Acceptances and Yields");
   string latexAuthorCL             = cl.getValue<string>  ("latexAuthors", "Ricardo Eusebi, Travis Lamb, Alexx Perloff");
   string latexEmailCL              = cl.getValue<string>  ("latexEmails", "eusebi@physics.tamu.edu, travis.t.lamb@gmail.com, aperloff@physics.tamu.edu");
   vector<string> processNamesCL    = cl.getVector<string> ("processNames", "DYJets:::QCD:::SingleEl_Data:::SingleMu_Data:::STopS_T:::STopS_Tbar:::STopT_T:::STopTW_T:::STopTW_Tbar:::TTbar:::WJets:::WW:::WZ:::ggH125");
   double muonLuminosityCL          = cl.getValue<double>  ("muonLuminosity", 1606);
   double electronLuminosityCL      = cl.getValue<double>  ("electronLuminosity", 1599);
   bool writeToLatexCL              = cl.getValue<bool>    ("writeToLatex", true);
   bool writeToRootCL               = cl.getValue<bool>    ("writeToRoot", false);
      
   if(!cl.check())
      return 1;
   cl.print();
   
   TableProduction tabProd = TableProduction(
      crossSectionFilePathCL,
      rootFoldersFileLocationCL,
      textOutputFilePathCL,
      rootOutputFilePathCL,
      latexOutputFilePathCL,
      latexTemplateFilePathCL,
      processNamesCL,
      muonLuminosityCL,
      electronLuminosityCL);
   
   tabProd.readAllInputs();
   
   tabProd.formAllTables();
   
   if(writeToRootCL)
      tabProd.writeAllTablesToRoot();
   if(writeToLatexCL)
      tabProd.writeAllTablesToLatex(latexTitleCL, latexAuthorCL, latexEmailCL);
   
   cout << endl << endl << "END OF PROGRAM" << endl << endl;
   
   return 0;
}
