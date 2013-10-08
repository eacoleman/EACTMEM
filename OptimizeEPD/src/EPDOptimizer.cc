//...
//ClassImp(EPDOptimizer)


#include "TAMUWW/OptimizeEPD/interface/EPDOptimizer.hh"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "TH1D.h"

using namespace ROOT::Minuit2;

//input values
DEFS::Ana::Type EPDOptimizer::anaType;
DEFS::TagCat EPDOptimizer::tagcat;
std::vector<PhysicsProcessForOpt*> EPDOptimizer::processes; //must be loaded outside of this class!
double EPDOptimizer::mhiggs; //NOTE: mhiggs is used in two places: to calculate FOM and when loading... in calculating it, mhiggs = 125
DEFS::JetBin EPDOptimizer::jetBin; //NOTE: optimizeEPDCoefficients.C seems to only call ONE type of this
bool EPDOptimizer::debug;


//intermediate values
TH1* EPDOptimizer::signalHisto;
TH1* EPDOptimizer::allBackHisto;
ProbsForEPD EPDOptimizer::normEPDcoeffs;



EPDOptimizer::EPDOptimizer(DEFS::Ana::Type ana, DEFS::TagCat tc, std::vector<PhysicsProcessForOpt*> prc,
			   double mh, DEFS::JetBin jb, bool d)
{
  //Set static variables
  anaType = ana;
  tagcat = tc;
  processes = prc;
  mhiggs = mh;
  jetBin = jb;
  debug = d;

  normEPDcoeffs = ProbsForEPD(
			    1./0.35e-09, // wh    115 GeV 
			    .1/0.50e-10, // hww    
			    1./0.22e-07, // schan  
			    1./0.45e-06, // tchan  S
			    0,           // tchan2 
			    1./0.003,    // tt     
			    1./0.25e-03, // wlight  
			    1./0.28e-05, // zlight 
			    1./0.34e-06, // wbb
			    0,           // wc     
			    1./0.125e-03,// wgg    
			    1./0.35e-06, // ww     
			    1./0.35e-07, // wz     
			    0,           // zz     
			    1./0.1       // qcd    
			    );

  // Create the templates based on EPDcoeffs for each template 
  int bins = 40;
  double minXaxis = 0;
  double maxXaxis = 1;
  signalHisto  = new TH1D("sH","sH",bins,minXaxis,maxXaxis);
  allBackHisto = new TH1D("bH","bH",bins,minXaxis,maxXaxis);  
  signalHisto->Sumw2();
  allBackHisto->Sumw2();

  //optimize();
}

void EPDOptimizer::optimize(){
  bestEPD = EPDOptimizer::optimizeEPDCoeffs(anaType, processes, tagcat, bestFOM);
}

ProbsForEPD EPDOptimizer::optimizeEPDCoeffs(DEFS::Ana::Type anaType, vector<PhysicsProcessForOpt*> processes, 
					    DEFS::TagCat tagcat, double & bestFigOfMerit){

  // The returning set starts with all ones.
  ProbsForEPD bestEPDcoeffs(1);

  // The set use for trial and error
  ProbsForEPD EPDcoeffs;
  
  // The set used for the normalization
  // ProbsForEPD normEPDcoeffs(
  // 			    1e+12*1./0.35e-09, // wh    115 GeV 
  // 			    1./0.50e-10, // hww    
  // 			    1./0.22e-07, // schan  
  // 			    1./0.45e-06, // tchan  S
  // 			    0,           // tchan2 
  // 			    1./0.003,    // tt     
  // 			    1./0.25e-03, // wlight  
  // 			    1./0.28e-05, // zlight 
  // 			    1./0.34e-06, // wbb
  // 			    0,           // wc     
  // 			    1./0.125e-03,// wgg    
  // 			    1./0.35e-06, // ww     
  // 			    1./0.35e-07, // wz     
  // 			    0,           // zz     
  // 			    1./0.1       // qcd    
  // 			    );
  
  // -------------------------------------------------------------
  // B- Loop over possible coefficient configurations obtaining the 
  //    most discriminant EPD.
  // -------------------------------------------------------------

  // Create the templates based on EPDcoeffs for each template 
  // int bins = 40;
  // double minXaxis = 0;
  // double maxXaxis = 1;
  // signalHisto  = new TH1D("sH","sH",bins,minXaxis,maxXaxis);
  // allBackHisto = new TH1D("bH","bH",bins,minXaxis,maxXaxis);  
  // signalHisto->Sumw2();
  // allBackHisto->Sumw2();

  // Create a file to store the best histograms. 
  //TFile bhf("optimizeEPD_templates.root","RECREATE");

  
  
  // Calculate the figure of merit for this processes and EPDcoeffs using Minuit2

  ROOT::Math::Minimizer* min = 
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  
  int nVar = 15;
  ROOT::Math::Functor minFunc(&calculateFOM,nVar);
  min->SetPrintLevel(3);

  min->SetStrategy(2);
  min->SetMaxFunctionCalls(1000);
  min->SetMaxIterations(1000);
  //EDM must be less than .001*tolerance*up for minimization to be sucessful
  min->SetTolerance(.01); //MIGHT NEED TO FIX
  min->SetErrorDef(.5);

  min->SetFunction(minFunc); 

  double lower_val[15];
  double upper_val[15];
  double step[15];
  double init_val[15];
  for(int i=0; i<15; i++){
    step[i] = 0.1;
    init_val[i] = 1; //what's a good initial value?
    //init_val[i] = i/100.;
    lower_val[i] = 0.01;
    upper_val[i] = 100;
  }


  // init_val[0] = 0.153789;
  // init_val[1] = 1.14755;
  // init_val[2] = 2.81832;
  // init_val[3] = 1.4027;
  // init_val[4] = 0;
  // init_val[5] = 1;
  // init_val[6] = 1;
  // init_val[7] = 1;
  // init_val[8] = 1;
  // init_val[9] = 0;
  // init_val[10] = 1;
  // init_val[11] = 5.32726;
  // init_val[12] = 0.1644;
  // init_val[13] = 0.;
  // init_val[14] = 0.108746;

  // step[0] = 10.;
  // step[1] = 1.;
  // step[2] = 100.;
  // step[3] = 0.1;
  // step[4] = 0.00001;
  // step[5] = 1.;
  // step[6] = 0.0001;
  // step[7] = 10.;
  // step[8] = 10.;
  // step[9] = 0.;
  // step[10] = 0.1;
  // step[11] = .001;
  // step[12] = .001;
  // step[13] = 0.;
  // step[14] = 0.0001;


  //Setting the variables, which are the parameters in ProbsForEPD
  min->SetLimitedVariable(0, "wh",     init_val[0], step[0], lower_val[0], upper_val[0]);
  //min->SetFixedVariable(0, "wh",     init_val[0]);
  min->SetLimitedVariable(1, "hww",    init_val[1], step[1], lower_val[1], upper_val[1]);
  min->SetLimitedVariable(2, "schan",  init_val[2], step[2], lower_val[2], upper_val[2]);
  //min->SetFixedVariable(2, "schan",  init_val[2]);
  min->SetLimitedVariable(3, "tchan",  init_val[3], step[3], lower_val[3], upper_val[3]);
  min->SetFixedVariable(3, "tchan",  init_val[3]);
  min->SetFixedVariable(4, "tchan2", 0); //Gives Hesse error
  //min->SetLimitedVariable(5, "tt",     init_val[5], step[5], lower_val[5], upper_val[5]);
  min->SetFixedVariable(5, "tt",     init_val[5]);
  min->SetLimitedVariable(6, "wlight", init_val[6], step[6], lower_val[6], upper_val[6]);
  //min->SetFixedVariable(6, "wlight", 1); //Hesse error, normalized EPD is not 0
  //min->SetLimitedVariable(7, "zlight", init_val[7], step[7], lower_val[7], upper_val[7]);
  min->SetFixedVariable(7, "zlight", 1); //Hesse error, normalized EPD is not 0
  //min->SetLimitedVariable(8, "wbb",    init_val[8], step[8], lower_val[8], upper_val[8]);
  min->SetFixedVariable(8, "wbb",    1); //Hesse error, normalized EPD is not 0
  min->SetFixedVariable(9, "wc",     0);
  //min->SetLimitedVariable(10,"wgg",    init_val[10], step[10], lower_val[10], upper_val[10]); //Hesse error
  min->SetFixedVariable(10,"wgg",    init_val[10]);
  //min->SetLimitedVariable(11,"ww",     init_val[11], step[11], lower_val[11], upper_val[11]);
  min->SetFixedVariable(11,"ww",     init_val[11]);
  min->SetLimitedVariable(12,"wz",     init_val[12], step[12], lower_val[12], upper_val[12]);
  //min->SetFixedVariable(12,"wz",     init_val[12]);
  min->SetFixedVariable(13,"zz", 0);
  min->SetLimitedVariable(14,"qcd",    init_val[14], step[14], lower_val[14], upper_val[14]);
  //min->SetFixedVariable(14,"qcd",    init_val[14]);


  //min->SetFixedVariable(0, "wh", 1);
  //min->SetFixedVariable(1, "hww", 1);
  //min->SetFixedVariable(2, "schan", 1);


  cout<<"Minimize() is about to be called"<<endl; 
  min->Minimize();
  cout<<"Minimize() is done"<<endl;
  //cout<<"min->CovMatrixStatus() "<<min->CovMatrixStatus()<<endl;
  //min->Hesse();
  cout<<"min's status is "<<min->Status()<<endl;
  min->PrintResults();

  EPDcoeffs = ProbsForEPD(min->X()[0], min->X()[1], min->X()[2], min->X()[3], min->X()[4], 
			    min->X()[5], min->X()[6], min->X()[7], min->X()[8], min->X()[9],
			    min->X()[10], min->X()[11], min->X()[12], min->X()[13], min->X()[14]);
  
  cout<<"EPDcoeffs is: "<<EPDcoeffs.getStringConstructor() << endl;

  bestFigOfMerit = -(min->MinValue());

  ///FIX ME!!!!
  bestSignalHisto = signalHisto;
  bestBackHisto = allBackHisto;

  /*


  // Save these histos to file
  stringstream ssB;
  ssB<<allBackHisto->GetName()<<"_iter"<<iter;
  allBackHisto->Write(ssB.str().c_str());
  stringstream ssS;
  ssS<<signalHisto->GetName()<<"_iter"<<iter;
  signalHisto->SetLineColor(2);
  signalHisto->Write(ssS.str().c_str()); 
  //cout<<"Wrote ssH and ssB"<<endl;
 
  */
  
  // Close the file
  // bhf.Close();

  // clean up
  delete signalHisto;
  delete allBackHisto;

  bestEPDcoeffs = EPDcoeffs;

  // Print the normEPDcoeffs
  cout<<"normalized constructor would be:"<<endl
      <<"\t" <<bestEPDcoeffs.getStringConstructor()<<endl;

  // return the coefficients.
  return bestEPDcoeffs * normEPDcoeffs;

    
}// optimizeEPDCoeffs


double EPDOptimizer::createHistoAndGetFOM( DEFS::Ana::Type anaType,
			     vector<PhysicsProcessForOpt*> processes, 
			     const ProbsForEPD & coeffs, 
			     DEFS::TagCat tagcat, double mhiggs,
			     TH1 * signal_Histo,  TH1 * allBack_Histo){
 
  static bool first_call = true;

  // Reset all the templates
  signal_Histo->Reset();
  allBack_Histo->Reset();

  // Fill the signal and background histos for this set of EPDcoeffs
  //testing
  for (unsigned p=0;p<processes.size();p++){
      
    //get this process' name
    TString thisProcName = processes[p]->getName();

    // put the WW or WX processes into the signal histogram
     
    // cout << "proc = " << thisProcName << endl;

    if ((anaType == DEFS::Ana::WWAnalysis && (thisProcName.EqualTo("WW") || thisProcName.EqualTo("WZ"))) || 
	(anaType == DEFS::Ana::HiggsAnalysis && (thisProcName.Contains("WH") || thisProcName.Contains("ggH") || thisProcName.Contains("qqH")))){

      // signal contains
      processes[p]->fillNormEPDHisto(signal_Histo, tagcat, mhiggs, coeffs);
				       
      if(first_call)
	cout<<"\tputting process "<<thisProcName<<" into signal."<<endl;
	  
    }else{ 
      // every other process is background
      processes[p]->fillNormEPDHisto(allBack_Histo, tagcat, mhiggs, coeffs);

      if(first_call)
	cout<<"\tputting process "<<thisProcName<<" into background."<<endl;	
    }
      
  }// for processes
  first_call = false;
    
  // Calculate the figure of merit
  cout<<"signalHisto mean = "<<signalHisto->GetMean()<<endl;
  double figOfMerit = FigureOfMerit::usingShapeFromTemplates(signalHisto, allBackHisto);
 
  return figOfMerit;

}// createHistoAndGetFOM

Double_t EPDOptimizer::calculateFOM(const Double_t *EPDco){

    ProbsForEPD testEPD(EPDco[0], EPDco[1], EPDco[2], EPDco[3], EPDco[4], EPDco[5], EPDco[6], EPDco[7],
			EPDco[8], EPDco[9], EPDco[10], EPDco[11], EPDco[12], EPDco[13], EPDco[14]);

    //TFile bhf("optimizeEPD_templates.root","RECREATE");

    cout<<"testEPD(..) is "<<'\t'<<testEPD.getStringConstructor()<<endl;
    Double_t FOM = createHistoAndGetFOM(anaType, processes, testEPD * normEPDcoeffs, tagcat, mhiggs, signalHisto, allBackHisto);

    //Minuit is a minimizer, not maximizer
    Double_t negFOM = -FOM;
    
    cout<<"FOM is "<<FOM<<endl;
    return negFOM;
  
}
