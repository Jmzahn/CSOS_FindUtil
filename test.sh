#!/bin/bash

echo -e "\nTest: find where-to-look\n"
echo -e "Linux utility:\n"
find testdir
echo -e "\nMy find utility:\n"
./find -w testdir

echo -e "\nTest: find where-to-look -name <name>\n"
echo -e "Linux utility:\n"
find testdir -name test5
echo -e "\nMy find utility:\n"
./find -w testdir -n test5

echo -e "\nTest: find where-to-look -mmin <num>\n"
echo -e "Linux utility:\n"
find testdir -mmin +5
echo -e "\nMy find utility:\n"
./find -w testdir -m +5

echo -e "\nTest: find where-to-look -inum <num>\n"
echo -e "Linux utility:\n"
find testdir -inum 1125899906951215
echo -e "\nMy find utility:\n"
./find -w testdir -i 1125899906951215

echo -e "\nTest: find where-to-look criteria -delete\n"
echo -e "Linux utility:\n"
find testdir -name test1 -delete
echo -e "\nMy find utility:\n"
./find -w testdir -n test2 -d

echo -e "\nTest: find where-to-look criteria -action\n"
echo -e "Linux utility: find testdir -name test1 -exec cat {} \\;\n"
find testdir -name test4 -exec cat {} \;
echo -e "\nMy find utility:\n"
./find -w testdir -n test4 -a cat

echo -e "\nMy find utility: rm\n"
./find -w testdir -n test5 -a rm

echo -e "\nMy find utility: mv\n"
./find -w testdir -n test4 -a mv test44

echo -e "\nMy find utility: ls\n"
./find -w testdir -n test4 -a ls

echo -e "\nMy find utility: touch\n"
./find -w testdir -n test4 -a touch touchtest

echo -e "\nMy find utility: mkdir\n"
./find -w testdir -n test4 -a mkdir mkdirtest