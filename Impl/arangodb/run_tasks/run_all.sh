#!/bin/bash

USERNAME=
PASSWORD=

echo "=============== Task 0 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task0_1.js
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task0_2.js
echo "========== Task 0 Completed =========="
echo

echo "=============== Task 2 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task2_1.js
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task2_2.js
echo "========== Task 2 Completed =========="
echo

echo "=============== Task 9 ==============="
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task9_1.js
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task9_2.js
echo "========== Task 9 Completed =========="
echo

echo "=============== Task 14 ==============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task14.js
echo "========== Task 14 Completed =========="
echo

echo "=============== Task 15 ==============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task15.js
echo "========== Task 15 Completed =========="
echo

echo "=============== Task 16 ==============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200 --javascript.execute Task16.js
echo "========== Task 16 Completed =========="
echo
