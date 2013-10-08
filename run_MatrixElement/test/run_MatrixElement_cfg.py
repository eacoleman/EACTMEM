import FWCore.ParameterSet.Config as cms
import os

#!
#! DEFAULT PARAMETERS (CHANGE WHEN NEEDED)
#!
#import TAMUWW.run_MatrixElement.Defaults_cff as Defaults;


#!
#! PROCESS
#!
process = cms.Process("MatrixElementProcess")


#!
#! SERVICES
#!
#process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
process.load('CommonTools.UtilAlgos.TFileService_cfi')
process.TFileService.fileName=cms.string('ME.root')


#!
#! INPUT
#!
inputFiles = cms.untracked.vstring(
##################
# WZ Monte Carlo #
##################

#####################
# WJets Monte Carlo #
#####################

######################
# ggH125 Monte Carlo #
######################

#####################
# WH120 Monte Carlo #
#####################

#############
# Muon Data #
#############

#################
# Electron Data #
#################

    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = inputFiles )

################### Take inputs from crab.cfg file ##############
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('python')
options.register ('inputFilename',
                  'unknown.root',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "Input Filename: <physics process>.root, unknown.root (default)")
options.register ('totalNumberOfEvents',
                  -1,
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.int,
                  "Total Number Of Events: -1 (default)")
options.register ('eventsPerJob',
                  150,
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.int,
                  "Events Per Job: 150 (default)")
options.parseArguments()

#!
#! PARAMETERS
#!
process.ME = cms.EDAnalyzer('MatrixElement')

    #-----Program Level Inputs
process.ME.inputFilename      = cms.string(options.inputFilename)
process.ME.outputFilename     = process.TFileService.fileName
process.ME.inputDirectoryName = cms.string("PS")     # name of the directory where the input tree is stored
process.ME.inputTreeName      = cms.string("jets2p") # name of the tree where the events are stored
process.ME.nEvents            = cms.int32(options.totalNumberOfEvents)
os.environ['NJob'] = -1;
process.ME.jobID              = cms.int32(int(os.environ['NJob']))
process.ME.nSkip              = cms.int32(int(os.environ['NJob'])*options.eventsPerJob)
process.ME.nPrescale          = cms.int32(1)         # prescale events by...

#!
#! PATH
#!
process.p = cms.Path(process.ME)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
