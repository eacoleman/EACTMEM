//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/MinimalNtuple.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include "TAMUWW/SpecialTools/interface/ProgressBar.hh"

//
// ROOT includes
//
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TProfile3D.h"
#include "TRandom3.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"
#include "TBenchmark.h"

//
// Standard Library Includes
//
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//  Declare Local Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
	CommandLine cl;
    if (!cl.parse(argc,argv)) return 0;

    TString ofilename = cl.getValue<TString> ("ofilename", "minimalNtuple");
    TString ofilepath = cl.getValue<TString> ("ofilepath", "/uscms_data/d2/aperloff/");
    int     nentries  = cl.getValue<int>     ("nentries",  1000);
    int     nbins     = cl.getValue<int>     ("nbins",     100);
   
    if (!cl.check()) 
       return 0;
    cl.print();

	TBenchmark* m_benchmark = new TBenchmark();
   	m_benchmark->Reset();
   	m_benchmark->Start("sample");

	gRandom->SetSeed(0);

	if(!ofilepath.EndsWith("/")) ofilepath += "/";
	TFile* ofile = new TFile(Form("%s%s_nentries%i_nbins%i.root",ofilepath.Data(),ofilename.Data(),nentries,nbins),"RECREATE");
	TTree* otree = new TTree("minimalTree","minimalTree");
	MinimalNtuple * minimalNtuple = new MinimalNtuple(nbins);
	otree->Branch("MinimalNtuple", "MinimalNtuple", &minimalNtuple);

	for(int ientry=0; ientry<nentries; ientry++) {
		ProgressBar::loadbar2(ientry+1,nentries);
		minimalNtuple->fillRandom(gRandom);
		otree->Fill();
	}
	cout << endl;

	minimalNtuple->printAvgTime(nentries);

	otree->Write();

   	m_benchmark->Stop("sample");
   	cout << "MakeMinimalNtuple_x" << endl
   		 << "\tFile size = " << ofile->GetSize()/(1024.0*1024.0*1024.0) << " GB" << endl
   		 << "\tCPU time = " << m_benchmark->GetCpuTime("sample") << " s" << endl
         << "\tReal time = " << m_benchmark->GetRealTime("sample") << " s" << endl;
	delete m_benchmark;

	ofile->Close();

    return 0;
}