#! /bin/sh

ALBION_CFG=Albion.cfg

SETUP_INI=
ALBION_EXE=
ALBIONCD=
ALBIONCD_ROOT=

FILE_RESULT=
INTRO_PLAY=on

find_file () {
    if [ -e "$1/$2" ]
    then
        FILE_RESULT="$2"
        return
    fi

    TEMP_FILE_UPPER=`echo "$2" | tr '[:lower:]' '[:upper:]'`

    if [ -e "$1/$TEMP_FILE_UPPER" ]
    then
        FILE_RESULT="$TEMP_FILE_UPPER"
        return
    fi

    TEMP_FILE_LOWER=`echo "$2" | tr '[:upper:]' '[:lower:]'`

    if [ -e "$1/$TEMP_FILE_LOWER" ]
    then
        FILE_RESULT="$TEMP_FILE_LOWER"
        return
    fi

    TEMP_FILE_CAMEL="${TEMP_FILE_UPPER#?}"
    TEMP_FILE_CAMEL="${TEMP_FILE_UPPER%%$TEMP_FILE_CAMEL}${TEMP_FILE_LOWER#?}"

    if [ -e "$1/$TEMP_FILE_CAMEL" ]
    then
        FILE_RESULT="$TEMP_FILE_CAMEL"
        return
    fi

    FILE_RESULT=
}

read_cfg () {
    if [ -f "$ALBION_CFG" ]
    then
        INTRO_PLAY=`grep -x ^Intro=.*$ $ALBION_CFG | head -n1 | sed 's/^Intro=//;s/\\r$//'`

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

play_smk () {
    find_file "." "$1"
    if [ -n "$FILE_RESULT" ]
    then
        ./smk_play "./$FILE_RESULT"
        return
    fi

    find_file "." "VIDEO"
    if [ -n "$FILE_RESULT" ]
    then
        VIDEO="$FILE_RESULT"

        find_file "./$VIDEO" "$1"
        if [ -n "$FILE_RESULT" ]
        then
            ./smk_play "./$VIDEO/$FILE_RESULT"
            return
        fi
    fi

    if [ -n "$ALBIONCD" ]
    then
        find_file "./$ALBIONCD" "$1"
        if [ -n "$FILE_RESULT" ]
        then
            ./smk_play "./$ALBIONCD/$FILE_RESULT"
            return
        fi

        find_file "./$ALBIONCD" "VIDEO"
        if [ -n "$FILE_RESULT" ]
        then
            VIDEO="$FILE_RESULT"

            find_file "./$ALBIONCD/$VIDEO" "$1"
            if [ -n "$FILE_RESULT" ]
            then
                ./smk_play "./$ALBIONCD/$VIDEO/$FILE_RESULT"
                return
            fi
        fi
    fi
}

cd "`echo $0 | sed 's/\/[^\/]*$//'`"

find_file "." "SETUP.ini"
SETUP_INI="$FILE_RESULT"

if [ -n "$SETUP_INI" ]
then
    find_file "." "ALBION.exe"
    ALBION_EXE="$FILE_RESULT"

    ALBIONCD=`grep -x ^SOURCE_PATH=.*$ "./$SETUP_INI" | head -n1 | sed 's/^SOURCE_PATH=//;s/\\r$//;s/\\\\/\\//g;s/\/$//'`

    find_file "." "$ALBIONCD"
    ALBIONCD="$FILE_RESULT"

    if [ -z "$ALBION_EXE" -a -n "$ALBIONCD" ]
    then
        find_file "./$ALBIONCD" "ROOT"
        if [ -n "$FILE_RESULT" ]
        then
            ALBIONCD_ROOT="$FILE_RESULT"

            find_file "./$ALBIONCD/$ALBIONCD_ROOT" "ALBION.exe"
            if [ -n "$FILE_RESULT" ]
            then
                cp "./$ALBIONCD/$ALBIONCD_ROOT/$FILE_RESULT" "./"

                find_file "." "ALBION.exe"
                ALBION_EXE="$FILE_RESULT"
            fi
        fi
    fi
fi


if [ -z "$SETUP_INI" -o -z "$ALBION_EXE" ]
then
    echo Albion game not found
    zenity --error --text="Albion game not found" --timeout=10

    exit 1
fi


find_file "." "SAVES"
if [ -z "$FILE_RESULT" ]
then
    mkdir "./SAVES"
fi

find_file "." "XLDLIBS"
XLDLIBS="$FILE_RESULT"

if [ -n "$XLDLIBS" ]
then
    find_file "./$XLDLIBS" "CURRENT"
    if [ -z "$FILE_RESULT" ]
    then
        mkdir "./$XLDLIBS/CURRENT"
    fi
fi

read_cfg

export LD_LIBRARY_PATH=`pwd`

if [ -n "$INTRO_PLAY" ]
then
    play_smk "INTRO.smk"
fi

while :
do
    ./SR-Main
    RESULT=$?
    sync

    if [ $RESULT -eq 2 ]
    then
        play_smk "INTRO.smk"
    elif [ $RESULT -eq 3 ]
    then
        play_smk "CREDITS.smk"
    else
        exit 0
    fi
done
