from run_PerformSelection_DataPreTriggerTemplate import *

#-----Trigger Information
process.PS.muTrigger                  = cms.vstring('HLT_IsoMu30_eta2p1_v6','HLT_IsoMu24_eta2p1_v6','HLT_IsoMu24_v12','HLT_IsoMu30_eta2p1_v7','HLT_IsoMu24_eta2p1_v7','HLT_IsoMu24_v13','HLT_Mu40_eta2p1_v5','HLT_Mu40_v10')          # Muon trigger name(s)
process.PS.eleTrigger                 = cms.vstring('HLT_Ele*')          # Electron trigger name(s)


#!
#! PATH
#!
process.p = cms.Path(process.PS)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
