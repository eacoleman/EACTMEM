#include "TAMUWW/MatrixElement/interface/MEConstants.hh"

#include <cmath>
#include <iostream>


namespace MEConstants
{
  using std::cout;
  using std::endl;

  double GetAdjusted_sgg(double alphas) {
    return std::sqrt(4 * TMath::Pi() * alphas);
  }
  double GetAdjusted_gg(double alphas) {
    return -std::sqrt(4 * TMath::Pi() * alphas);
  }

  //const double gh    = 0.00005;



  void PrintAllConstants(double TheAlphaS) {
    cout << "PRINTING VALUES OF THE CONSTANTS TO BE USED IN MADGRAPH" << endl;
    cout << " double precision " << "beamEnergy,bMass,cMass,wMass,wWidth,zMass,zWidth" << endl;
    cout << " double precision " << "tMass,sin2w,alpha,ee2,ee,sw,cw,ez,ey" << endl;
    cout << " double precision " << "alphas,temp_gwf,temp_gg, temp_sgg,gzu1,gzu2" << endl;
    cout << " double precision " << "gzd1,gzd2,gzl1,gzl2,temp_gwwz" << endl;
    cout << " double precision " << "v,temp_gwwh,temp_ghbot,temp_gh" << endl; 
    cout << " double complex gwf(2),gg(2),sgg(2),gzu(2),gzd(2),gzl(2)" << endl;
    cout << " double complex gwwz(2),gwwh(2),ghbot(2),gh(2)" << endl;
    cout << endl;
    cout << "beamEnergy = " << beamEnergy << endl;
    cout << "bMass = " << bMass << endl;
    cout << "cMass = " << cMass << endl;
    cout << "wMass = " << wMass << endl;
    cout << "wWidth = " << wWidth << endl;
    cout << "zMass = " << zMass << endl;
    cout << "zWidth = " << zWidth << endl;
    //    cout << "tMass = " << topMass << endl;<<<-May Need To Be Revisited
    cout << "sin2w = " << sin2w << endl;
    //    cout << "gFermi = " << gFermi << endl;
//     cout << "alpha = " << alpha << endl;
//     cout << "ee2 = " << ee2 << endl;
//     cout << "ee = " << ee << endl;
//     cout << "sw = " << sw << endl;
//     cout << "cw = " << cw << endl;
//     cout << "ez = " << ez << endl;
//     cout << "ey = " << ey << endl;
    cout << "gzu1 = " << gzu1 << endl;
    cout << "gzu2 = " << gzu2 << endl;
    cout << "gzd1 = " << gzd1 << endl;
    cout << "gzd2 = " << gzd2 << endl;
    cout << "gzl1 = " << gzl1 << endl;
    cout << "gzl2 = " << gzl2 << endl;
    cout << "v = " << v << endl;
    cout << "temp_gwwh = " << gwwh << endl;
    cout << "temp_ghbot = " << ghbot << endl;
    //    cout << "temp_gh = " << gh << endl;

    cout << "alphas = " << TheAlphaS << endl;
    cout << "temp_gwf = " << gwf << endl;
    cout << "temp_gwwz = " << gwwz << endl;
    cout << "temp_gg = " << GetAdjusted_gg(TheAlphaS) << endl;
    cout << "temp_sgg = " << GetAdjusted_sgg(TheAlphaS) << endl;
    cout << endl;
    //    cout << "double complex gwf(2),gg(2),sgg(2),gwwz(2),gzu(2),gzd(2),gzl(2)" << endl;
    cout << "gwf(1) = dcmplx(temp_gwf, Zero)" << endl;
    cout << "gwf(2) = dcmplx(Zero, Zero)" << endl;
    cout << "gwwz(1) = dcmplx(temp_gwwz, Zero)" << endl;
    cout << "gwwz(2) = dcmplx(Zero, Zero)" << endl;
    cout << "gg(1) = dcmplx(temp_gg, Zero)" << endl;
    cout << "gg(2) = dcmplx(temp_gg, Zero)" << endl;
    cout << "sgg(1) = dcmplx(temp_sgg, Zero)" << endl;
    cout << "sgg(2) = dcmplx(Zero, Zero)" << endl;
    cout << "gzu(1) = dcmplx(gzu1, Zero)" << endl;
    cout << "gzu(2) = dcmplx(gzu2, Zero)" << endl;
    cout << "gzd(1) = dcmplx(gzd1, Zero)" << endl;
    cout << "gzd(2) = dcmplx(gzd2, Zero)" << endl;
    cout << "gzl(1) = dcmplx(gzl1, Zero)" << endl;
    cout << "gzl(2) = dcmplx(gzl2, Zero)" << endl;
    cout << "gwwh(1) = dcmplx(temp_gwwh, Zero)" << endl;
    cout << "gwwh(2) = dcmplx(Zero, Zero)" << endl;
    cout << "ghbot(1) = dcmplx(temp_ghbot, Zero)" << endl;
    cout << "ghbot(2) = dcmplx(temp_ghbot, Zero)" << endl;
    cout << "gh(1) = dcmplx(temp_gh, Zero)" << endl;
    cout << "gh(2) = dcmplx(Zero, Zero)" << endl;
  }



}
