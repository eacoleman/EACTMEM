#ifndef PHYSICSPROCESS_HH
#define PHYSICSPROCESS_HH

#include <map>
#include <string>
#include <utility>

#include "TRandom3.h"

#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/AxisProjection.hh"
#include "TAMUWW/SpecialTools/interface/ProcessTree.hh"

class TChain;
class TH1;
class TTree;

class PhysicsProcess{

public:  
  
  // Default C'tors
  PhysicsProcess();

  // Copy C'tor
  PhysicsProcess(const PhysicsProcess & pro);

  // Other C'tors
  PhysicsProcess(DEFS::PhysicsProcessType, 
		 TChain * chain,
		 double expectedEvents = 1.0, 
		 double errorExpectedEvents = 0.0);

  PhysicsProcess(const std::string& name, const std::string& subname,
		 TChain * chain,
		 double expectedEvents = 1.0, 
		 double errorExpectedEvents = 0.0);
  
  // D'tor
  ~PhysicsProcess();

  // Getter of name and subname
  inline std::string getName() const {return m_name;}
  inline std::string getSubName() const {return m_subName;}  

  // Setter and getter of expected events
  inline double getExpectedEvents() const  {return m_expectedEvents ;}
  inline void   setExpectedEvents(double see)  {m_expectedEvents = see;}

  // Setter and getter of error of expected events
  inline double getErrorExpectedEvents() const {return m_errorExpectedEvents;}
  inline void   setErrorExpectedEvents(double seee){m_errorExpectedEvents = seee;}

  // Set the normalization for a given category
  void   setCategoryNorm(int, double, double);
  double getCategoryNorm(int cat);

  std::map<int, double> getMapExpectedEventsCat() const { return m_expectedEventsCat;}
  std::map<int, double> getMapErrorExpectedEventsCat() const { return m_errorExpectedEventsCat;}

  // Setter and getter of error of weight events
  inline std::string getWeight() const {return m_weight;}
  inline void setWeight(std::string we) {m_weight = we;}

  // Setter and getter of error of weight events
  inline std::string getCategory() const {return m_category;}
  inline void setCategory(std::string we) {m_category = we;}

  // This returns the total error (events) over to normalization (events)
  double getPercentualError() const {
    return m_errorExpectedEvents / m_expectedEvents; 
  }

  void setGaussianConstraint(bool gc) {  m_gaussianConstrained = gc ;}
  bool getGaussianConstraint() const { return  m_gaussianConstrained ;}
  

  // Setter of projections, without and with weights.
  void setProjections(std::vector <std::string> projs);

  // This fill the given templates with the info from the Tree
  void fillTH1Templates(std::vector < TH1* > & vec);

  // This fill the given templates with the info from the Tree
  void fillTH1Templates(std::vector < TH1* > & vec, int category);

  // This throws PSE from the chain 
  std::vector <const TreeRow *>  throwPSE() ;
  
  // This reports the average usage of the number of events to stdout
  void reportAverageUsageOfEvents();

  // Return the chain pointer
  TChain * getChain() { return m_chainPtr;}
  TChain * getChainClone() const  { return (TChain *) m_chainPtr->Clone();}

  void setConstrained() { m_constrained = 1; }
  void setUnconstrained() { m_constrained = 0; }    
  bool isConstrained() const { return m_constrained;}
  
  double logGaussianConstraint(double par);

  void setPrintLevel(int pl);

  ProcessTree* getProcessTree() { return m_procTree;}

private:

  // Process name: e.g. 'diboson'
  std::string m_name;

  // Sub-process name: e.g. 'WZ'
  std::string m_subName;

  // The pointer to the chain
  TChain * m_chainPtr;

  // The weight to apply to all events
  std::string m_weight;

  // The category an event belongs to
  std::string m_category;

  // Expected number of events, used in PSE thrower.
  double m_expectedEvents;
  
  // Error on the expected number of events, used in PSE thrower.
  double m_errorExpectedEvents;
  
  // The expected events per category
  std::map<int, double> m_expectedEventsCat;
  std::map<int, double> m_errorExpectedEventsCat;

  // This is true if we expect a gaussian distribution 
  // with mean=m_expectedEvents and width=m_errorExpectedEvents
  bool m_gaussianConstrained;

  // This class holds the tree, in memory or in file.
  ProcessTree* m_procTree;  

  // The random generator
  TRandom3 m_random;
  
  // Use for 1 for constrained, 0 non-constrained
  int m_constrained;

};//class PhysicsProcess

#endif
