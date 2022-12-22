echo -n "Compile via make, run, and verify.(Press y): "
read ans
if [[ ${ans} == "y" ]]; then
  echo -n "filename (e.g. ibm01 or adaptec1 and so on): "
  read filename
  set -x
# make clean
  make
  ../bin/hw4 "../testcase/${filename}/${filename}.aux" "../output/${filename}.result"
  ../verifier/verify "../testcase/${filename}/${filename}.aux" "../output/${filename}.result"
  set +x
fi