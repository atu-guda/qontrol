#!/bin/bash
# usage bk_to_remake.sh TARGET_DIR
tdir="$1"
if [[ ! -d "$tdir" ]] ; then
  echo "Target dir \"$tdir\" not exist"
  exit 1
fi
if [[ ! -w "$tdir" ]] ; then
  echo "Target dir \"$tdir\" not writable"
  exit 1
fi

for p in $( find . -name '*.qol' ) ; do
  f=$( basename "$p" )
  d=$( dirname "$p" )
  td="$tdir"'/'"$d"
  echo "$p  $d  $f $td"
  mkdir -p "$td"
  cp -a "$p" "$td"
done


