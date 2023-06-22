#!/bin/bash

file_name=$1
mkdir videos4
yaw=-90
height=10
for (( c=-5; c<=5; c++ ))
do 
   pitch=$((-45+c))

   cp $1 videos4/v_${pitch}_${yaw}_${height}_5.mp4
done
pitch=-45

for (( c=-5; c<=5; c++ ))
do 
   yaw=$((-90+c))

   cp $1 videos4/v_${pitch}_${yaw}_${height}_5.mp4
done
yaw=-90
for (( c=-5; c<=5; c++ ))
do 
   height=$((10+c))

   cp $1 videos4/v_${pitch}_${yaw}_${height}_5.mp4
done
