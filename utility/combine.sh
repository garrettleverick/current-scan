#!/bin/bash

scale=(0.700 0.775 0.850 0.925 1.000 1.075 1.150 1.225 1.300)
generator=("moller" "elastic" "inelastic" "beam")

for i in ${scale[@]}; do
    for j in ${generator[@]}; do
        cd ./$i/$j
        hadd -j 4 -f $j.root out/*
        cd ./../../
#        echo ./$i/$j
#        echo $j.root
#        echo ./../../
    done
done



