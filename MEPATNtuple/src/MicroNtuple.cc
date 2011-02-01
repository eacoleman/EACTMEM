#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"
#include "TAMUWW/MEPATNtuple/interface/TopLepType.hh"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "TF1.h"


using std::cout;
using std::endl;

ProbsForEPD::ProbsForEPD() 
{

  schan  = 0;
  tchan  = 0;
  tchan2 = 0;
  wbb    = 0;
  wcc    = 0;
  wc     = 0;
  wjg    = 0; 
  zjg    = 0;
  qcd    = 0;
  tt     = 0; 
  ww     = 0;
  wz     = 0;
  wh     = 0;

}


// This just print all the elements to screen
void ProbsForEPD::Show(std::string title){
  cout<<title<<endl;
  cout<<"\t ww    ="<< ww     <<endl;
  cout<<"\t wz    ="<< wz     <<endl;
  cout<<"\t wjg   ="<< wjg    <<endl;
  cout<<"\t zjg   ="<< zjg    <<endl;
  cout<<"\t tt    ="<< tt     <<endl;
  cout<<"\t tchan ="<< tchan  <<endl;
  cout<<"\t schan ="<< schan  <<endl;
  cout<<"\t qcd   ="<< qcd    <<endl;

  cout<<"\t tchan2="<< tchan2 <<endl;
  cout<<"\t wbb   ="<< wbb    <<endl;
  cout<<"\t wcc   ="<< wcc    <<endl;
  cout<<"\t wc    ="<< wc     <<endl;
  cout<<"\t wh    ="<< wh     <<endl;
  
}//Print


// This divides each member to the sum of all members
void ProbsForEPD::Normalize(){

  //Compute the total
  double tot = 0;
  tot += schan  ;
  tot += tchan  ;
  tot += tchan2 ;
  tot += wbb    ;
  tot += wcc    ;
  tot += wc     ;
  tot += wjg    ; 
  tot += zjg    ;
  tot += qcd    ;
  tot += tt     ; 
  tot += ww     ;
  tot += wz     ;
  tot += wh     ;

  //Normalize it
  schan  /= tot;
  tchan  /= tot;
  tchan2 /= tot;
  wbb    /= tot;
  wcc    /= tot;
  wc     /= tot;
  wjg    /= tot; 
  zjg    /= tot;
  qcd    /= tot;
  tt     /= tot; 
  ww     /= tot;
  wz     /= tot;
  wh     /= tot;

}// Normalize


// This divides each member to the sum of all members
void ProbsForEPD::NormalizeToMax(){

  //Compute the total
  double tot = 0;
  if (schan > tot)
    tot = schan;
  if (tchan > tot) 
    tot = tchan;
  if (tchan2 > tot)
    tot = tchan2;
  if (wbb > tot)
    tot = wbb;
  if (wcc > tot)
    tot = wcc;
  if (wjg > tot)
    tot = wjg;
  if (zjg > tot)
    tot = zjg;
  if (qcd > tot)
    tot = qcd;
  if (tt > tot)
    tot = tt;
  if (ww > tot)
    tot = ww;
  if (wz > tot)
    tot = wz;
  if (wh > tot)
    tot = wh;

  //Normalize it
  schan  /= tot;
  tchan  /= tot;
  tchan2 /= tot;
  wbb    /= tot;
  wcc    /= tot;
  wc     /= tot;
  wjg    /= tot; 
  zjg    /= tot;
  qcd    /= tot;
  tt     /= tot; 
  ww     /= tot;
  wz     /= tot;
  wh     /= tot;

}// NormalizeToMax

MicroNtuple::MicroNtuple()
{
   nJets = 2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets];
   bProb1 = new Double_t[nJets];
   bProb2 = new Double_t[nJets];

   clear();
}

MicroNtuple::MicroNtuple(int ijets) :
   nJets(ijets)
{
   knntag = new Double_t[ijets];
   tag = new Int_t[ijets];
   bProb0 = new Double_t[ijets];
   bProb1 = new Double_t[ijets];
   bProb2 = new Double_t[ijets];

   clear();
}

MicroNtuple::MicroNtuple(const MicroNtuple& rhs)
{
  //   nJets = rhs.nJets;
  nJets = 2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets]; //[ntag][nJets]
   bProb1 = new Double_t[nJets]; //[ntag][nJets]
   bProb2 = new Double_t[nJets]; //[ntag][nJets]

   for (unsigned i = 0; i < nEventProb; ++i)
   {
     eventProb[i] = rhs.eventProb[i];
     //eventProb[i] = m_tProbStat.tEventProb[i];
   }

   for (int i = 0; i < nJets; ++i)
   {
      knntag[i] = rhs.knntag[i];
      tag[i] = rhs.tag[i];
      bProb0[i] = rhs.bProb0[i];
      bProb1[i] = rhs.bProb1[i];
      bProb2[i] = rhs.bProb2[i];
      
   }
   nTagExt            = rhs.nTagExt;
   nTagExtLooseSV     = rhs.nTagExtLooseSV;
   tagProbExt0        = rhs.tagProbExt0;
   tagProbExt1        = rhs.tagProbExt1;
   tagProbExt2        = rhs.tagProbExt2;
   tagProbExtLooseSV0 = rhs.tagProbExtLooseSV0;
   tagProbExtLooseSV1 = rhs.tagProbExtLooseSV1;
   tagProbExtLooseSV2 = rhs.tagProbExtLooseSV2;


   weight = rhs.weight;
   epd1tag = rhs.epd1tag;
   epd2tag = rhs.epd2tag;
   epd1tagSchan = rhs.epd1tagSchan;
   epd2tagSchan = rhs.epd2tagSchan;
   epd1tagTchan = rhs.epd1tagTchan;
   epd2tagTchan = rhs.epd2tagTchan;

   h = rhs.h;
}

MicroNtuple& MicroNtuple::operator=(const MicroNtuple& rhs)
{
   nJets = rhs.nJets;

   delete [] knntag;
   delete [] tag;
   delete [] bProb0;
   delete [] bProb1;
   delete [] bProb2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets];
   bProb1 = new Double_t[nJets];
   bProb2 = new Double_t[nJets];

   for (unsigned i = 0; i < nEventProb; ++i)
   {
     eventProb[i] = rhs.eventProb[i];
     //eventProb[i] = m_tProbStat.tEventProb[i];
   }

   for (int i = 0; i < nJets; ++i)
   {
      knntag[i] = rhs.knntag[i];
      tag[i] = rhs.tag[i];
      bProb0[i] = rhs.bProb0[i];
      bProb1[i] = rhs.bProb1[i];
      bProb2[i] = rhs.bProb2[i];
      
   }

   nTagExt            = rhs.nTagExt;
   nTagExtLooseSV     = rhs.nTagExtLooseSV;
   tagProbExt0        = rhs.tagProbExt0;
   tagProbExt1        = rhs.tagProbExt1;
   tagProbExt2        = rhs.tagProbExt2;
   tagProbExtLooseSV0 = rhs.tagProbExtLooseSV0;
   tagProbExtLooseSV1 = rhs.tagProbExtLooseSV1;
   tagProbExtLooseSV2 = rhs.tagProbExtLooseSV2;
   weight = rhs.weight;
   epd1tag = rhs.epd1tag;
   epd2tag = rhs.epd2tag;
   epd1tagSchan = rhs.epd1tagSchan;
   epd2tagSchan = rhs.epd2tagSchan;
   epd1tagTchan = rhs.epd1tagTchan;
   epd2tagTchan = rhs.epd2tagTchan;

   h = rhs.h;

   return *this;
}

MicroNtuple::~MicroNtuple()
{
   delete [] knntag;
   delete [] tag;
   delete [] bProb0;
   delete [] bProb1;
   delete [] bProb2;
}

void MicroNtuple::clear()
{
   for (unsigned i = 0; i < nEventProb; ++i)
   {
      eventProb[i] = 0;
   }

   delete [] knntag;
   delete [] tag;
   delete [] bProb0;
   delete [] bProb1;
   delete [] bProb2;

   knntag = new Double_t[nJets];
   tag = new Int_t[nJets];
   bProb0 = new Double_t[nJets];
   bProb1 = new Double_t[nJets];
   bProb2 = new Double_t[nJets];

   
   bProb0[0] = 0.;
   bProb0[1] = 0.;
   bProb1[0] = 0.;
   bProb1[1] = 0.;
   bProb2[0] = 0.;
   bProb2[1] = 0.;

   nTagExt            = 0;
   nTagExtLooseSV     = 0;
   tagProbExt0        = 0;
   tagProbExt1        = 0;
   tagProbExt2        = 0;
   tagProbExtLooseSV0 = 0;
   tagProbExtLooseSV1 = 0;
   tagProbExtLooseSV2 = 0;
   weight = 0;
   epd1tag = 0;
   epd2tag = 0;
   epd1tagSchan = 0;
   epd2tagSchan = 0;
   epd1tagTchan = 0;
   epd2tagTchan = 0;

   h.clear();
}

Double_t MicroNtuple::getKNNTagProb(double knn)
{
   double tagProb = .5 * (1 + knn);
   double corrTag = 1 / (1 + (1 / tagProb - 1) * 1.5477);   
   return (corrTag >= 0 && corrTag <= 1) ? corrTag : 0;
}

Double_t MicroNtuple::calcEPD(unsigned ntag, EPDType type) const {

  if(ntag==0)
    return calcEPD(ntag, type, eventProb, bProb0, nJets);
  else if(ntag==1)
    return calcEPD(ntag, type, eventProb, bProb1, nJets);
 
  return calcEPD(ntag, type, eventProb, bProb2, nJets);
    
} // calcEPD



//------------------------------------------------------------------------------
const double * MicroNtuple::getSingleTopEPDCoefficients (unsigned ntag, EPDType type){

  static const double params1tag[3][9] = {{2.5, 0, 0, .5, .01, .165, 0, 0, 0},
				   {0, 1, 1, 2, .01, .45, 0, 0, 0},
				   {3, 3, 6, 1.513, 0.0454545, 0.25, 0.005,
				    0.005, .0001}};
  
  static const double params2tag[3][9] = {{2, 0, 0, .5, .01, .01, 0, 0, 0},
					  {0, 1, 1, 5, .01, .01, 0, 0, 0},
					  {5, 0.0227273, 0.605, 0.917, 0.0033,
					   0.017, 0.0033, 0.0067, 0.00003}};
  
  if (ntag==1)
    return params1tag[type];

  return  params2tag[type];

}//getSingleTopEPDCoefficients

//------------------------------------------------------------------------------
//EPD for single top 
Double_t MicroNtuple::calcEPD(unsigned ntag, EPDType type,
                              const double eventProb[nEventProb],
                              const double bProb[], Int_t nJets){
 
  if (nJets == 2){

    // Get the Higgs Coefficients
    const double* params = getSingleTopEPDCoefficients(ntag,type);  
    
    // Get the probabilities for the EPD. the indexWH is set to zero and it's irrelevant here 
    ProbsForEPD probs = getProbsForEPD(eventProb, params, 0, bProb, ntag);
    
    // There is less than three events per MC sample in which all the probabilities are zero. 
    // To avoid returning a NaN just return zero for those
    if (probs.schan + probs.tchan + probs.tchan2 == 0) return 0;

    // Return the wh probability. Interestingly there is not diboson here
    return (probs.schan+probs.tchan+probs.tchan2)/ (probs.schan +  probs.tchan + probs.tchan2 + probs.wbb + 
						    probs.wcc + probs.wc + probs.wjg + probs.zjg + probs.qcd + probs.tt );       
   
   
  }//njets==2
   
  return 0;

}//calcEPD




//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHiggsEPD(unsigned ntag, double mass) const
{
  if(ntag==0)
    return calcHiggsEPD(ntag, mass, eventProb, bProb0, nJets);
  else if(ntag==1)
    return calcHiggsEPD(ntag, mass, eventProb, bProb1, nJets);

  return calcHiggsEPD(ntag, mass, eventProb, bProb2, nJets);
  
}//calcHiggsEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHiggsEPD(unsigned ntag, double mass,
                                   const double eventProb[nEventProb],
                                   const double bProb[], Int_t nJets){

   
   // Translate mass to WH index
   unsigned indexWH  = getHiggsMassIndex(mass);
  
   // Get the Higgs Coefficients
   const double* params = getHiggsEPDCoefficients(indexWH,ntag,nJets);  

   // Get the probabilities for the EPD
   ProbsForEPD probs = getProbsForEPD(eventProb, params, indexWH, bProb, ntag);

   // There is less than three events per MC sample in which all the 
   // probabilities are zero.  To avoid returning a NaN just 
   // return zero for those
   if (probs.wh == 0) return 0;

   // Return the wh probability
   return probs.wh / (probs.wh + probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
		      probs.wcc + probs.wc + probs.wjg + probs.zjg + probs.qcd + probs.tt +
		      probs.ww + probs.wz);      

}// calcHiggsEPD

//------------------------------------------------------------------------------
unsigned int MicroNtuple::getHiggsMassIndex(double mass){

  if (mass == 100)
      return 0;
   else if (mass == 105)
      return 1;
   else if (mass == 110)
      return 2;
   else if (mass == 115)
      return 3;
   else if (mass == 120)
      return 4;
   else if (mass == 125)
      return 5;
   else if (mass == 130)
      return 6;
   else if (mass == 135)
      return 7;
   else if (mass == 140)
      return 8;
   else if (mass == 145)
      return 9;
   else if (mass == 150)
      return 10;

  //fail with error
  throw std::runtime_error("MicroNtuple::getHiggsMassIndex wrong mass requested");
  return 0;

}//getHiggsMassIndex

//------------------------------------------------------------------------------
const double * MicroNtuple::getHiggsEPDCoefficients(int indexWH, unsigned ntag, int nJets){

  
   unsigned tagIndex;
   if (ntag <= 1)   // for now, we use the parameters for the single tag case for the EPD with no KIT
      tagIndex = 0;
   else if (ntag == 2)
      tagIndex = 1;
   else{
      throw std::runtime_error("ERROR MicroNtuple::getHiggsEPDCoefficients. Invalid tag requested");
   }

   // Only allow 2 and 3 jets for now
   if (nJets != 2 && nJets != 3 )
     throw std::runtime_error("ERROR MicroNtuple::getHiggsEPDCoefficients. Invalid nJets requested");

   // Ricardo's optimized numbers using 
   // macros/findMENormalizationFactors.C and macros/optimizeEPDCoefficients.C
   // Parameters [indexWH][tag][parameter]
   static const double par_optimized[11][2][12] = {
     // 100 GeV 
     {{4.63597e+06, 591656, 5.86254e+06, 7.77704e+06, 3.14972e+06, 9.20856e+06, 49859.6, 49685.8, 351.092, 3.57324e+06, 3.04051e+07, 1.02566e+10}, // figOfMerit=0.399896
      {4.87901e+06, 534189, 2.46347e+06, 6.2523e+06, 953051, 13000.6, 2749.55, 4021.96, 624.351, 251372, 2.63765e+06, 1.116e+10}}, // figOfMerit=0.418224
     // 105 GeV 
     {{5.82659e+06, 4.15712e+06, 1.60875e+07, 8.69959e+06, 4.9701e+06, 8.34518e+06, 27072, 190546, 220.068, 3.03855e+06, 1.02298e+08, 8.40582e+09}, // figOfMerit=0.369226
      {3.48153e+06, 72114.7, 163900, 3.91539e+06, 570560, 39725.7, 20130.5, 4241, 483.142, 638694, 3.99292e+06, 8.10861e+09}}, // figOfMerit=0.393412
     // 110 GeV 
     {{3.01082e+06, 1.78476e+06, 1.48462e+07, 3.7367e+06, 6.89416e+06, 7.62412e+06, 296790, 18795.1, 315.187, 6.43759e+06, 4.97324e+07, 5.97079e+09}, // figOfMerit=0.330064
      {6.8423e+06, 169113, 1.78555e+06, 7.69738e+06, 655740, 86902.4, 34525.1, 14533.2, 651.029, 359324, 6.02245e+06, 1.76479e+10}}, // figOfMerit=0.352714
     // 115 GeV
     {{3.37478e+06, 1.37937e+06, 2.33025e+06, 4.63477e+06, 1.60375e+06, 8.94032e+06, 23353.4, 62083, 176.16, 4.29719e+06, 7.86635e+06, 7.29331e+09}, // figOfMerit=0.284919
      {3.43929e+06, 130914, 698294, 6.35886e+06, 216546, 476.888, 265.9, 32068.7, 547.023, 248895, 2.0439e+07, 1.42418e+10}}, // figOfMerit=0.305082
     // 120 GeV 
     {{9.25039e+06, 3.15653e+06, 4.60257e+06, 1.3065e+07, 4.02364e+06, 1.95523e+07, 73874.9, 361107, 347.059, 1.2354e+07, 1.03503e+08, 2.37359e+10}, // figOfMerit=0.226124
      {4.19342e+06, 455479, 670959, 3.15637e+06, 247548, 171405, 502.693, 4565.77, 493.635, 4.13941e+06, 9.19403e+07, 1.0582e+10}}, // figOfMerit=0.247794
     // 125 GeV 
     {{6.9525e+06, 7.25237e+06, 7.30086e+06, 5.15125e+06, 1.08771e+07, 1.12181e+07, 210125, 130255, 411.907, 1.09306e+07, 2.79694e+07, 7.62048e+09}, // figOfMerit=0.189647
      {1.27025e+07, 390282, 709434, 4.77026e+06, 657092, 360676, 41516.7, 40805, 816.565, 1.83571e+06, 1.12019e+08, 1.44683e+10}}, // figOfMerit=0.197976
     // 130 GeV 
     {{9.91669e+06, 5.42215e+06, 3.18688e+06, 8.7254e+06, 6.14887e+06, 1.60896e+07, 9426.34, 14606.9, 439.285, 5.56338e+06, 2.74658e+08, 9.47351e+09}, // figOfMerit=0.152044
      {1.05485e+07, 2.5474e+06, 3.2003e+06, 1.14144e+07, 2.05469e+06, 67861.7, 9958.66, 3022.15, 480.686, 1.6458e+06, 8.79742e+07, 2.48733e+10}}, // figOfMerit=0.163362
     // 135 GeV 
     {{1.83824e+06, 5.32127e+06, 1.5394e+07, 8.49281e+06, 6.83791e+06, 1.23491e+07, 20531.3, 3928.17, 549.51, 9.59766e+06, 1.4616e+08, 1.39954e+10}, // figOfMerit=0.111035
      {3.85175e+06, 120584, 779163, 4.90205e+06, 400778, 8396.85, 12947.6, 2394.73, 309.11, 672355, 5.87247e+07, 1.06295e+10}}, // figOfMerit=0.117482
     // 140 GeV 
     {{1.48491e+07, 7.35546e+06, 4.03095e+06, 1.04514e+07, 1.64232e+07, 2.84173e+07, 33262.6, 22538.1, 507.555, 1.71213e+07, 1.35374e+09, 1.45121e+10}, // figOfMerit=0.077284
      {5.58784e+06, 445283, 1.41695e+06, 6.90703e+06, 1.92896e+06, 128735, 25647.6, 1627.2, 500.666, 1.47377e+07, 9.4241e+07, 1.52431e+10}}, // figOfMerit=0.0831737
     // 145 GeV 
     {{1.55853e+06, 5.73412e+06, 1.70657e+07, 8.67131e+06, 2.20492e+06, 1.22639e+07, 71764.9, 4321.29, 177.714, 2.03767e+06, 2.44579e+08, 1.38833e+10}, // figOfMerit=0.0545032
      {6.4561e+06, 369218, 4.41018e+06, 7.03857e+06, 1.18485e+06, 178430, 9289.04, 3093.99, 894.658, 260419, 1.90162e+08, 1.72516e+10}}, // figOfMerit=0.0552059
     // 150 GeV 
     {{1.33576e+07, 4.98233e+06, 6.69003e+06, 8.26277e+06, 2.9023e+06, 1.09712e+07, 101123, 11741.5, 254.227, 1.13129e+07, 4.99774e+08, 1.94314e+10}, // figOfMerit=0.0347335
      {6.08745e+06, 422377, 1.06358e+06, 8.65099e+06, 1.63356e+06, 10250.8, 26191.2, 1374.89, 546.084, 4.00196e+06, 8.47752e+07, 1.91895e+10}} // figOfMerit=0.0356639
   };
   
   
   static const double par_optimized_3jets[11][2][12] = {
     // 100 GeV 
     {{1.14937e+06, 144144, 1.53698e+06, 173414, 566769, 0, 0, 0, 1.4313e+08, 0, 0, 5.88018e+10}, // figOfMerit=0.115101
      {480796, 118908, 444556, 122438, 17955.5, 0, 0, 0, 1.37147e+08, 0, 0, 7.43633e+10}}, // figOfMerit=0.112954
     // 105 GeV 
     {{1.66464e+06, 555345, 1.64004e+06, 60972.2, 727808, 0, 0, 0, 3.0274e+07, 0, 0, 7.4859e+10}, // figOfMerit=0.10859
      {606809, 171474, 27936, 212691, 128692, 0, 0, 0, 1.24326e+08, 0, 0, 5.39797e+10}}, // figOfMerit=0.102526
     // 110 GeV 
     {{523726, 569382, 4.16181e+06, 235005, 361125, 0, 0, 0, 2.88912e+07, 0, 0, 5.72479e+10}, // figOfMerit=0.0966809
      {1.58592e+06, 724348, 1.65145e+06, 147245, 129264, 0, 0, 0, 2.52591e+07, 0, 0, 5.96854e+10}}, // figOfMerit=0.0892342
     // 115 GeV 
     {{412553, 273062, 717663, 59001.9, 432201, 0, 0, 0, 1.5089e+08, 0, 0, 7.15232e+10}, // figOfMerit=0.0848021
      {887543, 323686, 2.97286e+06, 177956, 56714.1, 0, 0, 0, 2.85004e+08, 0, 0, 7.84507e+10}}, // figOfMerit=0.0793984
     // 120 GeV 
     {{701411, 456621, 3.05597e+06, 142623, 192490, 0, 0, 0, 2.64437e+07, 0, 0, 4.33601e+10}, // figOfMerit=0.0658526
      {536879, 523695, 771136, 85975.3, 28260.1, 0, 0, 0, 2.37631e+08, 0, 0, 3.82134e+10}}, // figOfMerit=0.0660687
     // 125 GeV 
     {{469967, 455490, 1.61995e+06, 58750.1, 309648, 0, 0, 0, 1.58551e+07, 0, 0, 8.02952e+10}, // figOfMerit=0.0578702
      {766971, 760299, 2.40739e+06, 216471, 33018.7, 0, 0, 0, 3.35415e+07, 0, 0, 4.96112e+10}}, // figOfMerit=0.0582664
     // 130 GeV 
     {{581568, 362988, 2.55818e+06, 131957, 200222, 0, 0, 0, 3.96014e+07, 0, 0, 1.39644e+11}, // figOfMerit=0.0468007
      {1.06745e+06, 85646.8, 3.69383e+06, 191985, 34680, 0, 0, 0, 2.5816e+08, 0, 0, 6.46382e+10}}, // figOfMerit=0.0447442
     // 135 GeV 
     {{631759, 479790, 885100, 129996, 315865, 0, 0, 0, 2.40713e+07, 0, 0, 1.91883e+11}, // figOfMerit=0.035223
      {270245, 575403, 2.68534e+06, 217112, 33747, 0, 0, 0, 4.44384e+08, 0, 0, 2.32706e+11}}, // figOfMerit=0.0327246
     // 140 GeV 
     {{503072, 576989, 2.54799e+06, 179326, 473962, 0, 0, 0, 2.78826e+07, 0, 0, 5.78089e+10}, // figOfMerit=0.0248736
      {507260, 540235, 4.24256e+06, 105724, 18739.9, 0, 0, 0, 9.69711e+07, 0, 0, 5.14769e+10}}, // figOfMerit=0.025419
     // 145 GeV
     {{477991, 310355, 4.24256e+06, 90985.5, 270830, 0, 0, 0, 3.02143e+07, 0, 0, 1.06308e+11}, // figOfMerit=0.0168696
      {205161, 145513, 3.55957e+06, 112224, 7352.9, 0, 0, 0, 2.10496e+08, 0, 0, 1.63475e+11}}, // figOfMerit=0.0166102
     // 150 GeV
     {{858884, 423908, 4.58882e+06, 208527, 202750, 0, 0, 0, 4.21274e+07, 0, 0, 3.18395e+11}, // figOfMerit=0.011375
     {1.24174e+06, 520726, 818434, 361040, 21176.8, 0, 0, 0, 6.26866e+07, 0, 0, 1.41327e+11}} // figOfMerit=0.0110532
   };
   

   // Do we return the 2 jet or the 3 jet bin coefficients ?
   if (nJets == 3)
     return  par_optimized_3jets[indexWH][tagIndex];
     

   // Return the 2 jets by default
   return  par_optimized[indexWH][tagIndex];
   
}//getHiggsEPDCoefficients

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHiggsMaxProbEPD(unsigned ntag, double mass) const
{
  if(ntag==0)
    return calcHiggsEPD(ntag, mass, eventMaxProb, bProb0, nJets);
  else if(ntag==1)
    return calcHiggsEPD(ntag, mass, eventMaxProb, bProb1, nJets);

  return calcHiggsEPD(ntag, mass, eventMaxProb, bProb2, nJets);
  
}//calcHiggsMaxProbEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHiggsMaxProbEPD(unsigned ntag, double mass,
					  const double eventMaxProb[nEventProb],
					  const double bProb[], Int_t nJets){

   
   // Translate mass to WH index
   unsigned indexWH  = getHiggsMassIndex(mass);
   
   // Get the Higgs Coefficients
   const double* params = getHiggsEPDMaxProbCoefficients(indexWH,ntag);  

   // Get the probabilities for the EPD
   ProbsForEPD probs = getProbsForEPD(eventMaxProb, params, indexWH, bProb, ntag);

   // There is less than three events per MC sample in which all the probabilities are zero. 
   // To avoid returning a NaN just return zero for those
   if (probs.wh == 0) return 0;

   // Return the wh probability. 
   return probs.wh / (probs.wh + probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
		      probs.wcc + probs.wc + probs.wjg + probs.zjg + probs.qcd + probs.tt +
		      probs.ww + probs.wz);      
   
}// calcHiggsMaxProbEPD

//------------------------------------------------------------------------------
const double * MicroNtuple::getHiggsEPDMaxProbCoefficients(int indexWH, unsigned ntag){

   unsigned tagIndex;
   if (ntag <= 1)   // for now, we use the parameters for the single tag case for the EPD with no KIT
      tagIndex = 0;
   else if (ntag == 2)
      tagIndex = 1;
   else{
      throw std::runtime_error("ERROR MicroNtuple::getHiggsEPDMaxProbCoefficients. Invalid tag requested");
   }

  //This numbers were optimized only for a 100 GeV Higgs
  static const double par[2][12] = {
    {713.523, 1.33844, 82.1406, 1092.2, 35597, 1.93064e-14, 2.55593e-16, 1.9576e-19, 2.68807e-07, 197.335, 54543.7, 30979.3}, // figOfMerit=1.29006
    {460.035, 36.62, 5.44116, 879.584, 4474.8, 2.53777e-15, 3.19525e-16, 6.59411e-23, 4.19812e-06, 629.831, 2232.07, 11506.4} // figOfMerit=1.46229
  };
 
  return  par[tagIndex];

}//getHiggsEPDMaxProbCoefficients

//------------------------------------------------------------------------------
ProbsForEPD MicroNtuple::getProbsForEPD(const double eventProb[nEventProb], 
					const double * params, unsigned indexWH,
					//const vector<double> & params, unsigned indexWH,
					const double bProb[], unsigned ntag){


  ProbsForEPD probs;
 
  probs.ww     = eventProb[0];
  probs.wz     = eventProb[1];
  probs.wjg    = eventProb[2];
  probs.zjg    = eventProb[3];
  probs.tt     = eventProb[4];
  probs.tchan  = eventProb[5];
  probs.schan  = eventProb[6];
  probs.qcd    = eventProb[7];

 
   if (ntag == 0) // this is the EPD with no KIT information
   {
      probs.schan  *= params[0];
      probs.tchan  *= params[1];
      probs.tchan2 *= params[2];
      probs.wbb    *= params[3];
      probs.wcc    *= params[4];
      probs.wc     *= params[5];
      probs.wjg    *= params[6];
      probs.zjg    *= params[7];
      probs.qcd    *= params[8];
      probs.tt     *= params[9];
      probs.ww     *= params[10];
      probs.wz     *= params[11];
      probs.wh     *= params[12];
   }
   else if (ntag == 1)
   {
     if (bProb[0] < 0){
       //cout<<" ERROR MicroNtuple::getProbsForEPD has ntag==1 and bProb0<0"<<endl;
       //send to cero the numerator of the EPDs that you want to get
       probs.wh     *= 0;
       probs.schan  *= 0;
       probs.tchan  *= 0;
       probs.tchan2 *= 0;
       probs.ww     *= 0;
       probs.wz     *= 0;
      
     }
     
      probs.schan  *= params[0] * bProb[0];
      probs.tchan  *= params[1] * bProb[0];
      probs.tchan2 *= params[2] * bProb[0];
      probs.wbb    *= params[3] * bProb[0];
      probs.wcc    *= params[4] * (1 - bProb[0]);
      probs.wc     *= params[5] * (1 - bProb[0]);
      probs.wjg    *= params[6] * (1 - bProb[0]);
      probs.zjg    *= params[7] * (1 - bProb[0]);
      probs.qcd    *= params[7] * (1 - bProb[0]);
      probs.tt     *= params[8] * bProb[0];
      probs.ww     *= params[9] * (1 - bProb[0]);
      probs.wz     *= params[10]* (1 - bProb[0]);
      probs.wh     *= params[11] * bProb[0];
   }
   else if (ntag == 2)
   {
     if (bProb[0] < 0 || bProb[1] < 0){
       //cout<<" ERROR MicroNtuple::getProbsForEPD has ntag==2 and (bProb[0]<0 || bProb[1]<0)"<<endl;
       //send to cero the numerator of the EPDs that you want to get
       probs.wh     *= 0;
       probs.schan  *= 0;
       probs.tchan  *= 0;
       probs.tchan2 *= 0;
       probs.ww     *= 0;
       probs.wz     *= 0;


     }

      probs.schan  *= params[0] * bProb[0] * bProb[1];
      probs.tchan  *= params[1] * bProb[0] * (1 - bProb[1]);
      probs.tchan2 *= params[2] * bProb[0] * (1 - bProb[1]);
      probs.wbb    *= params[3] * bProb[0] * bProb[1];
      probs.wcc    *= params[4] * (1 - bProb[0]) * (1 - bProb[1]);
      probs.wc     *= params[5] * (1 - bProb[0]) * (1 - bProb[1]);
      probs.wjg    *= params[6] * (1 - bProb[0]) * (1 - bProb[1]);
      probs.zjg    *= params[7] * (1 - bProb[0]) * (1 - bProb[1]);
      probs.qcd    *= params[7] * (1 - bProb[0]) * (1 - bProb[1]);
      probs.tt     *= params[8] * bProb[0] * bProb[1];
      probs.ww     *= params[9] * (1 - bProb[0]) * (1 - bProb[1]);
      probs.wz     *= params[10]* (1 - bProb[0]) * (1 - bProb[1]);
      probs.wh     *= params[11] * bProb[0] * bProb[1];
   }

  return probs;

}//getProbsForEPD


//Double_t MicroNtuple::calcHiggsEPD(unsigned ntag, double mass,
//                                 const double eventProb[nEventProb],
//                                 const double bProb[], Int_t nJets)



//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHiggsSuperEPD(unsigned ntag, double mass) const
{
  if(ntag==0)
    return calcHiggsSuperEPD(ntag, mass, eventProb, bProb0, nJets);
  else if(ntag==1)
    return calcHiggsSuperEPD(ntag, mass, eventProb, bProb1, nJets);

  return calcHiggsSuperEPD(ntag, mass, eventProb, bProb2, nJets);
  
}//calcHiggsSuperEPD

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcHiggsSuperEPD(unsigned ntag, double mass,
					const double eventProb[nEventProb],
					const double bProb[], Int_t nJets){
   
   // Translate mass to WH index
   unsigned indexWH  = getHiggsMassIndex(mass);
   
   // Get the Higgs Coefficients
   const double* params = getHiggsEPDCoefficients(indexWH,ntag,nJets);  

   // Get the probabilities for the EPD
   ProbsForEPD probs = getProbsForEPD(eventProb, params, indexWH, bProb, ntag);

   // Get the Higgs Coefficients
   const double* paramsMax = getHiggsEPDMaxProbCoefficients(indexWH,ntag);  

   // Get the probabilities for the EPD
   ProbsForEPD probsMax = getProbsForEPD(eventProb, paramsMax, indexWH, bProb, ntag);

   // There is less than three events per MC sample in which all the probabilities are zero. 
   // To avoid returning a NaN just return zero for those
   if (probs.wh+probsMax.wh == 0) return 0;

   // Return the wh probability
   return (probs.wh+probsMax.wh) / ( probs.wh     + probsMax.wh     + 
				     probs.schan  + probsMax.schan  + 
				     probs.tchan  + probsMax.tchan  + 
				     probs.tchan2 + probsMax.tchan2 + 
				     probs.wbb    + probsMax.wbb    + 
				     probs.wcc    + probsMax.wcc    + 
				     probs.wc     + probsMax.wc     + 
				     probs.wjg    + probsMax.wjg    + 
				     probs.zjg    + probsMax.zjg    + 
				     probs.qcd    + probsMax.qcd    + 
				     probs.tt     + probsMax.tt     + 
				     probs.ww     + probsMax.ww     + 
				     probs.wz     + probsMax.wz   );      
   
}// calcHiggsSuperEPD


//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWZEPD(unsigned ntag, unsigned secvtxtag) const
{

  //cout << "testing -1" << endl;
  if(ntag==0)
    return calcWZEPD(ntag,secvtxtag, eventProb, bProb0, nJets);
  else if(ntag==1)
    return calcWZEPD(ntag,secvtxtag, eventProb, bProb1, nJets);

  return calcWZEPD(ntag,secvtxtag, eventProb, bProb2, nJets);
  
}

//------------------------------------------------------------------------------
Double_t MicroNtuple::calcWZEPD(unsigned ntag, unsigned secvtxtag,
                                   const double eventProb[nEventProb],
                                   const double bProb[], Int_t nJets){


   unsigned tagIndex;
   if (ntag <= 1)   // for now, we use the parameters for the single tag case for the EPD with no KIT
      tagIndex = 0;
   else if (ntag == 2)
      tagIndex = 1;
   else{
      throw std::runtime_error("ERROR MicroNtuple::calcWZEPD. Invalid tag requested");
   }

   //Relative Weights:
   //Ricardo's optimized numbers for (ww+wz)/(ww+wz+wh+others) with a 100 GeV WH
//    static const double params[2][12] =
//      {{436559, 79818.6, 83189.6, 12905.8, 6.94955e+06, 3.54289e+06, 46012.2, 2.65097e+06, 312.269, 1.02771e+08, 7.65171e+08, 8.79845e+06}, // figOfMerit=1.75023
//       {38607.8, 8.86165e+06, 1.11483e+06, 12522.3, 4.4771e+07, 88213.8, 10117.3, 953.334, 868.76, 2.20512e+08, 2.07888e+08, 4.41401e+06} // figOfMerit=1.60874
//      };

   static const double params[2][13] =
     {{1.0/(6.53342e-08),1.0/(1.71543e-06),1,1,1,1,1.0/(2.86953e-05),1.0/(7.28967e-07),1.0/(0.00827712),1.0/(0.0100453),1.0/(6.60085e-07),1.0/(8.98321e-08),1},
      {1.0/(6.53342e-08),1.0/(1.71543e-06),1,1,1,1,1.0/(2.86953e-05),1.0/(7.28967e-07),1.0/(0.00827712),1.0/(0.0100453),1.0/(6.60085e-07),1.0/(8.98321e-08),1}
     };


   // Get the probabilities for the EPD. indexWH is set to zero and is irrelevant here.
   ProbsForEPD probs = getProbsForEPD(eventProb, params[tagIndex], 0, bProb, ntag);
   //cout << "testing" << endl;
   //probs.Show("TEST_TITLE");

   // There is less than three events per MC sample in which all the probabilities are zero. 
   // To avoid returning a NaN just return zero for those
   if (probs.wz + probs.ww == 0) return 0;

   // Return the wh probability
   return (probs.wz+probs.ww) / (probs.wh + probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
		      probs.wcc + probs.wc + probs.wjg + probs.zjg + probs.qcd + probs.tt +
		      probs.ww + probs.wz);    
   

//    return (probs.wz+probs.ww) / (probs.wh + probs.schan + probs.tchan + probs.tchan2 + probs.wbb + 
// 		      probs.wcc + probs.wc + probs.wjg + probs.zjg +
// 		      probs.ww + probs.wz);  
   
      
}//calcWZEPD 



//------------------------------------------------------------------------------
Double_t MicroNtuple::eventDump(const DumbClassToTrickRoot* h, double epd,
                                int ALPtype, double lepPt)
{
   std::cout << "Run " << h->run << " Event " << h->event << " " << h->ntag
             << " tags  Alpgentype " << ALPtype << " Detector " << h->det 
             << " LeptonPt: " << lepPt << " EPD: " << epd << std::endl;

   return h->run + h->event;
}

Bool_t MicroNtuple::cutEvents() const
{
   const unsigned size = 23;

   const int run[size] = {153694,
                          163064,
                          165873,
                          166615,
                          177633,
                          179039,
                          184068,
                          185249,
                          185973,
                          186047,
                          194845,
                          196901,
                          204339,
                          206259,
                          224366,
                          222652,
                          229448,
                          231929,
                          237795,
                          236781,
                          236806,
                          240801,
                          241051};

   const int event[size] = {557614  ,
                            1354337 ,
                            5882355 ,
                            5183226 ,
                            66008   ,
                            3181694 ,
                            3379    ,
                            7452122 ,
                            541781  ,
                            472073  ,
                            3548470 ,
                            1751044 ,
                            43109   ,
                            1045183 ,
                            15684981,
                            3739969 ,
                            12152032,
                            8694788 ,
                            2400487 ,
                            3939037 ,
                            19980866,
                            4180675 ,
                            160664};

   if (std::find(run, run + size, h.run) != run + size
       && std::find(event, event + size, h.event) != event + size)
      return false;

   return true;
}

Double_t MicroNtuple::triggerTO(double detector, double etalep, double metraw,
                                double etlep)
{
   if (detector != TopLepType::TPHX)
      return 1;

   using std::abs;

   static TF1 turnOnMETL3("turnOnMETL3", "1/(1.+TMath::Exp(-[2]*(x-[1])))", 0, 
                         500);
  
   static TF1 turnOnPEML2("turnOnPEML2", "1/(1.+TMath::Exp(-[2]*(x-[1])))", 0, 
                         500);
    
   static TF1 turnOnPEML3("turnOnPEML3", "1/(1.+TMath::Exp(-[2]*(x-[1])))", 0, 
                         500);

   double weight = 0.;

   const int Ntrigger = 3; //L1_MET15_L3, L2_PEM20, L3_PEM20
  
   const int Nperiods = 2; //0d+0h and 0i..mj
   const double periodweight[Nperiods] = {.316, .684};

   double alpha[Nperiods][Ntrigger];
   double beta[Nperiods][Ntrigger];
  
   //L1_MET15_L3

   alpha[0][0] = 14.79;
   beta[0][0] = 0.37;

   alpha[1][0] = 14.00;
   beta[1][0] = 0.37;

   //L3_PEM20

   alpha[0][2] = 19.44;
   beta[0][2] = 1.99;

   alpha[1][2] = 20.91;
   beta[1][2] = 1.66;

   //L2_PEM20, eta dependend

   //0h
   if (abs(etalep) < 1.52)
   {
      alpha[0][1] = 20.15;
      beta[0][1] = 0.4772;
   }
   if (abs(etalep) >= 1.52 && abs(etalep) < 2.11)
   {
      alpha[0][1] = 20.85;
      beta[0][1] = 0.4237;
   }
   if (abs(etalep) >= 2.11 && abs(etalep) < 2.61)
   {
      alpha[0][1] = 22.96;
      beta[0][1] = 0.415;
   }
   if (abs(etalep) >= 2.61)
   {
      alpha[0][1] = 29.09;
      beta[0][1] = 0.2842;
   }

   //0i
   if (abs(etalep) < 1.52)
   {
      alpha[1][1] = 22.03;
      beta[1][1] = 0.5684;
   }
   if (abs(etalep) >= 1.52 && abs(etalep) < 2.11)
   {
      alpha[1][1] = 21.64;
      beta[1][1] = 0.4601;
   }
   if (abs(etalep) >= 2.11 && abs(etalep) < 2.61)
   {
      alpha[1][1] = 23.57;
      beta[1][1] = 0.4407;
   }
   if (abs(etalep) >= 2.61)
   {
      alpha[1][1] = 29.61;
      beta[1][1] = 0.5717;
   }

   //averaging over periods
   for(int i=0; i< Nperiods; i++)
   {
      turnOnMETL3.SetParameter(1,alpha[i][0]);
      turnOnMETL3.SetParameter(2,beta[i][0]);
                   
      turnOnPEML2.SetParameter(1,alpha[i][1]);
      turnOnPEML2.SetParameter(2,beta[i][1]);
                   
      turnOnPEML3.SetParameter(1,alpha[i][2]);
      turnOnPEML3.SetParameter(2,beta[i][2]);

      weight += periodweight[i] * turnOnMETL3.Eval(metraw)
         * turnOnPEML2.Eval(etlep) * turnOnPEML3.Eval(etlep);
   }

   return weight;
}


DumbClassToTrickRoot::DumbClassToTrickRoot()
{
   clear();
}

void DumbClassToTrickRoot::clear()
{
   tagProb1 = 0;
   tagProb2 = 0;
   det = 0;   
   ntag = 0;
   run = 0;
   event = 0;
}

ClassImp(MicroNtuple)
   ClassImp(DumbClassToTrickRoot)
