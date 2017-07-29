#! /bin/sh

cd "`echo $0 | sed 's/\/[^\/]*$//'`"

export LD_LIBRARY_PATH=`pwd`

./SR-War
sync
