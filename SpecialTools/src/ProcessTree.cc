//Our libraries
#include "TAMUWW/SpecialTools/interface/ProcessTree.hh"

//C++ libraries
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cmath>

//ROOT libraries
#include "TFile.h"
#include "TH2.h"

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

//------------------------------------------------------------------------------
// This method adds a new row to the subsample
ProcessSubSample::ProcessSubSample():

  m_sampleWeight(0),
  m_sampleMaxWeight(0),
  m_printLevel(1){

}//C'tor


//------------------------------------------------------------------------------
// This method adds a new row to the subsample
void ProcessSubSample::addRow(TreeRow row) {
      
  // first add it to the container
  m_treeRows.push_back(row);

  // second update the sampleWeight
  m_sampleWeight += row.weight;

  // third update the sampleMaxWeight;
  if (row.weight > m_sampleMaxWeight)
    m_sampleMaxWeight = row.weight;

}//addRow

//---------------------------------------------------------------------------
// This reports the average number of times an event was used in this sample
double ProcessSubSample::getAverageUsageOfEvents(){

  //the returning double
  double result =0;

  for (unsigned row = 0; row < m_treeRows.size(); row++)
    result +=  m_treeRows[row].nPSEUsed;

  if (m_treeRows.size() > 0)
    return result/m_treeRows.size();
    
  return 0;

}//getAverageUsageOfEvents()


//------------------------------------------------------------------------------
// This method throws PSE for a given subsample. The number of events
// is Poisson fluctuated from the sampleExpectedEvents * subSampleWeight/totalSampleWeight
std::vector<TreeRow*> ProcessSubSample::throwPSE(double expectedEvents,
						double totalSampleWeight){
  //The returning PSE's
  vector<TreeRow*> pse;

  //Get the number of Poisson-Fluctuated events we need to throw
  unsigned numPSE = m_random.Poisson(expectedEvents * m_sampleWeight / totalSampleWeight);

  //report if requested
  if (m_printLevel >1){
    cout<<" ProcessSubSample::  exp="<<expectedEvents
	<<" sampleW="<<m_sampleWeight
	<<" totalSampleWeight="<<totalSampleWeight
	<<" expEvents="<<expectedEvents * m_sampleWeight / totalSampleWeight
	<<endl;
  }

  //The total elements in the sample
  unsigned sampleSize = m_treeRows.size();

  //for debugging purposes
  int counter = 0;

  //throw the events until we reach the desired number
  while (pse.size()<numPSE){

    //Get a random entry
    unsigned candidate = (unsigned) (sampleSize * m_random.Rndm());
    //if the normalized weight of the entry is less than a Rndm
    /*
    cout<<"\t\t sampleSize ="<< sampleSize <<" expect="<<(expectedEvents * m_sampleWeight / totalSampleWeight)
	<<" cand="<<candidate
	<<" nPSEUsed="<<  m_treeRows[candidate].nPSEUsed <<endl;
    */
    if (  
	m_treeRows[candidate].nPSEUsed < 20000  &&
	m_random.Rndm() < m_treeRows[candidate].weight / m_sampleMaxWeight
	) {      
      pse.push_back(&m_treeRows[candidate]);
      m_treeRows[candidate].nPSEUsed ++;
    }//if 

    counter ++;

  }//while

  if (m_printLevel && numPSE>0)
    cout<<" \t\t eff="<<1.0*pse.size()/counter<<endl;

  return pse;

}// throwPSE


//---------------------------------------------------------------------------
ProcessTree::ProcessTree(TChain * chain, bool loadIntoMemory, double weightRange ):
  m_chainPtr(chain),
  m_loadIntoMemory(loadIntoMemory),
  m_weightRange(weightRange),
  m_formulaWeight(0),
  m_formulaCategory(0){
 
}// C'tor

//---------------------------------------------------------------------------
ProcessTree::~ProcessTree(){

  // First delete all the formulas we have if already existing
  if (m_formulaWeight) delete m_formulaWeight;
  if (m_formulaCategory) delete m_formulaCategory;
  for (unsigned pro = 0; pro < m_formulas.size(); pro ++)
    delete m_formulas[pro];

  // Then delete the subsamples
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) 
    delete subS->second;

}//D'tor

//------------------------------------------------------------------------------
unsigned ProcessTree::setProjections(vector < string > projections, string weight,
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
    m_formulas.push_back(new TTreeFormula("selection",projections[pro].c_str(),m_chainPtr));  
  
  //create the weight formula
  m_formulaWeight = new TTreeFormula("weightForm",weight.c_str(),m_chainPtr);

  //create the category formula
  m_formulaCategory = new TTreeFormula("categoryForm",category.c_str(),m_chainPtr);

  //do the a first preprocessing
  return preProcessTree();

}//reLoadFromChain

//------------------------------------------------------------------------------
unsigned  ProcessTree::fillTH1Templates(vector < TH1* > & templates){

  // Check that the number of dimensions is correct
  if (m_formulas.size() != templates.size()){
    cout<<" ERROR ProcessTree::fillTH1Templates. The number of TH1's "
	<<" in the vector is different than the number of projections"
	<<endl;    
    return 0;
  }

  // the returning value
  unsigned totEntries = 0;

  // define the tree number. Only used when tree is not on memory.
  Int_t tnumber = -1;

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {

    //get the rows of the sub sample
    std::vector<TreeRow> rows = subS->second->getVectorOfRows();

    //Loop over all the subsample rows
    for (unsigned ro = 0; ro < rows.size(); ro ++){

      //if tree is in memory
      if (m_loadIntoMemory){

	for (unsigned te = 0 ; te < templates.size(); te++)
	  templates[te]->Fill(rows[ro].data[te], rows[ro].weight);       

      } 
      //if tree is not in memory
      else {

	// load the tree
	m_chainPtr->LoadTree(rows[ro].entry);

	// if this is a new tree ...   
	if (tnumber != m_chainPtr->GetTreeNumber()) {
	  tnumber = m_chainPtr->GetTreeNumber();
	  
	  // ... then make sure to update the formulas 
	  m_formulaWeight -> UpdateFormulaLeaves();
	  m_formulaCategory -> UpdateFormulaLeaves();
	  for (unsigned ff = 0; ff < m_formulas.size(); ff++)
	    m_formulas[ff]->UpdateFormulaLeaves();
	  
	}// if
	
	// and evaluate the formulas
	for (unsigned ff = 0; ff < m_formulas.size(); ff++)
	  templates[ff]->Fill(m_formulas[ff]->EvalInstance(),
			      m_formulaWeight->EvalInstance());     
			      
      }//if else
	
      //increment the counter
      totEntries ++;	     
 
    }//for rows   
	
  }//for subsamples

  return  totEntries;

}//fillTH1Templates

//------------------------------------------------------------------------------
unsigned  ProcessTree::fillTH1Templates(vector < TH1* > & templates, int category){

  // Check that the number of dimensions is correct
  if (m_formulas.size() != templates.size()){
    cout<<" ERROR ProcessTree::fillTH1Templates. The number of TH1's "
	<<" in the vector is different than the number of projections"
	<<endl;    
    return 0;
  }

  // the returning value
  unsigned totEntries = 0;

  // define the tree number. Only used when tree is not on memory.
  Int_t tnumber = -1;

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {

    //get the rows of the sub sample
    std::vector<TreeRow> rows = subS->second->getVectorOfRows();

    //Loop over all the subsample rows
    for (unsigned ro = 0; ro < rows.size(); ro ++){

      //if tree is in memory
      if (m_loadIntoMemory){

	for (unsigned te = 0 ; te < templates.size(); te++){
	  if (rows[ro].category == category)
	    templates[te]->Fill(rows[ro].data[te], rows[ro].weight);       
	}

      } 
      //if tree is not in memory
      else {

	// load the tree
	m_chainPtr->LoadTree(rows[ro].entry);

	// if this is a new tree ...   
	if (tnumber != m_chainPtr->GetTreeNumber()) {
	  tnumber = m_chainPtr->GetTreeNumber();
	  
	  // ... then make sure to update the formulas 
	  m_formulaWeight -> UpdateFormulaLeaves();
	  m_formulaCategory -> UpdateFormulaLeaves();
	  for (unsigned ff = 0; ff < m_formulas.size(); ff++)
	    m_formulas[ff]->UpdateFormulaLeaves();
	  
	}// if
	
	// and evaluate the formulas
	for (unsigned ff = 0; ff < m_formulas.size(); ff++){
	  if (rows[ro].category == category){
	    templates[ff]->Fill(m_formulas[ff]->EvalInstance(),
			      m_formulaWeight->EvalInstance());     
	  }
	}//for

      }//if else
	
      //increment the counter
      totEntries ++;	     
 
    }//for rows   
	
  }//for subsamples

  return  totEntries;

}//fillTH1Templates



//------------------------------------------------------------------------------
// This method loads the weights of all entries. 
// in m_loadIntoMemory also loads the information into memory.
unsigned ProcessTree:: preProcessTree(){

  // Clear the elements of the map;
  m_subSamplesMap.clear();

  // clear the totalWeight
  m_totalWeight = 0;

  // the returning value
  unsigned totEntries = 0;

  // define the tree number
  Int_t tnumber = -1;

  // Loop over all the entries
  for (unsigned ent = 0; ent < m_chainPtr->GetEntries(); ent++){
    
    // create a row object
    TreeRow row;
    
    // load the tree
    m_chainPtr->LoadTree(ent);

    // if this is a new tree ...   
    if (tnumber != m_chainPtr->GetTreeNumber()) {
      tnumber = m_chainPtr->GetTreeNumber();

      // ... then make sure to update the formulas 
      m_formulaWeight   -> UpdateFormulaLeaves();
      m_formulaCategory -> UpdateFormulaLeaves();
      for (unsigned ff = 0; ff < m_formulas.size(); ff++)
	m_formulas[ff]->UpdateFormulaLeaves();
        
    }// if
  
    // get the weight value for this entry
    row.weight = m_formulaWeight->EvalInstance();

    // get the category value for this entry
    row.category = (int) m_formulaCategory->EvalInstance();

    // set the entry number 
    row.entry = ent;

    // Load only the positive weight entries
    if (row.weight>0){

      // Update the total weight
      m_totalWeight += row.weight;

      // Based on the weight determine the name of the 
      // sub sample  this entry should go to.
      ProcessSubSample * subSample = getSubSample(row.weight);

      //Load the values of all formulas
      for (unsigned ff = 0; ff < m_formulas.size(); ff++)
	row.data.push_back(m_formulas[ff]->EvalInstance());     

      // add the row to the sample
      subSample->addRow(row);

      // increment the counter
      totEntries ++;

    }// if weight > 0 
  
  }// for entries

  return totEntries;

}// preProcessTree

//---------------------------------------------------------------------------
// This returns the subSample given the weight, the 
ProcessSubSample *  ProcessTree::getSubSample(double weight){

  // find the sample index and 
  int subSampleIndex = (int) (log(weight)/log(m_weightRange));

  // if it does not exist create it
  if (m_subSamplesMap.find(subSampleIndex) == m_subSamplesMap.end())
    m_subSamplesMap[subSampleIndex] = new ProcessSubSample();

  // return it
  return m_subSamplesMap[subSampleIndex];

}// getSubSample


//---------------------------------------------------------------------------
vector<const TreeRow *> ProcessTree::throwPSE(double expectedEvents){

  // The resulting vector with the indexes of all entries
  vector<const TreeRow *> pses;

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {
    
    // get the PSE from each sub sample
    vector<TreeRow*> thisSamplePSEs = subS->second->throwPSE(expectedEvents, m_totalWeight);

    // add it to the resulting vector
    pses.insert(pses.end(), thisSamplePSEs.begin(), thisSamplePSEs.end() );

  }// for samples

  // if we did not load into memory, then the pse have empty data members
  // so we need to fill them up but 
  if (!m_loadIntoMemory){
    cout<<" ERROR ProcessTree::throwPSE not implemented to read from CHAINS"<<endl;
    //pses = loadFromFile(pses);
  }

  return pses;

}// throwPSE


/*
//---------------------------------------------------------------------------
// This method clones all the pses and fills all the data members 
// of each TreeRow with the information from the file
vector<TreeRow*> loadFromFile(vector<TreeRow*> & pses_in){

  vector<TreeRow*> pses_out;

  // Define the tree number. Do not define static 
  // as it might conflict when redefining the projections
  Int_t tnumber = -1;

  //loop over the PSE's
  for (unsigned pse = 0; pse < pses_in.size(); pse++){

    //make a clone
    TreeRow * row = new TreeRow(pses_in[pse]);

    // load the tree
    m_chainPtr->LoadTree(row->entry);

    // if this is a new tree ...   
    if (tnumber != m_chainPtr->GetTreeNumber()) {
      tnumber = m_chainPtr->GetTreeNumber();

      // ... then make sure to update the formulas 
      for (unsigned ff = 0; ff < m_formulas.size(); ff++)
	m_formulas[ff]->UpdateFormulaLeaves();
        
    }// if
  
    //Load the values of all formulas
    for (unsigned ff = 0; ff < m_formulas.size(); ff++)
      row->data.push_back(m_formulas[ff]->EvalInstance());     

    pses_out.push_back(row);

  }//for pse

  return row;

}// loadFromFile
*/

//---------------------------------------------------------------------------
void ProcessTree::reportAverageUsageOfEvents(string title1, string title2){

  int counter = 0;

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {

    if (subS == m_subSamplesMap.begin())
      cout<<"\t"<<title1<<"\t"<<title2;
    else
      cout<<"\t\t";

    cout<<" sub-sample ="<< counter++ <<"\t usage (per entry)= "
	<<subS->second->getAverageUsageOfEvents()
	<<"\t number of entries="<<subS->second->getNEvents()<<endl;

  }//for

}// reportAverageUsageOfEvents

//---------------------------------------------------------------------------
//This reports the maximum value of the variable
double ProcessTree::getMaximumValue(unsigned int var){

  // The returning result
  double max_val = 0;

  // Do a basic check
  if (var > m_formulas.size()){
    cout<<" ERROR  ProcessTree::getMaximumValue requested for var="
	<<var<<" when only "<<  m_formulas.size()<<" exist."<<endl;
    return 0; 
  }

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {

    // Get the rows of the sub sample
    std::vector<TreeRow> rows = subS->second->getVectorOfRows();
    
    // Loop over all the subsample rows
    for (unsigned ro = 0; ro < rows.size(); ro ++){
      
      // Get the useful information
      double val =  rows[ro].data[var];
      if (val>max_val)
	max_val = val;
      
    }//for

  }//for subsample

  //return it
  return max_val;

}//getMaximumValue


//------------------------------------------------------------------------------
// This method loads the weights of all entries. 
// in m_loadIntoMemory also loads the information into memory.
void ProcessTree::setPrintLevel(int pl){

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) 
    subS->second->setPrintLevel(pl);

} //setPrintLevel



//---------------------------------------------------------------------------
void ProcessTree::printEvents(unsigned maxEvent){

  unsigned counter  = 0 ;

  // Loop over all the subsamples of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {

    cout<<" SS("<<subS->first<<")";
    
    std::vector<TreeRow> rows = subS->second->getVectorOfRows();
    
    for (unsigned row = 0; row < rows.size(); row++){
      cout<<"\t row="<<row<<"  w="<<rows[row].weight;
      
      for (unsigned da = 0; da < rows[row].data.size(); da++)
	cout<<"\t"<<rows[row].data[da];

      cout<<endl;

      counter++;

      if (counter>maxEvent) 
	return ;

    }//for rows

  }//for subsamples

}//printEvents

//---------------------------------------------------------------------------
//This method returns the covariance matrix between the first 
// nVars variables in the memory. 
TMatrixD ProcessTree::getCovarianceMatrix(const unsigned nVars){

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
TVectorD ProcessTree::getVectorOfAverages(unsigned nVars){

  //the returning matrix
  TVectorD vectorOfAverages(nVars);

  // The total weight
  double sum_weights = 0;

  // Loop over all subsamples/elements of the map
  for (map<int, ProcessSubSample*>::iterator subS = m_subSamplesMap.begin();
       subS != m_subSamplesMap.end(); subS++) {    
    std::vector<TreeRow> rows = subS->second->getVectorOfRows();    
    for (unsigned row = 0; row < rows.size(); row++){
      
      //for each element fill all the histograms to get covariances
      for (unsigned col = 0; col < nVars; col++){
	vectorOfAverages[col] += rows[row].data[col] * rows[row].weight;	
      }//for cols

      // Keep track of the total weight
      sum_weights += rows[row].weight;

    }// for rows

  }// for subSamples

  //Normalize each average to the total weight
  for (unsigned col = 0; col < nVars; col++)
    vectorOfAverages[col] =  vectorOfAverages[col]*(1.0/sum_weights);
  
  //Return the matrix
  return vectorOfAverages;

}//getVectorOfAverages
