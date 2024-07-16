echo "T0"
time agens -d ecommerce -f Task0.sql
echo
echo "T1"
time agens -d ecommerce -f Task1.sql
echo
echo "T2"
time agens -d ecommerce -f Task2.sql
echo
echo "T3"
time agens -d ecommerce -f Task3.sql
echo
echo "T4"
time agens -d ecommerce -f Task4.sql
echo
echo "T5"
time agens -d ecommerce -f Task5.sql
echo
echo "T6"
time agens -d healthcare -f Task6.sql
echo
echo "T7"
time agens -d healthcare -f Task7.sql
echo
echo "T8"
time agens -d healthcare -f Task8.sql
echo
echo "T9"
time agens -d healthcare -f Task9.sql
echo
echo "T10"
time agens -d disaster -f Task10.sql
echo
echo "T11"
time agens -d disaster -f Task11.sql
echo
echo "T12"
time agens -d disaster -f Task12.sql
echo
echo "T13"
time agens -d disaster -f Task13.sql
echo
T1=5
T2=10
echo "T14"
time agens -d disaster -v Z1=$T1 -v Z2=$T2 -f  Task14.sql
echo
echo "T15"
time agens  -d disaster -v Z1=$T1 -v Z2=$T2 -v CLON=-118.0614431 -v CLAT=34.068509 -f  Task15.sql
echo
echo "T16"
time agens -d disaster -f Task16.sql
echo




