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
PhysicsProcessForOpt::PhysicsProcessForOpt(PhysicsProcess pp): PhysicsProcess(pp){

  // Set the projections needed for the optimization
  setProjectionsForOpt();
  
  // Fill the static map in MicroNtuple and get it
  MicroNtuple::fillIndexMap();
  indexMap = MicroNtuple::indexMap;

}//C'tor

//------------------------------------------------------------------------------
void PhysicsProcessForOpt::setProjectionsForOpt(){

  // The vector of projections
  vector<string> projs;
  // --first add all the eventProbs
  for (unsigned int nn = 0; nn < MicroNtuple::nEventProb; nn++){

    std::stringstream ss;
    //ss<<"METree.m_tProbStat["<<nn<<"].tEventProb";
    ss<<"eventProb["<<nn<<"]";
    projs.push_back(ss.str());
  }
  
  // --second the last six are needed to calculate bProb[0] and bProb[1]
  projs.push_back("mnt.bProb[0]"); 
  projs.push_back("mnt.bProb[1]"); 

  // Put the projections into memory
  unsigned int nev = setProjections(projs);  

  // Report
  cout<<"\tProcess\t"<<getName()<<"\t"<<getSubName()
      <<"\tloaded with "<<nev<<"\tevents"<<endl;
  cout<<"\t weight="<<getWeight()<<endl;
  

}//setProjections

//------------------------------------------------------------------------------
// This just adds to the histogram. It is the user responsibility to 
// reset the histo before calling this method if so desired.
void PhysicsProcessForOpt::fillNormEPDHisto(TH1* histo, 
					    DEFS::TagCat tagcat, 
					    double mhiggs, 
					    const ProbsForEPD & coeffs
					    ){

  int tag = 0;
  if ((tagcat ==  DEFS::pretag) || (tagcat ==  DEFS::eq0TSV ))
    tag=0; 
  else if (tagcat ==  DEFS::eq1TSV)
    tag=1;
  else if (tagcat ==  DEFS::eq2TSV)
    tag=2;


  //Create the histograms for each detector category
  TH1 * histo_cat[DEFS::nEvtCat];
  for (unsigned int hdet = 0 ; hdet < DEFS::nEvtCat; hdet++){
    histo_cat[hdet] = (TH1*) histo->Clone();
    //histo_cat[hdet] -> Sumw2();
  }

  double evtProb[MicroNtuple::nEventProb];

  // Access the last two columns with mnt.knntag[0] and [1] to replace with bTag0 and bTag1
  map< int, ProcessSubSample * > subSamplesMap = getProcessTree()->getSubSamples();
  
  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = subSamplesMap.begin();
       subS != subSamplesMap.end(); subS++) {
      
    // Get the rows of the sub sample
    std::vector<TreeRow> rows = subS->second->getVectorOfRows();
      
    // Loop over all the subsample rows
    for (unsigned int ro = 0; ro < rows.size(); ro ++){
  
      // Get the useful information
      int    columns =  rows[ro].data.size();
      double bProb[2];
      bProb[0] = rows[ro].data[columns- 2];//  - 2*(2-tag)]; // 0T: -6 , 1T: -4, 2T: -2 
      bProb[1] = rows[ro].data[columns- 1];// - 2*(2-tag)]; //     -5       -3      -1

      for (int cc = 0; cc < columns - 6; cc++) // minus 6 because the last six are bProbs
	evtProb[cc] = rows[ro].data[cc];

      // Get the meProbs
      ProbsForEPD meProbs = MicroNtuple::getEventProbs(mhiggs, evtProb);

      // Get the probabilities for the EPD
      ProbsForEPD probs = MicroNtuple::getProbsForEPD(meProbs, coeffs, bProb, tagcat);
      
      // Return the WW+WZ probability
      double epd = (probs.wz+probs.ww) / (probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
					  probs.wc + probs.qcd + probs.tt +
					  probs.ww + probs.wz);    

      // Do a basic check to make sure the category is within bounds. Otherwise complain heavily.
      if ( rows[ro].category < 0 || rows[ro].category >= (int)  DEFS::nEvtCat) {
	cout<<"ERROR PhysicsProcessForOpt::fillNormEPDHisto (process "<<getName()
	    <<") found category="<< rows[ro].category
	    <<" which is not in the range [0,DEFS::nEvtCat="<< DEFS::nEvtCat<<") defined at SpecialTools/interface/Defs.hh"<<endl;
	cout<<"\t SKIPPING EVENT"<<endl;
      }
      else 
	// fill the histogram
	histo_cat[rows[ro].category]->Fill(epd,rows[ro].weight);
       
    }//for rows   
    
  }//for subsamples
 
  // Loop over the categories, 
  for (unsigned int hdet = 0; hdet < DEFS::nEvtCat; hdet++){

    // normalize the histo to the expected number of events, and add to the total
    if (histo_cat[hdet]->Integral() > 0){
      histo_cat[hdet]->Scale(getCategoryNorm(hdet).value/histo_cat[hdet]->Integral());
      histo->Add(histo_cat[hdet]);
    }

    // remove the histo
    delete histo_cat[hdet];

  }//for categories

}//fillNormEPDHisto
