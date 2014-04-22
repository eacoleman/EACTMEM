////////////////////////////////////////////////////////////////////////////////
//
// TAMUWWMVA
// ---------
//
//                         08/28/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

//
// User Defined Includes
//
#include "TAMUWW/MVA/interface/TAMUWWMVA.hh"

////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TAMUWWMVA::TAMUWWMVA() {
   cout << "Making the default TAMUWWMVA object...";
   TString dName = ".";
   if(!batch) {
      //Make and/or set the output directory
      TDatime* date = new TDatime();
      //TString dName = Form("/uscms_data/d2/aperloff/Summer12ME8TeV/%i_%i_%i_",date->GetYear(),date->GetMonth(),date->GetDay()) + TString("TMVA_output");
      dName = Form("./%i_%i_%i_",date->GetYear(),date->GetMonth(),date->GetDay()) + TString("TMVA_output");
      if(!gSystem->OpenDirectory(dName)) gSystem->mkdir(dName);
   delete date;
   }
   odir = dName+"/";
   cout << "DONE" << endl;
}

//______________________________________________________________________________
TAMUWWMVA::TAMUWWMVA(TString ml, vector<PhysicsProcess*> proc, vector<TString> s, vector<TString> b,
                     vector<int> ep, DEFS::LeptonCat lc, vector<TString> p, TString ofb, TString of, bool ba) {
   cout << "Making the TAMUWWMVA object...";
   batch = ba;
   myMethodList     = ml;
   processes        = proc;
   signals          = s;
   backgrounds      = b;
   leptonCat        = lc;
   plots            = p;
   ofileBase        = ofb;
   eventProbs       = ep;
   if (of.IsNull())
      ofile = getFilename(ofileBase);
   else
      ofile = of;

   TString dName = ".";
   if(!batch) {
      //Make and/or set the output directory
      TDatime* date = new TDatime();
      //TString dName = Form("/uscms_data/d2/aperloff/Summer12ME8TeV/%i_%i_%i_",date->GetYear(),date->GetMonth(),date->GetDay()) + TString("TMVA_output");
      TString dName = Form("./%i_%i_%i_",date->GetYear(),date->GetMonth(),date->GetDay()) + TString("TMVA_output");
      if(!gSystem->OpenDirectory(dName)) gSystem->mkdir(dName);
      delete date;
   }
   odir = dName+"/";
   cout << "DONE" << endl;
}

//______________________________________________________________________________
TAMUWWMVA::~TAMUWWMVA() {}

////////////////////////////////////////////////////////////////////////////////
// Member Functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TString TAMUWWMVA::getFilename(TString ofile) {
   int toAppNum = 1;
   char name[1024];
   
   TSystemDirectory dir("",".");
   TList *files = dir.GetListOfFiles();
   vector<string> vfname;
   if (files) {
      TIter next(files);
      TSystemFile *file;
      TString fname;
      
      while ((file=(TSystemFile*)next())) {
         fname = file->GetName();
         if (!file->IsDirectory() && fname.EndsWith(".root") && fname.BeginsWith("TMVA")) {
            vfname.push_back(string(fname));
         }
      }
      delete files;
      if (vfname.size()>0) {
         std::sort(vfname.begin(),vfname.end());
         TString num = TString(vfname[vfname.size()-1]);
         num.ReplaceAll(".root","");
         num.ReplaceAll("TMVA","");
         toAppNum = num.Atoi()+1;
      }
   }
   sprintf(name,"%d",toAppNum);
   return ofile + name + ".root";
}

//______________________________________________________________________________
int TAMUWWMVA::getTSize() {
   //int size(0);
   //TFile* ftemp = TFile::Open(ifilePath + "micro" + ifilesSignal[0] + "_EPDv01.root");
   //TTree* ttemp = (TTree*)ftemp->Get("METree");
   //m_tSize = ttemp->GetBranch("m_tSize")->GetLeaf("m_tSize")->GetValue(0);
   //ttemp->SetBranchAddress("m_tSize",&m_tSize);
   //ttemp->GetEntry(0);
   //return m_tSize;
   return 13;
   //return MicroNtuple::nEventProb;
}

//______________________________________________________________________________
void TAMUWWMVA::TMVAClassification() {
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the 
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   // if you like to use a method via the plugin mechanism, we recommend using
   // 
   // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
   // (an example is given for using the BDT as plugin (see below),
   // but of course the real application is when you write your own
   // method based)

   gSystem->Load("libPhysics");
   gSystem->Load("libTMVA.1");

   // this loads the library
   TMVA::Tools::Instance();

   //---------------------------------------------------------------
   // default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   Use["Cuts"]            = 1;
   Use["CutsD"]           = 1;
   Use["CutsPCA"]         = 1;
   Use["CutsGA"]          = 1;
   Use["CutsSA"]          = 1;
   // ---
   Use["Likelihood"]      = 1;
   Use["LikelihoodD"]     = 1; // the "D" extension indicates decorrelated input variables (see option strings)
   Use["LikelihoodPCA"]   = 1; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
   Use["LikelihoodKDE"]   = 1;
   Use["LikelihoodMIX"]   = 1;
   // ---
   Use["PDERS"]           = 1;
   Use["PDERSD"]          = 1;
   Use["PDERSPCA"]        = 1;
   Use["PDERSkNN"]        = 1; // depreciated until further notice
   Use["PDEFoam"]         = 1;
   // --
   Use["KNN"]             = 1;
   // ---
   Use["HMatrix"]         = 1;
   Use["Fisher"]          = 1;
   Use["FisherG"]         = 1;
   Use["BoostedFisher"]   = 1;
   Use["LD"]              = 1;
   // ---
   Use["FDA_GA"]          = 1;
   Use["FDA_SA"]          = 1;
   Use["FDA_MC"]          = 1;
   Use["FDA_MT"]          = 1;
   Use["FDA_GAMT"]        = 1;
   Use["FDA_MCMT"]        = 1;
   // ---
   Use["MLP"]             = 1; // this is the recommended ANN
   Use["MLPBFGS"]         = 1; // recommended ANN with optional training method
   Use["CFMlpANN"]        = 1; // *** missing
   Use["TMlpANN"]         = 1; 
   // ---
   Use["SVM"]             = 1;
   // ---
   Use["BDT"]             = 1;
   Use["BDTD"]            = 1;
   Use["BDTG"]            = 1;
   Use["BDTB"]            = 1;
   // ---
   Use["RuleFit"]         = 1;
   // ---
   Use["Plugin"]          = 1;
   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // Create a new root output file.
   if (ofile.IsNull())
      ofile = getFilename(ofileBase);
   TFile* outputFile = TFile::Open(odir+ofile, "RECREATE");

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory will
   // then run the performance analysis for you.
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/ 
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in 
   // front of the "Silent" argument in the option string
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, 
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D" );

   // If you wish to modify default settings 
   // (please check "src/Config.h" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   //
   // read training and test data
   //
   vector<TFile*> inputs;
   vector<TTree*> signal;
   vector<TTree*> background;
   // global event weights per tree (see below for setting event-wise weights)
   //Double_t signalWeight     = 1.0;
   //Double_t backgroundWeight = 1.0;
   vector<double> signalWeight;
   vector<double> backgroundWeight;

   cout << "--- Factory                  : Weight = ( xsec * BR * scale factor * lumi )/(Evts in PATtuple)" << endl;
   TString sob;
   for(unsigned int p=0; p<processes.size(); p++) {
      if (DefaultValues::vfind(signals,processes[p]->getName())>-1) {
         factory->AddSignalTree(processes[p]->chain, processes[p]->getScaleFactor(leptonCat));
         sob = "signal";
      }
      else if (backgrounds.size()>0 && DefaultValues::vfind(backgrounds,processes[p]->getName())>-1) {
         factory->AddBackgroundTree(processes[p]->chain, processes[p]->getScaleFactor(leptonCat));
         sob = "background";
      }
      else if (backgrounds.size()>0 && DefaultValues::vfind(backgrounds,processes[p]->getName())<0) {
         continue;
      }
      else {
         factory->AddBackgroundTree(processes[p]->chain, processes[p]->getScaleFactor(leptonCat));
         sob = "background";
      }
      cout << "                             : Weight for " << sob << " (" << processes[p]->getName() << ") is " 
           << processes[p]->sigma[leptonCat] << " * " << processes[p]->branching_ratio[leptonCat] << " * "
           << processes[p]->scaleFactor[leptonCat] << " * " << processes[p]->intLum[leptonCat] << " // " 
           << processes[p]->initial_events[leptonCat] << " = " << processes[p]->getScaleFactor(leptonCat) << endl;
   }

/*
   for (unsigned int i=0; i<inputs.size(); i++) {
      cout << "--- TMVAClassification : Using input file: " << inputs[i]->GetName() << "(METree)" << endl;
   }
*/

   // ====== register trees ====================================================
   //
   // the following method is the prefered one:
   // you can add an arbitrary number of signal or background trees
   //factory->AddSignalTree    ( signal,     signalWeight     );
   //factory->AddBackgroundTree( background, backgroundWeight );

   // To give different trees for training and testing, do as follows:
   //    factory->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
   //    factory->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );

   // Use the following code instead of the above two or four lines to add signal and background 
   // training and test events "by hand"
   // NOTE that in this case one should not give expressions (such as "var1+var2") in the input 
   //      variable definition, but simply compute the expression before adding the event
   // 
   //    // --- begin ----------------------------------------------------------
   //    std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
   //    Float_t  treevars[4];
   //    for (Int_t ivar=0; ivar<4; ivar++) signal->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //    for (Int_t i=0; i<signal->GetEntries(); i++) {
   //       signal->GetEntry(i);
   //       for (Int_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //       // add training and test events; here: first half is training, second is testing
   //       // note that the weight can also be event-wise	
   //       if (i < signal->GetEntries()/2) factory->AddSignalTrainingEvent( vars, signalWeight ); 
   //       else                            factory->AddSignalTestEvent    ( vars, signalWeight ); 
   //    }
   //
   //    for (Int_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //    for (Int_t i=0; i<background->GetEntries(); i++) {
   //       background->GetEntry(i); 
   //       for (Int_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //       // add training and test events; here: first half is training, second is testing
   //       // note that the weight can also be event-wise	
   //       if (i < background->GetEntries()/2) factory->AddBackgroundTrainingEvent( vars, backgroundWeight ); 
   //       else                                factory->AddBackgroundTestEvent    ( vars, backgroundWeight ); 
   //    }
   //    // --- end ------------------------------------------------------------
   //
   // ====== end of register trees ==============================================

   
   // This would set individual event weights (the variables defined in the 
   // expression need to exist in the original TTree)
   //    for signal    : factory->SetSignalWeightExpression("weight1*weight2");
   //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");
   // factory->SetBackgroundWeightExpression("weight");
   factory->SetSignalWeightExpression("1.0");
   factory->SetBackgroundWeightExpression("1.0");

   // Define the input variables that shall be used for the MVA training
   /*************************************************************/
   int size = getTSize(); //////////FIX THIS!!! FIX THIS!!! FIX THIS!!!
   cout << "--- Factory                  : Number of variables: " << size << endl;
   /*************************************************************/
   char name[1024];
   TString var;
   //Add non-higgs MEs
   if(eventProbs.size()>0) {
      for (unsigned int i=0; i<eventProbs.size(); i++) {
         //if (i==10 || i==1 || i==13 || i==12 | i==9 | i==14)
         //   continue;
         var = Form("eventProb%i := eventProb[%i]",eventProbs[i],eventProbs[i]);
         cout << "--- Factory                  : Adding variable: " << var << endl;
         factory->AddVariable(var);
      }
   }
   else {
      for (int i=0; i<size; i++) {
         //if (i==10 || i==1 || i==13 || i==12 || i==9 || i==14)
         //   continue;
         sprintf(name,"%d",i);
         var = TString("eventProb") + name + " := eventProb[" + name + "]";
         cout << "--- Factory                  : Adding variable: " << var << endl;
         factory->AddVariable(var);
      }
      //Add ggH125 ME
      sprintf(name,"%d",19);
      var = TString("eventProb") + name + " := eventProb[" + name + "]";
      cout << "--- Factory                  : Adding variable: " << var << endl;
      factory->AddVariable(var);
      //Add WH125 ME
      sprintf(name,"%d",54);
      var = TString("eventProb") + name + " := eventProb[" + name + "]";
      cout << "--- Factory                  : Adding variable: " << var << endl;
      factory->AddVariable(var);
      //Add Mjj
      //var = TString("Mjj := Mjj");
      //cout << "--- Factory                  : Adding variable: " << var << endl;
      //factory->AddVariable(var);
   }
  
   // You can add so-called "Spectator variables", which are not used in the MVA training, 
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the 
   // input variables, the response values of all trained MVAs, and the spectator variables
   factory->AddSpectator("run := run",     "I");
   factory->AddSpectator("lumi := lumi",   "I");
   factory->AddSpectator("event := event", "I");

   // Apply additional cuts on the signal and background samples (can be different)
   //   TCut mycuts = "abs(eta)>1.5"; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   //   TCut mycutb = "abs(eta)>1.5"; // for example: TCut mycutb = "abs(var1)<0.5";

   //Acceptance/Base cuts
   //leptonCat==1==muon
   //leptonCat==2==electron

   int leptonCatNumber = leptonCat;
   TString leptonCatString = Form("leptonCat==%i",leptonCatNumber);
   TCut leptonCatCut(leptonCatString);
   TCut lpt("((leptonCat==1) && (lLV[0].Pt()>25.0)) || ((leptonCat==2) && (lLV[0].Pt()>30.0))");
   TCut METPt("METLV[0].Pt()>25.0");
   TCut jPt1("jLV[0].Pt()>30.0");
   TCut jPt2("jLV[1].Pt()>25.0");
   TCut null("");
   TCut branchStatus1("Entry$>-2");

   //TCut DEtajj("TMath::Abs(jLV[0].Eta()-jLV[1].Eta()) < 1.5");
   //TCut jjPt("sqrt(pow(jLV[0].Px()+jLV[1].Px(),2)+pow(jLV[0].Py()+jLV[1].Py(),2)) > 20.0");
   //TCut DPhiMETj1("TMath::Abs(TMath::Abs(TMath::Abs(METLV[0].Phi()-jLV[0].Phi())-TMath::Pi())-TMath::Pi()) > 0.4");
   //TCut wmt("sqrt(pow(lLV[0].Et()+METLV[0].Pt(), 2) - pow(lLV[0].Px()+METLV[0].Px(), 2) - pow(lLV[0].Py()+METLV[0].Py(), 2)) > 50.0");
   //TCut test("Entry$>-2 && jLV[1].Pt()>30.0");
   //TCut test("Entry$>-2");

   //TCut mycuts (null);
   //TCut mycuts (branchStatus1 && leptonCatCut && lpt && METEt && jPt1 && jPt2 && DEtajj && jjPt && DPhiMETj1 && wmt);
   //TCut mycuts (test);
   TCut mycuts (branchStatus1 && leptonCatCut && lpt && METPt && jPt1 && jPt2);

   // tell the factory to use all remaining events in the trees after training for testing:
   factory->PrepareTrainingAndTestTree(mycuts, mycuts,"nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:"
                                       "V=true:VerboseLevel=Debug" ); //set this line to "!V" for less verbosity

   // If no numbers of events are given, half of the events in the tree are used for training, and 
   // the other half for testing:
   //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );  
   // To also specify the number of testing events, use:
   //    factory->PrepareTrainingAndTestTree( mycut, 
   //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );  

   // ---- Book MVA methods
   //
   // please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   // Cut optimisation
   if (Use["Cuts"])
      factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   if (Use["CutsD"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsD", 
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

   if (Use["CutsPCA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsPCA", 
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

   if (Use["CutsGA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                           "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );
   
   if (Use["CutsSA"])
      factory->BookMethod( TMVA::Types::kCuts, "CutsSA",
                           "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );
   
   // Likelihood
   if (Use["Likelihood"])
      factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood", 
                           "H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" ); 

   // test the decorrelated likelihood
   if (Use["LikelihoodD"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD", 
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" ); 

   if (Use["LikelihoodPCA"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA", 
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 
 
   // test the new kernel density estimator
   if (Use["LikelihoodKDE"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE", 
                           "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" ); 

   // test the mixed splines and kernel density estimator (depending on which variable)
   if (Use["LikelihoodMIX"])
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX", 
                           "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // test the multi-dimensional probability density estimator
   // here are the options strings for the MinMax and RMS methods, respectively:
   //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );   
   //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );   
   if (Use["PDERS"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERS", 
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

   if (Use["PDERSkNN"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSkNN", 
                           "!H:!V:VolumeRangeMode=kNN:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

   if (Use["PDERSD"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSD", 
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

   if (Use["PDERSPCA"])
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA", 
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );
   /*
   // Multi-dimensional likelihood estimator using self-adapting phase-space binning
   if (Use["PDEFoam"])
   factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam", 
   "H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0333:nActiveCells=500:nSampl=2000:nBin=5:CutNmin=T:Nmin=100:Kernel=None:Compress=T" );
   */
   // K-Nearest Neighbour classifier (KNN)
   if (Use["KNN"])
      factory->BookMethod( TMVA::Types::kKNN, "KNN", 
                           "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );
   // H-Matrix (chi2-squared) method
   if (Use["HMatrix"])
      factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V" ); 

   // Fisher discriminant   
   if (Use["Fisher"])
      factory->BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=60:NsmoothMVAPdf=10" );

   // Fisher with Gauss-transformed input variables
   if (Use["FisherG"])
      factory->BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

   // Composite classifier: ensemble (tree) of boosted Fisher classifiers
   if (Use["BoostedFisher"])
      factory->BookMethod( TMVA::Types::kFisher, "BoostedFisher", "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2");
   /*
   // Linear discriminant (same as Fisher)
   if (Use["LD"])
   factory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None" );
   */
   // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
   if (Use["FDA_MC"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );
   
   if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

   if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

   if (Use["FDA_MT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

   if (Use["FDA_GAMT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

   if (Use["FDA_MCMT"])
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

   // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
   if (Use["MLP"])
      factory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=500:HiddenLayers=N+5:TestRate=10:EpochMonitoring:CreateMVAPdfs=true:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=60:NsmoothMVAPdf=10" );

   if (Use["MLPBFGS"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=500:HiddenLayers=N+5:TestRate=10:TrainingMethod=BFGS:!EpochMonitoring" );


   // CF(Clermont-Ferrand)ANN
   if (Use["CFMlpANN"])
      factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  
  
   // Tmlp(Root)ANN
   if (Use["TMlpANN"])
      factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...
  
   // Support Vector Machine
   if (Use["SVM"])
      factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );
   
   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTG", 
                           "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.30:UseBaggedGrad:GradBaggingFraction=0.6:SeparationType=GiniIndex:nCuts=20:NNodesMax=5" );

   if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDT", 
                           "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:CreateMVAPdfs=true:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=60:NsmoothMVAPdf=10" );
   
   if (Use["BDTB"]) // Bagging
      factory->BookMethod( TMVA::Types::kBDT, "BDTB", 
                           "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTD", 
                           "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate" );
   
   // RuleFit -- TMVA implementation of Friedman's method
   if (Use["RuleFit"])
      factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );
   
   // For an example of the category classifier, see: TMVAClassificationCategory

   // --------------------------------------------------------------------------------------------------

   // As an example how to use the ROOT plugin mechanism, book BDT via
   // plugin mechanism
   if (Use["Plugin"]) {
      //
      // first the plugin has to be defined, which can happen either through the following line in the local or global .rootrc:
      //
      // # plugin handler          plugin name(regexp) class to be instanciated library        constructor format
      // Plugin.TMVA@@MethodBase:  ^BDT                TMVA::MethodBDT          TMVA.1         "MethodBDT(TString,TString,DataSet&,TString)"
      // 
      // or by telling the global plugin manager directly
      gPluginMgr->AddHandler("TMVA@@MethodBase", "BDT", "TMVA::MethodBDT", "TMVA.1", "MethodBDT(TString,TString,DataSet&,TString)");
      factory->BookMethod( TMVA::Types::kPlugins, "BDT",
                           "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=50" );
   }

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();    

   // --------------------------------------------------------------
   
   // Save the output
   outputFile->Close();
   //gROOT->ProcessLine(".mv " + ofile + " " + odir);
   gROOT->ProcessLine(".mv weights/* " + odir);

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl
             << std::endl
             << "==> To view the results, launch the GUI: \"root -l ./TMVAGui.C\"" << std::endl
             << std::endl;

   delete factory;

   // Launch the GUI for the root macros
   //if (!gROOT->IsBatch()) TMVAGui(ofile);
}

//______________________________________________________________________________
TList* TAMUWWMVA::GetKeyList(const TString& pattern, TList* TMVAGui_keyContent) {
   TList* list = new TList();

   TIter next( TMVAGui_keyContent );
   TKey* key(0);
   while ((key = (TKey*)next())) {
      if (TString(key->GetName()).Contains( pattern )) { 
         list->Add( new TObjString( key->GetName() ) ); 
      }
   }
   return list;
}

//______________________________________________________________________________
void TAMUWWMVA::ActionButton( vector<TString>& TMVAGui_inactiveButtons, TList* TMVAGui_keyContent,
                   const TString& title, TString requiredKey ) {
   // search    
   if (requiredKey != "") {
      Bool_t found = kFALSE;
      TIter next( TMVAGui_keyContent );
      TKey* key(0);
      while ((key = (TKey*)next())) {         
         if (TString(key->GetName()).Contains( requiredKey )) {
            found = kTRUE;
            break;
         }
      }
      if (!found) {
         cout << "DID NOT FIND KEY " << requiredKey << "!!!" << endl;
         TMVAGui_inactiveButtons.push_back( title );
      }
   }
}

//______________________________________________________________________________
TFile* TAMUWWMVA::OpenFile( const TString& fin ) {
   TFile* file = gDirectory->GetFile();
   if (file==0 || fin != file->GetName()) {
      if (file != 0) {
         gROOT->cd();
         file->Close();
      }
      cout << "--- Opening root file " << fin << " in read mode" << endl;
         file = TFile::Open( fin, "READ" );
   }
   else {
      file = gDirectory->GetFile();
   }
   
   file->cd();
   return file;
}

//______________________________________________________________________________
Int_t TAMUWWMVA::GetNumberOfInputVariables( TDirectory *dir ) {
   TIter next(dir->GetListOfKeys());
   TKey* key    = 0;
   Int_t noVars = 0;
   
   while ((key = (TKey*)next())) {
      if (key->GetCycle() != 1) continue;
      
         // count number of variables (signal is sufficient), exclude target(s)
      if (TString(key->GetName()).Contains("__Signal") || (TString(key->GetName()).Contains("__Regression") && !(TString(key->GetName()).Contains("__Regression_target")))) noVars++;
   }
   
   return noVars;
}

//______________________________________________________________________________
void TAMUWWMVA::Plot() {

   if (ofile.IsNull()) {
      cout << "TAMUWWMVA::WARNING The output filename (ofile) is NULL." << endl
           << "Please make sure this is set before the Plot() function is called." << endl;
      return;
   }

   // check if file exist
   TFile* file = TFile::Open( odir+ofile );
   //TFile* file = TFile::Open( ofile );
   if (!file) {
      cout << "==> Abort TMVAPlot, please verify filename" << endl;
      return;
   }
   // find all references   
   cout << "--- Reading keys ..." << endl;
   static TList* TMVAGui_keyContent = (TList*)file->GetListOfKeys()->Clone();

   // close file
   file->Close();

   TString defaultRequiredClassifier = "";

   // storeage containers
   TString title;
   TString command;
   TString subCommand;
   vector<pair<TString,TString> > lines_to_process;
   static vector<TString> TMVAGui_inactiveButtons;

   // configure buttons   
   Int_t ic = 1;

   // find all input variables types
   TList* keylist = GetKeyList( "InputVariables" , TMVAGui_keyContent );
   TListIter it( keylist );
   TObjString* str = 0;
   while ((str = (TObjString*)it())) {
      TString tmp   = str->GetString();
      title = Form( "Input variables '%s'-transformed (training sample)", 
                            tmp.ReplaceAll("InputVariables_","").Data() );
      if (tmp.Contains( "Id" )) title = "Input variables (training sample)";
      command = Form(".x $CMSSW_BASE/src/TAMUWW/MVA/macros/variables.C(\"%s\",\"%s\",\"%s\")", (odir+ofile).Data(), str->GetString().Data(), title.Data());
      lines_to_process.push_back(make_pair(title,command));
      ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, str->GetString());
   }
   ic++;

   // correlation scatter plots
   it.Reset();
   while ((str = (TObjString*)it())) {
      TString tmp   = str->GetString();
      title = Form( "Input variable correlations '%s'-transformed (scatter profiles)", 
                            tmp.ReplaceAll("InputVariables_","").Data() );
      if (tmp.Contains( "Id" )) title = "Input variable correlations (scatter profiles)";
      command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/CorrGui.C(\"%s\",\"%s\",\"%s\")", (odir+ofile).Data(), str->GetString().Data(), title.Data());

      // destroy all open cavases
      DefaultValues::DestroyCanvases(); 
      
      TString extension = str->GetString();
      extension.ReplaceAll( "InputVariables", ""  );

      // checks if file with name "fin" is already open, and if not opens one
      TFile* file = OpenFile(odir+ofile);
      //TFile* file = OpenFile(ofile);
      
      TDirectory* dir = (TDirectory*)gDirectory->Get( str->GetString() );
      if (!dir) {
         cout << "Could not locate directory '" << str->GetString() << "' in file: " << ofile << endl;
         return;
      }
      dir->cd();
      
      // how many variables  are in the directory?
      Int_t noVar = GetNumberOfInputVariables(dir);
      cout << "found number of variables='" << noVar << endl;
      vector<TString> Var(noVar,"");
      
      TIter next(dir->GetListOfKeys());
      Int_t it=0;

      TKey *key;
      while ((key = (TKey*)next())) {
         
         // make sure, that we only look at histograms
         TClass *cl = gROOT->GetClass(key->GetClassName());
         if (cl->InheritsFrom("TH1")) {
            TH1 *sig = (TH1*)key->ReadObj();
            TString hname = sig->GetName();
            // check for all signal histograms
            if (hname.Contains("__Signal") || (hname.Contains("__Regression") && !hname.Contains("__Regression_target"))) { // found a new signal plot
               hname.ReplaceAll(extension,"");
               hname.ReplaceAll("__Signal","");
               hname.ReplaceAll("__Regression","");
               Var[it] = hname;
            ++it;	
            }
         }
      }

      cout << "found histos for "<< it <<" variables='" << endl;

      for (Int_t ic=0;ic<it;ic++) {    
         TString subTitle = (Var[ic].Contains("_target") ? 
                             Form( "      Target: %s      ", Var[ic].ReplaceAll("_target","").Data()) : 
                             Form( "      Variable: %s      ", Var[ic].Data()));
         subCommand = Form( ".x %s/correlationscatters.C(\"%s\",\"%s\",\"%s\",\"%s\",%i)", 
                            "$CMSSW_BASE/src/TAMUWW/MVA/macros", (odir+ofile).Data(), Var[ic].Data(), str->GetString().Data(), subTitle.Data(), (Int_t)kFALSE );
         
         lines_to_process.push_back(make_pair(subTitle,subCommand));
         ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, subTitle, str->GetString());    
      }
      
      DefaultValues::DestroyCanvases();
      file->Close();
   }

   // coefficients
   title = Form( "(%i) Input Variable Linear Correlation Coefficients", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/correlations.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title);

   title = Form( "(%ia) Classifier Output Distributions (test sample)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/mvas.C(\"%s\",0)", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   title = Form( "   (%ib) Classifier Output Distributions (test and training samples superimposed)   ", ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/mvas.C(\"%s\",3)", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   title = Form( "(%ic) Classifier Probability Distributions (test sample)", ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/mvas.C(\"%s\",1)", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   title = Form( "(%id) Classifier Rarity Distributions (test sample)", ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/mvas.C(\"%s\",2)", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);
   
/*
   // cannot run a gui while running an executable or in batch mode (causes a segmentation violation)
   title = Form( "(%ia) Classifier Cut Efficiencies", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/mvaeffs.C+(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);
*/
   ++ic;
   vector<Float_t> fNSignal,fNBackground;
   fNSignal.push_back(1000);
   fNSignal.push_back(1898);
   //fNSignal.push_back(3796);
   fNBackground.push_back(1000);
   fNBackground.push_back(112883);
   //fNBackground.push_back(225766);
   if(fNSignal.size()!=fNBackground.size()) {
      cout << "--- TAMUWWMVA::Plot ERROR vectors fNSignal and fNBackground must have the same size." << endl;
      return;
   }
   for (unsigned int isig=0; isig<fNSignal.size(); isig++) {
      title = Form( "(%ia) Classifier Cut Efficiencies", ic );
      command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/mvaeffs_noGUI.C+(%f,%f,\"%s\")", fNSignal[isig], fNBackground[isig], (odir+ofile).Data() );
      lines_to_process.push_back(make_pair(title,command));

      DefaultValues::DestroyCanvases();
   }
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   title = Form( "(%ib) Classifier Background Rejection vs Signal Efficiency (ROC curve)", ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/efficiencies.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   title = Form( "(%i) PDFs of Classifiers (requires \"CreateMVAPdfs\" option set)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/probas.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   title = Form( "(%i) Likelihood Reference Distributiuons", ++ic);
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/likelihoodrefs.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "Likelihood");

   title = Form( "(%ia) Network Architecture (MLP)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/network.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "MLP");

   title = Form( "(%ib) Network Convergence Test (MLP)", ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/annconvergencetest.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "MLP");

   title = Form( "(%i) Decision Trees (BDT)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/BDT.C+(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "BDT");

   title = Form( "(%i) Decision Tree Control Plots (BDT)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/BDTControlPlots.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "BDT");

   title = Form( "(%i) Plot Foams (PDEFoam)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/PlotFoams.C", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "PDEFoam");

   title = Form( "(%i) General Boost Control Plots", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/BoostControlPlots.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, "Boost");

   title = Form( "(%i) Parallel Coordinates (requires ROOT-version >= 5.17)", ++ic );
   command = Form( ".x $CMSSW_BASE/src/TAMUWW/MVA/macros/paracoor.C(\"%s\")", (odir+ofile).Data() );
   lines_to_process.push_back(make_pair(title,command));
   ActionButton(TMVAGui_inactiveButtons, TMVAGui_keyContent, title, defaultRequiredClassifier);

   for (unsigned int i=0; i<lines_to_process.size(); i++) {
      if (DefaultValues::vfind(TMVAGui_inactiveButtons,lines_to_process[i].first)==-1) {
         cout << "Processing line: " << lines_to_process[i].second << " ... " << endl;
         gROOT->ProcessLine(lines_to_process[i].second);
         gROOT->ProcessLine(".mv plots/* " + odir);
      }
      else {
         cout << "Inactive Command: " << lines_to_process[i].second << endl;
      }
   }
  
   // remove default folder for storing plots
   //gROOT->ProcessLine(".rm plots/");

   // pauses the program at the end if uncommented
   /*
   string dummy;
   cout << "WAITING FOR INPUT!!! (Press ENTER)";
   cin >> dummy;
   */
   return;
}

ClassImp(TAMUWWMVA)
