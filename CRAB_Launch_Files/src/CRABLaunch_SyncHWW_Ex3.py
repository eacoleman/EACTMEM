###Runs with CMSSW_4_1_3_patch2
import FWCore.ParameterSet.Config as cms

## Define the process
#process = cms.Process("Test")

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

#################################################################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

# output file name
process.out.fileName = 'HWW_SyncExMC.root'



## #### Additional Modifications ####
process.GlobalTag.globaltag = cms.string('START311_V2::All')

#### Primary Vertex Filter ####
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24), 
                                           maxd0 = cms.double(2.0) 
                                           )

# HB + HE noise filtering
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')

# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.25)
                                    )


## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon.
process.patElectrons.usePV = cms.bool(False)
process.patMuons.usePV = cms.bool(False)

## ## Implement Simple Cut Based ID for electrons
## process.patElectrons.addElectronID = cms.bool(True)
## process.patElectrons.electronIDSources = cms.PSet(
##     eidRobustLoose= cms.InputTag("eidRobustLoose"),
##     eidRobustTight= cms.InputTag("eidRobustTight"),
##     simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
##     simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
##     simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
##     simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
##     simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
##     simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
##     simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
##     simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
##     simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
##     simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
##     simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
##     simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),    
##     )

## process.load("TAMUWW.CRAB_Launch_Files.simpleEleIdSequence_cff")
## process.patElectronIDs = cms.Sequence(process.simpleEleIdSequence)
## process.makePatElectrons = cms.Sequence(process.patElectronIDs*process.patElectrons)

#from PhysicsTools.PatAlgos.tools.pfTools import *
## from PhysicsTools.PatAlgos.tools.jetTools import *
## addJetCollection(process,cms.InputTag('ak5PFJets'),
##                  'AK5', 'PF',
##                  doJTA        = True,
##                  doBTagging   = True,
##                  jetCorrLabel = ('AK5', 'PF'),
##                  doType1MET   = False, ## there is no use to apply residual jet corrections on CaloMET 
##                  doL1Cleaning = True,                 
##                  doL1Counters = False,
##                  genJetCollection=cms.InputTag("ak5GenJets"),
##                  doJetID      = False  ## there is no use of jetID for Pflow jets
##                  )

#from PhysicsTools.PatAlgos.tools.pfTools import *

#process.load("PhysicsTools.PatAlgos.patSequences_cff")
## from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection
## switchJetCollection( process,
##                      jetCollection=cms.InputTag('ak5CaloJets'),
##                      jetCorrLabel=('AK5', 'Calo'))

removeMCMatching(process, ['All'])

from PhysicsTools.PatAlgos.tools.jetTools import *

process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## addJetCollection(process,cms.InputTag('ak5PFJets'),
##                  'AK5', 'PF',
##                  doJTA        = True,
##                  doBTagging   = True,
##                  jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
##                  doType1MET   = True,
##                  doL1Cleaning = True,
##                  doL1Counters = True,
##                  genJetCollection=cms.InputTag("ak5GenJets"),
##                  doJetID      = True
##                 )

addJetCollection(process,cms.InputTag('ak5PFJets'),
                 'AK5', 'PF',
##                 doJTA        = True,
                 doBTagging   = True,
                 jetCorrLabel = ('AK5PF',cms.vstring(['L2Relative', 'L3Absolute'])),
##                 doType1MET   = True,
##                 doL1Cleaning = False,
##                 doL1Counters = False,                 
                 genJetCollection = cms.InputTag("ak5GenJets")
##                 doJetID      = True
                 )


## # 1. re-cluster ak5 pfjets and pass them to pat (only for pre-36x input):
## ###process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## addJetCollection(process, cms.InputTag('ak5PFJets::Test'), 'AK5', 'PF', jetCorrLabel=('AK5','PF'), doType1MET=False, doJetID = False)
# make sure to run process.ak5PFJets before PAT, for example:
process.patDefaultSequence = cms.Sequence(process.ak5PFJets * process.patDefaultSequence)

## # 2. add jet plus tracks (only for pre-36x input):
## process.load("RecoJets.Configuration.RecoJPTJets_cff")
## addJetCollection(process, cms.InputTag('JetPlusTrackZSPCorJetAntiKt5'), 'AK5', 'JPT', jetCorrLabel = ('AK5JPT', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])), doType1MET=True )
## #run process.jetPlusTrackZSPCorJetAntiKt5 somewhere before PAT, e.g.,
## process.patDefaultSequence = cms.Sequence(process.jetPlusTrackZSPCorJetAntiKt5 * process.patDefaultSequence)


# 3. add tcMet and pfmet tp PAT:
from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, 'PF')
#addTcMET(process, 'TC')




#### Run the pat sequences ##
process.p1 = cms.Path(
 process.primaryVertexFilter*
 process.scrapingVeto*
 process.HBHENoiseFilter*
 process.patDefaultSequence
)

## process.p2 = cms.Path(
##   process.patDefaultSequence
##   *process.ElectronsKinCutBasic
##   *process.ElHLTFilter  
## )

#### Define and cleanup event content ####
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += ['keep *_selectedPat*_*_*','drop *_cleanPat*_*_*']
process.out.outputCommands += patExtraAodEventContent
process.out.outputCommands += ["drop *_towerMaker_*_*"]
process.out.outputCommands += ["drop l1extraParticles_*_*_*"]
process.out.outputCommands += ["drop *_selectedPatTaus*_*_*"]
process.out.outputCommands += ["drop *_selectedPatPhotons*_*_*"]
process.out.outputCommands += ["drop *_offlineBeamSpot_*_*"]
process.out.outputCommands += ["drop edmTriggerResults_TriggerResults*_*_*"]
process.out.outputCommands += ["drop *_hltTriggerSummaryAOD_*_*"]
##process.out.outputCommands += ["drop recoTracks_generalTracks*_*_*"]
process.out.outputCommands += ["drop recoGenParticles_genParticles*_*_*"]
process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]


process.out.outputCommands += [
#    "drop *_selectedPatElectrons_*_*",
#    "drop *_selectedPatMuons_*_*",
#    "drop *_selectedPatPFParticlesPFlow*_*_*",
#    "drop CaloTowers_*_*_*",
    #                               "drop *_selectedPatJetsAK5JPT*_*_*",
#                               "drop *_selectedPatJetsAK5PF_*_*",
#    "drop *_selectedPatJetsAK5PFPFlow_*_*",
#    "drop recoBaseTagInfosOwned_selectedPatJetsAK5PF_*_*",
#    "drop recoBaseTagInfosOwned_selectedPatJetsPFlow_*_*",
#    "drop patPFParticles_selectedPatPFParticlesPFlow_*_*",
    "drop *_selectedPatJets_*_*",
    #                               "drop *_patMETsPFPFlow_*_*",
    "drop *_patMETs_*_*"
]

#removeAllPATObjectsBut(process, ['Electrons','Muons','Jets','METs'])

## process.allLayer1Muons.isoDeposits = cms.PSet()
## process.allLayer1Electrons.isoDeposits = cms.PSet()
process.patMuons.isoDeposits = cms.PSet()
process.patElectrons.isoDeposits = cms.PSet()


## process.cleanPatMuons.cut = cms.string('pt> 10. & abs(eta) < 2.5')
## process.cleanPatElectrons.cut = cms.string('pt> 10. & abs(eta) < 2.5')
## process.cleanPatJets.cut = cms.string('pt> 10.')




##
# configure input/output
##
## reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)

###THIS IS NORMALLY OVERWRITTEN FROM crab.cfg
#number of events
process.maxEvents.input = 100

# source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0014/84872812-1F4B-DF11-8A07-00151796C158.root',
#    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FEBDE500-4046-DF11-87C6-00151796D9A8.root'
#    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F8769D5D-4146-DF11-850D-0024E8768099.root',
#    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F69DC71E-4046-DF11-841A-00151796D88C.root',
#    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F614C63F-4046-DF11-BA9B-0024E8769B87.root'
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FEFE3E46-CB7B-DF11-8488-0018F3D09678.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FEA6BEE7-CA7B-DF11-BEB0-00304867BED8.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FCE7711D-CB7B-DF11-B469-003048678C26.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FAA0922C-6C7B-DF11-82E6-00304867BFC6.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FA24AFE0-EE7B-DF11-BDB3-001A92971ACE.root'

##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0016/6E7C4631-9D47-DF11-96CE-003048C69288.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0011/A4121AB4-0747-DF11-8984-0030487F171B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE8DE204-C446-DF11-BF76-003048C693FA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE14F78B-C446-DF11-818D-003048C6930E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE0DD96C-B846-DF11-B5AE-0030487EB003.root'

       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0026/FE40206F-2453-E011-A011-003048F0E1EE.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0026/D62114D9-F952-E011-A486-003048F0E3AE.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0026/6A4D222D-EC52-E011-8021-003048F0E5A2.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0026/1C119FC9-EE52-E011-86CB-0030487F938B.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/F4AE4A60-E652-E011-B495-0030487F92FF.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/C40F587B-E752-E011-A002-0030487F9351.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/BA617B42-E752-E011-8F4B-0030487F9307.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/B80E704E-E652-E011-A6FC-0030487FA629.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/90AC4971-E752-E011-881A-0030487F932D.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/78C05C44-E752-E011-95E5-0030487F9307.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/50E94B31-E752-E011-8FD3-0030487F91DB.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/50CEAD5E-E652-E011-8BC5-0030487F92FF.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/30551D41-E752-E011-AA16-0030487F929B.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/2C6C1D71-E552-E011-B975-002481E945C4.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/1E1D17C5-E552-E011-880C-003048D4DFA8.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/083A7E32-E752-E011-9C7B-0030487F91DB.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0025/069C7D32-E752-E011-B3BD-0030487F91DB.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0024/EAA84FDA-C752-E011-9119-0030487E52A5.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0024/64F298DB-DC52-E011-B579-0030487F9367.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0024/5CF7BEDB-DC52-E011-8612-0030487F933D.root',
       '/store/mc/Spring11/GluGluToHToWWToLNuQQ_M-170_7TeV-powheg-pythia6/AODSIM/PU_S1_START311_V1G1-v1/0024/527222CB-DC52-E011-A990-0030487FA625.root'

   )
)

#process.p = cms.Path( process.patDefaultSequence)
