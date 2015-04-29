#include "TFile.h"
#include "TAMUWW/SpecialTools/interface/Table.hh"
#include "TAMUWW/SpecialTools/interface/TableRow.hh"
#include "TAMUWW/SpecialTools/interface/TableCell.hh"
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh"

#include <iostream>
#include <string>

using namespace std;

int main() {
   TFile* ifile0 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfile_SingleEl_A_20130312/PS.root");
   TFile* ifile1 = TFile::Open("/uscms/home/aperloff/nobackup/PS_outfiles_SingleEl_A_20130312_simple/PS.root");
   ifile0->cd("PS");
   gDirectory->pwd();
   Table* t_hard = (Table*)gDirectory->Get("ElectronTable");
   ifile1->cd("PS");
   gDirectory->pwd();
   Table* t_easy = (Table*)gDirectory->Get("ElectronTable");

   Table* diff = new Table(((*t_hard)-(*t_easy)));
   Table* sum = new Table(((*t_hard)+(*t_easy)));

   Table* pdiff = new Table(((*diff)/((*sum)/2))*100);

   pdiff->printTable(cout);

   return 0;
}
