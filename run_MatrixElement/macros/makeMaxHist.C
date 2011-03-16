#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"

#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

using std::cerr;
using std::endl;
using std::numeric_limits;
using std::runtime_error;
using std::string;
using std::vector;

void makeMaxHist(string rootfile, unsigned nBins)
{
   TFile* file = TFile::Open(rootfile.c_str(), "READ");
   if (!file || !file->IsOpen())
   {
      throw runtime_error("Cannot open file");
   }

   vector<double> maxVec;
   double max = numeric_limits<double>::min();
   double min = numeric_limits<double>::max();

   unsigned counter = 0;
   while (true)
   {
      string name = "MassProfile0_";
      name = AuxFunctions::concatString(name, counter++);

      TGraphErrors* graph = 0;
      file->GetObject(name.c_str(), graph);

      if (!graph)
         break;

      unsigned nPoints = graph->GetN();
      double maxX = 0;
      double maxY = numeric_limits<double>::min();
      for (unsigned i = 0; i < nPoints; ++i)
      {
         double x, y;
         graph->GetPoint(i, x, y);
         if (y > maxY)
         {
            maxX = x;
            maxY = y;
         }
      }

      maxVec.push_back(maxX);
      if (maxX < min)
         min = maxX;
      if (maxX > max)
         max = maxX;
   }

   if (maxVec.empty())
   {
      cerr << "No graphs found!\n";
      return;
   }

   double binSize = (max - min) / nBins;
   TH1F* hist = new TH1F("Mass Profile", "Mass Profile", nBins, min - binSize,
                         max + binSize);

   for (vector<double>::const_iterator it = maxVec.begin(); it != maxVec.end();
        ++it)
   {
      hist->Fill(*it);
   }

   TCanvas* can = new TCanvas("Mass Profile");
   hist->Draw();

   string filename = rootfile;
   if (AuxFunctions::stringSub(filename, ".root", ".gif"))
   {
      can->Print(filename.c_str());
   }

}
