#!/bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make -j8
# ./program1 ../resources justin localhost 
./program1 ../resources kkevlar 174.87.85.45 


cd ../

