dnl Copyright (C) Chris Ison 2001-2011
dnl Some parts (C) Quakeforge 2004
dnl
dnl This file is part of WildMidi
dnl
dnl WildMIDI is free software: you can redistribute and/or modify the player under the terms of the GNU General Public License and you can redistribute and/or modify the library under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the licenses, or(at your option) any later version.
dnl
dnl WildMIDI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License and the GNU Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License and the GNU Lesser General Public License along with WildMIDI. If not, see <http://www.gnu.org/licenses/>.
dnl

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([WildMidiA],[0.2.3.5],[http://sourceforge.net/tracker/?group_id=42635&atid=433744],[wildmidi],[http://wildmidi.sourceforge.net])
AC_REVISION([$Revision: 1.16 $])
AC_CONFIG_SRCDIR([src/wildmidi_lib.c])
AC_CONFIG_HEADERS([include/config.h])

AM_INIT_AUTOMAKE

# set libtool versioning
# +1 :  0 : +1   == new interface that does not break old one.
# +1 :  0 :  0   == changed/removed an interface. Breaks old apps.
#  ? : +1 :  ?   == internal changes that doesn't break anything.
# CURRENT : REVISION : AGE
LT_CURRENT=2
LT_REVISION=0
LT_AGE=1

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_LANG([C])

if test ${CFLAGS-unset} = unset; then
	CFLAGS=""
fi

if test ${LDFLAGS-unset} = unset; then
	LDFLAGS=""
fi


if test ${AR-unset} = unset; then
	AR="ar"
fi
export AR

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_RANLIB
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL


set $CC
if test "$1" = gcc; then
	shift
	args="$*"
	AC_MSG_CHECKING(for gcc version)
	CCVER="gcc `gcc --version`"
	AC_MSG_RESULT($CCVER)
	CFLAGS="$CFLAGS -std=c99 -U__STRICT_ANSI__"
fi

LDFLAGS="$LDFLAGS -lm -lc"

case "$host_os" in
	mingw32*)
		mingw=yes
		CFLAGS="$CFLAGS -I\$(top_srcdir)/include/win32"
		LDFLAGS="$LDFLAGS -luser32 -lgdi32 -lwinmm"
		if test "x$host" != "x$build"; then
			case "$build_os" in
				cygwin*)
					CFLAGS="$CFLAGS -mno-cygwin -mconsole"
					LDFLAGS="$LDFLAGS -L/usr/lib/w32api $LDFLAGS"
					CPPFLAGS="$CPPFLAGS $CFLAGS"
					;;
				*)
					CC="$host_cpu-$host_os-gcc"
					AS="$CC"
					AR="$host_cpu-$host_os-ar"
					RANLIB="$host_cpu-$host_os-ranlib"
					;;
			esac
		fi
		endian="little"
		;;
	cygwin*)
		cygwin=yes
		if test "x$host" != "x$build"; then
			CC="$host_cpu-$host_os-gcc"
			AS="$CC"
			AR="$host_cpu_$host_os-ar"
			RANLIB="$host_cpu-$host_os-ranlib"
			endian="little"
		fi
	;;
esac

AC_CHECK_HEADERS(
	alsa/asoundlib.h ctype.h errno.h fcntl.h getopt.h linux/soundcard.h machine/soundcard.h math.h mmsystem.h pwd.h \
      stdarg.h stdio.h stdlib.h string.h sys/ioctl.h sys/mman.h sys/soundcard.h sys/stat.h sys/types.h \
      termios.h unistd.h windows.h
)

AC_C_BIGENDIAN

CFLAGS="$CFLAGS -Wall -Wextra -pedantic"

AC_ARG_ENABLE([werror],
    [AS_HELP_STRING([--disable-werror],
        [disable fail on compile warnings])],
    [werror=$enable_werror],
	[werror=yes]
)
if test "x$werror" = xyes; then
	CFLAGS="$CFLAGS -Werror"
fi


AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [compile with debugging])],
	[debug=$enable_debug],
	[debug=no]
)
if test "x$debug" = xyes; then
	BUILD_TYPE="$BUILD_TYPE Debug"
	CFLAGS="$CFLAGS -g"
fi


AC_ARG_ENABLE([profile],
	[AS_HELP_STRING([--enable-profile],
        [compile with profiling (for development)])],
	[profile=$enable_profile],
	[profile=no]
)
if test "x$profile" = xyes; then
	BUILD_TYPE="$BUILD_TYPE Profile"
	CFLAGS="`echo $CFLAGS | sed -e 's/-fomit-frame-pointer//g'` -pg -fprofile-arcs"
	LDFLAGS="$LDFLAGS -pg -fprofile-arcs"
fi


AC_ARG_ENABLE([optimize],
    [AS_HELP_STRING([--disable-optimize],
        [compile without optimizations (for development)])],
    [optimize=$enable_optimize],
	[optimize=yes]
)
if test "x$optimize" = xyes; then
	if test "x$GCC" = xyes; then
		BUILD_TYPE="$BUILD_TYPE Optimized"
		CFLAGS="$CFLAGS -O2 -std=gnu99 -frename-registers -finline-limit=32000 -Winline -ffast-math -funroll-loops -fexpensive-optimizations -finline-functions"
    else
		CFLAGS="$CFLAGS -O2"
	fi
fi


AC_ARG_ENABLE([temps],
	[AS_HELP_STRING([--enable-temps],
        [compile with save-temps])],
	[temps=$enable_temps],
	[temps=no]
)
if test "x$temps" = xyes; then
	CFLAGS="$CFLAGS -save-temps"
fi


AC_MSG_CHECKING(for __builtin_expect)
AC_TRY_COMPILE(
	[long (*foo) (long, long) = __builtin_expect;],
	[],
	AC_DEFINE(HAVE___BUILTIN_EXPECT)
	AC_MSG_RESULT(yes),
	AC_MSG_RESULT(no)
)

AH_VERBATIM([HAVE___BUILTIN_EXPECT],
[/* Define this if the GCC __builtin_expect keyword is available */
#undef HAVE___BUILTIN_EXPECT
#ifndef HAVE___BUILTIN_EXPECT
# define __builtin_expect(x,c) x
#endif])

dnl CFLAGS for release and devel versions
AC_ARG_WITH(efence,
        [  --with-efence           link with -lefence],
		AC_CHECK_LIB(efence,malloc,LDFLAGS="$LDFLAGS -lefence")
)


case "$host_os" in
	*mingw*)
		default_wildmidiconf="wildmidi.cfg"
		;;
	*freebsd*)
		default_timifityconf="/usr/local/share/wildmidi/wildmidi.cfg"
		;;
	*)
		default_wildmidiconf="/etc/wildmidi.cfg"
		;;
esac

AC_ARG_WITH(wildmidi-cfg,
[  --with-wildmidi-cfg=FILE  If set will change the name and location of the
                          wildmidi config file used by WildMidi.  Defaults to
                          ./wildmidi.cfg - win32 mingw
                          /usr/local/share/wildmidi/wildmidi.cfg - FreeBSD
                          /etc/wildmidi.cfg - everything else],
wildmidiconf="$withval", wildmidiconf="auto")
if test "x$wildmidiconf" = "xauto" || test "x$wildmidiconf" = "xyes" || \
	test "x$wildmidiconf" = "xno"; then  dnl yes/no sanity checks
	wildmidiconf="$default_wildmidiconf"
fi

AC_DEFINE_UNQUOTED(WILDMIDI_CFG, "$wildmidiconf", [Define this to the location of the wildmidi config file])


AC_ARG_WITH(amd,
    [  --with-amd              Optimize for AMD processors instead of Intel],
	HAVE_AMD="yes",
	HAVE_AMD="no"
)


AC_ARG_WITH(arch,
	[  --with-arch=ARCH        control compiler architecture directly],
	arch="$withval", arch=auto
)

case "$arch" in
    no)
        ;;
	auto)
		case "${host_cpu}" in
			i?86)
				if test "x$HAVE_AMD" = "xyes"; then
					CFLAGS="$CFLAGS -march=k6"
				else
					CFLAGS="$CFLAGS -march=${host_cpu}"
				fi
				;;
		esac
		;;
	*)
		CFLAGS="$CFLAGS -march=$arch"
		;;

esac

case "$host_os" in
        *mingw*)
                AUDDR="-lwinmm"
        ;;
        *)
		USE_ALSA=no
		USE_OSS=no
		AC_CHECK_HEADERS(alsa/asoundlib.h)
		if test "x$ac_cv_header_alsa_asoundlib_h" = "xyes"; then
			AC_CHECK_LIB(asound,snd_pcm_mmap_commit,USE_ALSA=yes)
                        AC_ARG_WITH(oss,[  --with-oss              use oss instead of alsa (not required on systems]
					[          that don't have alsa sound drivers installed)], USE_OSS=yes)

			if test "x$USE_ALSA" = xyes ; then
				if test "x$USE_OSS" = xno ; then
					AUDDR="-lasound"
					AC_DEFINE(HAVE_ALSA, 1, [Define this if your system uses alsa])
					AC_TRY_COMPILE(
						[#include <alsa/asoundlib.h>],
						[void f(void) {
							snd_pcm_t *pcm;
							int rate = 44100;
							snd_pcm_hw_params_t *hw;
							int err;
							err = snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
						}],
						AC_DEFINE(ALSA_NEW, 1, [Define this if using Alsa 1.0.1 or higher])
					)
				fi
			fi
		fi
       ;;
esac


AC_SUBST(HAVE_ALSA)
AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(AUDDR)
AC_SUBST(AS)
AC_SUBST(AR)
AC_SUBST(RANLIB)

AC_OUTPUT(
 Makefile
 src/Makefile
 include/Makefile
 docs/Makefile

)

AC_MSG_RESULT([
	You may now type make to build WildMidi
])
