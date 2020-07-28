## Copyright (C) Chris Ison 2001-2011
##
## This file is part of WildMidi
##
## WildMIDI is free software: you can redistribute and/or modify the player under the terms of the GNU General Public License and you can redistribute and/or modify the library under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the licenses, or(at your option) any later version.
##
## WildMIDI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License and the GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU General Public License and the GNU Lesser General Public License along with WildMIDI. If not, see <http://www.gnu.org/licenses/>.
##

AUTOMAKE_OPTIONS=	foreign
INCLUDES=		-I$(top_srcdir)/include

lib_LTLIBRARIES=	libWildMidi.la
libWildMidi_la_SOURCES=		wm_error.c file_io.c reverb.c lock.c gus_pat.c wildmidi_lib.c
libWildMidi_la_CFLAGS=		$(PREFER_NON_PIC)
libWildMidi_la_LDFLAGS=		-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -shared $(LDFLAGS) -no-undefined -export-symbols-regex '^WildMidi.*'
libWildMidi_la_LIBADD=      -lm -lc

bin_PROGRAMS= wildmidi
wildmidi_libs=			libWildMidi.la
wildmidi_SOURCES=		wildmidi.c
wildmidi_LDADD=			$(AUDDR) $(LDFLAGS) $(wildmidi_libs)
wildmidi_DEPENDENCIES=	$(wildmidi_libs)

## wildmididebug_SOURCES=		wm_error.c file_io.c reverb.c lock.c gus_pat.c wildmidi_lib.c wildmidi.c
## wildmididebug_LDADD=		$(AUDDR) $(LDFLAGS)

