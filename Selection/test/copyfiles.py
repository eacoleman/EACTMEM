#! /usr/bin/env python
import os, sys, getopt, argparse
from subprocess import call

#program name available through the %(prog)s command
#can use prog="" in the ArgumentParser constructor
#can use the type=int option to make the parameters integers
#can use the acction='append' option to make a list of options
parser = argparse.ArgumentParser(description="Move files from storage to local disk to be able to hadd the files",
                                 epilog="And those are the options available. Deal with it.")
group = parser.add_mutually_exclusive_group()

#path = "/eos/uscms/store/user/aperloff/MatrixElement/PS_outfiles_20130916_MC/"
#local = "/uscms_data/d2/aperloff/PS_outfiles_20130916_NTUPLES/"
parser.add_argument("path", help="the location of the stored files")
parser.add_argument("local", help="the location that the files will be copied to")
parser.add_argument("-hf", "--haddfiles", help="files will have HADD run on them to create a single file named by the process",
                    action="store_true")
parser.add_argument("-mc","--multicrab", help="indicates that the files were made using multicrab and will have a certain file structure",
                    action="store_true")
parser.add_argument("-mf", "--makefolder", help="files will be stored in a separate folder with a name determines by the process",
                    action="store_true")
parser.add_argument("-p", "--process", help="the name of the phyiscs process",
                    nargs='+', required=False, default=[""])
group.add_argument("-q", "--quiet", help="decrease output verbosity to minimal amount",
                   action="store_true")
parser.add_argument("-rm", "--rmfiles", help="copied files will be removed at the end of the process",
                    action="store_true")
parser.add_argument("-s", "--sample", help="Shared portion of the name of the files to be copied",
                    nargs='+', required=True, default=["PS"])
group.add_argument("-v", "--verbose", help="Increase output verbosity of lcg-cp (-v) or srm (-debug) commands",
                   action="store_true")
parser.add_argument('--version', action='version', version='%(prog)s 1.0')
args = parser.parse_args()

if(args.verbose and args.debug):
  print 'Number of arguments:', len(sys.argv), 'arguments.'
  print 'Argument List:', str(sys.argv)
  print "Argument ", args

path = args.path
local = args.local
HADDFILES = args.haddfiles
MAKEFOLDER = args.makefolder
MULTICRAB = args.multicrab
PROCESS = args.process
RMFILES = args.rmfiles
SAMPLE = args.sample
VERBOSE = args.verbose
if (not MULTICRAB):
  PROCESS = [""]

if any(x in path for x in ["/uscms_data/d2/","nobackup","/uscms/home/"]):
  isLocal = True
else:
  isLocal = False

for pp in PROCESS:

  if (MAKEFOLDER):
    if (os.path.exists(local+"/"+pp+"_files/")):
      print "directory "+local+"/"+pp+"_files/"+" exists"
    else:
      os.mkdir(local+"/"+pp+"_files/")
      local += "/"+pp+"_files/"
            
  for ss in SAMPLE:
    if (MULTICRAB):
      FILES = os.listdir(path+"/"+pp+"/")
    else:
      FILES = os.listdir(path+"/")

    gen = (ll for ll in FILES if not isLocal)
    for ll in gen:
      #print ll
      if (ll.find(ss) != -1):
        print ll
                
        if(MULTICRAB):
          command = "dccp "+path+"/"+pp+"/"+ll+" "+local+"/"
        else:
          command = "dccp "+path+"/"+ll+" "+local+"/"
          
        print command
        os.system(command)

    #command = "cd "+local+"/"
    #os.system(command)

    if isLocal and not MULTICRAB:
      filesFrom = path
    elif isLocal and MULTICRAB:
      filesFrom = path+"/"+pp+"/"+"res/"
    else:
      filesFrom = local

    if (pp != ""):
      command = "hadd -f "+local+"/"+pp+".root "+filesFrom+"/"+ss+"_*.root"
    else:
      command = "hadd -f "+local+"/"+ss+".root "+filesFrom+"/"+ss+"_*.root"
    if (HADDFILES):
      os.system(command)
                
    command = "rm "+local+"/"+ss+"_*.root"
    if (RMFILES and not isLocal):
      os.system(command)
