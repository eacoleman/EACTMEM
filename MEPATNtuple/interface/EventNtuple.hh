/////////////////////////////////////////////////////////////////////////////////////////////////
//// CMS 
//// WW CrossSection Measurement using Matrix Element 
//// Created by Osipenkov, Ilya : ilyao@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef EVENTNTUPLE_HH
#define EVENTNTUPLE_HH

#include "TLorentzVector.h"
#include "TObject.h"
//#include "TROOT.h"
//#include "TTree.h"
#include <string>
#include <vector>
//#include <math>
using std::vector;


class EventNtuple : public TObject
{
public:

  EventNtuple();
  ~EventNtuple();


  //Needed for ME
  TLorentzVector lLV;
  vector < TLorentzVector > jLV;
  TLorentzVector METLV;
  vector < int > jBtag;
  int lQ;
  int ldetComp;
  int run;
  int event;

  //Additional
  double        Mjj;
  int           passAll;

  ClassDef(EventNtuple, 1)

};

#endif
