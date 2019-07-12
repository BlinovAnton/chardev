#!/bin/bash

x=1
rm -f kek
touch kek
a=$1
c=$2
n=$((a / 2))
while [ $x -le $n ]
do
echo "$2" >> kek
x=$(( $x+1 ))
done
