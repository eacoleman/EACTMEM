#include "tdrstyle_mod.C"

void rootlogon()
{
   gSystem->SetIncludePath(" -I../include/");
   gSystem->Load("libPhysics");
   gROOT->SetStyle("Plain");
   
   gSystem->Load("../shlib/libHighLevelObjects.so");
   gSystem->Load("../shlib/libSingleTopNtuple.so");
   gSystem->Load("../shlib/libIntegrator.so");
   gSystem->Load("../shlib/libPeterTools.so");
   gSystem->Load("../shlib/libMethodIITools.so");
   gSystem->Load("../shlib/libPlots.so");
   gSystem->Load("../shlib/libJointPhysics.so");

}
