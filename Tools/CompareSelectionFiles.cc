#include <iostream>
#include <iomanip>
#include <strstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;
using std::vector;

// #include <TString.h>
// #include <TChain.h>


/////////////////Useful Functions//////////////
void recordEvents (const char* asciiFileName, vector<int> & EvtVector, int & EvtCount)
/// Assumes the first string on the line is the EventNumber
{
  char linein[300];
  int Event;
  int NEvt=0;
  
  ifstream infile;
  infile.open(asciiFileName);
  EvtCount=0;
  EvtVector.clear();

  while (infile.good()) {
    infile.getline(linein,300);
    istrstream str(linein);
    str >> Event ;
    EvtVector.push_back(Event);
    EvtCount++;
  }
}

bool eventMatch (int ToMatchEvent, vector<int> EvtVector, int EvtCount)
{
  bool ismatch=false;
  for (Int_t i=0; i<EvtCount;i++) {
    if ( ToMatchEvent==EvtVector[i] ) {
      ismatch=true;
    }
  }

  return ismatch;
}


///////////////////////////////////////////////

void CompareSelectionFiles(const char* FileNameA, const char* FileNameB)
{
  int NEvtA, NEvtB, NEvtAnotB, NEvtBnotA;
  vector<int> EvtVectorA, EvtVectorB, EvtVectorAnotB, EvtVectorBnotA;

  recordEvents(FileNameA,EvtVectorA,NEvtA);
  recordEvents(FileNameB,EvtVectorB,NEvtB);

  ///Record the events which are in file A but not in B
  NEvtAnotB=0;
  EvtVectorAnotB.clear();
  for (Int_t j=0; j<NEvtA;j++) {
    if ( eventMatch(EvtVectorA[j],EvtVectorB,NEvtB)==false ) {
      EvtVectorAnotB.push_back(EvtVectorA[j]);
      NEvtAnotB++;
    }
  }

  cout << FileNameA << " : " << endl;
  cout << "Has " << NEvtAnotB << " events, which are not in " << FileNameB << endl;
  cout << "Events: " ;
  for (Int_t k=0; k<NEvtAnotB; k++) {
    cout << EvtVectorAnotB[k] << ", ";
  }
  cout << endl;
}


