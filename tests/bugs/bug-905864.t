#!/bin/bash

. $(dirname $0)/../include.rc

cleanup;

## Start and create a volume
TEST glusterd;
TEST pidof glusterd;
TEST $CLI volume info;

TEST $CLI volume create $V0 replica 2 stripe 2 $H0:$B0/${V0}{1,2,3,4,5,6,7,8};
TEST $CLI volume start $V0;

TEST glusterfs --entry-timeout=0 --attribute-timeout=0 -s $H0 --volfile-id $V0 $M0;
TEST glusterfs --entry-timeout=0 --attribute-timeout=0 -s $H0 --volfile-id $V0 $M1;

TEST touch $M0/file1;

#following C program tries open up race(s) if any, in F_GETLK/F_SETLKW codepaths
#of locks xlator
gcc -lpthread -g3 $(dirname $0)/bug-905864.c -o $(dirname $0)/bug-905864
$(dirname $0)/bug-905864 $M0/file1 &
$(dirname $0)/bug-905864 $M1/file1;
wait
rm -f $(dirname $0)/bug-905864

cleanup

