#!/bin/bash
g++ -std=c++14 main2.cpp -o test.exe `pkg-config --cflags --libs gtkmm-3.0`
