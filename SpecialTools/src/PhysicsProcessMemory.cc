// Name: PhysicsProcessMemory.cc
// Author: Alexx Perloff

// This class will eventually replace an older Physics class. It derives from PhysicsProcessNEW and
// adds the ability to load a TTree into memory

#include "TFile.h"

#include "TAMUWW/SpecialTools/interface/PhysicsProcessMemory.hh"

using namespace std;

// #########################################################
// ################ PHYSICS PROCESS MEMORY #################
// #########################################################

PhysicsProcessMemory::PhysicsProcessMemory (string procName,
                                            string groupingName,
                                            string fileNameTEMP,
                                            string treeName,
                                            bool loadIntoMemory):
  PhysicsProcess(procName, groupingName, fileNameTEMP, treeName),
   m_loadIntoMemory(loadIntoMemory),
   m_formulaWeight(0),
   m_formulaCategory(0)
{

}// C'tor

//------------------------------------------------------------------------------
PhysicsProcessMemory::~PhysicsProcessMemory(){

   // First delete all the formulas we have if already existing
   if (m_formulaWeight) delete m_formulaWeight;
   if (m_formulaCategory) delete m_formulaCategory;
   for (unsigned pro = 0; pro < m_formulas.size(); pro ++)
      delete m_formulas[pro];

}//D'tor

//------------------------------------------------------------------------------
PhysicsProcessMemory::PhysicsProcessMemory(const PhysicsProcess& ppn):
   PhysicsProcess(ppn)
{
   m_formulaCategory = 0;
   m_formulaWeight = 0;
   m_loadIntoMemory = true;
}//Copy C'tor

//------------------------------------------------------------------------------
PhysicsProcessMemory & PhysicsProcessMemory::operator=(const PhysicsProcess & right){
   if (this != &right) {
      name = right.name;
      groupName = right.groupName;
      fileName = right.fileName;
      sigma = right.sigma;
      branching_ratio = right.branching_ratio;
      intLum = right.intLum;
      initial_events = right.initial_events;
      chain = right.chain;
      m_formulaCategory = 0;
      m_formulaWeight = 0;
      m_loadIntoMemory = true;
   }
   return *this;
}//Assignment Operator

//------------------------------------------------------------------------------
unsigned PhysicsProcessMemory::setProjections(vector < string > projections, string weight,
                                              string category){

   //first delete all the formulas we have if already existing
   if (m_formulaWeight) delete m_formulaWeight;
   if (m_formulaCategory) delete m_formulaCategory;
   for (unsigned pro = 0; pro < m_formulas.size(); pro ++)
      delete m_formulas[pro];

   //then clear the container    
   m_formulas.clear();

   //and finaly recreate them all again because TTreeFormula::SetTree does not work.
   for (unsigned pro = 0; pro < projections.size(); pro ++)
      m_formulas.push_back(new TTreeFormula("selection",projections[pro].c_str(),chain));  
  
   //create the weight formula
   m_formulaWeight = new TTreeFormula("weightForm",weight.c_str(),chain);

   //create the category formula
   m_formulaCategory = new TTreeFormula("categoryForm",category.c_str(),chain);

   //do the a first preprocessing
   return preProcessTree();

}//reLoadFromChain

//------------------------------------------------------------------------------
unsigned  PhysicsProcessMemory::fillTH1Templates(vector < TH1* > & templates){

   // Check that the number of dimensions is correct
   if (m_formulas.size() != templates.size()){
      cout<<" ERROR PhysicsProcessMemory::fillTH1Templates. The number of TH1's "
          <<" in the vector is different than the number of projections"
          <<endl;    
      return 0;
   }

   // the returning value
   unsigned totEntries = 0;

   // define the tree number. Only used when tree is not on memory.
   Int_t tnumber = -1;

   //Loop over all the TreeRows of the vector
   for (unsigned ro = 0; ro < m_treeRows.size(); ro ++){

      //if tree is in memory
      if (m_loadIntoMemory){

         for (unsigned te = 0 ; te < templates.size(); te++)
            templates[te]->Fill(m_treeRows[ro].data[te], m_treeRows[ro].weight);       

      } 
      //if tree is not in memory
      else {

         // load the tree
         chain->LoadTree(m_treeRows[ro].entry);

         // if this is a new tree ...   
         if (tnumber != chain->GetTreeNumber()) {
            tnumber = chain->GetTreeNumber();
	  
            // ... then make sure to update the formulas 
            m_formulaWeight -> UpdateFormulaLeaves();
            m_formulaCategory -> UpdateFormulaLeaves();
            for (unsigned ff = 0; ff < m_formulas.size(); ff++){
               m_formulas[ff]->GetNdata();
               m_formulas[ff]->UpdateFormulaLeaves();
            }
         }// if
	
         // and evaluate the formulas
         for (unsigned ff = 0; ff < m_formulas.size(); ff++)
            templates[ff]->Fill(m_formulas[ff]->EvalInstance(),
                                m_formulaWeight->EvalInstance());     
			      
      }//if else
	
      //increment the counter
      totEntries ++;	     
 
   }//for rows   

   return  totEntries;

}//fillTH1Templates

//------------------------------------------------------------------------------
unsigned  PhysicsProcessMemory::fillTH1Templates(vector < TH1* > & templates, int category){

   // Check that the number of dimensions is correct
   if (m_formulas.size() != templates.size()){
      cout<<" ERROR PhysicsProcessMemory::fillTH1Templates. The number of TH1's "
          <<" in the vector is different than the number of projections"
          <<endl;    
      return 0;
   }

   // the returning value
   unsigned totEntries = 0;

   // define the tree number. Only used when tree is not on memory.
   Int_t tnumber = -1;

   //Loop over all the TreeRows of the vector
   for (unsigned ro = 0; ro < m_treeRows.size(); ro ++){

      //if tree is in memory
      if (m_loadIntoMemory){

         for (unsigned te = 0 ; te < templates.size(); te++){
            if (m_treeRows[ro].category == category)
               templates[te]->Fill(m_treeRows[ro].data[te], m_treeRows[ro].weight);       
         }

      } 
      //if tree is not in memory
      else {

         // load the tree
         chain->LoadTree(m_treeRows[ro].entry);

         // if this is a new tree ...   
         if (tnumber != chain->GetTreeNumber()) {
            tnumber = chain->GetTreeNumber();
	  
            // ... then make sure to update the formulas 
            m_formulaWeight -> UpdateFormulaLeaves();
            m_formulaCategory -> UpdateFormulaLeaves();
            for (unsigned ff = 0; ff < m_formulas.size(); ff++){
               m_formulas[ff]->GetNdata();
               m_formulas[ff]->UpdateFormulaLeaves();
            }
	  
         }// if
	
         // and evaluate the formulas
         for (unsigned ff = 0; ff < m_formulas.size(); ff++){
            if (m_treeRows[ro].category == category){
               templates[ff]->Fill(m_formulas[ff]->EvalInstance(),
                                   m_formulaWeight->EvalInstance());     
            }
         }//for

      }//if else
	
      //increment the counter
      totEntries ++;	     
 
   }//for rows   

   return  totEntries;

}//fillTH1Templates


//------------------------------------------------------------------------------
// This method loads the weights of all entries. 
// in m_loadIntoMemory also loads the information into memory.
unsigned PhysicsProcessMemory:: preProcessTree(){

   // Clear the elements of the vector;
   m_treeRows.clear();

   // clear the totalWeight
   m_totalWeight = 0;

   // the returning value
   unsigned totEntries = 0;

   // define the tree number
   Int_t tnumber = -1;

   // Loop over all the entries
   for (unsigned ent = 0; ent < chain->GetEntries(); ent++){
    
      // create a row object
      TreeRow row;
    
      // load the tree
      chain->LoadTree(ent);

      // if this is a new tree ...   
      if (tnumber != chain->GetTreeNumber()) {
         tnumber = chain->GetTreeNumber();

         // ... then make sure to update the formulas 
         m_formulaWeight   -> UpdateFormulaLeaves();
         m_formulaCategory -> UpdateFormulaLeaves();
         for (unsigned ff = 0; ff < m_formulas.size(); ff++)
            m_formulas[ff]->UpdateFormulaLeaves();
        
      }// if
  
      // get the weight value for this entry
      m_formulaWeight->GetNdata();
      row.weight = m_formulaWeight->EvalInstance();

      // get the category value for this entry
      m_formulaCategory->GetNdata();
      row.category = (int) m_formulaCategory->EvalInstance();

      // set the entry number 
      row.entry = ent;

      // Load only the positive weight entries
      if (row.weight>0){

         // Update the total weight
         m_totalWeight += row.weight;

         //Load the values of all formulas
         for (unsigned ff = 0; ff < m_formulas.size(); ff++) {
            if(m_formulas[ff]->GetNdata())
               row.data.push_back(m_formulas[ff]->EvalInstance());
         }     

         // add the row to the sample
         m_treeRows.push_back(row);

         // increment the counter
         totEntries ++;

      }// if weight > 0 
  
   }// for entries

   return totEntries;

}// preProcessTree

//---------------------------------------------------------------------------
// This method throws PSE for all the TreeRows. The number of events
// is Poisson fluctuated from the sampleExpectedEvents * subSampleWeight/totalSampleWeight
vector<const TreeRow *> PhysicsProcessMemory::throwPSE(double expectedEvents){

   //The returning PSE's
   // The resulting vector with the indexes of all entries
   vector<const TreeRow*> pses;

  //Get the number of Poisson-Fluctuated events we need to throw
  double m_sampleWeight = m_totalWeight;
  unsigned numPSE = m_random.Poisson(expectedEvents * m_sampleWeight / m_totalWeight);

  //report if requested
  if (m_printLevel >1){
    cout<<" ProcessSubSample::  exp="<<expectedEvents
	<<" sampleW="<<m_sampleWeight
	<<" m_totalWeight="<<m_totalWeight
	<<" expEvents="<<expectedEvents * m_sampleWeight / m_totalWeight
	<<endl;
  }

  //The total elements in the sample
  unsigned sampleSize = m_treeRows.size();

  //for debugging purposes
  int counter = 0;

  //throw the events until we reach the desired number
  while (pses.size()<numPSE){

    //Get a random entry
    unsigned candidate = (unsigned) (sampleSize * m_random.Rndm());
    //if the normalized weight of the entry is less than a Rndm
    /*
    cout<<"\t\t sampleSize ="<< sampleSize <<" expect="<<(expectedEvents * m_sampleWeight / m_totalWeight)
	<<" cand="<<candidate
	<<" nPSEUsed="<<  m_treeRows[candidate].nPSEUsed <<endl;
    */
    if (  
	m_treeRows[candidate].nPSEUsed < 20000  &&
	m_random.Rndm() < m_treeRows[candidate].weight / m_sampleMaxWeight
	) {      
      pses.push_back(&m_treeRows[candidate]);
      m_treeRows[candidate].nPSEUsed ++;
    }//if 

    counter ++;

  }//while

  if (m_printLevel && numPSE>0)
    cout<<" \t\t eff="<<1.0*pses.size()/counter<<endl;

   // if we did not load into memory, then the pse have empty data members
   // so we need to fill them up but 
   if (!m_loadIntoMemory){
      cout<<" ERROR PhysicsProcessMemory::throwPSE not implemented to read from CHAINS"<<endl;
      //pses = loadFromFile(pses);
   }

   return pses;

}// throwPSE

//---------------------------------------------------------------------------
// This reports the average number of times an event was used in this sample
double PhysicsProcessMemory::getAverageUsageOfEvents(){

  //the returning double
  double result =0;

  for (unsigned row = 0; row < m_treeRows.size(); row++)
    result +=  m_treeRows[row].nPSEUsed;

  if (m_treeRows.size() > 0)
    return result/m_treeRows.size();
    
  return 0;

}//getAverageUsageOfEvents()

//---------------------------------------------------------------------------
void PhysicsProcessMemory::reportAverageUsageOfEvents(string title1, string title2){

   cout<<"\t"<<title1<<"\t"<<title2<<"\t usage (per entry)= "<<getAverageUsageOfEvents()<<"\t number of entries="<<getNEvents()<<endl;

}// reportAverageUsageOfEvents

//---------------------------------------------------------------------------
//This reports the maximum value of the variable
double PhysicsProcessMemory::getMaximumValue(unsigned int var){

   // The returning result
   double max_val = 0;

   // Do a basic check
   if (var > m_formulas.size()){
      cout<<" ERROR  PhysicsProcessMemory::getMaximumValue requested for var="
          <<var<<" when only "<<  m_formulas.size()<<" exist."<<endl;
      return 0; 
   }

   // Loop over all the TreeRows in the vector
   for (unsigned ro = 0; ro < m_treeRows.size(); ro ++){
      
      // Get the useful information
      double val =  m_treeRows[ro].data[var];
      if (val>max_val)
         max_val = val;
      
   }//for
   
   //return it
   return max_val;

}//getMaximumValue

//---------------------------------------------------------------------------
void PhysicsProcessMemory::printEvents(unsigned maxEvent){

   unsigned counter  = 0 ;
    
   // Loop over all the TreeRows in the vector
   for (unsigned row = 0; row < m_treeRows.size(); row++){
      cout<<" row="<<row<<"  w="<<m_treeRows[row].weight;
      
      for (unsigned da = 0; da < m_treeRows[row].data.size(); da++)
         cout<<"\t"<<m_treeRows[row].data[da];
      
      cout<<endl;
      
      counter++;
      
      if (counter>maxEvent) 
         return ;
      
   }//for rows
   
}//printEvents

//---------------------------------------------------------------------------
//This method returns the covariance matrix between the first 
// nVars variables in the memory. 
TMatrixD PhysicsProcessMemory::getCovarianceMatrix(const unsigned nVars){

   // The returning matrix
   TMatrixD matrix(nVars,nVars);

   /*
   // Define a set of 2D histograms
   TH2 * auxH2[nVars][nVars];
   static int unique_index = 0;
  
   // Create the 2D histograms: loop over columns and rows of the Matrix
   // but just over the lower half, including the diagonal elements
   for (unsigned rowM = 0; rowM < nVars; rowM++){
   for (unsigned colM = 0; colM <= rowM; colM++){

   // Create the histogram for rowM and colM
   unique_index++;
   char auxname[400];
   sprintf(auxname,"auxH4Cov_%i",unique_index);
   auxH2[rowM][colM] = new TH2F(auxname,auxname,100,-100,100,100,-100,100);

   }//col
   }//for cols

   // Loop over all subsamples/elements of the map
   for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
   subS != m_subSamplesMap.end(); subS++) {    
   std::vector<TreeRow> rows = subS->second->getVectorOfRows();    
   for (unsigned row = 0; row < rows.size(); row++){
      
   // For each element fill all the histograms to get covariances
   for (unsigned rowM = 0; rowM < nVars; rowM++){
   for (unsigned colM = 0; colM <= rowM; colM++){
   auxH2[rowM][colM]->Fill(rows[row].data[rowM],
   rows[row].data[colM],
   rows[row].weight);

   }//col
   }//for cols

   }// for rows

   }// for subSamples
  
   //Write the covariance values to the Matrix
   for (unsigned rowM = 0; rowM < nVars; rowM++){
   for (unsigned colM = 0; colM <= rowM; colM++){

   double cov = auxH2[rowM][colM]->GetCovariance();
   matrix[rowM][colM] = cov;
   matrix[colM][rowM] = cov;

   //Clean up
   delete auxH2[rowM][colM];

   }//col
   }//for rows
  
   */

   //Return the matrix
   return matrix;

}//getCovarianceMatrix

//---------------------------------------------------------------------------
//This method returns a matrix of 1 x nVars containing the average of the  
//first nVars variables in memory.
TVectorD PhysicsProcessMemory::getVectorOfAverages(unsigned nVars){

   //the returning matrix
   TVectorD vectorOfAverages(nVars);

   // The total weight
   double sum_weights = 0;

   // Loop over all rows of the vector  
   for (unsigned row = 0; row < m_treeRows.size(); row++){
      
      //for each element fill all the histograms to get covariances
      for (unsigned col = 0; col < nVars; col++){
         vectorOfAverages[col] += m_treeRows[row].data[col] * m_treeRows[row].weight;	
      }//for cols
      
      // Keep track of the total weight
      sum_weights += m_treeRows[row].weight;
      
   }// for m_treeRows

   //Normalize each average to the total weight
   for (unsigned col = 0; col < nVars; col++)
      vectorOfAverages[col] =  vectorOfAverages[col]*(1.0/sum_weights);
   
   //Return the matrix
   return vectorOfAverages;
   
}//getVectorOfAverages
