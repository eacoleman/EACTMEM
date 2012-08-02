from run_PerformSelection_DataPreTriggerTemplate_QCD import *

#-----Trigger Information
process.PS.muTrigger                  = cms.vstring('HLT_IsoMu30_eta2p1_v3','HLT_IsoMu24_eta2p1_v3','HLT_IsoMu24_v9','HLT_IsoMu20_v9','HLT_Mu40_eta2p1_v1','HLT_Mu40_v6')          # Muon trigger name(s)
process.PS.eleTrigger                 = cms.vstring('HLT_Ele*')          # Electron trigger name(s)


#!
#! PATH
#!
process.p = cms.Path(process.PS)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
