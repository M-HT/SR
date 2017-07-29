#!/bin/sh

GEOSCAPE=./SR-Geoscape
TACTICAL=./SR-Tactical
RESULT=0

cd "`echo $0 | sed 's/\/[^\/]*$//'`"

export LD_LIBRARY_PATH=`pwd`

$GEOSCAPE "0"
RESULT=$?
sync

if test -e $GEOSCAPE & test -x $GEOSCAPE & test -e $TACTICAL & test -x $TACTICAL
then
	while test $RESULT -ge 2
	do
		$TACTICAL "1"
		sync
		$GEOSCAPE "1"
		RESULT=$?
		sync
	done
fi

