
// This macro is intended to calculate the most discriminating EPD coefficients
// Author: Ricardo Eusebi, November 10th, 2008

//ROOT libraries
#include <TStyle.h>
#include "TDirectory.h"
#include "TH1F.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TString.h"

//C++ libraries
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

//Other libraries
#include "TAMUWW/OptimizeEPD/interface/PhysicsProcessForOpt.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/SpecialTools/interface/FigureOfMerit.hh"
#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"



using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;
using std::ofstream;
using std::stringstream;



// ===================================================================
void modifyEPDCoefficients(ProbsForEPD & EPDcoeffs, double maxPower, 
			   double factor){

  // Select a coeff to modify randomly until we pick one we should be optimizing
  int aux_coef = (int) (gRandom->Rndm()* 15 );  
  
  // Find the multiplier to which multipliy that factor 
  double aux_pwr  = (gRandom->Rndm()-0.5)*2*maxPower;
  double multiplier = pow(factor,aux_pwr);

  // actually multiply that factor
  switch(aux_coef){
  case 0:    
    EPDcoeffs.wh     *= multiplier; 
    break;
  case 1:
    EPDcoeffs.hww    *= multiplier; 
    break;
  case 2:
    EPDcoeffs.schan  *= multiplier;
    break;
  case 3:
    EPDcoeffs.tchan  *= multiplier;
    break;
  case 4:
    EPDcoeffs.tchan2 *= multiplier;
    break;
  case 5:
    EPDcoeffs.tt     *= multiplier;
    break;
  case 6:
    EPDcoeffs.wlight *= multiplier; 
    break;
  case 7:
    EPDcoeffs.zlight *= multiplier;
    break;
  case 8:
    EPDcoeffs.wbb    *= multiplier;
    break;
  case 9:
    EPDcoeffs.wc     *= multiplier;
    break;
  case 10:
    EPDcoeffs.wgg    *= multiplier;
    break;
  case 11:
    EPDcoeffs.ww     *= multiplier;
    break;
  case 12:
    EPDcoeffs.wz     *= multiplier;
    break;
  case 13:
    EPDcoeffs.zz     *= multiplier;
    break;
  case 14:
    EPDcoeffs.qcd    *= multiplier;
    break;

  }//end switch

}// modifyEPDCoefficients



// ===================================================================
double createHistoAndGetFOM( vector<PhysicsProcessForOpt*> processes, 
			     const ProbsForEPD & coeffs, 
			     DEFS::TagCat tagcat, double mhiggs,
			     TH1 * signalHisto,  TH1 * allBackHisto){

    // Reset all the templates
    signalHisto->Reset();
    allBackHisto->Reset();

    // Fill the signal and background histos for this set of EPDcoeffs
    for (unsigned p=0;p<processes.size();p++){

      //get this process' name
       string thisProcName = (string)processes[p]->getName();

      // put the WW or WX processes into the signal histogram
      if (thisProcName.find("WW") == 0 || thisProcName.find("WZ") == 0){

	// signal contains WH
	processes[p]->fillNormEPDHisto(signalHisto, tagcat, mhiggs, coeffs);				       

      }else{ 
	// every other process is background
	processes[p]->fillNormEPDHisto(allBackHisto, tagcat, mhiggs, coeffs);

      }
      
    }// for processes

    // Calculate the figure of merit
    double figOfMerit = FigureOfMerit::usingShapeFromTemplates(signalHisto, allBackHisto);
    //cout<<"   figOfMerit shape= "<< figOfMerit<<endl;
    //cout<<"   figOfMerit chi2 = "<<  FigureOfMerit::usingChi2(signalHisto, allBackHisto)<<endl;
    //cout<<"   figOfMerit S2OB = "<<  FigureOfMerit::usingS2OverB(signalHisto, allBackHisto)<<endl;
    

    return figOfMerit;

}// createHistoAndGetFOM

// ===================================================================
// Loop over possible coefficient configurations and keep the most 
// most discriminant EPD.
// -------------------------------------------------------------
ProbsForEPD optimizeEPDCoeffs( vector<PhysicsProcessForOpt*> processes, 
			       DEFS::TagCat tagcat, double & bestFigOfMerit){

  // The returning set starts with all ones.
  ProbsForEPD bestEPDcoeffs(1);

  // The set use for trial and error
  ProbsForEPD EPDcoeffs;
  
  // The set used for the normalization
  ProbsForEPD normEPDcoeffs(
			    1./0.35e-09, // wh    115 GeV 
			    1./0.50e-10, // hww    
			    1./0.22e-07, // schan  
			    1./0.45e-06, // tchan  
			    0,           // tchan2 
			    1./0.003,    // tt     
			    1./0.25e-03, // wlight  
			    1./0.28e-05, // zlight 
			    0,           // wbb    
			    0,           // wc     
			    0,           // wgg    
			    1./0.35e-06, // ww     
			    1./0.35e-07, // wz     
			    0,           // zz     
			    1./0.1       // qcd    
			    );

  double mhiggs = 0;
  
// -------------------------------------------------------------
// B- Loop over possible coefficient configurations obtaining the 
//    most discriminant EPD.
// -------------------------------------------------------------

  // Create the templates based on EPDcoeffs for each template 
  int bins = 40;
  double minXaxis = 0;
  double maxXaxis = 1;
  TH1 * signalHisto  = new TH1D("sH","sH",bins,minXaxis,maxXaxis);
  TH1 * allBackHisto = new TH1D("bH","bH",bins,minXaxis,maxXaxis);  
  signalHisto->Sumw2();
  allBackHisto->Sumw2();

  // Define the parameters of the run 
  double maxPower = 1;// 1 is the default
  double factor   = 2;
  int maxIter     = 2000;// 1000 is default

  // Flag to indicate if at least one fom was sucessfuly computed
  bool foundAtLeastOneFigureOfMerit = false; 

  // Create a file to store the best histograms. 
  TFile bhf("optimizeEPD_templates.root","RECREATE");

  // Perform the iterations
  for (int iter = 0; iter < maxIter; iter++){
    
    // report every 100's of events
    if (iter % 100 == 0)  
      cout<<"Doing iteration "<<iter<<endl;
    
    // reset all the factors to the best factors
    EPDcoeffs = bestEPDcoeffs; 
    
    // don't modify anything the first pass around
    if (iter != 0)
      modifyEPDCoefficients(EPDcoeffs, maxPower, factor);

    // Calculate the figure of merit for this processes and EPDcoeffs'
    double figOfMerit = createHistoAndGetFOM(processes, EPDcoeffs * normEPDcoeffs,
					     tagcat, mhiggs,
					     signalHisto,  allBackHisto);

    // If this is the best figure of merit 
    if (figOfMerit > bestFigOfMerit) {   

      // save these coefficients as the best coefficients
      bestFigOfMerit = figOfMerit;
      foundAtLeastOneFigureOfMerit = true;
      cout<<"   BEST set found at iteration "<<iter<<" with fom="<<bestFigOfMerit<<endl;
      bestEPDcoeffs = EPDcoeffs;
      
      // Save these histos to file
      stringstream ssB;
      ssB<<allBackHisto->GetName()<<"_iter"<<iter;
      allBackHisto->Write(ssB.str().c_str());
      stringstream ssS;
      ssS<<signalHisto->GetName()<<"_iter"<<iter;
      signalHisto->SetLineColor(2);
      signalHisto->Write(ssS.str().c_str());      

    }// if best fom


    //if it did not found anything after 100 more iterations double the power
    if (!foundAtLeastOneFigureOfMerit  && iter !=0 && (iter % 100 == 0)){
      maxPower *=  2;
      cout<<" Nothing found in the first "<<iter<<" iterations. Increasing the power to "<<maxPower<<endl;
    }
 
  }//for 

  // Close the file3
  bhf.Close();

  // clean up
  delete signalHisto;
  delete allBackHisto;


  // Print the normEPDcoeffs
  cout<<"normalized constructor would be:"<<endl
      <<"\t" <<bestEPDcoeffs.getStringConstructor()<<endl;

  // return the coefficients.
  return bestEPDcoeffs * normEPDcoeffs;

    
}// optimizeEPDCoeffs
 

// ===================================================================
// This method gets all the PhysicsProcesses
// and loads the relevant info into memory. 
vector<PhysicsProcessForOpt*> loadProcessesIntoMemory(DEFS::JetBin jetBin, DEFS::TagCat tagcat){

  // Get the physical processes for WW. The false means "data" is not one of the processes. 
  vector<PhysicsProcess*> phy_processes  = DefaultValues::getProcessesWW(jetBin, tagcat,false);

  // Now construct a set of PhysicsProcessForOpt out of them
  vector<PhysicsProcessForOpt*> processes ;
  for (unsigned int pp = 0 ; pp < phy_processes.size(); pp++){
     PhysicsProcessMemory ppm = (PhysicsProcessMemory)*phy_processes[pp];
     processes.push_back(new PhysicsProcessForOpt(ppm));
  }

  // Report processes to screen
  for (unsigned p=0;p<processes.size();p++){
    cout<<"\tprocess name="<<processes[p]->getName()
       <<"\ttitle="<<processes[p]->groupName
       //<<"\tnorm="<<processes[p]->getTotalExpectedEvents().value
       //<<"\tpercentualError="
       //<<processes[p]->getTotalExpectedEvents().error/processes[p]->getTotalExpectedEvents().value
	<<endl;
    
  }// for

 
  return  processes ;

}//loadProcessesIntoMemory

// ===================================================================
// This is done in several steps
// A- Get the set of PhysicsProcess' 
// B- Create the set of coefficients and give initial values
// C- Load formulas into memory.
// D- Find coefficients for given PhysicsProcess and coefficients
//      - Loop over possible coefficient configurations obtaining the 
//        most discriminant EPD.
ProbsForEPD optimizeEPDCoeffs(DEFS::TagCat tagcat, double & bestFigOfMerit){

  // jetbin we are optimizing over
  DEFS::JetBin jetBin = DEFS::jets2;

  // load all the processes into memory
  vector<PhysicsProcessForOpt*>  processes  = loadProcessesIntoMemory(jetBin, tagcat);

  // do the actual computation of best EPD coefficients for the given processes
  ProbsForEPD bestEPDCoeffs = optimizeEPDCoeffs(processes, tagcat, bestFigOfMerit);

  // Free memory
  for (unsigned p=0;p<processes.size();p++)
    delete processes[p];   
   
  // Print the best EPD coefficients
  cout<<endl<<" BEST Coefficients with fig of merit="<< bestFigOfMerit<<" follow"<<endl;
  cout<<bestEPDCoeffs.getStringConstructor()<<endl;

  // return the best coefficients
  return bestEPDCoeffs; 

}// OptimizeEPDCoefficients




// ===================================================================
// This method just calls the method that calculates the optimized 
// coefficients and then just print them to the output.
void optimizeEPDCoefficientsWW(){

  // Select the analysis type, only to report to the screen...
  DEFS::Ana::Type anaType = DEFS::Ana::WWAnalysis;

  cout<<endl;
  cout<<"----------------------------------------------"<<endl;
  cout<<" OPTIMIZING  coefficients for "<<DEFS::Ana::getTypeString(anaType)<<endl;
  cout<<"----------------------------------------------"<<endl<<endl; 

  // The resulting figure of merit of the optimization for PreTag,=1T and >= 2 tags
  double figOfMerit0 = 0;

  // Optimize the coefficients for the pretag
  ProbsForEPD coeffs0 = optimizeEPDCoeffs(DEFS::pretag, figOfMerit0);

  // - - - - - - - - - - - - - - - - - - - - - - - - - -
  // The rest just create and print the optimized 
  //     coefficients to the output file
  // - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create the outfile; no need to close it as it will be destructed automatically.
  ofstream outFile("EPDCoefficients.txt");

  // Print Instructions
  outFile <<" You can copy and paste from this to MicroNtuple.cc"<<endl;
  outFile << endl << endl << endl;
  outFile << coeffs0.getStringConstructor()<<" // DEFS::TagCat="<<DEFS::pretag<<" figOfMerit="<<figOfMerit0<<endl;
  
  // close the outFile
  outFile.close();

}// optimizeEPDCoefficientsWW


// ===================================================================
// The MAIN program
int main(int argc, char* argv[]){

  optimizeEPDCoefficientsWW();

}

  
