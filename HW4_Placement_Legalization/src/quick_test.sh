testcasePool="adaptec1 adaptec3 ibm01 ibm07 ibm09"

make clean
make
for filename in $testcasePool; do
  ../bin/hw4 "../testcase/${filename}/${filename}.aux" "../output/${filename}.result"
done