from run_PerformSelection_DataPreTriggerTemplate import *

#-----Trigger Information
process.PS.muTrigger                  = cms.vstring('HLT_Mu9','HLT_Mu11','HLT_Mu13','HLT_Mu15_v*','HLT_Mu17_v*','HLT_Mu24_v*')          # Muon trigger name(s)
process.PS.eleTrigger                 = cms.vstring('HLT_Ele10_*','HLT_Ele15_*','HLT_Ele17_*')          # Electron trigger name(s)


#!
#! PATH
#!
process.p = cms.Path(process.PS)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
