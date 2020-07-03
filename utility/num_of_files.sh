#!/bin/bash

scale=(0.700 0.775 0.850 0.925 1.000 1.075 1.150 1.225 1.300)
generator=("moller" "elastic" "inelastic" "beam")
touch nof.txt

for i in ${scale[@]}; do
    for j in ${generator[@]}; do
        echo $i/$j: >> nof.txt
        ls ./$i/$j/out -1 | wc -l >> nof.txt
    done
done



