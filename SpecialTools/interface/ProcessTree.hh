#ifndef PROCESSTREE_HH
#define PROCESSTREE_HH

#include <vector>
#include <map>

#include "TRandom3.h"
#include "TChain.h"
#include "TH1.h"
#include "TTreeFormula.h"
#include "TVectorD.h"
#include "TMatrixD.h"

//---------------------------------------------------------------------------
// This  class holds the information of 
// one row (or one entry) of the tree
class TreeRow {
  
public:
  
  TreeRow(): 
    weight(0), nPSEUsed(0), entry(0){}
  // This prints the entry information
  void printEntry();
  
  // The data for the entry
  std::vector< double > data;  

  //The category this entry belongs to
  int category;

  // The weight of the entry
  double weight;
  
  // The number of times this was used in the PSE throwing.
  unsigned nPSEUsed;
  
  //the entry number of entry in the Chain
  unsigned entry;

};// end of class TreeRow 

//---------------------------------------------------------------------------
// This  class holds the information of 
// one row (or one entry) of the tree
class TreeRowInterface {

public:

  std::vector< double > data(){
    if (inMemory)
      return ptr->data;

    return m_data;
  }//data


private: 
  TreeRow * ptr;

  std::vector< double > m_data;

  bool inMemory;

};// end of class  TreeRowInterface

//---------------------------------------------------------------------------
class ProcessSubSample {
public:

  // C'tor
  ProcessSubSample();

  //This adds a new Row to the sample
  void addRow(TreeRow);

  // This throws the PSE and returns the number of entries
  // The provided expectedEvents is for the full sample
  std::vector<TreeRow*> throwPSE(double expectedEvents, double totalSampleWeight);

  double getSampleWeight(){ return m_sampleWeight; }

  void   setSampleWeight(double sw){ m_sampleWeight = sw;}

  void setPrintLevel(int pl) { m_printLevel = pl;}

  unsigned getNEvents(){ return m_treeRows.size(); } 

  std::vector<TreeRow> & getVectorOfRows(){ return m_treeRows; }

  double getAverageUsageOfEvents();

private:

  //The random generator
  TRandom3 m_random;

  std::vector<TreeRow> m_treeRows;
  double m_sampleWeight;
  double m_sampleMaxWeight;

  //The print level
  int m_printLevel;
  
};// end of class ProcessSubSample 


//---------------------------------------------------------------------------
class ProcessTree {

public:

  // C'tor.  
  // loadIntoMemory determines whether we load the tree to memory or not
  // weightRange is used to improve efficiency in the PSE throwing
  ProcessTree(TChain * c,  
	      bool loadIntoMemory = true, 
	      double weightRange  = 2.0);

  // D'tor
  ~ProcessTree();

  // This set the variables we are going to get info from and the weight 
  // It returns the number of events with those conditions
  unsigned setProjections(std::vector < std::string > projections,
			  std::string weight = "1", std::string cate="0");

  // This fills the given templates with information from 
  // the projection of the tree. User need to take care of 
  // creating the histograms.
  unsigned fillTH1Templates(std::vector < TH1* > & vec);

  // This fills the given templates with information from 
  // the projection of the tree. User need to take care of 
  // creating the histograms.
  unsigned fillTH1Templates(std::vector < TH1* > & vec, int category);

  // This throws the PSE's
  std::vector<const TreeRow *> throwPSE(double nExpectedEvents );

  // Report the average usage of events
  void reportAverageUsageOfEvents(std::string title1, std::string title2);

  // This method prints the events in the Range [minEvent, maxEvent);
  void printEvents(unsigned maxEvent);

  // This sets the printLevel of the class
  void setPrintLevel(int pl);

  // This returns the maximum stored value of variable var
  double getMaximumValue(unsigned int var);

  std::map< int, ProcessSubSample * > getSubSamples(){ return m_subSamplesMap;}

  // This returns the covariance matrix between the first nVar variables
  TMatrixD getCovarianceMatrix(unsigned nVar);

  // This returns the vector of weighted averages of the first nVar variables
  TVectorD getVectorOfAverages(unsigned nVars);


private:

  // The chain pointer
  TChain * m_chainPtr;

  // Flag  that determines whether we load into memory or not
  bool m_loadIntoMemory;

  // Factor that determine the bins of sub-samples
  double m_weightRange;

  // The vector of formulas 
  std::vector<TTreeFormula *> m_formulas;

  //The formula for the weight
  TTreeFormula * m_formulaWeight;

  //The formula for the category
  TTreeFormula * m_formulaCategory;

  // A map containing all the subsamples with weights
  // in the range given by  m_weightRange;
  std::map< int, ProcessSubSample * > m_subSamplesMap;

  // This holds the total weight of all events.
  double m_totalWeight;

  // This does a preprocessing of the tree just after
  // new projections or weight were loaded.
  unsigned preProcessTree();

  // This returns the SubSample for the given weight
  // it creates a new one if necessary
  ProcessSubSample *  getSubSample(double weight);

  // This method fills the pses[]->data members with the 
  // information from the chain. 
  //  std::vector<TreeRow*> loadFromFile(std::vector<TreeRow*> & pses);

};//end of class ProcessTree


#endif
