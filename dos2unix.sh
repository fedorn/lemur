#!/bin/sh

# removes ^M in source files

for i in `ls */include/*.hpp */include/*.h */src/*.c */src/*.cpp`
do
cp $i $i-tmp
perl -ne '{s/\x0d//g; print;}' < $i-tmp > $i
rm $i-tmp
echo $i
done
