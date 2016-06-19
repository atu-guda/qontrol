#!/bin/bash
echo ' comment next line to use'
exit 1

fl=$( find . -name '*.qol' -type f )

for f in $fl ; do
  echo "======================= $f ======================="
  qontrol -c "$f" -N "$f"
done

