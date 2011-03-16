#ifndef MECONSTANTS_HH
#define MECONSTANTS_HH

#include <cmath>
#include <iostream>

#include "TMath.h"

#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

namespace MEConstants
{
  using std::cout;
  using std::endl;

   // Beam constants
   const double beamEnergy = 3500;

   // Masses and widths
   const double bMass = 4.7;
   const double cMass = 1.40;
   const double wMass = 80.419;
   const double wWidth = 2.04759;
   const double zMass = 91.118;
   const double zWidth = 2.441;
   const double topMass = 172.5;

   // Charged Electroweak couplings
   const double sin2w = 0.2312;
   const double gFermi = 1.16639e-5;
   const double alpha = std::sqrt(2) * gFermi * AuxFunctions::Math::square(wMass) * sin2w / TMath::Pi();
   const double ee2 = alpha * 4 * TMath::Pi();
   const double ee = std::sqrt(ee2);
   const double gwf = -ee / std::sqrt(2 * sin2w);
   

   //// Strong couplings
   
   // default values
   const double alphas = .119;
   const double sgg = std::sqrt(4 * TMath::Pi() * alphas);//same as g in the FORTRAN functions;
   const double gg = -sgg;
   // Since alphas is process dependent, we can take the value from MadGraph and reevaluate the relevant constants.
   double GetAdjusted_sgg(double alpha);
   double GetAdjusted_gg(double alpha);


   // Neutral electroweak couplings
   const double sw = std::sqrt(sin2w);
   const double cw = std::sqrt(1 - sin2w);
   const double ez = ee / (sw * cw);
   const double ey = ee * sw / cw;
   const double gzu1 = -ez * (.5 - 2 * sin2w / 3);
   const double gzu2 = 2 * ey / 3;
   const double gzd1 = -ez * (-.5 + sin2w / 3);
   const double gzd2 = -ey / 3;
   const double gzl1 = -ez*(-0.5 + sin2w);
   const double gzl2 = -ey;
   const double gwwz = ee * cw / sw;

  


   // Adding more constants for the Higgs diagrams   
   const double v = 2 * wMass * sw / ee2;
   const double gwwh = ee2 / sin2w * .5 * v;
   const double ghbot = -bMass / v;

   // const double gh   = 1.0; // g*g*series_t/(4.0 * TMath::Pi() * 3 * TMath::Pi() * V 
   const double gh    = 0.00005;




  void PrintAllConstants(double TheAlphaS);

}


#endif
