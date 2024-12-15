#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
export LD_LIBRARY_PATH="`pwd`"
export LIBQUICKTIME_PLUGIN_DIR="`pwd`"
export WINEPREFIX="`pwd`/wine"

if [ -z "$WINEPREFIX" ]
then
    mkdir "$WINEPREFIX"
fi

# Uncomment and set SDI_INSTALL_PATH to contain path to the directory where the game is installed (in case this file is not inside the directory)
# export SDI_INSTALL_PATH="/path/to/game/install/directory"

# Uncomment and set SDI_CD_PATH to contain path to the directory where the game CDs are copied (in case it's not in the DATA subdirectory of the directory where the game is installed)
# export SDI_CD_PATH="/path/to/game/cd/directory"

# Set SDI_LANG to change the language in game (text only)
# 0 = german (deutsch)
# 1 = english
# 2 = french (français) - use only when your CD contains french resources (i.e. FRA directory)
SDI_LANG=1

wine SR-BI3.exe.so -$SDI_LANG
sync
