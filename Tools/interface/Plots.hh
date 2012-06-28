#ifndef PLOTS_DEF
#define PLOTS_DEF

// ROOT libraries
#include "TH1.h"
#include "TChain.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TString.h"

// Our libraries
#include "TAMUWW/Tools/interface/PhysicsProcessNEW.hh"

// C++ libraries
#include <string>
#include <vector>

// ##################################################
// ################### PLOT CLASS ###################
// ##################################################

// A basic class that holds all of the process histograms for a given plot.
class Plot: public TNamed
{
public :

   // Constructor
   Plot() { scaled = false; }

   // Create a new histo here
   void prepareToFillProcess(PhysicsProcessNEW* proc);
      
   // Fill the last histo here
   void Fill(double h, double w = 1);

   // Do the scaling to luminosity or data.
   void scaleToData(std::vector<PhysicsProcessNEW*> procs);
   
   // Saves the histograms to the filename
   void saveHistogramsToFile(TString histoFile);

   // Do the grouping of the histograms according to some rules.
   // For example join all histos for singleTop into a single one, 
   // also WW and WZ are usually put together...
   std::vector<TH1*>  doGrouping(std::vector<PhysicsProcessNEW*> procs);
   
   std::vector<TH1*> getHistos() { return histos; }

   TH1* templateHisto;
   std::vector<TH1*> histos;
   
   bool stacked;
   bool normToData;

private:
   bool scaled;
   
   ClassDef (Plot,1);
};

// ##################################################
// ############## FORMATTED PLOT CLASS ##############
// ##################################################

// Adds some formatting options to Plot
class FormattedPlot: public Plot
{
public:
   // Make the canvas here
   TCanvas* getCanvas(std::vector<PhysicsProcessNEW*> procs);
   
   // Create a new histo here
   void prepareToFillProcess(ColoredPhysicsProcessNEW* proc);
   
   // Take care of the formatting
   void formatRatio(TH1* hRatio);
   void formatStack(THStack * stack, double maxi);
   
   std::vector<std::string> axisTitles;
   std::pair<double,double> range;
   
   ClassDef (FormattedPlot,1);
};

#endif
