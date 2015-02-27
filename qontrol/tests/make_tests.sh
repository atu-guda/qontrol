#!/bin/bash

all_rc=0

for i in *.qol ; do
  base="${i%.qol}"
  test_fun="${base}();"
  echo "RUN: ../qontrol -M -d -b -e -x $test_fun $i"
  ../qontrol -M -b -e -x "$test_fun" "$i" ; rc=$?
  if [[ "$rc" -ne 0 ]] ; then
    echo "*************** ERROR DETECTED in \"$i\" -------------------"
  fi
  all_rc=$(( all_rc | rc ))
done

if [[ "$all_rc" -ne 0 ]] ; then
  echo "*************** ERRORS DETECTED *******************"
fi

exit "$all_rc"

