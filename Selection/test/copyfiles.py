#! /usr/bin/env python
import os

SAMPLE = ["PS"]
PROCESS = "WJets_part2"
MULTICRAB = False
if (not MULTICRAB):
  PROCESS = ""

path = "/pnfs/cms/WAX/resilient/aperloff/MatrixElement/PS_outfiles_20121102_WJets_part2/"
local = "/uscms_data/d2/aperloff/PS_outfiles_20121102_WJets_part2/"
for ss in SAMPLE:
  #if (os.path.exists(local+"/"+PROCESS+"/")):
  #  print "directory "+local+"/"+PROCESS+"/"+" exists"
  #else:
  #  os.mkdir(local+"/"+PROCESS+"/")
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
  os.system(command)
  command = "rm "+local+"/"+ss+"_*.root"
  os.system(command)
                                                                    

