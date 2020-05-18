#!/usr/bin/bash

echo "making temp directory.."
mkdir tmp

echo "copying tarbal into tmp"
cp smedit-0.0.1.tar.gz ./tmp
cd ./tmp

echo "extract code"
tar -xzf smedit-0.0.1.tar.gz

echo "debian time - copying debian/ in"
cd smedit-0.0.1
dh_make -y -C=s -e red3king@gmail.com -f ../smedit-0.0.1.tar.gz
cp -R ../../debian ./



echo "building..."
dpkg-buildpackage -rfakeroot -kred3king@gmail.com



