// This code libraries
#include "TAMUWW/OptimizeEPD/interface/PhysicsProcessForOpt.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/METree.hh"

//ROOT libraries
#include "TMath.h"

//C++ libraries
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <math.h>

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

   // Setting the default EPD Function
   userEPDFunc = &defaultEPDFunc;

   // Filling the index map of PhysicsProcess
   fillMETreeIndexMap();  

}//C'tor

//------------------------------------------------------------------------------
void PhysicsProcessForOpt::setEPDFunction(double (*userEPDFuncAux) (const ProbsForEPD &)){
   userEPDFunc = userEPDFuncAux;
}

//------------------------------------------------------------------------------
void PhysicsProcessForOpt::setProjectionsForOpt(){

   // The vector of projections
   vector<string> projs;

   // --first add all the eventProbs
   for (unsigned int nn = 0; nn < MicroNtuple::nEventProb; nn++){

      std::stringstream ss;
      //ss<<"eventProb["<<nn<<"]";
      ss<< "m_tProbStat.tEventProb["<<nn<<"]";
      projs.push_back(ss.str());  
   }
  
   // --second the last two are needed to calculate bProb[0] and bProb[1]
   //projs.push_back("EvtTree.jBtag[0]"); 
   //projs.push_back("mnt.bProb[0]"); 
   //projs.push_back("bProb[1]"); 

   // Put the projections into memory
   unsigned int nev = setProjections(projs, "1", "lLV.leptonCat[0]");  

   // Report
   cout<<"\tProcess\t"<<getName()<<"\t"<<groupName
       <<"\tloaded with "<<nev<<"\tevents"
       <<"\t weight="<<getWeight()<<endl;

   for (unsigned int hdet = 0; hdet < DEFS::nLeptonCat; hdet++)
      cout<<"\t\t\t\tScale Factor "<< getLeptonCatString((DEFS::LeptonCat)hdet)
          <<" =  " << getScaleFactor((DEFS::LeptonCat)hdet)<<endl;

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

   MicroNtuple::setIndexMap(indexMapOfME);

   //Create the histograms for each detector category
   TH1 * histo_cat[DEFS::nLeptonCat];
   for (unsigned int hdet = 0 ; hdet < DEFS::nLeptonCat; hdet++){
      histo_cat[hdet] = (TH1*) histo->Clone();
   }

   double evtProb[MicroNtuple::nEventProb];

   // Loop over all the rows
   for (unsigned int ro = 0; ro < getNEvents(); ro ++){

      // Get the useful information
      int    columns =  m_treeRows[ro].data.size();

      double bProb[2];
      bProb[0] = 0.5;
      bProb[1] = 0.5;

      for (int cc = 0; cc < columns; cc++){ // minus 6 because the last six are bProbs 
         evtProb[cc] = m_treeRows[ro].data[cc];
      }
    
      // Get the meProbs
      ProbsForEPD meProbs  =  MicroNtuple::getEventProbs(mhiggs, evtProb);
    
      // Get the probabilities for the EPD
      ProbsForEPD probs = MicroNtuple::getProbsForEPD(meProbs, coeffs, bProb, tagcat); 
      
      // Return the user epd
      double epd = userEPDFunc(probs);

    
      if(TMath::IsNaN(epd) || std::isinf(epd)){

         //stores the failed event entry in the map, so it won't print it again
         if(failed_entries_map.find(ro) == failed_entries_map.end()){
            failed_entries_map[ro] = true;
          
            cout<<"ERROR::PhysicsProcessForOpt::fillNormEPDHisto() epd is nan or inf in process "<<getName()<<endl
                <<"\t  on row "<<ro<<" on file "<<fileName<<endl
                <<"\t  SKIPPING EVENT FOREVER!"<<endl;
            probs.show("probs");
            meProbs.show("meProbs");
            cout<<"epd is "<<epd<<endl;
          
         }
         continue;
      }
    
      if(TMath::IsNaN(m_treeRows[ro].weight) || std::isinf(m_treeRows[ro].weight)){
         cout<<"ERROR::PhysicsProcessForOpt::fillNormEPDHisto() weight is nan or inf!  SKIPPING EVENT!"<<endl;
         cout<<"weight is "<<m_treeRows[ro].weight<<endl;
         continue;
      }
    
      // Do a basic check to make sure the category is within bounds. Otherwise complain heavily.
      if ( m_treeRows[ro].category < 0 || m_treeRows[ro].category >= (int)  DEFS::nLeptonCat) {
         cout<<"ERROR PhysicsProcessForOpt::fillNormEPDHisto (process "<<name //changed getName() to name for ppNEW
             <<") found category="<< m_treeRows[ro].category
             <<" which is not in the range [0,DEFS::nLeptonCat="<< DEFS::nLeptonCat<<") defined at SpecialTools/interface/Defs.hh"<<endl;
         cout<<"\t SKIPPING EVENT"<<endl;
      }
      else{
         // fill the histogram
         histo_cat[m_treeRows[ro].category]->Fill(epd,m_treeRows[ro].weight);
      }
    
   }//for rows   
  
   // Loop over the categories, 
   for (unsigned int hdet = 0; hdet < DEFS::nLeptonCat; hdet++){
      // normalize the histo to the expected number of events, and add to the total
      if (histo_cat[hdet]->Integral() > 0){
         if(TMath::IsNaN(getScaleFactor((DEFS::LeptonCat)hdet)) || std::isinf(getScaleFactor((DEFS::LeptonCat)hdet))){
            cout<<"ERROR::PhysicsProcessForOpt::fillNormEPDHisto()  getScaleFactor returns a nan or inf!"<<endl
                <<"\tSkipping process " << name << endl;
            continue;
         }
         else{
            histo_cat[hdet]->Scale(getScaleFactor((DEFS::LeptonCat)hdet));
         }
         histo->Add(histo_cat[hdet]);
      }
     
      // remove the histogram
      delete histo_cat[hdet];

   }//for categories

}//fillNormEPDHisto
