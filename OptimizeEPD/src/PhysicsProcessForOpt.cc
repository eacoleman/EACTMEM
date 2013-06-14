// This code libraries
#include "TAMUWW/OptimizeEPD/interface/PhysicsProcessForOpt.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"


//ROOT libraries

//C++ libraries
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iostream>
#include <sstream>


using std::map;
using std::cout;
using std::endl;
using std::vector;
using std::string;

//------------------------------------------------------------------------------
// C'tor
PhysicsProcessForOpt::PhysicsProcessForOpt(const PhysicsProcessMemory & pp): PhysicsProcessMemory(pp){

  // Set the projections needed for the optimization
  setProjectionsForOpt();
  
}//C'tor

//------------------------------------------------------------------------------
void PhysicsProcessForOpt::setProjectionsForOpt(){

  // The vector of projections
  vector<string> projs;

  // --first add all the eventProbs
  for (unsigned int nn = 0; nn < MicroNtuple::nEventProb; nn++){

    std::stringstream ss;
    ss<<"eventProb["<<nn<<"]";
    projs.push_back(ss.str());  
  }
  
  // --second the last two are needed to calculate bProb[0] and bProb[1]
  projs.push_back("EvtTree.jBtag[0]"); 
  //projs.push_back("mnt.bProb[0]"); 
  //projs.push_back("bProb[1]"); 

  // Put the projections into memory
  unsigned int nev = setProjections(projs);  

  // Report
  cout<<"\tProcess\t"<<getName()//<<"\t"<<getSubName()
      <<"\tloaded with "<<nev<<"\tevents"<<endl;
  cout<<"\t weight="<<getWeight()<<endl;
  

}//setProjections

//------------------------------------------------------------------------------
// This just adds to the histogram. It is the user responsibility to 
// reset the histo before calling this method if so desired.
void PhysicsProcessForOpt::fillNormEPDHisto(TH1* histo, 
                                            DEFS::TagCat tagcat, 
                                            double mhiggs, 
                                            const ProbsForEPD & coeffs){

  int tag = 0;
  if ((tagcat ==  DEFS::pretag) || (tagcat ==  DEFS::eq0TSV ))
    tag=0; 
  else if (tagcat ==  DEFS::eq1TSV)
    tag=1;
  else if (tagcat ==  DEFS::eq2TSV)
    tag=2;

  MicroNtuple mnt;
  mnt.fillIndexMap();
  //MicroNtuple::fillIndexMap();

  //Create the histograms for each detector category
  TH1 * histo_cat[DEFS::nLeptonCat];
  for (unsigned int hdet = 0 ; hdet < DEFS::nLeptonCat; hdet++){
    histo_cat[hdet] = (TH1*) histo->Clone();
    histo_cat[hdet] -> Sumw2();
  }

  double evtProb[MicroNtuple::nEventProb];

  vector<TreeRow> localTreeRows = getVectorOfRows();

   // Loop over all the rows
  for (unsigned int ro = 0; ro < getNEvents(); ro ++){

    // Get the useful information
    int    columns =  localTreeRows[ro].data.size();

    double bProb[2];
    bProb[0] = 0.5;
    bProb[1] = 0.5;

    for (int cc = 0; cc < columns; cc++){ // minus 6 because the last six are bProbs 
      evtProb[cc] = localTreeRows[ro].data[cc];
    }
    
    // Get the meProbs
    ProbsForEPD meProbs  =  MicroNtuple::getEventProbs(mhiggs, evtProb);
    
    // Get the probabilities for the EPD
    ProbsForEPD probs = MicroNtuple::getProbsForEPD(meProbs, coeffs, bProb, tagcat);
      
    // Return the WW+WZ probability
    double epd = (probs.wz+probs.ww) / (probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
					probs.wc + probs.qcd + probs.tt +
					probs.ww + probs.wz);   
      
    // Do a basic check to make sure the category is within bounds. Otherwise complain heavily.
    if ( localTreeRows[ro].category < 0 || localTreeRows[ro].category >= (int)  DEFS::nLeptonCat) {
      cout<<"ERROR PhysicsProcessForOpt::fillNormEPDHisto (process "<<name //changed getName() to name for ppNEW
	  <<") found category="<< localTreeRows[ro].category
	  <<" which is not in the range [0,DEFS::nLeptonCat="<< DEFS::nLeptonCat<<") defined at SpecialTools/interface/Defs.hh"<<endl;
      cout<<"\t SKIPPING EVENT"<<endl;
    }
    else{
      // fill the histogram
      histo_cat[localTreeRows[ro].category]->Fill(epd,localTreeRows[ro].weight);
    }
    
  }//for rows   
 
  // Loop over the categories, 
  for (unsigned int hdet = 0; hdet < DEFS::nLeptonCat; hdet++){
    // normalize the histo to the expected number of events, and add to the total
    if (histo_cat[hdet]->Integral() > 0){
       histo_cat[hdet]->Scale(getScaleFactor((DEFS::LeptonCat)hdet));
       histo->Add(histo_cat[hdet]);
    }

    // remove the histogram
    delete histo_cat[hdet];

  }//for categories

}//fillNormEPDHisto
