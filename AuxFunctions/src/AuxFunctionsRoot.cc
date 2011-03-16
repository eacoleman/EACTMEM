#include "TAMUWW/AuxFunctions/interface/AuxFunctionsRoot.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <TDirectory.h>
#include <TFile.h>
#include <TH1.h>
#include <THStack.h>
#include <TLatex.h>
#include <TLorentzVector.h>
#include <TROOT.h>
#include <TSeqCollection.h>
#include <TSystem.h>
#include <TTree.h>

using std::string;
using std::vector;
using std::ostream;

namespace AuxFunctionsRoot
{
   TTree* openAndGetTree(const TFile* file, const char* treeName)
   {
      if (!file->IsOpen())
      {
         throw std::runtime_error("Unable to open file!\n");
      }

      TTree* tree = dynamic_cast<TTree*>(gDirectory->Get(treeName));
      if(!tree)
      {
         throw std::runtime_error("Unable to find tree!\n");
      }
      return tree;
   }

   double getKSD(TH1* hist1, TH1* hist2)
   {      
      TH1* ihist1 = intg(hist1,"name1",true);
      TH1* ihist2 = intg(hist2,"name2",true);
      ihist2->Add(ihist1,-1);
      float maxD = 0.0;
      float total1 = 0;
      float total2 = 0;
      for(int i=0; i<(ihist2->GetNbinsX())+1; i++)
      {
         if (fabs(ihist2->GetBinContent(i)) > maxD) maxD = fabs(ihist2->GetBinContent(i));
         total1 += hist1->GetBinContent(i);
         total2 += hist2->GetBinContent(i);
      }
      return maxD;
   }


   TH1* intg(TH1* hist, const char* histname, bool norm)
   {

// Make a copy of a histogram
      TH1* intH = (TH1*)hist->Clone();
      intH->Reset();
      intH->SetNameTitle(histname,"Integral");
      float total = hist->Integral();
      //printf(" Total: %f \n",total);
      if(!norm) total = 1.0;

// Now Loop over the two histograms and take the ratio
      float intval = 0.0;
      for(int i=0; i<(hist->GetNbinsX())+1; i++)
      {
         intval = intval+(hist->GetBinContent(i));
         //printf(" Bin %i Int %f \n",i,intval/total);
         intH->SetBinContent(i,intval/total);	    
      }

      return intH;
   }

   double getKSProb(TH1* data, TH1* mc, unsigned pseudo)
   {
      double dataKS = getKSD(data, mc);

//      std::cerr << dataKS << std::endl;

      TH1* temp = dynamic_cast<TH1*>(data->Clone("temp"));
   
      unsigned prob = 0;
      for (unsigned i = 0; i < pseudo; ++i)
      {
         temp->Reset();
         temp->SetEntries(0);
         temp->FillRandom(mc, static_cast<int>(data->Integral()));
         double newKS = getKSD(temp, mc);
         if (newKS > dataKS)
            ++prob;
      }
      return static_cast<double>(prob) / pseudo;
   }

   void cleanUpRoot()
   {
      gROOT->Reset();
      gDirectory->DeleteAll();

      TSeqCollection* coll = gROOT->GetListOfFiles();
      TIter it(coll);
      while (TFile* file = (dynamic_cast<TFile*>(it.Next())))
         file->Close();
      
      TSeqCollection* coll2 = gROOT->GetListOfCleanups();
      TIter it2(coll2);
      while (TObject* object = (it2.Next()))
         object->Delete();
   }

   TText* doKSandChi2(TH1* data, TH1* prediction, float x, float y)
   {
      std::ostringstream stream;
      stream  << std::setprecision(1) << std::fixed
             //<< "#chi^{2} prob.: " << data->Chi2Test(prediction, "") * 100
	     << " KS prob.: "
             << getKSProb(data, prediction, 1000) * 100 << "%";

      TLatex* text = new TLatex(x, y, stream.str().c_str());
      return text;
   }

   TH1* makeSingleHist(const THStack* stack)
   {
      TH1* responseHist = 0;

      TIter next(stack->GetHists());
      while (TH1* hist = dynamic_cast<TH1*>(next()))
      {
         if (!responseHist)
         {
            std::string name(hist->GetTitle());
            name += " single";
            responseHist = dynamic_cast<TH1*>(hist->Clone(name.c_str()));
         }
         else
         {
            responseHist->Add(hist);
         }
      }

      return responseHist;
   }

   void treeFromTextFile(const string& inputFilename,
                         const string& outputFilename,
                         const string& branchNameFile, const string& treeName)
   {
      TFile* file = TFile::Open(outputFilename.c_str(), "RECREATE");
      if (!file)
         throw std::runtime_error("Unable to create output file!");

      TTree* tree = new TTree(treeName.c_str(), treeName.c_str());

      std::ifstream input(inputFilename.c_str());
      if (!input)
         throw std::runtime_error("Unable to open file!");

      std::ifstream branches(branchNameFile.c_str());
      if (!branches)
         throw std::runtime_error("Unable to open file!");

      vector<float*> variables;

      string name;
      while (branches >> name)
      {
         float* temp = new float(0);

         tree->Branch(name.c_str(), temp, (name + "/F").c_str());
         variables.push_back(temp);
      }

      while (input)
      {
         for (vector<float*>::iterator it = variables.begin();
              it != variables.end(); ++it)
         {
            input >> **it;
         }
         tree->Fill();
      }

      file->Write();

// cleanup
      for (vector<float*>::iterator it = variables.begin();
           it != variables.end(); ++it)
      {
         delete *it;
      }

      delete tree;
      delete file;
   }

   void getFileList(const string& filter, vector<string>& vec)
   {
      string command("ls -1 ");
      command += filter;
      command += "> tempfile.txt";

      gSystem->Exec(command.c_str());
      std::ifstream input("tempfile.txt", std::ios::in);
      if (!input)
         throw std::runtime_error("Unable to read tempfile.txt!");

      string reader;

      while (input >> reader)
      {
         vec.push_back(reader);
      }

      gSystem->Exec("rm tempfile.txt");
   }

   void makeArray(const TLorentzVector& vec, double array[])
   {
      array[0] = vec.E();
      array[1] = vec.Px();
      array[2] = vec.Py();
      array[3] = vec.Pz();
   }
}

ostream& operator<<(ostream& stream, const TLorentzVector& lv)
{
   stream << "(" << lv.Px() << ", " << lv.Py() << ", " << lv.Pz() << ", "
          << lv.E() << ")";
   return stream;
}
