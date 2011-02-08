//Class AxisProjection
//Author: Ricardo Eusebi
//Creation date: 08/11/08

#ifndef AXISPROJ_HH
#define AXISPROJ_HH

//ROOT libraries
#include "TH1.h"
#include "TTreeFormula.h"
#include "TChain.h"

//C++ libraries
#include <iostream>
#include <string>
#include <vector>
#include <map>

class AxisProjection {

public:

  //Copy Constructor
  AxisProjection(const AxisProjection & axis );

  //Constructor with a variable
  AxisProjection(std::string var);

  //Constructor with a variable
  AxisProjection(std::string var, unsigned ibinNum, 
		 double ilower, double iupper);

  //Assignement operator
  AxisProjection operator=(const AxisProjection &);

  ~AxisProjection();
  
  //This reads the formula from the tree and fills the data histo
  void updatePSEFromTree();

  //This resets the "data" histogram, also used for PSE's.
  void resetPSE();

  //This adds an empty template and returns its pointer
  TH1 * addEmptyTemplate();

  //This returns the data histogram
  std::string getVariableName() const { return m_variable;}

  //This returns the data histogram
  TH1 * getData() const { return m_data;}

  //This returns the vector of templates
  std::vector <TH1* >  getTemplates() const { return  m_templates;}

  //This normalizes the area of all the templates to the given value
  void normalizeTemplates(double norm);

  //This normalizes the max height all the templates to the given value
  void normalizeHeightTemplates(double norm);

  //This saves the templates to a root file
  void saveTemplates(); //save to a file named <m_variable>.root
  void saveTemplates(std::string rootFile);
  void saveTemplatesInDir(std::string directory); //save templates in directory

  //This loads the templates from a root file
  void loadTemplates();//load from a file named <m_variable>.root
  void loadTemplates(std::string rootFile);

  //This returns a single template 
  TH1* getTemplate(unsigned pro) const;

  //This returns the log(Poisson(mu,n)) between the templates with 
  // weight par[] and the data. This is used for the likelihood
  double getLogP(Double_t *par);

  //This draws all the templates
  void drawTemplates(bool stacked);  
  
  void addToData(double da){ m_data->Fill(da);}

  //This draw the signal and the background templates
  void  drawSignalBackgroundTemplates(unsigned signalVar, double signalScale);

private:


  //Variable that represent the axis projection
  std::string m_variable;

  //the ordered list of templates: one TH1 for one process.
  std::vector<TH1*> m_templates;

  //the data. It could be real data of thrown from pse's
  TH1 * m_data;

  //Create a unique Number
  static int uniqueNumber;

};
  
#endif
