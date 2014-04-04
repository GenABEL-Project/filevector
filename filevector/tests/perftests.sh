#!/bin/bash -v

#measure times for different files and different cache sizes

READSPEED=`dirname $0`/../bin/readspeed
LOGFILE=`dirname $0`/perftest.log
rm $LOGFILE

TIME="/usr/bin/time -o $LOGFILE -a "
echo ============= reading small file
echo ============= cache 32
$TIME $READSPEED data/small/ERF.merlin.22.collected.ped.out.mldose 32

echo ============= cache 64
$TIME $READSPEED data/small/ERF.merlin.22.collected.ped.out.mldose 64

echo ============= cache 128
$TIME $READSPEED data/small/ERF.merlin.22.collected.ped.out.mldose 128

echo ============= cache 256
$TIME $READSPEED data/small/ERF.merlin.22.collected.ped.out.mldose 256

echo ============= reading BIG file
echo ============= cache 32
$TIME $READSPEED data/ERF.merlin.1-22.collected.ped.out.mldose 32

echo ============= cache 64
$TIME $READSPEED data/ERF.merlin.1-22.collected.ped.out.mldose 64

echo ============= cache 128
$TIME $READSPEED data/ERF.merlin.1-22.collected.ped.out.mldose 128

echo ============= cache 256
$TIME $READSPEED data/ERF.merlin.1-22.collected.ped.out.mldose 256