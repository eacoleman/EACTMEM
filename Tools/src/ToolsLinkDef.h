#include "TAMUWW/Tools/interface/Plots.hh"
#include "TAMUWW/Tools/interface/PlotMap.hh"
#include "TAMUWW/Tools/interface/PlotFiller.hh"
#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"
#include "TAMUWW/Tools/interface/CutOptimization.hh"
#include "TAMUWW/SpecialTools/interface/DefaultValues.hh"
#include "TAMUWW/Tools/interface/CreateCondorScriptME.hh"

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all nestedclasses;

#pragma link C++ class DefaultValues+;
#pragma link C++ class TProfileMDF+;
#pragma link C++ class PhysicsProcess+;
#pragma link C++ class Plot+;
#pragma link C++ class FormattedPlot+;
#pragma link C++ class PlotMap+;
#pragma link C++ class FormattedPlotMap+;
#pragma link C++ class PlotFiller+;
#pragma link C++ class CutOptimization+;
#pragma link C++ class CreateCondorScriptME+;

#endif
