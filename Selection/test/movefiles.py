#! /usr/bin/env python
import os, sys, getopt, argparse, fnmatch, errno, subprocess
from subprocess import call

class Error(EnvironmentError):
    pass

siteDBDict = {
    'T1_US_FNAL_STORE' : ('cmssrm.fnal.gov', '//srm/managerv2?SFN=',         '/11/store/user/',   '/pnfs/cms/WAX/'),
    'T1_US_FNAL_EOS'   : ('cmseos.fnal.gov', '/srm/v2/server?SFN=' ,  '/eos/uscms/store/user/',                 ''),
    'T1_US_FNAL_RES'   : ('cmssrm.fnal.gov', '//srm/managerv2?SFN=',             '/resilient/',   '/pnfs/cms/WAX/'),
    'T1_US_FNAL_LPC'   : ('localhost/'     , ''                    ,                        '', os.environ['HOME']),
    'T3_US_TAMU'       : ('hurr.tamu.edu'  , '/srm/v2/server?SFN=' , '/fdata/hepx/store/user/',                 '')
}

#program name available through the %(prog)s command
#can use prog="" in the ArgumentParser constructor
#can use the type=int option to make the parameters integers
#can use the action='append' option to make a list of options
parser = argparse.ArgumentParser(description="Transfer files from one location on the OSG to another",
                                 epilog="And those are the options available. Deal with it.")
group = parser.add_mutually_exclusive_group()
parser.add_argument("STARTserver", help="The name of the server where the files are initially located",
                    choices=siteDBDict.keys())
parser.add_argument("STARTpath", help="The location of the files within the users store or resilient areas")
parser.add_argument("ENDserver", help="The name of the server to which the files should be copied",
                    choices=siteDBDict.keys())
parser.add_argument("ENDpath", help="The location of the files within the users fdata area")
parser.add_argument("-d","--debug", help="Shows some extra information in order to debug this program",
					action="store_true")
parser.add_argument("-diff","--diff",
                    help="Tells the program to do a diff between the two directories and only copy the missing files. Only works for two local directories.",
                    action="store_true")
parser.add_argument("-i","--ignore", help="Patterns of files/folders to ignore",
                    nargs='+', type=str, default=())
parser.add_argument("-lcg", "--lcg", help="Tells the program to use the lcg-cp command rather than srmcp",
                    action="store_true")
parser.add_argument("-pnfs", "--pnfs", help="dCache location of original files", choices=["store","resilient"],
                    default="store")
group.add_argument("-q", "--quiet", help="decrease output verbosity to minimal amount",
                   action="store_true")
parser.add_argument("-r", "--recursive", help="Recursively copies directories and files",
                    action="store_true")
parser.add_argument("-s", "--sample", help="Shared portion of the name of the files to be copied", nargs='+',
                    default=["*"])
parser.add_argument("-srt","--sendreceive_timeout", help="Sets the send/recieve timeout for the lcg command",
					default="20")
parser.add_argument("-u", "--user", help="username of the person transfering the files",
                    default=os.environ['USER'])
group.add_argument("-v", "--verbose", help="Increase output verbosity of lcg-cp (-v) or srm (-debug) commands",
                    action="store_true")
parser.add_argument('--version', action='version', version='%(prog)s 1.6')
args = parser.parse_args()

if(args.debug):
     print 'Number of arguments:', len(sys.argv), 'arguments.'
     print 'Argument List:', str(sys.argv)
     print "Argument ", args

SAMPLE = args.sample
USER = args.user
PNFS = args.pnfs
LCG = args.lcg
SRT = args.sendreceive_timeout
START = args.STARTserver
STARTpath = args.STARTpath
END = args.ENDserver
ENDpath = args.ENDpath
DIFF = args.diff
VERBOSE = args.verbose
RECURSIVE = args.recursive
IGNORE = tuple(args.ignore)
DEPTH = 1
scommand = ""
ecommand = ""

def run_checks():
    global RECURSIVE
    global END

    if RECURSIVE :
        global DEPTH
        DEPTH = 9999

	if os.system("voms-proxy-info")!=0 :
		print "WARNING::You must have a valid proxy for this script to work.\nRunning \"voms-proxy-init -voms cms\"...\n"
		call(["voms-proxy-init","-voms","cms"])
		if os.system("voms-proxy-info")!=0 :
			print "ERROR::Sorry, but I still could not find your proxy.\nWithout a valid proxy, this program will fail spectacularly.\nThe program will now exit." 
			sys.exit()

def init_commands():
    if(LCG):
        if(VERBOSE):
            scommand = "lcg-cp -v -b -n 15 --sendreceive-timeout "+SRT+" --srm-timeout 60 -D srmv2"
        else:
            scommand = "lcg-cp -b -n 15 --sendreceive-timeout "+SRT+" --srm-timeout 60 -D srmv2"
    else:
        if(VERBOSE):
            scommand = "srmcp -2 -pushmode=true -debug=true"
        else:
            scommand = "srmcp -2 -pushmode=true"
        
    if(START == 'T1_US_FNAL_LPC'):
		scommand += " \"file:////"+siteDBDict[START][1]+siteDBDict[START][2]+"/"+STARTpath+"/"
    else:
        scommand += " \"srm://"+siteDBDict[START][0]+":8443"+siteDBDict[START][1]+siteDBDict[START][2]+"/"+USER+"/"+STARTpath+"/"

    if(END == 'T1_US_FNAL_LPC'):
        ecommand = "\"file:////"+siteDBDict[END][0]+siteDBDict[END][1]+siteDBDict[END][2]+"/"+ENDpath+"/"
    else:
        ecommand = "\"srm://"+siteDBDict[END][0]+":8443"+siteDBDict[END][1]+siteDBDict[END][2]+"/"+USER+"/"+ENDpath+"/"

    return (scommand,ecommand)


def make_directory(END, path):
    if END.find("FNAL") > 0 :
        if not os.path.exists(path) :
            print "making directory "+path
            os.makedirs(path)
    else:
        if LCG :
            cmd = "lcg-ls -v -b -D srmv2 \"srm://"+siteDBDict[END][0]+":8443"+siteDBDict[END][1]+path+"\""
        else:
            cmd = "srmls -2 -pushmode=true \"srm://"+siteDBDict[END][0]+":8443"+siteDBDict[END][1]+path+"\""
        if os.system(cmd)!=0 :
            print "making directory "+path
            cmd = "srmmkdir -2 -pushmode=true \"srm://"+siteDBDict[END][0]+":8443"+siteDBDict[END][1]+path+"\""
            os.system(cmd)

def get_list_of_files(SAMPLE, path):
    FILES_UNFILTERED = []
    if START.find("FNAL") > 0 and os.environ['HOSTNAME'].find("fnal.gov") > 0 :
        FILES_UNFILTERED = os.listdir(path)
    else:
        options = '-2 -pushmode=true "srm://'+siteDBDict[START][0]+':8443'+siteDBDict[START][1]+path+'"'
        proc = subprocess.Popen(['srmls',options], stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate()[0]
        FILES_UNFILTERED = proc.splitlines()
        FILES_UNFILTERED = [x.strip(' ') for x in FILES_UNFILTERED]
        FILES_UNFILTERED = [x.strip('\n') for x in FILES_UNFILTERED]
    FILES = []
    #if(len(SAMPLE)==1):
    #    SAMPLE = ["*"]
    for ss in SAMPLE:
        FILES += fnmatch.filter(FILES_UNFILTERED, '*'+ss+'*')
    return FILES

def filter_list_of_files(SAMPLE, FILES_UNFILTERED):
    FILES = []
    if(len(SAMPLE)==1):
        SAMPLE = ["*"]
    for ss in SAMPLE:
        FILES += fnmatch.filter(FILES_UNFILTERED, '*'+ss+'*')
    return FILES

def ignore_patterns(patterns):
    """Function that can be used as copytree() ignore parameter.

    Patterns is a sequence of glob-style patterns
    that are used to exclude files"""
    def _ignore_patterns(path, names):
        ignored_names = []
        for pattern in patterns:
            if len(fnmatch.filter([path],'*'+pattern+'*')) > 0:
                ignored_names.extend(names)
            #names_extended = [os.path.join(path,n) for n in names]
            #ignored_names.extend(fnmatch.filter(names_extended, '*'+pattern+'*'))
            else:
                ignored_names.extend(fnmatch.filter(names, '*'+pattern+'*'))
        return set(ignored_names)
    return _ignore_patterns

def copytree(src, dst, DEPTH, CURRENTdepth, symlinks=False, ignore=None):
    global args

    if args.debug :
        print "CURRENTdepth="+str(CURRENTdepth)
    if CURRENTdepth >= DEPTH :
        return
    
    make_directory(END,dst)

    global SAMPLE
    FILES = get_list_of_files(SAMPLE, src)
    if DIFF :
        FILES1 = FILES
        if END.find("FNAL") > 0 and os.environ['HOSTNAME'].find("fnal.gov") > 0 :
            FILES2 = os.listdir(dst)
        else:
            options = '-2 -pushmode=true "srm://'+siteDBDict[END][0]+':8443'+siteDBDict[END][1]+dst+'"'
            proc = subprocess.Popen(['srmls',options], stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate()[0]
            proc_splitlines = proc.splitlines()
            FILES2 = []
            for f in proc_splitlines :
                if len(f.split()) > 0 and f.split()[0].isdigit() :
                    FILES2.append(os.path.basename(f.split()[1]))
        FILES_DIFF = [f for f in FILES1 if f not in FILES2 or os.path.isdir(dst+"/"+f)]
        print "Adding an additional "+str(len(FILES_DIFF))+" files/folders"
        FILES = FILES_DIFF
    
    if ignore is not None:
        ignored_names = ignore(src, FILES)
    else:
        ignored_names = set()

    errors = []
    for FILE in FILES:
        if FILE in ignored_names:
            continue
        srcname = os.path.join(src, FILE)
        dstname = os.path.join(dst, FILE)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto,dstname)
            elif os.path.isdir(srcname):
                copytree(srcname, dstname, DEPTH, CURRENTdepth+1, symlinks, ignore)
            else:
                global TOPsrc
                srel = os.path.relpath(src,TOPsrc)+"/"
                scommand, ecommand = init_commands()
                print "Copying file: "+FILE
                command = scommand+srel+FILE+"\" "+ecommand+srel+FILE+"\""
                print command
                os.system(command)
                
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except Error, err:
            errors.extend(err.args[0])
        except EnvironmentError, why:
            errors.append((srcname, dstname, str(why)))
    if errors:
        raise Error, errors
    
run_checks()

TOPsrc = ""
if(START == 'T1_US_FNAL_LPC'):
	TOPsrc = STARTpath+"/"
else:
	TOPsrc = siteDBDict[START][3]+"/"+siteDBDict[START][2]+"/"+USER+"/"+STARTpath+"/"

TOPdst = siteDBDict[END][3]+"/"+siteDBDict[END][2]+"/"+USER+"/"+ENDpath+"/"

if args.debug:
	print "DEPTH="+str(DEPTH)
#copytree(TOPsrc,TOPdst, DEPTH, 0, False, ignore_patterns('DYJets','WJets','TTJets','STopT_T','ggH1?0'))
copytree(TOPsrc,TOPdst, DEPTH, 0, False, ignore_patterns(IGNORE))
