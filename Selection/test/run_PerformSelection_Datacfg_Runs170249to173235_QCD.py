from run_PerformSelection_DataPreTriggerTemplate_QCD import *

#-----Trigger Information
process.PS.muTrigger                  = cms.vstring('HLT_IsoMu17_v13','HLT_IsoMu20_v8','HLT_IsoMu24_v8','HLT_Mu30_v7','HLT_Mu40_v5')          # Muon trigger name(s)
process.PS.eleTrigger                 = cms.vstring('HLT_Ele*')          # Electron trigger name(s)


#!
#! PATH
#!
process.p = cms.Path(process.PS)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
