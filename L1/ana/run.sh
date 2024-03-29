#!/bin/bash

make l1mb_savehist
make l1mb_drawhist
# https://cmsoms.cern.ch/cms/triggers/l1_rates?cms_run=327327&props.11273_11270.selectedCells=L1A%20physics:2

inputs=(
    # configs/run326776.conf
    # configs/run326822.conf
    # configs/run326822-cent.conf
    # configs/run327327.conf 
    # configs/run327327_1240b.conf
    # configs/mcHydjetDrum5F122X.conf
    # configs/run362294.conf 
    # configs/run362294-cent.conf 
)

for ii in ${inputs[@]}
do
    # IFS=',' ; params=(${ii}) ; unset IFS;

    [[ ${1:-0} -eq 1 ]] && ./l1mb_savehist $ii
    [[ ${2:-0} -eq 1 ]] && ./l1mb_drawhist $ii

done

