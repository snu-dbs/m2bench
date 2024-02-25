USERNAME=
PASSWORD=

echo "T0 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task0_1.js  
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task0_2.js  
echo "T0 finished ======"
echo
echo

echo "T1 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task1.js  
echo "T1 finished ======"
echo
echo

echo "T2 ===============" 
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task2_1.js  
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task2_2.js  
echo "T2 finished ======"
echo
echo

echo "T3 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task3.js
echo "T3 finished ======"

echo
echo


echo "T4 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task4.js
echo "T4 finished ======"

echo
echo

echo "T5 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task5.js
echo "T5 finished ======"

echo
echo

echo "T6 ==============="
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task6.js
echo "T6 finished ======"

echo
echo

echo "T7 ===============" 
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task7.js
echo "T7 finished ======" 

echo
echo

echo "T8 ==============="
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task8_1.js
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task8_1.js
echo "T8 finished ======"

echo
echo


echo "T9 ==============="
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task9_1.js
time arangosh --server.database Healthcare --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task9_2.js
echo "T9 finished ======"

echo
echo


echo "T10 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task10.js
echo "T10 finished ====="


echo
echo


echo "T11 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task11_1.js
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task11_2.js
echo "T11 finished ====="


echo
echo


echo "T12 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task12.js
echo "T12 finished =====" 


echo
echo


echo "T13 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task13.js
echo "T13 finished =====" 

echo
echo

echo "T14 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task14.js
echo "T14 finished =====" 

echo
echo

echo "T15 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task15.js
echo "T15 finished ====="

echo
echo

echo "T16 =============="
time arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --server.request-timeout 7200  --javascript.execute  Task16.js
echo "T16 finished ====="

echo
echo



