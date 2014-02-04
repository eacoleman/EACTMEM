// C++ libraries
#include <iostream>

// ROOT libraries

// This code libraries
#include "TAMUWW/MEPATNtuple/interface/METree.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/EventNtuple.hh"

using namespace std;

int main(int argc,char**argv) {

	//int * a = new int(2);
	//delete a;

	cout << "Making EventNtuple ... ";
	EventNtuple * ntuple      = new EventNtuple();
	//cout << "ntuple = " << ntuple << endl;
	//EventNtuple* a = new EventNtuple(*ntuple);
	//cout << "a = " << a << endl;
	//EventNtuple b = *a;
	cout << "DONE" << endl;

	cout << "Deleting EventNtuple ... ";
	delete ntuple;
	//delete a;
	cout << "DONE" << endl;



	cout << "Making METree ... ";
	METree * metree      = new METree();
	//cout << "metree = " << metree << endl;
	//METree* a = new METree(*metree);
	//cout << "a = " << a << endl;
	//METree b = *a;
	cout << "DONE" << endl;

	cout << "Deleting METree ... ";
	delete metree;
	//delete a;
	cout << "DONE" << endl;



	cout << "Making microNtuple ... ";
	MicroNtuple * microNtuple      = new MicroNtuple(2);
	cout << "microNtuple = " << microNtuple << endl;
	MicroNtuple* a = new MicroNtuple(*microNtuple);
	cout << "a = " << a << endl;
	MicroNtuple b = *a;
	cout << "DONE" << endl;

	cout << "Testing microNtuple in a tree ...";
	TFile* tmp = new TFile("tmp.root","RECREATE");
	TTree* t = new TTree("t","t");
	t->Branch("mnt","MicroNtuple",&microNtuple);
	microNtuple->clear();
	t->Fill();
	t->Write();
	tmp->Close();
	cout << "DONE" << endl;


	cout << "Deleting microNtuple ... ";
	delete microNtuple;
	//cout << "microNtuple = " << microNtuple << endl;
	delete a;
	//delete t;
	//cout << "a = " << a << endl;
	cout << "DONE" << endl;

	return 0;
}