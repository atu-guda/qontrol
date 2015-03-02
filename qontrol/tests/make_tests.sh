#!/bin/bash

all_rc=0
failed=''

for i in *.qol ; do
  base="${i%.qol}"
  echo "RUN: ../qontrol -M -d -b -e -x test0() $i"
  ../qontrol -M -b -e -x "test0();" "$i" ; rc=$?
  if [[ "$rc" -ne 0 ]] ; then
    echo "*************** ERROR DETECTED in \"$i\" -------------------"
    failed+=" $i"
  fi
  all_rc=$(( all_rc | rc ))
done

if [[ "$all_rc" -ne 0 ]] ; then
  echo "*************** ERRORS DETECTED *******************"
  echo "failed: $failed"
fi

exit "$all_rc"

