// ROOT libraries
#include "TH1.h"
#include "TChain.h"
#include "TCanvas.h"

// C++ libraries
#include <string>
#include <vector>

using namespace std;

class proc{

public:
   proc(std::string procName, std::string fileName, double cross_section, double lum, 
        unsigned int in_ev, int col, std::string treeName = "EvtTree");

   double getScaleFactor(){return sigma * intLum / initial_events;};

   std::string name;    // the given name of the process
   TChain * chain; // the pointer the file's TChain
   double sigma ;  // the cross section
   double intLum;  // the integrated luminosity 
   unsigned int initial_events; // the initial number of events in the MC sample
   int color; // the color used to represent this process

};

class aPlot {
   public :

      // Constructor
      aPlot() { scaled = false;}

      // Create a new histo here
      void prepareToFillProcess(string procName);
      
      // Fill the last histo here
      void Fill(double h, double w = 1);

      // Do the scaling to luminosity or data.
      void doScaling(std::vector<proc*> procs);

      // Make the Canvas here
      TCanvas * getCanvas(std::vector<proc*> procs);

      //Make the historam here
      TH1 * getHisto(std::vector<proc*> procs);

      TH1* templateHisto;
      std::vector<TH1*> histos;
      std::vector<std::string> axisTitles;
      bool stacked;
      bool normToData;
      std::pair<double,double> range;

   private:
      bool scaled;
};

