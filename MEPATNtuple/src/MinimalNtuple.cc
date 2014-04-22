/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// For testing matrix elements
//// Created by Perloff, Alexx : aperloff@physics.tamu.edu
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "TAMUWW/MEPATNtuple/interface/MinimalNtuple.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
MinimalNtuple::MinimalNtuple() {
	realTime = 0;
	cpuTime = 0;
	profile = 0;
	nbins = 100;
}

//______________________________________________________________________________
MinimalNtuple::MinimalNtuple(int num_profile_bins) {
	realTime = 0;
	cpuTime = 0;
	profile = 0;
	nbins = num_profile_bins;
}


//______________________________________________________________________________
MinimalNtuple::~MinimalNtuple() {
	delete profile;
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void MinimalNtuple::fillRandom(TRandom* rand)
{
	TBenchmark* m_benchmark = new TBenchmark();
   	m_benchmark->Reset();
   	m_benchmark->Start("entry");

	if(!profile) profile = new TProfile3D("randomTProfile3D","randomTProfile3D",nbins,0,1000,nbins,0,1000,nbins,0,1000);
	if(profile && profile->GetEntries()>0) {
		delete profile;
		profile = new TProfile3D("randomTProfile3D","randomTProfile3D",nbins,0,1000,nbins,0,1000,nbins,0,1000);
		//profile->Reset();
		//profile->SetNameTitle("randomTProfile3D","randomTProfile3D");
	}

	//profile->SetEntries(1);

	run = rand->Integer(100);
	lumi = rand->Integer(100);
	event = rand->Integer(100);

	double v,e;
	for(int binx=0; binx<=profile->GetNbinsX()+1; binx++) {
		for(int biny=0; biny<=profile->GetNbinsY()+1; biny++) {
			for(int binz=0; binz<=profile->GetNbinsZ()+1; binz++) {
				rand->Rannor(v,e);
				//cout << "v=" << v << "\te=" << e << endl;
				profile->Fill(profile->GetXaxis()->GetBinCenter(binx),profile->GetYaxis()->GetBinCenter(biny),profile->GetZaxis()->GetBinCenter(binz),v);
				//profile->SetBinContent(binx,biny,binz,v);
				profile->SetBinError(binx,biny,binz,e);
				//cout << "v="<<v<<"\tProfile("<<binx<<","<<biny<<","<<binz<<")=" << profile->GetBinContent(binx,biny,binz)<<"+/-"<< profile->GetBinError(binx,biny,binz)<< endl;
				//cout << binx << "," << biny << "," << binz << endl;
				assert(v==profile->GetBinContent(binx,biny,binz));
			}
		}
	}

	m_benchmark->Stop("entry");
   	cpuTime += m_benchmark->GetCpuTime("entry");
    realTime += m_benchmark->GetRealTime("entry");
	delete m_benchmark;
}

void MinimalNtuple::printAvgTime(int entries) {
	cout << "MinimalNtuple::fillRandom" << endl << "\tAverage CPU time = " << cpuTime/double(entries) << " s" << endl
         << "\tAverage Real time = " << realTime/double(entries) << " s" << endl;
}

ClassImp(MinimalNtuple)