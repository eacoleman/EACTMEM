// This class calculates a figure of merit between signal and background histograms
// All the methods are static.
// Author: Ricardo Eusebi, Nov 2008.

// C++ libraries
#include <iostream>
#include <cmath>

// ROOT libraries
#include "TMinuit.h"

// This class libraries
#include "TAMUWW/SpecialTools/interface/FigureOfMerit.hh"

using std::cout;
using std::endl;

//Declare the static members
TH1* FigureOfMerit::signal;
TH1* FigureOfMerit::background;

// ----------------------------------------------------------------------------
FigureOfMerit::FigureOfMerit(){

}// C'tor


// ----------------------------------------------------------------------------
FigureOfMerit::~FigureOfMerit(){

}// D'tor

// ----------------------------------------------------------------------------
// This checks a histogram for empty bins
// Returns true if no empty bins
bool FigureOfMerit::checkBackgroundHistogram(TH1* back, double relativeBinErrorLimit){

  // Report an error if there is an something in the 
  // underflow or overflow bins
  if (back->GetBinContent(0)       > 0 ||
      back->GetBinContent(back->GetNbinsX()+1) > 0 ){
    
    cout<<" WARNING FigureOfMerit::checkBackgroundHistogram background"
	<<" histogram have underflow/overflow bins."<<endl;
  }

  // Loop over all bins
  for (int ibin = 1; ibin <= back->GetNbinsX(); ++ibin){

    // If there is no background in this bin return false
    if (back->GetBinContent(ibin) == 0){

      std::cerr << "ERROR FigureOfMerit::checkBackgroundHistogram bin "<<ibin<<" is not filled."<<endl;
      return false;

    } 
    // If the background contribution is smaller than three time its error return a warning.
    else if (back->GetBinContent(ibin)  < relativeBinErrorLimit * back->GetBinError(ibin) ){

      std::cerr << "WARNING FigureOfMerit::checkBackgroundHistogram "
		<<"  background error in bin "<<ibin<<" is too large!\n";
      return false;
    }
    
  }//for

 return true;

}// checkBackgroundHistogram;

// ----------------------------------------------------------------------------
// This is the one to use when estimating cross section as one would do calculate
// limits on the cross section.
// Note that if trying to find the cross section of the signal (sigma_sig) the 
// figure of merit (fom) returned here is sigma_sig=1/fom. This is, the fom is 
// the inverse of the error on the cross section.
double FigureOfMerit::usingShapeFromTemplates(TH1* _signal, TH1* _background, double relativeBinErrorLimit){

  // The returning figure of merit
  double fom = 0;

  //Try to run only of the background histo does not have empty bins
  if (checkBackgroundHistogram(_background, relativeBinErrorLimit)){

    // Assign both histograms to static functions
    signal     = _signal;
    background = _background;

    // Create the minimization fitter with one parameter.
    // Do not create minuit as an object, always create in the heap,
    // otherwise it seems that running twice gives different results!!
    TMinuit  * minuit = new TMinuit(1);
    
    // Set the minimization function
    minuit->SetFCN(minimizationFunction);
    
    Double_t arglist[10];
    Int_t ierflg = 0;
    
    // Make Minuit run in a quiet (i.e. non-verbose) mode
    minuit->SetPrintLevel(-1);
    arglist[0] = 0;
    gMinuit->mnexcm("SET NOWarnings",&arglist[0],1,ierflg);    
    arglist[0] = -1;
    gMinuit->mnexcm("SET PRint",&arglist[0],1,ierflg);

    
    // Set the error as half a point around the NLL
    arglist[0] = 1;
    minuit->mnexcm("SET ERR", arglist ,1,ierflg);
    
    // Set starting values and step sizes for parameters
    //            (par# , name  start val , step  , min val,  max val, ierflg);
    minuit->mnparm(    0, "a1",   0.50    , 0.1   ,  0     ,   200   , ierflg);
    
    // Do the minimization
    arglist[0] = 0.5; // 0.5 for 1 sigma. 2 for 2 sigmas
    minuit->mnexcm("SET NOG",arglist,0,ierflg); //
    minuit->mnexcm("MIGRAD", arglist ,2,ierflg); // -
    arglist[0] = 50;
    arglist[1] = 0.01;
    minuit->mnexcm("MINOS",arglist,2,ierflg);
    
    // Retrieve the central value (central) and errors (up,down)
    double central, err_down, err_up, junk, junk1, junk2;
    minuit->GetParameter(0, central, junk);
    minuit->mnerrs(0, err_up, err_down, junk1, junk2);
    
    // Report ?
    // cout<<" central Sig ="<<central<<" +"<<err_up<<"-"<<err_down<<" junk ="<<junk<<endl;
    // cout<<" Number of calls = "<<minuit->fNfcn<<endl;
    // Clean up
    delete minuit;

    // Do a basic check
    if (err_up ==0){
      cout<<" ERROR  FigureOfMerit::usingShapeFromTemplates err_up= "<<err_up<<endl
	  <<"   This can sometimes happen when the fom is < 1/max_val"<<endl
	  <<"   To solve it just increase the value of max_val! " <<endl;     
      return 0;
    }

    // The figure of merit is one over the error    
    fom = 1/err_up;

  }// background does not have empty bins

  return fom;

}// usingShapeFromTemplates

// ----------------------------------------------------------------------------
// The minimization function used in ::usingShapeFromTemplates
void FigureOfMerit::minimizationFunction(Int_t &npar, Double_t *gin, Double_t &f, 
			  Double_t *par, Int_t iflag){

   //calculate chisquare
   Double_t sum_nll = 0;
   for (int ibin = 1 ; ibin <= signal->GetNbinsX() ; ibin++) {

     double bin_num  = par[0]*signal->GetBinContent(ibin) * par[0]*signal->GetBinContent(ibin);
     double bin_den  = par[0]*signal->GetBinContent(ibin) + background->GetBinContent(ibin)       
       + par[0]*signal->GetBinError(ibin) * par[0]*signal->GetBinError(ibin)
       + background->GetBinError(ibin) * background->GetBinError(ibin);

     sum_nll +=  bin_num/bin_den;     

   }//for    

   //cout<<" par[0]="<<par[0]<<" sum_nll="<<sum_nll<<endl;
   
   f = sqrt(sum_nll);

}// minimizationFunction


// ----------------------------------------------------------------------------
// This method computes the sensitivity as 
//     S = sqrt(sum_bin S_bin^2/ (S_bin+B_bin+DeltaS_bin^2+DeltaB_bin^2))
// The sensitivity is set to zero if any given bin does not have a background 
// contribution, or  if that contribution is smaller than 3 times the error 
// on that contribution
double FigureOfMerit::usingChi2(TH1* signal, TH1* background){

  // Check that the number of bins is the same
  if (signal->GetNbinsX() != background->GetNbinsX()){
    cout<<"   FigureOfMerit::usingChi2 was given two histograms of different sizes!"<<endl;
    return 0;
  }
 
  //Check that this histo passes the basic cuts
  if (checkBackgroundHistogram(background)){

    // Get the number of bins
    unsigned int nBins = signal->GetNbinsX();    
    
    // Loop over bins
    double total = 0;
    for (unsigned ibin = 1; ibin <= nBins; ++ibin){
      
      double dataStat2 = signal->GetBinContent(ibin) + background->GetBinContent(ibin) ;      
      double mcStat2   = signal->GetBinError(ibin)     * signal->GetBinError(ibin) 
	+ background->GetBinError(ibin) * background->GetBinError(ibin); 
            
      // Compute this bin's sensitivity
      double signal2         = signal->GetBinContent(ibin)*signal->GetBinContent(ibin);
      double bin_sensitivity = signal2 / ( dataStat2 + mcStat2 );
      
      // Add it to the total
      total += bin_sensitivity;
      
    }//for bins
    
    //return the square of the total
    return sqrt(total);

  }

  // if we are still here return zero
  return 0;

}//using Chi2


// ----------------------------------------------------------------------------
// This method computes the sensitivity as S = sqrt(sum_bin S_bin^2/B_bin)
// The sensitivity is set to zero if any given bin does not have a background 
// contribution, or  if that contribution is smaller than 3 times the error 
// on that contribution
double FigureOfMerit::usingS2OverB(TH1* signal, TH1* background){

  // Check that the number of bins is the same
  if (signal->GetNbinsX() != background->GetNbinsX()){
    cout<<"   FigureOfMerit::usingS2OverB was given two histograms of different sizes!"<<endl;
    return 0;
  }
 
  //Check that this histo passes the basic cuts
  if (checkBackgroundHistogram(background)){

    // Get the number of bins
    unsigned int nBins = signal->GetNbinsX();    
    
    // Loop over bins
    double total = 0;
    for (unsigned ibin = 1; ibin <= nBins; ++ibin){
      
      // Compute this bin's sensitivity
      double signal2         = signal->GetBinContent(ibin)*signal->GetBinContent(ibin);
      double bin_sensitivity = signal2 / background->GetBinContent(ibin) ;
      
      // Add it to the total
      total += bin_sensitivity;
      
    }//for bins
    
    //return the square of the total
    return sqrt(total);

  }

  // if we are still here return zero
  return 0;

}//using S2/B
