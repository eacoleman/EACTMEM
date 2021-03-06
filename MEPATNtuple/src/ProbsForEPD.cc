#include "TAMUWW/MEPATNtuple/interface/ProbsForEPD.hh"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

ProbsForEPD::ProbsForEPD() 
{
  wh     =0; // for whatever mass we want
  hww    =0; // for whatever mass we want
  schan  =0;
  tchan  =0;
  tchan2 =0;
  tt     =0;
  wlight =0; 
  zlight =0;
  wbb    =0;
  wc     =0;
  wgg    =0;
  ww     =0;
  wz     =0;
  zz     =0;
  qcd    =0;

}// Basic C'tor

ProbsForEPD::ProbsForEPD(double dd) 
{
  wh     = dd; // for whatever mass we want
  hww    = dd; // for whatever mass we want
  schan  = dd;
  tchan  = dd;
  tchan2 = dd;
  tt     = dd;
  wlight = dd; 
  zlight = dd;
  wbb    = dd;
  wc     = dd;
  wgg    = dd;
  ww     = dd;
  wz     = dd;
  zz     = dd;
  qcd    = dd;

}// Basic C'tor

// Make sure we have a quick c'tor. Please keep the order in which the 
// probabilities are defined in the signature.
ProbsForEPD::ProbsForEPD(double _wh    ,
			 double _hww   ,
			 double _schan ,
			 double _tchan ,
			 double _tchan2,
			 double _tt    ,
			 double _wlight,
			 double _zlight,
			 double _wbb   ,
			 double _wc    ,
			 double _wgg   ,
			 double _ww    ,
			 double _wz    ,
			 double _zz    ,
			 double _qcd   ){
  wh    = _wh    ;
  hww   = _hww   ;
  schan = _schan ;
  tchan = _tchan ;
  tchan2= _tchan2;
  tt    = _tt    ;
  wlight= _wlight;
  zlight= _zlight;
  wbb   = _wbb   ;
  wc    = _wc    ;
  wgg   = _wgg   ;
  ww    = _ww    ;
  wz    = _wz    ;
  zz    = _zz    ;
  qcd   = _qcd   ;
  
}// C'tor

// The multiplication assignement 
ProbsForEPD& ProbsForEPD::operator *=(const ProbsForEPD& b){

  wh    *= b.wh    ;
  hww   *= b.hww   ;
  schan *= b.schan ;
  tchan *= b.tchan ;
  tchan2*= b.tchan2;
  tt    *= b.tt    ;
  wlight*= b.wlight;
  zlight*= b.zlight;
  wbb   *= b.wbb   ;
  wc    *= b.wc    ;
  wgg   *= b.wgg   ;
  ww    *= b.ww    ;
  wz    *= b.wz    ;
  zz    *= b.zz    ;
  qcd   *= b.qcd   ;

  return *this;

}//operator *=

// The multiplication assignement 
ProbsForEPD ProbsForEPD::operator *(const ProbsForEPD& b) const {

  ProbsForEPD res;

  res.wh     = wh    * b.wh    ;
  res.hww    = hww   * b.hww   ;
  res.schan  = schan * b.schan ;
  res.tchan  = tchan * b.tchan ;
  res.tchan2 = tchan2* b.tchan2;
  res.tt     = tt    * b.tt    ;
  res.wlight = wlight* b.wlight;
  res.zlight = zlight* b.zlight;
  res.wbb    = wbb   * b.wbb   ;
  res.wc     = wc    * b.wc    ;
  res.wgg    = wgg   * b.wgg   ;
  res.ww     = ww    * b.ww    ;
  res.wz     = wz    * b.wz    ;
  res.zz     = zz    * b.zz    ;
  res.qcd    = qcd   * b.qcd   ;

  return res;

}//operator *


// This just print all the elements to screen
void ProbsForEPD::show(std::string title){

  cout<<title<<endl;
  cout << "\t wh     =" << wh      << endl;; 
  cout << "\t hww    =" << hww     << endl;; 
  cout << "\t schan  =" << schan   << endl;;
  cout << "\t tchan  =" << tchan   << endl;;
  cout << "\t tchan2 =" << tchan2  << endl;;
  cout << "\t tt     =" << tt      << endl;;
  cout << "\t wlight =" << wlight  << endl;; 
  cout << "\t zlight =" << zlight  << endl;;
  cout << "\t wbb    =" << wbb     << endl;;
  cout << "\t wc     =" << wc      << endl;;
  cout << "\t wgg    =" << wgg     << endl;;
  cout << "\t ww     =" << ww      << endl;;
  cout << "\t wz     =" << wz      << endl;;
  cout << "\t zz     =" << zz      << endl;;
  cout << "\t qcd    =" << qcd     << endl;;
  
}//show


// This divides each member to the sum of all members
void ProbsForEPD::normalize(){

  // Compute the total
  double tot = 0;
  tot += wh     ;
  tot += hww    ; 
  tot += schan  ;
  tot += tchan  ;
  tot += tchan2 ;
  tot += tt     ;
  tot += wlight ; 
  tot += zlight ;
  tot += wbb    ;
  tot += wc     ;
  tot += wgg    ;
  tot += ww     ;
  tot += wz     ;
  tot += zz     ;
  tot += qcd    ;

  // Normalize it
  wh     /= tot;;
  hww    /= tot;; 
  schan  /= tot;;
  tchan  /= tot;;
  tchan2 /= tot;;
  tt     /= tot;;
  wlight /= tot;; 
  zlight /= tot;;
  wbb    /= tot;;
  wc     /= tot;;
  wgg    /= tot;;
  ww     /= tot;;
  wz     /= tot;;
  zz     /= tot;;
  qcd    /= tot;;

}// Normalize


// This just prints how to call the constructor
// used in optimizeCoefficients.
string ProbsForEPD::getStringConstructor(){

  stringstream res;

  res << "ProbsForEPD(";
  res << wh     << ",";
  res << hww    << ","; 
  res << schan  << ",";
  res << tchan  << ",";
  res << tchan2 << ",";
  res << tt     << ",";
  res << wlight << ","; 
  res << zlight << ",";
  res << wbb    << ",";
  res << wc     << ",";
  res << wgg    << ",";
  res << ww     << ",";
  res << wz     << ",";
  res << zz     << ",";
  res << qcd    << ");";

  return res.str();

}//getStringConstructor
