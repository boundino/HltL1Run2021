#!/bin/bash

if [[ $# -ne 5 ]]; then
    echo "usage: ./skim-condor-checkfile.sh [input dir] [input L1] [output dir] [max jobs] [log dir]"
    exit 1
fi

FILELIST=$1
FILEL1=$2
DESTINATION=$3
MAXFILES=$4
LOGDIR=$5

PROXYFILE=$(ls /tmp/ -lt | grep $USER | grep -m 1 x509 | awk '{print $NF}')

tag="evtmatch"

DEST_CONDOR=${DESTINATION}

if [ ! -d $DESTINATION ]
then
    if [[ $DESTINATION == /mnt/T2_US_MIT/* ]]
    then
        SRM_PREFIX="/mnt/T2_US_MIT/hadoop/" ; SRM_PATH=${DEST_CONDOR#${SRM_PREFIX}} ;
        # gfal-mkdir -p srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN=$DESTINATION
        LD_LIBRARY_PATH='' PYTHONPATH='' gfal-mkdir -p gsiftp://se01.cmsaf.mit.edu:2811/${SRM_PATH} # default for T2 hadoop output
    else
        mkdir -p $DESTINATION
    fi
fi

mkdir -p $LOGDIR

counter=0
for i in `cat $FILELIST`
do
    if [ $counter -ge $MAXFILES ]
    then
        break
    fi
    inputname=${i}
    infn=${inputname##*/}
    infn=${infn%%.*} # no .root
    outputfile=${tag}_${infn}.root
    if [ ! -f ${DESTINATION}/${outputfile} ]
    then
        echo -e "\033[38;5;242mSubmitting a job for output\033[0m ${DESTINATION}/${outputfile}"
        
        cat > skim-${tag}.condor <<EOF

Universe     = vanilla
Initialdir   = $PWD/
Notification = Error
Executable   = $PWD/skim-${tag}-checkfile.sh
Arguments    = $inputname $FILEL1 $DEST_CONDOR ${outputfile} $PROXYFILE 
GetEnv       = True
Output       = $LOGDIR/log-${infn}.out
Error        = $LOGDIR/log-${infn}.err
Log          = $LOGDIR/log-${infn}.log
Rank         = Mips
+AccountingGroup = "group_cmshi.$(whoami)"
requirements = GLIDEIN_Site == "MIT_CampusFactory" && BOSCOGroup == "bosco_cmshi" && HAS_CVMFS_cms_cern_ch && BOSCOCluster == "ce03.cmsaf.mit.edu"
job_lease_duration = 240
should_transfer_files = YES
transfer_input_files = evtmatch_inverse.exe,/tmp/$PROXYFILE
Queue 
EOF

condor_submit skim-${tag}.condor -name submit04.mit.edu
mv skim-${tag}.condor $LOGDIR/log-${infn}.condor
counter=$(($counter+1))
    fi
done

echo -e "Submitted \033[1;36m$counter\033[0m jobs to Condor."
