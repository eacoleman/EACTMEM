#include "TAMUWW/MatrixElement/interface/MEJob.hh"

//#define MEONLY

// C++ libraries
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// ROOT libraries
#include <TBenchmark.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TTree.h>

// Our libraries
#include "TAMUWW/Integrator/interface/Integrator.hh"
#include "TAMUWW/MatrixElement/interface/EventFile.hh"
#include "TAMUWW/MatrixElement/interface/EventProb.hh"
#include "TAMUWW/MatrixElement/interface/topEventProb.hh"
#include "TAMUWW/MatrixElement/interface/HiggsEventProb.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"
#include "TAMUWW/MatrixElement/interface/PartonColl.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctionsRoot.hh"

using std::endl;
using std::cout;
using std::string;
using std::vector;


EventProb* MEJob::sm_eventProb(0);


//-----------------------------------------------------------------------------
MEJob::MEJob(EventFile& inputFile, const std::string& outputFilename) :
  m_inputFile(inputFile),
  m_outputFile(new TFile(outputFilename.c_str(), "RECREATE")),
  m_output(new METree()),
  m_benchmark(new TBenchmark()),
  m_useDynamicBounds(false)
{}


//-----------------------------------------------------------------------------
MEJob::~MEJob()
{
  delete m_benchmark;
  delete m_output;
  delete m_outputFile;
}


//-----------------------------------------------------------------------------
void MEJob::addEventProb(EventProb& prob, unsigned jetBin)
{
  m_probs[jetBin].push_back(&prob);
}


//-----------------------------------------------------------------------------
// This is the main method called when the MEJob is ran over all events.
// This method sets the basic framework, loop over all events, and calls
// the method m_doEvent for each event.
void MEJob::loopOverEvents()
{
  // Set the tree
  TTree* outputTree = new TTree("METree", "Output tree for matrix element");
  outputTree->Branch("METree.", "METree", &m_output);

  // not-so-elegant code to attach tnt to tree if it's a tnt input
  EventFile* myfile = &m_inputFile;
  if (dynamic_cast<EventNtupleEventFile*>(myfile)){
    EventNtuple* myntuple = dynamic_cast<EventNtupleEventFile*>(myfile)->getPointer();
    outputTree->Branch("EvtTree.","EventNtuple",&myntuple);
  }
  
  PartonColl partons;
  
  unsigned counter = 1;
  while (m_inputFile.fillNextEvent(partons)){
    
    m_output->clear();
    std::cout << "  Processing event " << counter++ << " with " << partons.getNJets() << " jets." << std::endl;
    m_doEvent(partons);
    
    outputTree->Fill();
    outputTree->Write("", TObject::kOverwrite);
  }

  delete outputTree;

}// loopOverEvents


//-----------------------------------------------------------------------------
// This method performs a mass scan on the top or Higgs mass in the range 
// ["low","high"] using a total of "steps" steps.
// Example 1: massScan(100,150,6,"Top")   will set mtop   to 100,110,120,130,140,150
// Example 2: massScan(100,120,2,"Higgs") will set mHiggs to 100,120
// Example 3: massScan(100,100,1,"Higgs") will set mHiggs to 100
// NOTE: The mass of the top (Higgs) will be changed in each and every process 
// that inherits from topEventProb (HiggsEventProb).
void MEJob::massScan(float low, float high, unsigned steps, 
		     std::string process){

  unsigned ievent = 0;
  PartonColl partons;
  unsigned counter = 1;

  // Loop over events
  while (m_inputFile.fillNextEvent(partons)){

      std::cout << "Processing event " << counter++ << std::endl;

      vector<TGraphErrors*> graphs;
   
      // Loop over the masses
      if (low > high) std::swap(low, high);
      float eachStep = (high - low) / steps;
      for (float imass = low; imass <= high; imass += eachStep) {

	// clear the output
	m_output->clear();
	
	// Change the requested mass
	if ( process == "Top"   ) m_setTopMass(imass);
	if ( process == "Higgs" ) m_setHiggsMass(imass);
	
	std::cout <<"Mass scan: "<< process<<" mass set to " << imass << std::endl;
	
	// Calculate all the ME's
	m_doEvent(partons);
	
	// Initialize graphs if not done before
	if (graphs.empty()) 
	  for (Int_t i = 0; i < m_output->getNProbStat(); ++i)
	    graphs.push_back(new TGraphErrors());
	       
	
	// Loop over all Graphs adding the points in it
	for (Int_t igraph = 0; igraph < m_output->getNProbStat(); ++igraph) {
	  unsigned counter = static_cast<unsigned>((imass - low) / eachStep);
	  const ProbStat* tempProb = m_output->getProbStat(igraph);
	  graphs[igraph]->SetPoint(counter, imass, tempProb->tEventProb);
	  graphs[igraph]->SetPointError(counter, 0,
					tempProb->tEventErr
					* tempProb->tEventProb); 
	}// for graphs

	// Save each graph to a file
	if (!graphs.empty()){
	  for (Int_t igraph = 0; igraph < m_output->getNProbStat(); ++igraph){
	    std::ostringstream name;
	    name << "MassProfile" << igraph << '_' << ievent++;
	    m_outputFile->cd();
	    graphs[igraph]->Write(name.str().c_str());
	    delete graphs[igraph];
	  }
	}
	graphs.clear();

      } // loop over masses
      
  }// while next event

}//massScan method


//-----------------------------------------------------------------------------
// This method set the mass of all Matrix Elements related to top 
void MEJob::m_setTopMass(double mass){

  // Loop over all the EventProbs  // Choose which vector of EventProbs to use, based on nJets
  for (EventProbMap::const_iterator itEventProbMap = m_probs.begin();
       itEventProbMap != m_probs.end() ;  itEventProbMap ++){

    //Loop over the ME probability objects
    for (EventProbVec::const_iterator it = itEventProbMap->second.begin();
	 it != itEventProbMap->second.end(); ++it) {
     
      //cast the object to topEventProb*
      topEventProb * top_eventProb = dynamic_cast<topEventProb*> (*it);

      //if the object cast to topEventProb then change its mass
      if (top_eventProb)
	top_eventProb->setTopMassAndWidth(mass);

    }//for

  }//for map

}//m_setTopMass


//-----------------------------------------------------------------------------
// This method set the mass of all Matrix Elements related to Higgs
void MEJob::m_setHiggsMass(double mass){

  // Loop over all the EventProbs  // Choose which vector of EventProbs to use, based on nJets
  for (EventProbMap::const_iterator itEventProbMap = m_probs.begin();
       itEventProbMap != m_probs.end() ;  itEventProbMap ++){

    //Loop over the ME probability objects
    for (EventProbVec::const_iterator it = itEventProbMap->second.begin();
	 it != itEventProbMap->second.end(); ++it) {
     
      //cast the object to topEventProb*
      HiggsEventProb * higgs_eventProb = dynamic_cast<HiggsEventProb*> (*it);

      //if the object cast to topEventProb then change its mass
      if (higgs_eventProb)
	higgs_eventProb->setHiggsMassAndWidth(mass);

    }//for

  }//for map

}//m_setTopMass


//-----------------------------------------------------------------------------
// Report the input values of the parton collection
void MEJob::m_reportInputValues(const PartonColl& partons){
  std::cout << "  Input values: \n"
	    << "\tLepton= " << partons.getLepton() << " Charge= "
	    << partons.getLepCharge() << "\n";
  std::cout << "\tNeutrino= " << partons.getNeutrino() << "\n";
  for (unsigned ijet = 0; ijet < partons.getNJets(); ++ijet){
    std::cout <<"\tJet  " << ijet << "= " << partons.getJet(ijet)
	      <<" tagged= "<< (partons.getFullJet(ijet).tag ==1 ? "yes" : "no") <<std::endl;      
  }

}//reportInputValues


//-----------------------------------------------------------------------------
// This method calculates all the ME probabilities for a single event. 
// The relevant input event information is taken from the input variable partons.
// This method loops over the Matrix Elements and the output of each is stored 
// in an object ProbStat.
// The ProbStat's of each ME is added to the METree object that holds the output Tree. 
void MEJob::m_doEvent(PartonColl& partons){

  using AuxFunctions::Math::square;

  std::pair<int, int> runEvent = m_inputFile.getRunEvent();
  m_output->setRunEvent(runEvent.first, runEvent.second);

  // Choose which vector of EventProbs to use, based on nJets
  EventProbMap::const_iterator itEventProbMap
    = m_probs.find(partons.getNJets());

  if (itEventProbMap == m_probs.end()){
    std::cout << "Invalid number of jets= " << partons.getNJets()
	      << "!  Skipping event." << std::endl;
    return;
  }

  //Report the input values from the partons variable
  m_reportInputValues(partons);

  //Loop over the ME probabilities to be calculated
  for (EventProbVec::const_iterator it = itEventProbMap->second.begin();
       it != itEventProbMap->second.end(); ++it) {

    // get the current probability
    sm_eventProb = *it;

    // set the static members of the event probability
    EventProb::setPartonColl(partons);
    EventProb::setMeasuredColl(partons);

    // report to the screen the Integration to be used
    std::cout << "  Starting " << sm_eventProb->getName() << " integration:\n"
	      << "\tUsing integrator= "
	      << sm_eventProb->getIntegrator().getName() << std::endl;

    // set the integrator dimension and integrand
    sm_eventProb->getIntegrator().setNdim(sm_eventProb->getDimension());
    sm_eventProb->getIntegrator().setIntegrand(sm_integrand);

    //Create the probability object and set it's ME creator
    ProbStat probstat(sm_eventProb->getProbMax());
    probstat.tmeType  = sm_eventProb->getEventProbType();
    probstat.tmeParam = sm_eventProb->getEventProbParam();

    unsigned counter   = 0;
    double   totalProb = 0;
    double   totalErr2 = 0;
    double   maxProb   = 0;
    m_benchmark->Reset();
    m_benchmark->Start("event");

    //Loop over all the permutations of one ME probability
    while (sm_eventProb->incrementLoop()){

      //Reset the maximum ME Probability
      sm_eventProb->resetMaxProbNumber();
	
      double returnVal[1], error[1], prob[1];
      int fail = 0, neval = 0;
      int* failPtr = &fail;
      int* nevalPtr = &neval;

      if (m_useDynamicBounds)
	sm_eventProb->setDynamicBounds();
	
      sm_eventProb->prepareForIntegral();


      //KLUDGE
#ifdef MEONLY
      double foghorn[3] = {0};
      returnVal[0] = sm_eventProb->execute(foghorn);
      error[0] = prob[0] = 0;
#else
      //	 std::cout << "initiate doIntegral" << std::endl;
      sm_eventProb->getIntegrator().doIntegral(returnVal, error, failPtr,
					       nevalPtr, prob); //<-Might Fail
#endif

      //save the number of evaluations
      probstat.tNevl[counter] = *nevalPtr;

      //save the error of the integration
      if (!(error[0] >= 0) && !(error[0] >= 0))
	error[0] = 0;

      if (returnVal[0])
	probstat.tRelerr[counter] = error[0] / returnVal[0];
      else
	probstat.tRelerr[counter] = 0;

      // save the fail, Prob and MaxProbs
      probstat.tIfail  [counter] = *failPtr;
      probstat.tProb   [counter] = returnVal[0];
      probstat.tMaxProb[counter] = sm_eventProb->getMaxProbNumber();

      // Calculate the total Prob, error and the max Prob
      totalProb += returnVal[0];
      totalErr2 += square(error[0]);
      if ( probstat.tMaxProb[counter] > maxProb)
	maxProb =  probstat.tMaxProb[counter];

      // report to screen
      std::cout << "\tIteration= " << counter
		<< "\tFail= " << *failPtr << " nEval= " << *nevalPtr << std::endl
		<< "\tProb= " << returnVal[0] 
		<< " RelError= "<< probstat.tRelerr[counter] << std::endl
		<< "\tMaxProb= "<< probstat.tMaxProb[counter]  << std::endl;

      // increment the 'permutations' counter
      ++counter;
    }//while permutations

    // Stop the watch
    m_benchmark->Stop("event");
    probstat.tRealTime = m_benchmark->GetRealTime("event");
    probstat.tCpuTime  = m_benchmark->GetCpuTime("event");
    // Normalize to the number of permutations
    if (counter > 0) {
      totalProb /= counter;
      totalErr2 /= counter*counter;
    }
    // Fill all the fields of probstat
    probstat.tEventProb    = totalProb;
    probstat.tEventMaxProb = maxProb;
    if (totalProb)
      probstat.tEventErr = std::sqrt(totalErr2) / totalProb;
    else
      probstat.tEventErr = 0;

    // Add the probstat to the vector output
    m_output->addProbStat(probstat);

    // Report to the screen
    std::cout << "\tTotal Probability= " << totalProb 
	      << "\t relError= "<< probstat.tEventErr << std::endl;
    std::cout << "\tMaximum Probability= " << sm_eventProb->getMaxProbNumber() << std::endl;
    std::cout << "\tCPU time= " << probstat.tCpuTime << " s   Real time= "
	      << probstat.tRealTime << " s" << std::endl;


  }//loop over ME probabilities

}//m_doEvent

//-----------------------------------------------------------------------------
void MEJob::sm_integrand(const int*, const double params[],
                         const int*, double results[]){

  results[0] = sm_eventProb->execute(params);

}// sm_integrand

//-----------------------------------------------------------------------------
// This method is supposed to find the peaks in parameter space
// IT PROBABLY DOES NOT WORK, SO VERIFICATION IS NEEDED BEFORE USING IT!!!!!
void MEJob::sm_peakFinder(const int* nDim, const double bounds[], int* nPoints,
                          double peaks[]){

  EventProb::VecVecDouble answer;
  sm_eventProb->getPeaks(answer, bounds);

  if (answer.size() > static_cast<unsigned>(*nPoints))
    {
      throw std::runtime_error("Too many peaks returned in sm_peakFinder!");
    }

  *nPoints = answer.size();

  unsigned counter = 0;
  for (EventProb::VecVecDouble::const_iterator it = answer.begin();
       it != answer.end(); ++it)
    {
      for (vector<double>::const_iterator it2 = it->begin(); it2 != it->end();
           ++it2)
	{
	  peaks[counter++] = *it2;
	}
    }

}//sm_peakFinder


