USERNAME= 
PASSWORD=

echo "T1 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task1.js  
echo "T1 finished ======\n"

echo "T2 ===============" 
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task2_1.js  
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task2_2.js  
echo "T2 finished ======\n"

echo "T3 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task3.js
echo "T3 finished ======\n"

echo "T4 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task4.js
echo "T4 finished ======\n"

echo "T5 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task5.js
echo "T5 finished ======\n"

echo "T6 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task6.js
echo "T6 finished ======\n"

echo "T7 ===============" 
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task7.js
echo "T7 finished ======\n" 

echo "T8 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task8_1.js
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task8_1.js
echo "T8 finished ======\n"


echo "T9 ==============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task9_1.js
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task9_2.js
echo "T9 finished ======\n"


echo "T10 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task10.js
echo "T10 finished =====\n"


echo "T11 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task11_1.js
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task11_2.js
echo "T11 finished =====\n"


echo "T12 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task12.js
echo "T12 finished =====\n" 


echo "T13 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task13.js
echo "T13 finished =====\n" 

echo "T14 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task14.js
echo "T14 finished =====\n" 

echo "T15 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task15.js
echo "T15 finished =====\n"

echo "T16 =============="
time arangosh --server.database Ecommerce --server.username $USERNAME --server.password $PASSWORD  --javascript.execute  Task16.js
echo "T16 finished =====\n"

