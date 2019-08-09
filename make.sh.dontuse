#!/bin/bash


if [ $# -eq 0 ]
then
    echo "building smedit.exe..."
    MAIN="main.cpp"
    OFILE="smedit.exe"
    OPTS=""
elif [ $1 = "test" ]
then
    echo "building test.exe..."
    MAIN="test_main.cpp"
    OFILE="test.exe"
    OPTS="-DUNITTEST"
elif [ $1 = "clean" ]
then
    echo "removing binaries"
    rm -f test.exe
    rm -f smedit.exe
    exit 0
else
    echo "unknown option, supported options = "
    echo "make.sh"
    echo "make.sh test"
    echo "make.sh clean"
    exit 0
fi


g++ -std=c++14 -iquote ./ -g $OPTS -o $OFILE $MAIN cursor.cpp utils.cpp signals.cpp log.cpp lib/nanovg/nanovg.c models/*.cpp historymanager/*.cpp historymanager/operations/*.cpp gui/*.cpp gui/models/*.cpp gui/operations/*.cpp controllers/*.cpp controllers/selected_item/*.cpp `pkg-config --cflags --libs gtkmm-3.0 epoxy` 
