#!/bin/bash
trap "echo Exited!; exit;" SIGINT
for X in $(find src/* -maxdepth 0 -type d); do
  X=$(basename $X)
  if [ ! -e "src/$X/bindings" ]; then
    echo "No bindings: $X"
    continue
  fi
  if [[ -e "src/$X/bindings/modulegen__gcc_ILP32.py" && "src/$X/bindings/modulegen__gcc_LP64.py"  ]]; then
    echo "Already done: $X"
    continue
  fi
  ./waf --apiscan=$X 2> /dev/null > /dev/null
  if [ $? -ne 0 ]; then
    #echo "Trying again..."
    ./waf --apiscan=$X 2> /dev/null > /dev/null
    if [ $? -ne 0 ]; then
      echo "FAILED: $X"
      continue
    fi
  fi
  echo "Success: $X"
done
