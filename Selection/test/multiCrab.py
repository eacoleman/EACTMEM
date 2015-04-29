from os import system

#jobs=[#'REQUESTNAME','INPUTDATASET','PUBLISHDATANAME','LBSPERJOB','DBSURL'],
jobs=[#'REQUESTNAME','INPUTDATASET','PUBLISHDATANAME','FILESPERJOB','DBSURL'],
#	['WJets_part1_JESUp','/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM','WJets_part1_JESUp',1,'global'],
#	['WJets_part2_JESUp','/WJetsToLNu_TuneZ2Star_8TeV-madgraph-tarball/Summer12_DR53X-PU_S10_START53_V7A-v2/AODSIM','WJets_part2_JESUp',1,'global'],
#	['TTbar_JESUp','/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM','TTbar_JESUp',1,'global'],
	['qqH125_JESUp','/LQ-vbf125_GENSIM/ajkumar-LQ-qqh125_AODSIM_Summer12_START53_V7A-c8f8ed334db8a7d6f56c62266b1dfa5b/USER','qqH125_JESUp',4,'phys02'],
    ]

template='crabTemplate.py'

#---------------------------------------------------------------------------------------------------------------------------------------------------

for job in jobs:
    requestName=job[0]
    inputDS=job[1]
    publishDN=job[2]
    Files=str(job[3])
    dbsUrl=job[4]
    
    config=requestName+'.py'
    system('cp '+template+' '+config)
    system('sed s%REQUESTNAME%'+requestName+'%g '+config+' --in-place')
    system('sed s%INPUTDATASET%'+inputDS+'%g '+config+' --in-place')
    system('sed s%PUBLISHDATANAME%'+publishDN+'%g '+config+' --in-place')
    system('sed s%FILESPERJOB%'+Files+'%g '+config+' --in-place')
    system('sed s%DBSURL%'+dbsUrl+'%g '+config+' --in-place')

    system('crab submit '+config)
