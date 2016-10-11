/*
	wm_error.c

 	error reporting

    Copyright (C) Chris Ison 2001-2011

    This file is part of WildMIDI.

    WildMIDI is free software: you can redistribute and/or modify the player
    under the terms of the GNU General Public License and you can redistribute
    and/or modify the library under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation, either version 3 of
    the licenses, or(at your option) any later version.

    WildMIDI is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License and
    the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License and the
    GNU Lesser General Public License along with WildMIDI.  If not,  see
    <http://www.gnu.org/licenses/>.

    Email: wildcode@users.sourceforge.net
*/

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "wm_error.h"

void
WM_ERROR_NEW (const char * wmfmt, ...) {
	va_list		args;
	fprintf(stderr,"\r");
    va_start (args, wmfmt);
	vfprintf (stderr, wmfmt, args);
	va_end (args);
	fprintf(stderr,"\n");
}

void
WM_ERROR( const char * func, unsigned long int lne, int wmerno, const char * wmfor, int error) {
	static const char * errors[] = {
		"Unable to obtain memory\0",
		"Unable to stat\0",
		"Unable to load\0",
		"Unable to open\0",
		"Unable to read\0",
		"Invalid or Unsuported file format\0",
		"File corrupt\0",
		"Library not Initialized\0",
		"Invalid argument\0"
	};
	if (wmfor != NULL) {
		if (error != 0) {
			fprintf(stderr,"\rlibWildMidi(%s:%lu): ERROR %s %s (%s)\n",func, lne, errors[wmerno], wmfor, strerror(error));
		} else {
			fprintf(stderr,"\rlibWildMidi(%s:%lu): ERROR %s %s\n",func, lne, errors[wmerno], wmfor);
		}
	} else {
		if (error != 0) {
			fprintf(stderr,"\rlibWildMidi(%s:%lu): ERROR %s (%s)\n",func, lne, errors[wmerno], strerror(error));
		} else {
			fprintf(stderr,"\rlibWildMidi(%s:%lu): ERROR %s\n",func, lne, errors[wmerno]);
		}
	}

}


