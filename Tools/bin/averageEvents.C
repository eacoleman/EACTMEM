//
// EXAMPLE
//
/*
.L TAMUWW/Tools/bin/averageEvents.C
averageEvents("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20150202_MC12/WJets_part2_JESUp/")
*/

void averageEvents(TString basepath = "./", const char *ext=".root", int lowerBound = 0, int upperBound = -1) {
   gROOT->ProcessLine(".L TAMUWW/Tools/bin/list_files.C");
   vector<TString> files = vector<TString> files = list_files(basepath.Data(),ext);
   int count = 0;
   int entry_sum = 0;
   bool print = false;
   int job_number = -1;
   for(int i=0;i<files.size();i++){
      TString filename = files[i];
      TObjArray* filename_pieces = filename.Tokenize("_");
      TObjString* file_number = (TObjString*)(*filename_pieces)[1];
      job_number = file_number->GetString().Atoi();
      if(job_number < lowerBound || (upperBound > -1 && job_number > upperBound) )
         continue;

      if(!print&&i%100==0) {
         cout << "\tDoing File " << i << endl;
      }
      if(print) {
         cout << Form("%s%s",basepath.Data(),files[i].Data()) << endl;
      }
      TFile* a = new TFile(Form("%s%s",basepath.Data(),files[i].Data()),"READ");
      PS->cd();
      if(print) {
         cout << jets2p->GetEntries() << endl;
      }
      entry_sum+=jets2p->GetEntries();
      count++;
      a->Close();
   }
   cout << "NFiles = " << count << endl;
   cout << "Sum = " << entry_sum << endl;
   cout << "Average = " << entry_sum/double(count) << endl;
}

void listEntries(TString basepath = "/eos/uscms/store/user/aperloff//MatrixElement/Summer12ME8TeV/MEInput/") {
//   TString filenames[20] = {"WJets_part2_JESDown_part1.root","WJets_part2_JESDown_part2.root","WJets_part2_JESDown_part3.root","WJets_part2_JESDown_part4.root","WJets_part2_JESDown_part5.root","WJets_part2_JESDown_part6.root","WJets_part2_JESDown_part7.root","WJets_part2_JESDown_part8.root","WJets_part2_JESUp_part1.root","WJets_part2_JESUp_part2.root","WJets_part2_JESUp_part3.root","WJets_part2_JESUp_part4.root","WJets_part2_JESUp_part5.root","WJets_part2_JESUp_part6.root","WJets_part2_JESUp_part7.root","WJets_part2_JESUp_part8.root","WJets_part1_JESDown.root","WJets_part1_JESUp.root","WJets_part2_JESDown.root","WJets_part2_JESUp.root"}
   TString filenames[8] = {"WJets_part1_JESDown.root","WJets_part1_JESDown_part0.root","WJets_part1_JESDown_part1.root","WJets_part1_JESDown_part2.root","WJets_part1_JESDown_part3.root","WJets_part1_JESDown_part4.root","WJets_part1_JESDown_part5.root","WJets_part1_JESDown_part6.root"}
   
   for(int i=0;i<3;i++) {
      cout << basepath+filenames[i] << endl;
      TFile* a = new TFile(basepath+filenames[i],"READ");
      PS->cd();
      cout << jets2p->GetEntries() << endl;
   }
}
