#!/bin/bash
dstt=$( date '+%Y%m%d' )
proj='qmo2'
git archive --format=tar --prefix=${proj}-${dstt}/ -o ../${proj}-${dstt}.tar HEAD
gzip ../${proj}-${dstt}.tar


