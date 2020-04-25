#!/bin/bash

echo -e "\nTest: find where-to-look\n"
echo -e "Linux utility:\n"
find testdir
echo -e "\nMy find utility:\n"
./find testdir

echo -e "\nTest: find where-to-look -name <name>\n"
echo -e "Linux utility:\n"
find testdir -name test5
echo -e "\nMy find utility:\n"
./find testdir -name test5

echo -e "\nTest: find where-to-look -mmin <num>\n"
echo -e "Linux utility:\n"
find testdir -mmin +5
echo -e "\nMy find utility:\n"
./find testdir -mmin +5

echo -e "\nTest: find where-to-look -inum <num>\n"
echo -e "Linux utility:\n"
find testdir -inum 1125899906951215
echo -e "\nMy find utility:\n"
./find testdir -inum 1125899906951215

echo -e "\nTest: find where-to-look criteria -delete\n"
echo -e "Linux utility:\n"
find testdir -name test1 -delete
echo -e "\nMy find utility:\n"
./find testdir -name test2 -delete

