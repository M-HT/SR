#!/bin/sh

UFO_CFG=Ufo.cfg

INTRO_PLAY=on

GEOSCAPE=./SR-Geoscape
TACTICAL=./SR-Tactical
RESULT=0

read_cfg () {
    if [ -f "$UFO_CFG" ]
    then
        INTRO_PLAY=`grep -x ^Intro=.*$ $UFO_CFG | head -n1 | sed 's/^Intro=//;s/\\r$//'`

        if [ "x$INTRO_PLAY" = "xoff" ]
        then
            INTRO_PLAY=
        else
            INTRO_PLAY=on
        fi
    else
        INTRO_PLAY=on
    fi
}

cd "`echo $0 | sed 's/\/[^\/]*$//'`"

read_cfg

export LD_LIBRARY_PATH=`pwd`

if [ -n "$INTRO_PLAY" ]
then
    ./SR-Intro
fi

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

