/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// Transfer functions for WW matrix element analysis
//// Created by Perloff, Alexx : aperloff@physics.tamu.edu
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Use to find the jet to genParton transfer functions necessary to run a matrix element analysis.

//User Defined Ntuple
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/FileLocationTable.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"
#include "TAMUWW/SpecialTools/interface/TableCellText.hh"
#include "TAMUWW/SpecialTools/interface/Value.hh"

// Root includes
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

// Standars library includes
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <utility>

using namespace std;

///////////////////
///// Classes /////
///////////.///////

class channel {
public:

  channel(string _name): name(_name),
                         numJets(0),
                         numJetsPart(0),
                         numJetsAntiPart(0),
                         numJetsFailDR(0){ }

  string name;
  vector<int> pdgids;
  int numJets;
  int numJetsPart;
  int numJetsAntiPart;
  int numJetsFailDR;
};

///////////////////////////
///// Global Variables/////
///////////////////////////

static TChain *chain;
static EventNtuple *ntuple;
int nEventNtuple;
double maxDeltaR = 0.25;
int currentChannelIndex = 0;
vector<channel> channels;
TF2* JEvsPEfit;


///////////////////////////
///// Custom Functions/////
///////////////////////////

/*****************************************************************************/

vector<channel> GetListOfChannels()
{

vector<channel> channels;
/*
  //The UDS channel
  channels.push_back(channel("UDS"));
  channels.back().pdgids.push_back(1);
  channels.back().pdgids.push_back(2);
  channels.back().pdgids.push_back(3);
  
  //The C channel
  channels.push_back(channel("C"));
  channels.back().pdgids.push_back(4);
*/
  //The B channel
  channels.push_back(channel("B"));
  channels.back().pdgids.push_back(5);
/*
  //The G channel
  channels.push_back(channel("G"));
  channels.back().pdgids.push_back(21);
*/
  return channels;
}

/*****************************************************************************/

// ---- concatString ----
// Put two things together and make them a string
// Must be a streamable object
template <class T, class U>
std::string concatString(const T& obj1, const U& obj2)
{
   std::ostringstream output;
   output << obj1 << obj2;
   return output.str();
}

/*****************************************************************************/

//This function will print out the time to the console
string print_time()
{
   time_t rawtime;
   struct tm *timeinfo;
   char buffer[30];
//Get the current time
   time (&rawtime);
//Turn the current time into a local time format and safe it in the struct
   timeinfo = localtime(&rawtime);
   strftime(buffer,30,"%c",timeinfo);
   string mystring;
   mystring += buffer;
   return mystring;
}

/*****************************************************************************/

string filename_creator()
{
   /*Sets up the filename of the .ROOT file where the graphs will be stored*/
   time_t rawtime;
   struct tm *timeinfo;
   //Buffer to store the converted c_string
   char buffer[35];
   //Get the current time
   time (&rawtime);
   //Turn the current time into a local time format and safe it in the struct
   timeinfo = localtime(&rawtime);
   //Set up the c_string command that will format the filename
   string filename_template = "%d-%b-%Y-TransferFunctions";
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

//Make the list of events/jets/partons to access
bool makeChain(string inputFilepath)
{
  TFile *inputFile = new TFile(inputFilepath.c_str());
  if(inputFile->IsOpen())
    {
      chain = (TChain*)inputFile->Get("EvtTree");
      ntuple = new EventNtuple();
      chain->SetBranchAddress("EvtNtuple", &ntuple);
      return true;
    }
  else
    {
      cout << "makeChain::File " << inputFilepath << " does not exist" << endl;
      return false;
    }
}

/*****************************************************************************/

TableRow*  makeBreakRow(vector<string> &cellTitles, int titleWidth=4)
{
  int cellWidth = 0;
  for(unsigned int i=0; i<cellTitles.size(); i++)
    {
      if(cellTitles[i].size()> (unsigned int)cellWidth) cellWidth = cellTitles[i].size();
    }
  string titleString;
  for(int i=0; i<titleWidth; i++) titleString+="*";
  string cellString;
  for(int i=0; i<cellWidth; i++) cellString+="*";
  TableRow* tableRow = new TableRow();
  for(unsigned int i=0; i<cellTitles.size(); i++)
    {
      TableCellText* tableCellText = new TableCellText(cellTitles[i]);
      tableCellText->text = cellString;
      tableRow->addCellEntries(tableCellText);
    }
  
  return tableRow;
}

/*****************************************************************************/

Table* makeTable(TMinuit* gMinuit)
{
  Table* table = new Table("TF_Par_"+channels[currentChannelIndex].name);
  TableRow* tableRow;
  TableCellVal* tableCellVal;
  int  i1;
  //double d1,d2,err,val;
  double pars[4];
  TString chnam;

  for(int i=0; i<gMinuit->GetNumPars(); i++)
    {
      gMinuit->mnpout(i,chnam,pars[0],pars[1],pars[2],pars[3],i1); // Provides the user with information concerning the current status
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

Table* makeNumJetsTable()
{
  Table* table = new Table("JetsPerChannel");
  TableRow* tableRow;
  TableCellText* tableCellText;
  int totalJets = 0;
  int totalPart = 0;
  int totalAntiPart = 0;
  stringstream out;
  vector<string> cellTitles; cellTitles.push_back("Number of Jets"); cellTitles.push_back("Particle Parton"); cellTitles.push_back("Anti-Particle Parton");

  for(unsigned int i=0; i<channels.size(); i++)
    {
      tableRow = new TableRow(channels[i].name);

      tableCellText = new TableCellText(cellTitles[0]);
      totalJets += channels[i].numJets;
      out << channels[i].numJets;
      tableCellText->text = out.str();
      tableRow->addCellEntries(tableCellText);
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

Table* makeAllJetsTable()
{
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

Table* makeEventContentTable(bool deltaRCut)
{
  Table* table;
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
  map<pair<int,int>,int> ECMap;
  pair<int,int> currentPair;
  vector<string> cellTitles(1,"Number of Events");
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
      out << "|(" << (*it).first.first << ")(" << (*it).first.second << ")|";
      tableRow = new TableRow(out.str());
      out.str("");
      
      tableCellText = new TableCellText("Number of Events");
      total += (*it).second;
      out << (*it).second;
      tableCellText->text = out.str();
      tableRow->addCellEntries(tableCellText);
      
      table->addRow((*tableRow));
      out.str("");
      delete tableRow;
    }
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
//Jet Energy = x
//Parton Energy = y
void HistogramsNormalizedByPartonEnergy(channel &cha, map<string,TH2D*> &histoMap2D)
{
  histoMap2D[string(cha.name+"_JEvsPE_norm")] = HistogramsNormalizedByPartonEnergy(histoMap2D[string(cha.name+"_JEvsPE")],cha.name);
}

/*****************************************************************************/

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
        }
    }
}

/*****************************************************************************/
//1D tf
//x-axis is E_p - E_j
//par[10] is E_p and needs to be fixed by user
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
//2d tf
//x-axis is E_p - E_j
//y-axis is E_p
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
//2d tf
//x-axis is E_j
//y-axis is E_p
double fitTFJEvsPE(double* x, double *par)
{
  return JEvsPEfit->Eval(x[0]-x[1],x[1]);
}

/*****************************************************************************/

//Used to set the pointer to the correct fit function
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

TF2* makeFit(TMinuit* gMinuit)
{
  int  i1;
  double pars[4];
  TString chnam;
  string fitName = "TF_Fit2D_"+channels[currentChannelIndex].name;
  TF2* fit = new TF2(fitName.c_str(),fitTF2D,-500,500,0,500,10);

  for(int i=0; i<gMinuit->GetNumPars(); i++)
    {
      gMinuit->mnpout(i,chnam,pars[0],pars[1],pars[2],pars[3],i1); // Provides the user with information concerning the current status
      fit->SetParameter(i,pars[0]);
      fit->SetParError(i,pars[1]);      
    }
  return fit;
}

/*****************************************************************************/
TF2* makeJEvsPEFit(TF2* fit)
{
  JEvsPEfit = (TF2*)fit->Clone();
  string fitName = "TF_Fit2D_" + channels[currentChannelIndex].name + "_JEvsPE"; 
  TF2* newfit = new TF2(fitName.c_str(),fitTFJEvsPE,0,500,0,500,10);
  return newfit;
}

/*****************************************************************************/

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

void setFitParams(TF2* fit,vector<double> &fitParams, vector<double> &fitParamErrors)
{
  for(unsigned int i=0; i<fitParams.size(); i++)
    {
      fit->SetParameter(i,fitParams[i]);
      fit->SetParError(i,fitParamErrors[i]);
    }
}

/*****************************************************************************/

void setFitParams(TF1* fit,vector<double> &fitParams, vector<double> &fitParamErrors)
{
    for(unsigned int i=0; i<fitParams.size(); i++)
    {
      fit->SetParameter(i,fitParams[i]);
      fit->SetParError(i,fitParamErrors[i]);
    }
}

/*****************************************************************************/

void makeHistFromFunc(TH1D* hist, TF1* fit)
{
  for(int i=0; i<=hist->GetNbinsX()+1; i++)
    {
      hist->SetBinContent(i,fit->Eval(hist->GetXaxis()->GetBinCenter(i)));
      hist->SetBinError(i,0);
    }
}

/*****************************************************************************/

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

void printKSandChi2(TH2D* hist, TH2D* fitHist)
{
  TPaveStats *st = (TPaveStats*)hist->FindObject("stats");
  st->SetName("updatedStats");
  st->AddText(Form("KolmogorovTest = %g",hist->KolmogorovTest(fitHist)));
  st->AddText(Form("Chi2Test = %g",fitHist->Chi2Test(hist,"UW")));
  hist->SetStats(0); //to avoid a new "stats" to be drawn 
 }

/*****************************************************************************/

void formatForDraw(TH1D* hist)
{
  hist->GetXaxis()->SetTitle("E_{Jet}-E_{P} [GeV]");
}

/*****************************************************************************/

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
//Example:: OverlayFunction("TF_Histo2D_B","fitTF2D","TF_ttbar_B.txt")
//Example:: OverlayFunction("TF_Histo2D_B_JEvsPE_norm","fitTFJEvsPE","TF_TTbarMG_B.txt","default",0,true,"TF_Fit2D_B")
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
      else //Case where the histogram has E_jet on the x axis and E_p on the y axis
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

//void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
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

///////////////////////////
// ///////////////////// //
// // Main Subroutine // //
// ///////////////////// //
///////////////////////////

/*
TransferFunctions takes the path of an EventNtuple and outputs a table of jet energy transfer functions
(one for each flavor of jet). In addition, the function will create a histogram for each jet flavor in bins
of E_jet - E_parton.

Example:
.x TransferFunctions.C++(false,"ttbar","/uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/Selection/bin/TTBarTest_status3/SelectionTest_outfile.root")

TransferFunctions(false,"TTbarMG","/uscms_data/d1/ilyao/Stage2SkimResults_3_8_7/MC/TTbarMG_outfileAll.root")

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
  if(!makeChain(inputFilepath))
    {
      cout << "TransferFunctions::The function will now exit" << endl;
      cout << endl << "*****************************************************************************" << endl << endl;
      return;
    }
  nEventNtuple = static_cast<int>(chain->GetEntries());
  cout << "End time: " << print_time() << endl;

  vector<Table*> tables;
  vector<TF2*> fits;
  map<string,TH1D*> histoMap1D;
  map<string,TH2D*> histoMap2D;

  string outputFilename = "TF_" + process + ".root";
  TFile* file = TFile::Open(outputFilename.c_str(),"RECREATE");

  // Loop over all channels
  for (unsigned int i=0;i<channels.size(); i++)
    {
      cout << endl << "TransferFunctions::Start fitting the recoJets and genParticles by their pdgId." << endl << "Start time: " << print_time() << endl;
      // Analyze this channel and perform the minimization
      currentChannelIndex=i;
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
      // print a canvas and show histos
      HistogramsByFlavor(channels[i],histoMap1D,histoMap2D);
      HistogramsByFlavorAndEnergy(channels[i],histoMap1D);
      HistogramsNormalizedByPartonEnergy(channels[i],histoMap2D);
      cout << "End time: " << print_time() << endl;

  }//for (unsigned int i=0;i<channels.size(); i++)

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
/*
Example: runAllTF(false)
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
