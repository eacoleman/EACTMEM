
// Title:     PlotFiller.hh
// Author:    Travis Lamb
// Started:   22nd of June, 2012

// Uses Plot and PhysicsProcessNEW to return a plot for a given process and yield.

// Our libraries
#include "TAMUWW/Tools/interface/Plots.hh"
#include "TAMUWW/Tools/interface/PlotMap.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/Tools/interface/PUreweight.hh"
#include "TAMUWW/Tools/interface/TriggerEfficiency.hh"

//C++ libraries
#include <iostream>
#include <string>
#include <vector>
#include <map>

// PlotFiller is a modular solution to extracting plots from root files.
// It allows the user to customize how he/she wants to produce the plots.
// The user can set various functions to change how the class calculates cut, weights, etc.
class PlotFiller
{
public:
   // NOTE That the user must provide the fill function at construction (because it is a required function).
   PlotFiller(PlotMap &plotsTemp,
                  std::vector<PhysicsProcessNEW*> &procsTemp,
                  void (*userFillFuncTemp) (PlotMap &, EventNtuple*, double) );
   ~PlotFiller();
   
   // Simple functions to change the functionality of the code.
   void setWeightFunction(double (*userWeightFuncTemp) (EventNtuple*, const PhysicsProcessNEW*));
   void setCutFunction(bool (*userCutFuncTemp) (EventNtuple*, const PhysicsProcessNEW*));
   void setProcessFunction(void (*userProcessFuncTemp) (EventNtuple*, const PhysicsProcessNEW*));
   
   // Debug functions
   void setMaximumEventsDEBUG(unsigned int maxEvts);
   
   // Runs the events and produces the output.
   void run();
   
private:
   // NOTE That the plots and processes are references.
   PlotMap &plots;
   std::vector<PhysicsProcessNEW*> &processes;
   unsigned int numberOfEvents;
   bool debug;
   
   // These are the custom functions.
   // Fills the plots
   void (*userFillFunc) (PlotMap &, EventNtuple*, double);
   // Returns a double that will multiply the weight
   double (*userWeightFunc) (EventNtuple*, const PhysicsProcessNEW*);
   // Returns true if the event passes the cut
   bool (*userCutFunc) (EventNtuple*, const PhysicsProcessNEW*);
   // This function is called once for each process before the events are run
   void (*userProcessFunc) (EventNtuple*, const PhysicsProcessNEW*);
   
   // These default functions allow the user to only have to create functions for weights etc that he/she wants to add.
   static bool defaultCutFunc(EventNtuple*, const PhysicsProcessNEW*)
   {
      return true;
   }
   static double defaultWeightFunc(EventNtuple*, const PhysicsProcessNEW*)
   {
      return 1.0;
   }
   static void defaultProcessFunc (EventNtuple*, const PhysicsProcessNEW*)
   {
      return;
   }
};
