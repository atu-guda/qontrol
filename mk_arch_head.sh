#!/bin/bash
dstt=$( date '+%Y%m%d' )
proj='qmo2x'
git archive --format=tar --prefix=${proj}-${dstt}/ -o ../${proj}-${dstt}.tar HEAD
gzip ../${proj}-${dstt}.tar


