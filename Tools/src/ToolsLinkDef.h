#include "TAMUWW/Tools/interface/Style.hh"
#include "TAMUWW/Tools/interface/mymath.hh"
#include "TAMUWW/Tools/interface/Plots.hh"
#include "TAMUWW/Tools/interface/PlotMap.hh"
#include "TAMUWW/Tools/interface/PlotFiller.hh"
#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"
#include "TAMUWW/Tools/interface/CutOptimization.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/SpecialTools/interface/MVAVar.hh"
#include "TAMUWW/Tools/interface/CreateCondorScriptME.hh"
#include "TAMUWW/Tools/interface/MicroNtupleMaker.hh"
#include "TAMUWW/Tools/interface/BackgroundEstimator.hh"
#include "TAMUWW/Tools/interface/Fitter.hh"

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all nestedclasses;

#pragma link C++ class Style+;
#pragma link C++ namespace mymath;
#pragma link C++ function mymath::deltaPhi;
#pragma link C++ function mymath::deltaR;
#pragma link C++ class DefaultValues+;
#pragma link C++ class MVAVar+;
#pragma link C++ class TProfileMDF+;
#pragma link C++ class PhysicsProcess+;
#pragma link C++ class Plot+;
#pragma link C++ class FormattedPlot+;
#pragma link C++ class PlotMap+;
#pragma link C++ class FormattedPlotMap+;
#pragma link C++ class PlotFiller+;
#pragma link C++ class CutOptimization+;
#pragma link C++ class CreateCondorScriptME+;
#pragma link C++ class MicroNtupleMaker+;
#pragma link C++ class BackgroundEstimator+;
#pragma link C++ class Fitter+;

#endif
