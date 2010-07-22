#include "TAMUWW/MatrixElement/interface/MEConstants.hh"

#include <cmath>
#include <iostream>


namespace MEConstants
{
  using std::cout;
  using std::endl;


  void PrintAllConstants() {
     cout << "PRINTING VALUES OF THE CONSTANTS TO BE USED IN MADGRAPH" << endl;
     cout << "double precision " << "beamEnergy,bMass,cMass,wMass,wWidth,zMass,topMass,sin2w,gFermi,alpha,ee2,ee,tgwf,tgg,alphas,tsgg,sw,cw,ez,ey,gzu1,gzu2,gzd1,gzd2,gwwz,v,gwwh,ghbot,gh" << endl;
    
    cout << "beamEnergy = " << beamEnergy << endl;
    cout << "bMass = " << bMass << endl;
    cout << "cMass = " << cMass << endl;
    cout << "wMass = " << wMass << endl;
    cout << "wWidth = " << wWidth << endl;
    cout << "zMass = " << zMass << endl;
    cout << "topMass = " << topMass << endl;
    cout << "sin2w = " << sin2w << endl;
    cout << "gFermi = " << gFermi << endl;
    cout << "alpha = " << alpha << endl;
    cout << "ee2 = " << ee2 << endl;
    cout << "ee = " << ee << endl;
    cout << "tgwf = " << gwf << endl;
    cout << "tgg = " << gg << endl;
    cout << "alphas = " << alphas << endl;
    cout << "tsgg = " << sgg << endl;
    cout << "sw = " << sw << endl;
    cout << "cw = " << cw << endl;
    cout << "ez = " << ez << endl;
    cout << "ey = " << ey << endl;
    cout << "gzu1 = " << gzu1 << endl;
    cout << "gzu2 = " << gzu2 << endl;
    cout << "gzd1 = " << gzd1 << endl;
    cout << "gzd2 = " << gzd2 << endl;
    cout << "gwwz = " << gwwz << endl;
    cout << "v = " << v << endl;
    cout << "gwwh = " << gwwh << endl;
    cout << "ghbot = " << ghbot << endl;
    cout << "gh = " << gh << endl;

  }

}
