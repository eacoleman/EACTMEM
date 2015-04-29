#! /usr/bin/env python
import os, sys, getopt, argparse, fnmatch, errno, subprocess, tempfile
from subprocess import call
from glob import glob

#output_file = "WJets_part2_JESUp_part"
#output_file = "WJets_part2_JESDown_part"
#output_file = "WJets_part1_JESUp_part"
output_file = "WJets_part1_JESDown_part"
#files = glob("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20150202_MC12/WJets_part2_JESUp/PS_*.root")
#files = glob("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20150202_MC13/WJets_part2_JESDown/PS_*.root")
#files = glob("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20150202_MC12/WJets_part1_JESUp/PS_*.root")
files = glob("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20150202_MC13/WJets_part1_JESDown/PS_*.root")
#print files
#raw_input()

file_split = len(files)/900
for i in range(1+file_split):
	current_set_of_files = files[i*900:min(len(files),(i+1)*900)]
	#print len(current_set_of_files)
	command = "nohup hadd " + output_file+str(i)+".root "
	for f in current_set_of_files:
		command += f+" "
	command += " > out_"+str(i)+".txt &"
	#print command
	os.system(command)
