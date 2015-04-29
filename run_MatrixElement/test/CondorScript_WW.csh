#!/bin/csh
echo "Starting" 
cd /uscms/home/aperloff/MatrixElement/CMSSW_5_2_5/src
source /uscmst1/prod/sw/cms/cshrc uaf
pwd
eval `scram runtime -csh` 
cd -
@ JobNumber = $argv[1]
echo "JobNumber=$JobNumber"
@ NEvtsPerJob = $argv[2]
echo "NEvtsPerJob=$NEvtsPerJob"
@ TheJob = 0 + $JobNumber
echo "TheJob=$TheJob"
@ StartEvt = $TheJob * $NEvtsPerJob
echo "StartEvt=$StartEvt"
run_MatrixElement root://cmsxrootd.fnal.gov//store/user/aperloff/MatrixElement/Summer12ME8TeV/MEInput/WW.root WW$TheJob.root jets2 $NEvtsPerJob $StartEvt 1 1 PS
echo "Finished"

#cmsRun -j FrameworkJobReport.xml -p PSet.py