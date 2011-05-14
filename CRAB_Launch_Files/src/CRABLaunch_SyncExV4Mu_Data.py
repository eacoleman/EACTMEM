###Run with 3_8_1_patch2
#import FWCore.ParameterSet.Config as cms

## Define the process
#process = cms.Process("Test")

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
#from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
## uncomment this line to run on an 35X input sample in 36X
#run36xOn35xInput(process)
#run36xOn35xInput(process, genJets = "ak5GenJets")
#run36xOn35xInput(process, genJets = "ak5GenJets", postfix = "PF2PAT")

#################################################################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p2') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

# output file name
process.out.fileName = 'SyncExV4MuPF2PAT_Data.root'

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
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
#process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI")
process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
# Uncomment this to filter on 1E31 HLT menu
#process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]

## process.ElHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
## # Uncomment this to access 8E29 menu and filter on it
## # process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
## process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI")
## process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]
## # Uncomment this to filter on 1E31 HLT menu
## #process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]

## #### Additional Modifications ####
#process.GlobalTag.globaltag = cms.string('START38_V14::All')
#process.GlobalTag.globaltag = cms.string('START38_V12::All')
process.patJets.addBTagInfo = False
process.patJets.tagInfoSources = []
process.GlobalTag.globaltag = cms.string('GR10_P_V10::All')

## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon.
###process.patElectrons.usePV = cms.bool(False)
##process.patMuons.usePV = cms.bool(False)

removeMCMatching(process, ['All'])

## from PhysicsTools.PatAlgos.tools.jetTools import *

## switchJetCollection( process,
##                      jetCollection=cms.InputTag('ak5CaloJets'),
##                      jetCorrLabel=('AK5Calo', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])))


## process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## addJetCollection(process,cms.InputTag('ak5PFJets'),
##                  'AK5', 'PF',
## #                 doJTA        = False,
##                  doBTagging   = True,
## #                jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
##                  jetCorrLabel = ('AK5','PF'),
## #                 doType1MET   = True,
##                  doType1MET   = False,
## #                 doL1Cleaning = False,
## #                 doL1Counters = False,
## #                 genJetCollection=cms.InputTag("ak5GenJets"),
##                  doJetID      = False
## #                 doJetId      = False
##                 )

## # 1. re-cluster ak5 pfjets and pass them to pat (only for pre-36x input):
## ## process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## ## addJetCollection(process, cms.InputTag('ak5PFJets::Test'), 'AK5', 'PF', jetCorrLabel=('AK5','PF'), doType1MET=False, doJetID = False)
## ## # make sure to run process.ak5PFJets before PAT, for example:
## ## process.patDefaultSequence = cms.Sequence(process.ak5PFJets * process.patDefaultSequence)

## # 2. add jet plus tracks (only for pre-36x input):
## process.load("RecoJets.Configuration.RecoJPTJets_cff")
## addJetCollection(process, cms.InputTag('JetPlusTrackZSPCorJetAntiKt5'), 'AK5', 'JPT', doBTagging = True,
## #                 jetCorrLabel = ('AK5JPT', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
##                  jetCorrLabel = ('AK5','JPT'),
##                  doType1MET=False )
## #run process.jetPlusTrackZSPCorJetAntiKt5 somewhere before PAT, e.g.,
## #process.patDefaultSequence = cms.Sequence(process.jetPlusTrackZSPCorJetAntiKt5 * process.patDefaultSequence)


## # 3. add tcMet and pfmet tp PAT:
## from PhysicsTools.PatAlgos.tools.metTools import *
## addPfMET(process, 'PF')
## addTcMET(process, 'TC')


####For PF2PAT####
## process.load("PhysicsTools.PatAlgos.patSequences_cff")
## from PhysicsTools.PatAlgos.tools.pfTools import *
## postfix = "PFlow"
## usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC= False , postfix=postfix) 



## from PhysicsTools.PatAlgos.tools.metTools import *
## addPfMET(process, 'PF')


#### Run the pat sequences ##
process.p2 = cms.Path(
#    process.jetPlusTrackZSPCorJetAntiKt5*
#    process.ak5PFJets*
    process.patDefaultSequence
 #   getattr(process,"patPF2PATSequence"+postfix)
    #  *process.MuonsKinCutBasic
#    *process.MuHLTFilter  
)

## process.p2 = cms.Path(
##   process.patDefaultSequence
##   *process.ElectronsKinCutBasic
##   *process.ElHLTFilter  
## )

#### Define and cleanup event content ####
## from PhysicsTools.PatAlgos.patEventContent_cff import *
## process.out.outputCommands += ['keep *_selectedPat*_*_*','drop *_cleanPat*_*_*']
## process.out.outputCommands += patExtraAodEventContent
## process.out.outputCommands += ["drop *_towerMaker_*_*"]
## process.out.outputCommands += ["drop l1extraParticles_*_*_*"]
## process.out.outputCommands += ["drop *_selectedPatTaus*_*_*"]
## process.out.outputCommands += ["drop *_selectedPatPhotons*_*_*"]
## process.out.outputCommands += ["drop *_offlineBeamSpot_*_*"]
## process.out.outputCommands += ["drop edmTriggerResults_TriggerResults*_*_*"]
## process.out.outputCommands += ["drop *_hltTriggerSummaryAOD_*_*"]
## ###process.out.outputCommands += ["drop recoTracks_generalTracks*_*_*"]
## process.out.outputCommands += ["drop recoGenParticles_genParticles*_*_*"]
## #process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
## #process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]


#process.out.outputCommands += ["drop recoGenJets_*_*_*"]
#process.out.outputCommands += ["drop recoPFCandidates_*_*_*"]

## process.patMuons.isoDeposits = cms.PSet()
## process.patElectrons.isoDeposits = cms.PSet()


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
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0016/6E7C4631-9D47-DF11-96CE-003048C69288.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0011/A4121AB4-0747-DF11-8984-0030487F171B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE8DE204-C446-DF11-BF76-003048C693FA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE14F78B-C446-DF11-818D-003048C6930E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE0DD96C-B846-DF11-B5AE-0030487EB003.root'
    
#       '/store/data/Run2010B/Mu/AOD/Dec22ReReco_v1/0047/8AD8D267-F012-E011-95B9-E0CB4E1A114B.root',
#       '/store/data/Run2010B/Mu/AOD/Dec22ReReco_v1/0047/56F16FF0-CB13-E011-BD29-E0CB4E5536BB.root',
#       '/store/data/Run2010B/Mu/AOD/Dec22ReReco_v1/0031/DAAD8469-2D11-E011-9BCE-E0CB4E4408DE.root',
#       '/store/data/Run2010B/Mu/AOD/Dec22ReReco_v1/0030/E0482B52-0711-E011-8347-90E6BA19A212.root',
#       '/store/data/Run2010B/Mu/AOD/Dec22ReReco_v1/0029/E818A394-7A10-E011-AAA6-E0CB4E29C4F9.root'

#       'file:/afs/cern.ch/cms/Tutorials/TWIKI_DATA/CMSDataAnaSch/CMSDataAnaSch_Data_387.root'

       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/inputfiles/Run2010BMuAODPromptReco-v2_BEBB57F5-8DE7-DF11-8E71-003048F024F6.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/inputfiles/Run2010BMuAODPromptReco-v2_EC2E0313-92E7-DF11-9BCB-001617DBD556.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/inputfiles/Run2010BMuAODPromptReco-v2_7AA51D3E-2FE7-DF11-8E15-003048F118D4.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/inputfiles/Run2010BMuAODPromptReco-v2_AAB07454-1DE7-DF11-9E39-0030487CD906.root'

    

       
##        '/store/relval/CMSSW_3_8_5/Mu/RECO/GR_R_38X_V13_RelVal_wzMu_realData-v1/0075/F4184FFB-9DEC-DF11-80BB-002354EF3BE1.root',
##        '/store/relval/CMSSW_3_8_5/Mu/RECO/GR_R_38X_V13_RelVal_wzMu_realData-v1/0075/EC6AF8FD-9DEC-DF11-96F3-0026189438DF.root',
##        '/store/relval/CMSSW_3_8_5/Mu/RECO/GR_R_38X_V13_RelVal_wzMu_realData-v1/0075/EAA927FB-9DEC-DF11-AE54-003048679182.root',
##        '/store/relval/CMSSW_3_8_5/Mu/RECO/GR_R_38X_V13_RelVal_wzMu_realData-v1/0075/D08FF5FC-9DEC-DF11-AF21-0026189438CC.root'

##       '/store/mc/Fall10/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/START38_V12-v2/0009/E8B2CA4D-42E7-DF11-988C-90E6BA442F16.root',
##       '/store/mc/Fall10/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/START38_V12-v2/0009/E847D402-12E7-DF11-97C5-003048D4EF1D.root',
##       '/store/mc/Fall10/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/START38_V12-v2/0009/6EE559BE-11E7-DF11-B575-00145E5513C1.root',
##       '/store/mc/Fall10/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/START38_V12-v2/0008/DC4963A1-E0E5-DF11-807E-00D0680BF898.root'
###W+J###


##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/GEN-SIM-RECO/START38_V12-v1/0044/F2A82E83-D4FC-DF11-924A-00238BBD754A.root',
##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/GEN-SIM-RECO/START38_V12-v1/0044/C8A2378C-D4FC-DF11-A0A8-00238BCE4648.root',
##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/GEN-SIM-RECO/START38_V12-v1/0044/A2D79790-D4FC-DF11-8682-00238BBD7586.root',
##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/GEN-SIM-RECO/START38_V12-v1/0044/8E5E598E-D4FC-DF11-802F-A4BADB38EAD3.root'

##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/AODSIM/START38_V12-v1/0044/E2C35B09-D5FC-DF11-9A1A-00238BBD7678.root',
##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/AODSIM/START38_V12-v1/0044/32B541DA-D2FC-DF11-A915-A4BADB38E6A7.root',
##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/AODSIM/START38_V12-v1/0043/F6CAF13B-63F9-DF11-AAB3-001731EB2252.root',
##        '/store/mc/Fall10/WJetsToLNu_TuneD6T_7TeV-madgraph-tauola/AODSIM/START38_V12-v1/0043/F23FFF73-5CF9-DF11-B22E-0013D4891F58.root'

       

   )
)

#process.p = cms.Path( process.patDefaultSequence)
