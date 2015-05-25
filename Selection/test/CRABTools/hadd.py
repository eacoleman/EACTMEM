#! /usr/bin/env python
import os, sys, getopt, argparse, fnmatch, errno, subprocess, tempfile
from subprocess import call
from glob import glob

#output_file = "/eos/uscms/store/user/eusebi/Winter12to13ME8TeV/rootOutput/WJets_partialHadd/WJets_part"
output_file = "/uscms_data/d2/aperloff/WJets_partialHadd/WJets_part"
#files = glob("/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20130916_MC13/WJets_part1_nominal/PS_*.root")
files = glob("/eos/uscms/store/user/eusebi/Winter12to13ME8TeV/rootOutput/WJets/WJets*.root")
#print files
#raw_input()

file_split = len(files)/900
for i in range(1+file_split):
	current_set_of_files = files[i*900:min(len(files),(i+1)*900)]
	#print len(current_set_of_files)
	command = "nohup hadd " + output_file+str(i)+".root "
	for f in current_set_of_files:
		command += f+" "
	command += " > " + output_file+str(i)+".txt &"
	#print command
	os.system(command)
