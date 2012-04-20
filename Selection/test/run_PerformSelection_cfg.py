import FWCore.ParameterSet.Config as cms

#!
#! DEFAULT PARAMETERS (CHANGE WHEN NEEDED)
#!
#import TAMUWW.Selection.Defaults_cff as Defaults;


#!
#! PROCESS
#!
process = cms.Process("PerformSelectionProcess")


#!
#! SERVICES
#!
#process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
process.load('CommonTools.UtilAlgos.TFileService_cfi')
process.TFileService.fileName=cms.string('PS.root')


#!
#! INPUT
#!
inputFiles = cms.untracked.vstring(
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_10_1_jEn.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_11_1_YeB.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_12_1_sK4.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_13_1_rwX.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_14_1_qdE.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_15_1_GX4.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_16_1_6Cw.root'
#	'file:/gluster/hepx/store/user/aperloff/lnjj/pat_42x_fall11_10_1_jEn.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = inputFiles )


#!
#! PARAMETERS
#!
process.PS = cms.EDAnalyzer('PerformSelection')

    #-----Input Tags For Handles
    #
    # From PF2PAT, standard postfix=PFlow
    #
process.PS.vtxSource         = cms.InputTag('offlinePrimaryVertices')               # InputTag for vertex collection
process.PS.genParticleSource = cms.InputTag('genParticles')                         # InputTag for genParticle collection
process.PS.pfJetSource       = cms.InputTag('selectedPatJetsPFlow')                 # InputTag for jet collection
process.PS.electronSource    = cms.InputTag('selectedPatElectronsPFlow')            # InputTag for electron collection
process.PS.muonSource        = cms.InputTag('selectedPatMuonsPFlow')                # InputTag for muon collection
process.PS.METSource         = cms.InputTag('patMETsPFlow')                         # InputTag for MET collection
process.PS.rhoSource         = cms.InputTag("kt6PFJetsChsForIsolationPFlow", "rho") # InputTag for event rho source
process.PS.triggerSource     = cms.InputTag('patTriggerEvent')                      # InputTag for trigger collection

    #-----Trigger Information
process.PS.muTrigger         = cms.string('HLT_IsoMu24_v*')                                # Muon trigger name
process.PS.eleTrigger        = cms.string('HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v2') # Electron trigger name 

    #-----Program Level Inputs
process.PS.outtablefilename  = cms.string("outputTable.txt") # name of the file containing the output/efficiency tables
process.PS.outtablenameEl    = cms.string("ElectronTable")   # name of the event selection table for electrons
process.PS.outtablenameMu    = cms.string("MuonTable")       # name of the event selection table for muons
process.PS.outtablenameLp    = cms.string("LeptonTable")     # name of the event selection table for leptons
process.PS.runtype           = cms.string("crab")            # tells the program if it will be run through cmsRun or CRAB
                                                             # - crab - the event table will be saved in the root file only
							     # - cmsRun - the event table will be printed to a text file as well
process.PS.cutOnTrigger      = cms.bool(False)               # if true, only event that pass the trigger requirements will be saved
process.PS.SQWaT_Version     = cms.int32(4)                  # version number used to create the common PATtuples
process.PS.doRelIso          = cms.bool(True)                # if true, do rho corrected isolation, else do tracker only isolation

process.PS.Data              = cms.bool(False)               # is the dataset from real data or Monte Carlo
process.PS.noMETCut          = cms.bool(False)               # disregard the MET cut when storing the events
process.PS.invertEID         = cms.bool(False)               # electrons which *fail* at least two of the EID requirements will be kept instead
process.PS.PFlowLoose        = cms.bool(False)               # use the collections with the PF2PAT postfix=PFlowLoose (instead of PFlow)
process.PS.elONLY            = cms.bool(False)               # only save the output if the lepton is an electron
process.PS.muONLY            = cms.bool(False)               # only save the output if the lepton is a muon


#!
#! PATH
#!
process.p = cms.Path(
  process.PS
)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
