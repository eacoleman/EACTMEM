#include "TAMUWW/SpecialTools/interface/PhysicsProcess.hh"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include "TChain.h"
#include "TH1.h"

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;

using DEFS::PhysicsProcessType;

//---------------------------------------------------------------------------
PhysicsProcess::PhysicsProcess() :
  m_name("PhysicsProcess"),
  m_subName("PhysicsSubProcess"),
  m_chainPtr(0),
  m_weight("1"),
  m_categoryFormula("0"){
 
  m_procTree = new ProcessTree(0);
 
}// C'tor
  
//---------------------------------------------------------------------------
PhysicsProcess::PhysicsProcess(const PhysicsProcess & pro){
  m_name     = pro.getName();
  m_subName  = pro.getSubName();
  m_chainPtr = pro.getChainClone();
  m_weight   = pro.getWeight();
  m_categoryFormula   = pro.getCategory();
  m_expectedEventsCat = pro.getMapExpectedEventsCat();

  //Careful, this might not be complete. Check the class ProcessTree
  //  m_procTree = new ProcessTree(*pro.getProcessTree());
  m_procTree = new ProcessTree(m_chainPtr);

  m_gaussianConstrained = pro.getGaussianConstraint();
  m_constrained = pro.isConstrained();

}// Copy C'tor

//---------------------------------------------------------------------------
PhysicsProcess::PhysicsProcess(PhysicsProcessType type,
			       TChain * chain){

  string name = getTypeString(type);
  m_name = name;
  m_subName = name;
  m_chainPtr = chain; 
  m_weight = "1";
  m_categoryFormula = "0";

  m_procTree = new ProcessTree(chain);
  
}// C'tor

//---------------------------------------------------------------------------
PhysicsProcess::PhysicsProcess(const string& name, 
			       const string& subname,
			       TChain * chain):
  m_name(name),
  m_subName(subname),
  m_chainPtr(chain), 
  m_weight("1"),
  m_categoryFormula("0"){

  m_procTree = new ProcessTree(chain);

}// C'tor

//---------------------------------------------------------------------------
Value PhysicsProcess::getCategoryNorm(int cat){
  return  m_expectedEventsCat[cat]; 
}


//---------------------------------------------------------------------------
void PhysicsProcess::setCategoryNorm(int cat, Value norm){
  
  // Save in the map
  m_expectedEventsCat[cat] = norm;

}//setCategoryNorm

//---------------------------------------------------------------------------
PhysicsProcess::~PhysicsProcess(){
  delete m_procTree;
}//D'tor



//---------------------------------------------------------------------------
void PhysicsProcess::setPrintLevel(int pl){

  if (m_procTree)
    m_procTree->setPrintLevel(pl);

}// setPrintLevel

//---------------------------------------------------------------------------
void PhysicsProcess::setProjections(vector <string> projs){

  // set the projections on the tree
  unsigned nev = m_procTree->setProjections(projs, m_weight, m_categoryFormula);

  cout<<"\tProcess\t"<<getName()<<"\t"<<getSubName()
      <<"\tloaded with "<<nev<<"\tevents"<<endl;

}// setProjections


/*
//---------------------------------------------------------------------------
TH1* PhysicsProcess::makeHist(const string& variable, const TCut& cuts,
			      EPD::HistBounds bounds, TCut normCuts,
			      bool prepareForPSE)
{
  TH1* hist = getHist(m_tree, variable, cuts, bounds, prepareForPSE);
  
  float scale;
  if (!string(cuts.GetTitle()).empty())
    {
      string cutString(cuts.GetTitle());
      string normCutsString(normCuts.GetTitle());
      if (cutString != normCutsString && getNorm())
	{
	  cout<<"Now will doing with normCuts="<<normCuts<<endl;
	  TH1* scaleHist = getHist(m_tree, variable, normCuts, bounds);
	  scale = scaleHist->Integral();
	  
	  delete scaleHist;
	}
      else
	{
	  scale = hist->Integral();
	}
    }
  else
    {
      scale = hist->Integral();
    }
  
  if (getNorm() && scale)
    hist->Scale(getNorm() / scale);      
  
  RootClass::finishSample(hist->Integral());
  
  return hist;
}

*/

//---------------------------------------------------------------------------
// This  method throws events from the tree in memory. 
vector < const TreeRow *>  PhysicsProcess::throwPSE(){

  Value norm = getTotalExpectedEvents();

  // If the process is Gaussian-Constrained vary the 
  // expected number of events from a gaussian
  double gausNum = m_gaussianConstrained ? 
                   m_random.Gaus(norm.value, norm.error) : 
                   norm.value;
  

  // Get the PSE's from the tree in memory.
  // The poisson fluctuation needs to be done in m_memTree
  // to handle weights correctly
  return m_procTree->throwPSE(gausNum);

}// PhysicsProcess::throwPSE()

//---------------------------------------------------------------------------
//This fills the TH1 templates
void PhysicsProcess::fillTH1Templates(std::vector < TH1* > & templates){ 

  Value norm = getTotalExpectedEvents();

  // First fill the templates with all the entries
  m_procTree->fillTH1Templates(templates);

  // Second normalize them to the expected number of events
  for (unsigned temp = 0; temp < templates.size(); temp++)
    templates[temp]->Scale(norm.value/templates[temp]->Integral());

}//fillTH1Templates

//---------------------------------------------------------------------------
//This fills the TH1 templates
void PhysicsProcess::fillTH1Templates(std::vector < TH1* > & templates, int category){ 

  // First  fill the templates with all the entries
  m_procTree->fillTH1Templates(templates, category);

  // Second normalize them to the expected number of events
  for (unsigned temp = 0; temp < templates.size(); temp++)
    templates[temp]->Scale( m_expectedEventsCat[category].value/templates[temp]->Integral());

}//fillTH1Templates


//---------------------------------------------------------------------------
//This return the positive log(Gaussian(par)). Notice that the positive
// log(Gaussian) is a negative number.
// Technically we should return res, defined as 
// double res  =  - 0.5 * std::pow((par - 1.0) / getPercentualError(),2); 
//        res +=  - std::log(getPercentualError())-0.91893;
//However the last line is ommited as it is a constant function of par  
double PhysicsProcess::logGaussianConstraint(double par){

  if (m_constrained){    
    Value norm = getTotalExpectedEvents();
    double percentualError = norm.error/norm.value;
    return  - 0.5 * std::pow( (par - 1.0) / percentualError, 2);     
  }

  return 0;
  
}//constraint
  

//---------------------------------------------------------------------------
Value PhysicsProcess::getTotalExpectedEvents() const{

  Value res; 

  // Loop over all the map's element
  for (map<int, Value>::const_iterator mapEl = m_expectedEventsCat.begin();
       mapEl != m_expectedEventsCat.end(); mapEl++) {
 
    res += mapEl->second;

  }//loop

  return res;
  
}//getTotalExpectedEvents


//---------------------------------------------------------------------------
// This reports the average usage of the number of events to stdout
void PhysicsProcess::reportAverageUsageOfEvents() { 
  
  m_procTree->reportAverageUsageOfEvents(m_name,m_subName);
  
}// reportAverageUsageOfEvents() 
