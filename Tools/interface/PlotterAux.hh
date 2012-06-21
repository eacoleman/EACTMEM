// ROOT libraries
#include "TH1.h"
#include "TChain.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TString.h"

// C++ libraries
#include <string>
#include <vector>

class proc{

public:
   proc(std::string procName, std::string fileName, double cross_section, double lum, 
        unsigned int in_ev, int col, std::string treeName = "PS/EvtTree");

   double getScaleFactor(){return sigma * intLum / initial_events;};

   TString  name ;  // the given name of the process
   TChain * chain;  // the pointer the file's TChain
   double  sigma ;  // the production cross section
   double  intLum;  // the integrated luminosity to which to normalize
   unsigned int initial_events; // the initial number of events in the MC sample
   int color; // the color used to draw this process

};

class aPlot {
   public :

      // Constructor
      aPlot() { scaled = false;}

      // Create a new histo here
      void prepareToFillProcess(proc* proc);
      
      // Fill the last histo here
      void Fill(double h, double w = 1);

      // Do the scaling to luminosity or data.
      void doScaling(std::vector<proc*> procs);

      // Do the grouping of the histograms according to some rules.
      // For example join all histos for singleTop into a single one, 
      // also WW and WZ are usually put together...
      std::vector<TH1*>  doGrouping(std::vector<proc*> procs);

      // Take care of the formatting
      void formatRatio(TH1* hRatio);
      void formatStack(THStack * stack, double maxi);

      // Make the Canvas here
      TCanvas * getCanvas(std::vector<proc*> procs);

      TH1* templateHisto;
      std::vector<TH1*> histos;
      std::vector<std::string> axisTitles;
      bool stacked;
      bool normToData;
      std::pair<double,double> range;

   private:
      bool scaled;
};

