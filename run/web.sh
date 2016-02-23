#! /bin/bash

for mydir in "$@"
do
    echo "Working on $mydir"
    ./compile.sh
    rm -f plots/*.pdf plots/*.png data
    ln -s ~/cmssw/CMSSW_7_4_15/src/PhysicsTools/TagAndProbe/test/$mydir/ data
    ./run/print_all.exe &
    ./run/print_scale_factors.exe &
    wait
    rsync -vaz -P plots/*.pdf lxplus:~/www/TagAndProbe/25ns/details/$mydir/all_pdfs
    rsync -vaz -P plots/*.png lxplus:~/www/TagAndProbe/25ns/details/$mydir/all_pngs
    ssh lxplus "cd ~/www/TagAndProbe/25ns/details/$mydir && ./.move_plots.sh" 
done
