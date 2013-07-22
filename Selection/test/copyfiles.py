#! /usr/bin/env python
import os

SAMPLE = ["PS"]
PROCESS = "SingleEl_D_v1_p8"
MULTICRAB = True
MAKEFOLDER = False
HADDFILES = True
RMFILES = True
if (not MULTICRAB):
  PROCESS = ""

path = "/pnfs/cms/WAX/resilient/aperloff/MatrixElement/PS_outfiles_20121102_FullIsoMVA/"
local = "/uscms_data/d2/aperloff/PS_outfiles_20121102_FullIsoMVA/"

if (MAKEFOLDER):
  if (os.path.exists(local+"/"+PROCESS+"_files/")):
    print "directory "+local+"/"+PROCESS+"_files/"+" exists"
  else:
    os.mkdir(local+"/"+PROCESS+"_files/")
  local += "/"+PROCESS+"_files/"

for ss in SAMPLE:
  if (MULTICRAB):
    FILES = os.listdir(path+"/"+PROCESS+"/")
  else:
    FILES = os.listdir(path+"/")
    
  for ll in FILES:
    #print ll
    if (ll.find(ss) != -1):
      print ll
      
      if(MULTICRAB):
        command = "dccp "+path+"/"+PROCESS+"/"+ll+" "+local+"/"
      else:
        command = "dccp "+path+"/"+ll+" "+local+"/"
        
      print command
      os.system(command)

  command = "cd "+local+"/"
  os.system(command)
  if (PROCESS != ""):
    command = "hadd -f "+local+"/"+PROCESS+".root "+local+"/"+ss+"_*.root"
  else:
    command = "hadd -f "+local+"/"+ss+".root "+local+"/"+ss+"_*.root"
  if (HADDFILES):
    os.system(command)
  command = "rm "+local+"/"+ss+"_*.root"
  if (RMFILES):
    os.system(command)
