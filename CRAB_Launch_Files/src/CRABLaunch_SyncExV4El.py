###Runs with 3.5.6
###~7.5 K per event
import FWCore.ParameterSet.Config as cms

## Define the process
#process = cms.Process("Test")

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
## uncomment this line to run on an 35X input sample in 36X
#run36xOn35xInput(process)
run36xOn35xInput(process,
                               genJets = "ak5GenJets")

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
process.out.fileName = 'SyncExV4El.root'

#### Lepton Candidate Filters ####
#Kinematic Filters
process.MuonsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patMuons"),                              
  cut = cms.string('pt > 10. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)
process.ElectronsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patElectrons"),                              
  cut = cms.string('pt > 10. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)

#### HLT filters ####
## import HLTrigger.HLTfilters.hltHighLevel_cfi
## process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
## # Uncomment this to access 8E29 menu and filter on it
## #process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
## process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI")
## process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
## # Uncomment this to filter on 1E31 HLT menu
## #process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]

## process.ElHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
## # Uncomment this to access 8E29 menu and filter on it
## # process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
## process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI")
## process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]
## # Uncomment this to filter on 1E31 HLT menu
## #process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]

## #### Additional Modifications ####
## #process.GlobalTag.globaltag = cms.string('START36_V9::All')
## #process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = cms.string('START38_V14::All')
process.GlobalTag.globaltag = cms.string('START38_V12::All')

## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon.
process.patElectrons.usePV = cms.bool(False)
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
addJetCollection(process,cms.InputTag('ak5PFJets'),
                 'AK5', 'PF',
                 doJTA        = False,
                 doBTagging   = False,
                 jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
                 doType1MET   = True,
                 doL1Cleaning = False,
                 doL1Counters = False,
                 genJetCollection=cms.InputTag("ak5GenJets"),
                 doJetID      = False
                )

## # 1. re-cluster ak5 pfjets and pass them to pat (only for pre-36x input):
## ###process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## addJetCollection(process, cms.InputTag('ak5PFJets::Test'), 'AK5', 'PF', jetCorrLabel=('AK5','PF'), doType1MET=False, doJetID = False)
# make sure to run process.ak5PFJets before PAT, for example:

#process.patDefaultSequence = cms.Sequence(process.ak5PFJets * process.patDefaultSequence)

# 2. add jet plus tracks (only for pre-36x input):
process.load("RecoJets.Configuration.RecoJPTJets_cff")
addJetCollection(process, cms.InputTag('JetPlusTrackZSPCorJetAntiKt5'), 'AK5', 'JPT', jetCorrLabel = ('AK5JPT', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])), doType1MET=True )
#run process.jetPlusTrackZSPCorJetAntiKt5 somewhere before PAT, e.g.,
#process.patDefaultSequence = cms.Sequence(process.jetPlusTrackZSPCorJetAntiKt5 * process.patDefaultSequence)


# 3. add tcMet and pfmet tp PAT:
from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, 'PF')
addTcMET(process, 'TC')


#### Run the pat sequences ##
process.p1 = cms.Path(
  process.ak5PFJets*
  process.jetPlusTrackZSPCorJetAntiKt5*
  process.patDefaultSequence
#  *process.MuonsKinCutBasic
#  *process.MuHLTFilter  
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
## process.out.outputCommands += ["drop *_towerMaker_*_*"]
## process.out.outputCommands += ["drop l1extraParticles_*_*_*"]
## process.out.outputCommands += ["drop *_cleanPatTaus*_*_*"]
## process.out.outputCommands += ["drop *_cleanPatPhotons*_*_*"]
## process.out.outputCommands += ["drop *_offlineBeamSpot_*_*"]
## process.out.outputCommands += ["drop edmTriggerResults_TriggerResults*_*_*"]
## process.out.outputCommands += ["drop *_hltTriggerSummaryAOD_*_*"]
## ###process.out.outputCommands += ["drop recoTracks_generalTracks*_*_*"]
## process.out.outputCommands += ["drop recoGenParticles_genParticles*_*_*"]
## #process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
## #process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]


#process.out.outputCommands += ["drop recoGenJets_*_*_*"]
#process.out.outputCommands += ["drop recoPFCandidates_*_*_*"]

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
####THIS IS NORMALLY OVERWRITTEN FROM crab.cfg

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

        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0016/6E7C4631-9D47-DF11-96CE-003048C69288.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0011/A4121AB4-0747-DF11-8984-0030487F171B.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE8DE204-C446-DF11-BF76-003048C693FA.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE14F78B-C446-DF11-818D-003048C6930E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE0DD96C-B846-DF11-B5AE-0030487EB003.root'

   )
)

#process.p = cms.Path( process.patDefaultSequence)
