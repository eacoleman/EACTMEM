#ifndef AUXFUNCTIONSROOT_HH
#define AUXFUNCTIONSROOT_HH

#include <string>
#include <vector>

#include <TClonesArray.h>

class TFile;
class TH1;
class THStack;
class TLorentzVector;
class TText;
class TTree;

namespace AuxFunctionsRoot
{
   TTree* openAndGetTree(const TFile* file, const char* treeName);

// Calculate KS probability for two histograms
   double getKSProb(TH1* data, TH1* mc, unsigned pseudo);
// utility functions for getKSProb
   double getKSD(TH1* hist1, TH1* hist2);
   TH1* intg(TH1* hist, const char* histname, bool norm);

// General cleanup for calling a script many times
   void cleanUpRoot();

// Create a text object with KS and Chi2
   TText* doKSandChi2(TH1* data, TH1* prediction, float x, float y);

// Make a single histogram out of a stack (stupid Root!)
   TH1* makeSingleHist(const THStack* stack);

// Fill a tree (floats only) from a text file
   void treeFromTextFile(const std::string& inputFilename,
                         const std::string& outputFilename,
                         const std::string& branchNameFile,
                         const std::string& treeName);

// Fills the vector with all files passing "ls <filter>"
   void getFileList(const std::string& filter, std::vector<std::string>& vec);

// Converts a TLorentzVector to an array
   void makeArray(const TLorentzVector& vec, double array[]);

// Adds an element to a TClonesArray
   template<class T>
   void addToTCA(TClonesArray* array, const T& object)
   {
      new((*array)[array->GetEntries()]) T(object);
   }

// Copies a TClonesArray
   template<class T>
   void copyTCA(const TClonesArray* input, TClonesArray* output)
   {
      TIter iter(input);
      while (T* obj = dynamic_cast<T*>(iter()))
      {
         addToTCA(output, *obj);
      }
   }

}

std::ostream& operator<<(std::ostream& stream, const TLorentzVector& lv);

#endif
