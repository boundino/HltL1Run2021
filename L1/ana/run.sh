#!/bin/bash

comp="$(root-config --libs --cflags) -I$HOME"
[[ $# == 0 || ${1:-0} -eq 1 ]] && { g++ l1mb_savehist.cc $comp -g -o l1mb_savehist.exe || exit 1 ; }
[[ $# == 0 || ${2:-0} -eq 1 ]] && { g++ l1mb_drawhist.cc $comp -g -o l1mb_drawhist.exe || exit 1 ; }

# https://cmsoms.cern.ch/cms/triggers/l1_rates?cms_run=327327&props.11273_11270.selectedCells=L1A%20physics:2

inputs=(
    configs/run326822.conf
    configs/run327327.conf 
    # /raid5/data/wangj/L1PbPb2022/crab_HiForestADC_20220828_HydjetDrum5F_5p02TeV_122X_2021_realistic_HI_v10.root,
)

for ii in ${inputs[@]}
do
    # IFS=',' ; params=(${ii}) ; unset IFS;

    [[ ${1:-0} -eq 1 ]] && ./l1mb_savehist.exe $ii
    [[ ${2:-0} -eq 1 ]] && ./l1mb_drawhist.exe $ii

done

rm *.exe
