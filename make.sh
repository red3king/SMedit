#!/bin/bash


if [ $# -eq 0 ]
then
    echo "building smedit.exe"
    MAIN="main.cpp"
    OFILE="smedit.exe"
    OPTS=""
else
    echo "building test.exe"
    MAIN="test_main.cpp"
    OFILE="test.exe"
    OPTS="-DUNITTEST"
fi


# g++ -std=c++14 -iquote ./ -o test.exe `pkg-config --cflags --libs gtkmm-3.0` $MAIN utils.cpp id.cpp models/*.cpp


g++ -std=c++14 -iquote ./ -g $OPTS -o $OFILE $MAIN utils.cpp models/*.cpp historymanager/*.cpp historymanager/operations/*.cpp `pkg-config --cflags --libs gtkmm-3.0` 
