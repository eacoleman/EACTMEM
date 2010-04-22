#!/bin/bash

#WHEN RUNNING ON CAFFARM='CAF' 600 is too little!.
#JUST DO NOT USE!!!!

# This is the list of commands used to submit all the jobs 
# essentially we read from /cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008/
# end write to /cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008/Event_Probs_Sep1_2008

# Nominal samples at low-lumi 
INDIR="/cdf/disk02/g6/tnt_Fall2009_Nominal/"


#OUTDIR="/cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008_pass2/Event_Probs_Sep9_2008"
#OUTDIR="/cdf/disk01/g6/caf_ICHEP2008/tnt_ICHEP2008_pass2/Event_Probs_Summer09_newNNTF_atleastoneTightSecVtx/"
OUTDIR="/cdf/disk01/g6/eusebi/tnt_Spring2010_CEPD_TF/"

JETS="jet2"
#JETS="jet3"

# Use TRUE for recovery, anything else to regular jobs
RECOVERY="FALSE"
if [[ "$RECOVERY" == "TRUE" ]] ;
        then
    SCRIPTTORUN="./submit_me_recovery.sh"
else
    SCRIPTTORUN="./submit_me_other.sh"
fi

# TO TEST THE CORRELATED EPD
$SCRIPTTORUN ${INDIR}/tnt_WHlvbb/\*_chgt1b\*.root  ${OUTDIR}/WHlvbb/ WHlvbb cdfgrid  medium ${JETS} 100 eusebi  atleastoneTSVtag  force

$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenBB0\*.root     ${OUTDIR}/WemtBB012p/  WenBB0p  cdfgrid  medium ${JETS} 100 balvarez     atleastoneTSVtag  force

$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenBB[12]p\*.root ${OUTDIR}/WemtBB012p/  WenBB12p cdfgrid  medium ${JETS} 100 palencia   atleastoneTSVtag  force
			
exit
# END TO TEST THE CORRELATED EPD

# Total disk space about 23Gb. We have 12 potential caf users, => 2Gb/user
$SCRIPTTORUN ${INDIR}/tnt_WHlvbb/\*_chgt1[abc]\*.root  ${OUTDIR}/WHlvbb/ WHlvbb cdfgrid  medium ${JETS} 500 balvarez  atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_WHlvbb/\*_chgt1[def]\*.root  ${OUTDIR}/WHlvbb/ WHlvbb cdfgrid  medium ${JETS} 500 casal     atleastoneTSVtag  force 
$SCRIPTTORUN ${INDIR}/tnt_WHlvbb/\*_chgt2[klm]\*.root  ${OUTDIR}/WHlvbb/ WHlvbb cdfgrid  medium ${JETS} 500 ruiza     atleastoneTSVtag  force 
$SCRIPTTORUN ${INDIR}/tnt_WHlvbb/\*_chgt2[no]\*.root   ${OUTDIR}/WHlvbb/ WHlvbb    cnaf  medium ${JETS} 600 hurwitz   atleastoneTSVtag  force
	
# Z+jets	
$SCRIPTTORUN ${INDIR}/tnt_Zee/\*ztop[p234]\*.root ${OUTDIR}/Zjets/      Zee      cdfgrid  medium ${JETS} 600 ruiza     atleastoneTSVtag  force 
$SCRIPTTORUN ${INDIR}/tnt_Zee/\*ztop[bc]\*.root   ${OUTDIR}/Zjets/      Zee      cdfgrid  medium ${JETS} 500 ruiza     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Zmm/\*ztop[p789]\*.root ${OUTDIR}/Zjets/      Zmm      namgrid  medium ${JETS} 600 ruiza     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Zmm/\*ztop[bc]\*.root   ${OUTDIR}/Zjets/      Zmm        cnaf   medium ${JETS} 600 ruiza     atleastoneTSVtag  force
		
$SCRIPTTORUN ${INDIR}/tnt_Ztt/\*ztopt\*.root      ${OUTDIR}/Zjets/      Ztt         cnaf  medium ${JETS} 500 rocio     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_schan/\*.root           ${OUTDIR}/schan/     schan     cdfgrid  medium ${JETS} 600 rocio     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_tchan/\*.root           ${OUTDIR}/tchan/     tchan     cdfgrid  medium ${JETS} 600 palencia  atleastoneTSVtag  force
		
# Diboson	
$SCRIPTTORUN ${INDIR}/tnt_WZ/\*.root              ${OUTDIR}/diboson/     WZ       cdfgrid medium ${JETS} 600 hurwitz   atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_WW/\*.root              ${OUTDIR}/diboson/     WW        cnaf   medium ${JETS} 600 rocio     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_ZZ/\*.root              ${OUTDIR}/diboson/     ZZ        cnaf   medium ${JETS} 600 rocio     atleastoneTSVtag  force
		
# ttbar, for 3-jet bin need to split in half		
$SCRIPTTORUN ${INDIR}/tnt_ttbar/\*_48_\*.root     ${OUTDIR}/ttbar/       tt       cdfgrid  medium ${JETS} 600 canelli  atleastoneTSVtag  force
# $SCRIPTTORUN ${INDIR}/tnt_ttbar/\*_54_\*.root     ${OUTDIR}/ttbar/       tt       cdfgrid  medium ${JETS} 600 group    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_ttbar/\*_54_\*split.root   ${OUTDIR}/ttbar/    tt       cdfgrid  medium ${JETS} 600 group    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_ttbar/\*_54_\*split_1.root ${OUTDIR}/ttbar/    tt       cdfgrid  medium ${JETS} 600 group    atleastoneTSVtag  force
		
# W->Electron + neutrino
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenBB0\*.root     ${OUTDIR}/WemtBB012p/  WenBB0p  cdfgrid  medium ${JETS} 600 bernd     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenBB[12]p\*.root ${OUTDIR}/WemtBB012p/  WenBB12p cdfgrid  medium ${JETS} 600 canelli   atleastoneTSVtag  force
			
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenCC0\*.root     ${OUTDIR}/WenCC012p/   WenCC0p  cdfgrid  medium ${JETS} 600 group     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenCC1\*.root     ${OUTDIR}/WenCC012p/   WenCC1p  cdfgrid  medium ${JETS} 600 ferrazza  atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenCC2\*.root     ${OUTDIR}/WenCC012p/   WenCC2p    cnaf   medium ${JETS} 600 group     atleastoneTSVtag  force
			
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenC0\*.root      ${OUTDIR}/WenC0123p/   WenC0p   cdfgrid  medium ${JETS} 600 group     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenC1\*.root      ${OUTDIR}/WenC0123p/   WenC1p   cdfgrid  medium ${JETS} 600 hurwitz   atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenC2\*.root      ${OUTDIR}/WenC0123p/   WenC2p    cdfgrid medium ${JETS} 600 palencia  atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*WenC3\*.root      ${OUTDIR}/WenC0123p/   WenC3p     cnaf   medium ${JETS} 600 palencia  atleastoneTSVtag  force
			
# W->Muon + neutrino	
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnBB0\*.root     ${OUTDIR}/WemtBB012p/  WmnBB0p   cdfgrid medium ${JETS} 600 ferrazza  atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnBB[12]p\*.root ${OUTDIR}/WemtBB012p/  WmnBB12p  cdfgrid medium ${JETS} 600 wallny    atleastoneTSVtag  force
			
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnCC0\*.root     ${OUTDIR}/WmnCC012p/   WmnCC0p  cdfgrid  medium ${JETS} 600 ferrazza  atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnCC1\*.root     ${OUTDIR}/WmnCC012p/   WmnCC1p  cdfgrid  medium ${JETS} 600 bernd     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnCC2\*.root     ${OUTDIR}/WmnCC012p/   WmnCC2p    cnaf   medium ${JETS} 600 bernd     atleastoneTSVtag  force

$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnC0\*.root      ${OUTDIR}/WmnC0123p/   WmnC0p  cdfgrid   medium ${JETS} 600 bernd     atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnC1\*.root      ${OUTDIR}/WmnC0123p/   WmnC1p  cdfgrid   medium ${JETS} 600 cuevas    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnC2\*.root      ${OUTDIR}/WmnC0123p/   WmnC2p  cdfgrid   medium ${JETS} 600 cuevas    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*WmnC3\*.root      ${OUTDIR}/WmnC0123p/   WmnC3p  cdfgrid   medium ${JETS} 600 eusebi    atleastoneTSVtag  force
			
# W->Tau+neutrino	
$SCRIPTTORUN ${INDIR}/tnt_Wtn/\*WtnBB\*.root      ${OUTDIR}/WemtBB012p/  WtnBB012p cdfgrid medium ${JETS} 600 eusebi    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wtn/\*WtnCC\*.root      ${OUTDIR}/WtnCC012p/   WtnCC012p  cnaf   medium ${JETS} 600 eusebi    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_Wtn/\*WtnC[0123]\*.root ${OUTDIR}/WtnC0123p/   WtnC0123p lcgcaf  medium ${JETS} 600 eusebi    atleastoneTSVtag  force

# W->Light flavor, running over taggable to enhance statistics. Running without prescale.
#  for 3-jet bin need to split /tnt_Wen[34] into Wen3 and Wen4 and the same for muons
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*Wen[01]\*.root    ${OUTDIR}/Wen01234p/   Wen01p   cdfgrid  medium ${JETS} 600 wallny    taggable force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*Wen2\*.root       ${OUTDIR}/Wen01234p/   Wen2p       cnaf  medium ${JETS} 600 cuevas    taggable force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*Wen3\*.root       ${OUTDIR}/Wen01234p/   Wen3p   cdfgrid  medium ${JETS} 600 balvarez  taggable force
$SCRIPTTORUN ${INDIR}/tnt_Wen/\*Wen4\*.root       ${OUTDIR}/Wen01234p/   Wen4p   cdfgrid  medium ${JETS} 600 balvarez  taggable force

$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*Wmn[01]\*.root    ${OUTDIR}/Wmn01234p/   Wmn01p  cdfgrid  medium ${JETS} 600 pdong     taggable force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*Wmn2\*.root       ${OUTDIR}/Wmn01234p/   Wmn2p   cdfgrid  medium ${JETS} 600 balvarez  taggable force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*Wmn3\*.root       ${OUTDIR}/Wmn01234p/   Wmn3p   cdfgrid  medium ${JETS} 600 ferrazza  taggable force
$SCRIPTTORUN ${INDIR}/tnt_Wmn/\*Wmn4\*.root       ${OUTDIR}/Wmn01234p/   Wmn4p   cdfgrid  medium ${JETS} 600 ferrazza  taggable force

$SCRIPTTORUN ${INDIR}/tnt_Wtn/\*Wtn[01234]\*.root ${OUTDIR}/Wtn01234p/    Wtn01234p  cnaf  medium ${JETS} 600 balvarez  taggable force

$SCRIPTTORUN ${INDIR}/tnt_data/\*.root ${OUTDIR}/data/tag/                 data  cdfgrid   medium ${JETS} 600 cuevas    atleastoneTSVtag  force
$SCRIPTTORUN ${INDIR}/tnt_data/\*.root ${OUTDIR}/data/untagtaggable/       data     cnaf   medium ${JETS} 600 cuevas    untagtaggable force

$SCRIPTTORUN ${INDIR}/tnt_nonWdata/\*.root ${OUTDIR}/nonWdata/tag/            nonWdata  cdfgrid   medium ${JETS} 600 hurwitz  atleastoneTSVtag force
$SCRIPTTORUN ${INDIR}/tnt_nonWdata/\*.root ${OUTDIR}/nonWdata/untagtaggable/  nonWdata  cdfgrid   medium ${JETS} 600 casal  untagtaggable force



exit



