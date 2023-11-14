#! /bin/sh

FILE_RESULT=

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


find_file "." "septerra.mft"

if [ -z "$FILE_RESULT" ]
then
    echo Septerra Core game not found
    zenity --error --text="Septerra Core game not found" --timeout=10

    exit 1
fi

export LD_LIBRARY_PATH="`pwd`"
export LIBQUICKTIME_PLUGIN_DIR="`pwd`"

./SR-Septerra
sync
