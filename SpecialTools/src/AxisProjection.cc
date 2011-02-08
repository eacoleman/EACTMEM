#include "TAMUWW/SpecialTools/interface/AxisProjection.hh"

//ROOT libraries
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "THStack.h"

//C++ libraries
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

//Other libraries


using std::cout;
using std::endl;
using std::string;
using std::vector;

int AxisProjection::uniqueNumber  = 0;

//------------------------------------------------------------------------------
AxisProjection::AxisProjection(std::string var, unsigned ibinNum, 
			       double ilower, double iupper):
  m_variable(var){
  
  char uniqueName[400];
  sprintf(uniqueName,"%s_%i",m_variable.c_str(),uniqueNumber++);
 
  //create the data histogram
  m_data = new TH1D(uniqueName, uniqueName ,ibinNum, ilower, iupper);

}//C'tor

//------------------------------------------------------------------------------
//Copy constructor
AxisProjection::AxisProjection(const AxisProjection & axis ):

  m_variable(axis.getVariableName()), 
  m_templates(axis.getTemplates()),
  m_data(axis.getData()) {
  
}//Copy C'tor

//------------------------------------------------------------------------------
//Assignement operator
AxisProjection AxisProjection::operator=(const AxisProjection & axis){

  return  AxisProjection(axis);

}//operator=

//------------------------------------------------------------------------------
AxisProjection::~AxisProjection(){
}//D'tor

//------------------------------------------------------------------------------
//This resets the data histogram
void AxisProjection::resetPSE(){

  m_data->Clear();
  m_data->Reset();

}//resetPSE

//------------------------------------------------------------------------------
//This returns  a template 
TH1* AxisProjection::getTemplate(unsigned pro) const {

  if (pro > m_templates.size()){
    cout<<" ERROR  AxisProjection::getTemplate called process "<<pro<<endl;
    cout<<"\t but there are only "<<m_templates.size()<<" processes!"<<endl;
    return 0;
  }

  return m_templates[pro];

}//getTemplate


//------------------------------------------------------------------------------
TH1 * AxisProjection::addEmptyTemplate() {

  //create a unique Name
  char uniqueName[400];
  sprintf(uniqueName,"%s_%i",m_variable.c_str(),uniqueNumber++);

  //create an empty histogram with the same characteristics than "data"
  TH1D * emptyH  = (TH1D*)m_data->Clone(uniqueName);

  //add it to the templates
  m_templates.push_back(emptyH);

  // return the pointer
  return emptyH;

}//addEmptyTemplate

//------------------------------------------------------------------------------
//This normalizes the area of all templates to the given value
void AxisProjection::normalizeTemplates(double norm){

  for (unsigned int te = 0 ; te < m_templates.size() ; te++){   
    double inte = m_templates[te]->Integral();
    
    if (inte >0)
      m_templates[te]->Scale(norm/inte);
    else{
      cout<<" Template "<< m_templates[te]->GetName()
	  <<" could not be normalized, as its integral is zero!"<<endl;
    }
  
  }//for

}//normalizeTemplates

//------------------------------------------------------------------------------
//This normalizes the maximum height of all templates to the given value
void AxisProjection::normalizeHeightTemplates(double norm){

  for (unsigned int te = 0 ; te < m_templates.size() ; te++){   

    double maxi = m_templates[te]->GetMaximum();
    
    if (maxi >0)
      m_templates[te]->Scale(norm/maxi);
    else{
      cout<<" Template "<< m_templates[te]->GetName()
	  <<" could not be normalized, as its integral is zero!"<<endl;
    }
  
  }//for

}//normalizeTemplates

//------------------------------------------------------------------------------
//This saves the templates to a root file
void AxisProjection::saveTemplates(){
  saveTemplates(m_variable+".root");
}

//------------------------------------------------------------------------------
//This saves the templates to a root file
void AxisProjection::saveTemplates(string rootFile){

  cout<<"Saving Templates from AxisProjection="<<m_variable<<" to file "<<rootFile<<endl;

  TFile file(rootFile.c_str(),"RECREATE");
  for (unsigned int te = 0 ; te < m_templates.size() ; te++){   
    char templateName[400];
    sprintf(templateName,"template_%i",te);
    m_templates[te]->Write(templateName);
  }
  
}//saveTemplates
 
//------------------------------------------------------------------------------
void  AxisProjection::saveTemplatesInDir(std::string directory){
  saveTemplates(directory+"/"+m_variable+".root");
}//saveTemplatesInDir

//------------------------------------------------------------------------------
//This loads the templates from a root file
void AxisProjection::loadTemplates(){
  loadTemplates(m_variable+".root");
}

//------------------------------------------------------------------------------
//This loads the templates from a root file
void AxisProjection::loadTemplates(string rootFile){

  TFile file(rootFile.c_str(),"READ");
 
  int templateCounter  = 0;
  
  TH1 * aux  =0;
  char templateName[400];

  do {

    sprintf(templateName,"template_%i",templateCounter);
    aux = (TH1*) file.Get(templateName);
    if (aux){
      TH1 * temp = (TH1*) aux->Clone(templateName);
      temp->SetDirectory(0);
      m_templates.push_back(temp);
      templateCounter ++;
    }

  } while (aux);
    
}//loadTemplates


//------------------------------------------------------------------------------
//This returns the sum of LogP over all bins. LogP, in each bin, is the 
// logarithm of the poisson function with mean given by the sum of all 
// templates with weights par[] and the number of events in data.
double  AxisProjection::getLogP(Double_t *par){

  //the returning value
  double logP = 0;

  //Loop over bins, including the under/overflow
  for (int bin = 1; bin <= m_data->GetNbinsX(); bin++){

    //Calculate the expected bin content, mu.
    double mu = 0;

    //loop over processes
    for (unsigned pro = 0; pro < m_templates.size(); pro++){
	 
      mu += m_templates[pro]->GetBinContent(bin) * par[pro];
      
    }//for processes


    if (mu > 0){
      double databin = m_data->GetBinContent(bin);	 

      logP += -mu + databin * std::log(mu);
      double logdatabinfactorial = databin * std::log(databin) - databin;
      if (0 && bin == 20){
	cout<<" bin "<<bin<<" mu="<<mu<<" data="<<databin
	    <<" [-mu + databin * std::log(mu)]="<< (-mu + databin * std::log(mu))
	    <<endl;       
      }

      if ( logdatabinfactorial > 0 )
	logP -=  logdatabinfactorial;
    }

  }//for

  return logP;

}//getLogP

//------------------------------------------------------------------------------
// This method draws all the templates in the same canvas. All templates except
// the signalVar are sum and drawn in a stack plot. Then the signalVar template
// is drawn atop scaled by signalScale.
void  AxisProjection::drawSignalBackgroundTemplates(unsigned signalVar, double signalScale){

  // To make sure that we can plot AxisProjections with the same name
  string canvasName="SignalBakcgroundTemplate";
 
  // Create the canvas and the stack histogram
  new TCanvas(canvasName.c_str(),canvasName.c_str());
  THStack * hs = new THStack("hsSB",canvasName.c_str());

  // Fill the stacked histogram
  for (unsigned t = 0; t < m_templates.size();t++){

    if (t != signalVar){
      m_templates[t]->SetFillStyle(1001);
      m_templates[t]->SetFillColor(t+21);
      m_templates[t]->SetLineColor(t+21);
      hs->Add(m_templates[t]);
    }

  }// for

  // Draw it
  hs->Draw("H");

  // Make a copy and scale the signal
  TH1 * sig =  (TH1*) m_templates[signalVar]->Clone();
  sig->Scale(signalScale);
  sig->SetLineColor(2);
  sig->SetLineWidth(4);

  // Draw the signal too
  sig->Draw("sameH");    

}//drawSignalBackgroundTemplates(uns
						    

//------------------------------------------------------------------------------
// This method draws all the templates in the same canvas
void  AxisProjection::drawTemplates(bool stacked){

  //to make sure that we can plot AxisProjections with the same name
  static int uniqueNumber = -1;
  uniqueNumber ++;
  char canvasName[400];
  sprintf(canvasName,"%s_%i",m_variable.c_str(),uniqueNumber);
  
  //Create the canvas and the stack histogram
  new TCanvas(canvasName,canvasName);
  THStack * hs = new THStack("hs",canvasName);

  //fill the stacked histogram
  for (unsigned t = 0; t < m_templates.size();t++){

    m_templates[t]->SetFillStyle(1001);
    m_templates[t]->SetFillColor(t+21);
    m_templates[t]->SetLineColor(t+21);
    hs->Add(m_templates[t]);

  }//for

  //draw it
  if (stacked)
    hs->Draw("H");
  else
    hs->Draw("nostack");

  //draw the data too
  if (m_data) {
    cout<<"plotting data"<<endl;
    m_data->SetMarkerStyle(20);
    //data->SetMarkerSize();
    m_data->Draw("samep");    
    cout<<"data->GetBincontent(24)="<<m_data->GetBinContent(24)<<endl;
    cout<<"data->Integral="<<m_data->Integral()<<endl;
  }

}//drawTemplates

 
