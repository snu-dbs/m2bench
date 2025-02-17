#!/bin/bash

echo "=============== Task 0 ==============="
time agens -d ecommerce -f Task0.sql
echo "========== Task 0 Completed =========="
echo

echo "=============== Task 2 ==============="
time agens -d ecommerce -f Task2.sql
echo "========== Task 2 Completed =========="
echo

echo "=============== Task 9 ==============="
time agens -d healthcare -v $X=9 -f Task9.sql
echo "========== Task 9 Completed =========="
echo

T1=5
T2=10
echo "=============== Task 14 ==============="
time agens -d disaster -v Z1=$T1 -v Z2=$T2 -f Task14.sql
echo "========== Task 14 Completed =========="
echo

echo "=============== Task 15 ==============="
time agens  -d disaster -v Z1=$T1 -v Z2=$T2 -v CLON=-118.0614431 -v CLAT=34.068509 -f Task15.sql
echo "========== Task 15 Completed =========="
echo

echo "=============== Task 16 ==============="
time agens -d disaster -v Z1=3 -v Z2=4 -f Task16.sql
echo "========== Task 16 Completed =========="
echo