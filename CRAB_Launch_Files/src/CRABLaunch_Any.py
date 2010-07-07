###Runs with 3.5.6
###~7.5 K per event

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
## uncomment this line to run on an 35X input sample in 36X
## run36xOn35xInput(process)

#################################################################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

# output file name
process.out.fileName = 'WpJ_TestRun.root'

#### Lepton Candidate Filters ####
#Kinematic Filters
process.MuonsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patMuons"),                              
  cut = cms.string('pt > 15. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)
process.ElectronsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patElectrons"),                              
  cut = cms.string('pt > 15. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)

#### HLT filters ####
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
# Uncomment this to filter on 1E31 HLT menu
#process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]

process.ElHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]
# Uncomment this to filter on 1E31 HLT menu
#process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]

#### Additional Modifications ####
#process.GlobalTag.globaltag = cms.string('START36_V9::All')

## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon. 
#process.patElectrons.usePV = cms.bool(False)
process.patMuons.usePV = cms.bool(False)

## Implement Simple Cut Based ID for electrons
process.patElectrons.addElectronID = cms.bool(True)
process.patElectrons.electronIDSources = cms.PSet(
    eidRobustLoose= cms.InputTag("eidRobustLoose"),
    eidRobustTight= cms.InputTag("eidRobustTight"),
    simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
    simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
    simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
    simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
    simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
    simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
    simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
    simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
    simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
    simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
    simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
    simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),    
    )

process.load("TAMUWW.CRAB_Launch_Files.simpleEleIdSequence_cff")
process.patElectronIDs = cms.Sequence(process.simpleEleIdSequence)
process.makePatElectrons = cms.Sequence(process.patElectronIDs*process.patElectrons)


#### Run the pat sequences ##
process.p1 = cms.Path(
  process.patDefaultSequence
  *process.MuonsKinCutBasic
  *process.MuHLTFilter  
)
process.p2 = cms.Path(
  process.patDefaultSequence
  *process.ElectronsKinCutBasic
  *process.ElHLTFilter  
)


#### Define and cleanup event content ####
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += patExtraAodEventContent
process.out.outputCommands += ["drop *_towerMaker_*_*"]
process.out.outputCommands += ["drop l1extraParticles_*_*_*"]
process.out.outputCommands += ["drop *_cleanPatTaus*_*_*"]
process.out.outputCommands += ["drop *_cleanPatPhotons*_*_*"]
process.out.outputCommands += ["drop *_offlineBeamSpot_*_*"]
process.out.outputCommands += ["drop edmTriggerResults_TriggerResults*_*_*"]
process.out.outputCommands += ["drop *_hltTriggerSummaryAOD_*_*"]
##process.out.outputCommands += ["drop recoTracks_generalTracks*_*_*"]
process.out.outputCommands += ["drop recoGenParticles_genParticles*_*_*"]
process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]

#removeAllPATObjectsBut(process, ['Electrons','Muons','Jets','METs'])

removeMCMatching(process, ['All'])
## process.allLayer1Muons.isoDeposits = cms.PSet()
## process.allLayer1Electrons.isoDeposits = cms.PSet()
process.patMuons.isoDeposits = cms.PSet()
process.patElectrons.isoDeposits = cms.PSet()


process.cleanPatMuons.cut = cms.string('pt> 15. & abs(eta) < 2.5')
process.cleanPatElectrons.cut = cms.string('pt> 15. & abs(eta) < 2.5')
process.cleanPatJets.cut = cms.string('pt> 10.')




##
# configure input/output
##
####THIS IS NORMALLY OVERWRITTEN FROM crab.cfg

#number of events
process.maxEvents.input = 100

# source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0014/84872812-1F4B-DF11-8A07-00151796C158.root',
    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FEBDE500-4046-DF11-87C6-00151796D9A8.root',
    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F8769D5D-4146-DF11-850D-0024E8768099.root',
    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F69DC71E-4046-DF11-841A-00151796D88C.root',
    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F614C63F-4046-DF11-BA9B-0024E8769B87.root'
   )
)

#process.p = cms.Path( process.patDefaultSequence)
