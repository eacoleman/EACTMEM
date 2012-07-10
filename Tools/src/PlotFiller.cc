
// Title:     PlotFiller.cc
// Author:    Travis Lamb
// Started:   22nd of June, 2012
// Last Edit: 22nd of June, 2012

// Uses Plot and PhysicsProcessNEW to return a plot for a given process and yield.

// Our libraries
#include "TAMUWW/Tools/interface/PlotFiller.hh"
//#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

using namespace std;

//#####################################################################
//#####################################################################
//#####################################################################

PlotFiller::PlotFiller(map<string, Plot*> &plotsTemp,
                       vector<PhysicsProcessNEW*> &procsTemp,
                       void (*userFillFuncTemp) (map<string, Plot*> &, EventNtuple*, double)):
   plots(plotsTemp),
   processes(procsTemp),
   userFillFunc(userFillFuncTemp)
{
   userWeightFunc = &defaultWeightFunc;
   userCutFunc = &defaultCutFunc;
   userProcessFunc = &defaultProcessFunc;
   userInitEventFunc = &defaultInitEventFunc;
   
   numberOfEvents = 0;
   
   debug = false;
}

PlotFiller::~PlotFiller()
{
   
}

void PlotFiller::setWeightFunction(double (*userWeightFuncTemp) (EventNtuple*, const PhysicsProcessNEW*))
{
   userWeightFunc = userWeightFuncTemp;
}

void PlotFiller::setCutFunction(bool (*userCutFuncTemp) (EventNtuple*, const PhysicsProcessNEW*))
{
   userCutFunc = userCutFuncTemp;
}

void PlotFiller::setProcessFunction(void (*userProcessFuncTemp) (EventNtuple*, const PhysicsProcessNEW*))
{
   userProcessFunc = userProcessFuncTemp;
}

void PlotFiller::setInitializeEventFunction(void (*userInitEventFuncTemp) (EventNtuple*, const PhysicsProcessNEW*))
{
   userInitEventFunc = userInitEventFuncTemp;
}

void PlotFiller::setMaximumEventsDEBUG(unsigned int maxEvts)
{
   debugNumberOfEvents = maxEvts;
   debug = true;
}

void PlotFiller::run()
{
   for(unsigned int i = 0; i < processes.size(); i++)
   {
      cout << "\nDoing Process " << processes[i]->name;
      
      // Tell all plots to prepare for filling 
      for (map<string, Plot*>::iterator p = plots.begin() ; p != plots.end() ; p++)
         p->second->prepareToFillProcess(processes[i]);
      
      // Create the eventntuple and set the branch address
      EventNtuple * ntuple = new EventNtuple();
      TChain * c = processes[i]->chain;
      c->SetBranchAddress("EvtNtuple",&ntuple);

      // The counter for how many events pass the cuts in each process
      unsigned int numProcEvts = 0;
      double sumW = 1.;
   
      // Define the weight variable and set the default as 1.0
      double weight = 1.0;

      // Loop over events in the process
      if(debug && debugNumberOfEvents < c->GetEntries())
      {
         numberOfEvents = debugNumberOfEvents;
      }
      else
      {
         numberOfEvents = c->GetEntries();
      }
      
      cout << " with " << numberOfEvents << " events." << endl;
      
      // This runs once for each process before the events are run.
      userProcessFunc(ntuple, processes[i]);

      for (unsigned int ev = 0 ; ev < numberOfEvents ; ev++)
      {
         // Report every now and then
         if ((ev % 10000) == 0)
            cout<<"\tevent "<<ev<<endl;
         
         // Get the given entry
         c->GetEntry(ev);
         
         // Runs before any cuts are made
         userInitEventFunc(ntuple, processes[i]);
         
         // Cut events here
         if(!userCutFunc(ntuple, processes[i]))
            continue;
         
         // reset the weight for each event, so the default will be 1.0
         weight = 1.0;
         weight *= userWeightFunc(ntuple, processes[i]);
         
         // Fill plots
         userFillFunc(plots, ntuple, weight);
         
         // Keep track of the total number of events & weights passing the cuts 
         sumW += weight;
         numProcEvts++;
      }
      
      cout << "Number of " << processes[i]->name << " events passing the cuts: " << numProcEvts << " with weights: " << sumW << endl;
   }
}
