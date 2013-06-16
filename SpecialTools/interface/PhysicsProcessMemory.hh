// Name: PhysicsProcessMemory.hh
// Author: Alexx Perloff

// This class will eventually replace an older Physics class. It derives from PhysicsProcessNEW and
// adds the ability to load a TTree into memory

#ifndef PHYSICSPROCESSMEMORY_DEF
#define PHYSICSPROCESSMEMORY_DEF

#include "TString.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TTreeFormula.h"
#include "TVectorD.h"
#include "TMatrixD.h"

#include <iostream>
#include <vector>
#include <map>

#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"

// This  class holds the information of 
// one row (or one entry) of the tree
class TreeRow {
  
public:
  
  TreeRow(): 
    weight(0), nPSEUsed(0), entry(0){}
  // This prints the entry information
  //void printEntry();
  
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

class PhysicsProcessMemory: public PhysicsProcess
{
public:
  // C'tor.  
  // loadIntoMemory determines whether we load the tree to memory or not
   PhysicsProcessMemory(std::string procName,
                        std::string groupingName,
                        std::string fileNameTEMP,
                        std::string treeName = "PS/jets2p",
                        bool loadIntoMemory = true);

   // D'tor
   ~PhysicsProcessMemory();
   // Copy C'tor
   PhysicsProcessMemory(const PhysicsProcess& ppn);
   // Assignment Operator
   PhysicsProcessMemory & operator=(const PhysicsProcess & right);

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

   // Get the average usage of events
   double getAverageUsageOfEvents();

  // Report the average usage of events
  void reportAverageUsageOfEvents(std::string title1, std::string title2);

  // This method prints the events in the Range [minEvent, maxEvent);
  void printEvents(unsigned maxEvent);

  // This sets the printLevel of the class
   void setPrintLevel(int pl) { m_printLevel = pl;}

  // This returns the maximum stored value of variable var
  double getMaximumValue(unsigned int var);

  // This returns the covariance matrix between the first nVar variables
  TMatrixD getCovarianceMatrix(unsigned nVar);

  // This returns the vector of weighted averages of the first nVar variables
  TVectorD getVectorOfAverages(unsigned nVars);

   unsigned int getNEvents(){ return m_treeRows.size(); }

   std::vector<TreeRow> & getVectorOfRows(){ return m_treeRows; }

   // Setter and getter of error of weight events
   inline double getWeight() const {return m_totalWeight;}
   inline void setWeight(double we) {m_totalWeight = we;}

protected:

  // The chain pointer
  TChain * m_chainPtr;

  // Flag  that determines whether we load into memory or not
  bool m_loadIntoMemory;

  // The vector of formulas 
  std::vector<TTreeFormula *> m_formulas;

  //The formula for the weight
  TTreeFormula * m_formulaWeight;

  //The formula for the category
  TTreeFormula * m_formulaCategory;

  // A vector containing all the samples with weights
  std::vector<TreeRow> m_treeRows;

  // This holds the total weight of all events.
  double m_totalWeight;

  // This does a preprocessing of the tree just after
  // new projections or weight were loaded.
  unsigned preProcessTree();

  //The print level
  int m_printLevel;

  //The random generator
  TRandom3 m_random;

  double m_sampleMaxWeight;

};//end of class PhysicsProcessMemory

#endif
