#ifndef TOPLEPTYPE_HH
#define TOPLEPTYPE_HH

namespace TopLepType {

  const static int NONE       = 0;
  //Traditionally Tight (Trigger) Isolated Leptons
  const static int TCEM       = 1;
  const static int TPHX       = 2;
  const static int TCMUP      = 3;
  const static int TCMX       = 4;
  const static int TBMU       = 5;
  //Traditionally Tight (Trigger) Non-Isolated Leptons
  const static int TCEMNI     = 6;
  const static int TPHXNI     = 7;
  const static int TCMUPNI    = 8;
  const static int TCMXNI     = 9;
  const static int TBMUNI     = 10;
  //Traditionally Loose (Non-Trigger) Leptons
  const static int TCMU       = 11;
  const static int TCMUNI     = 12;
  const static int TCMP       = 13;
  const static int TCMPNI     = 14;
  const static int TCMXNT     = 15;   //Non-trigger CMX
  const static int TCMIO      = 16;    //Stubless muons
  const static int TSCMIO     = 17;   //"Stubbed" stubless muons
  //Other types
  const static int TCEMCONV   = 18;
  const static int TCEMCONVNI = 19;
 
  //Total number of types (Keep one higher than highest type number)
  const static int NTYPES     = 20;

  //General Lepton Categories
  const static int TIGHT = 1;  //Isolated trigger leptons
  const static int NONISO = 2;  //Non-Isolated trigger leptons
  const static int LOOSE = 3;  //Non-trigger Leptons

}




#endif

