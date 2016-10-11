/*
	wm_error.h

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

#ifndef __WM_ERROR_H
#define __WM_ERROR_H

#define WM_ERR_MEM		0
#define WM_ERR_STAT		1
#define WM_ERR_LOAD		2
#define WM_ERR_OPEN		3
#define WM_ERR_READ		4
#define WM_ERR_INVALID		5
#define WM_ERR_CORUPT		6
#define WM_ERR_NOT_INIT		7
#define WM_ERR_INVALID_ARG	8
#define WM_ERR_ALR_INIT     9

extern void WM_ERROR_NEW (const char * wmfmt, ...)
#ifdef __GNUC__
		__attribute__((format(printf, 1, 2))) // for tracking down formatting issues
#endif
		;
extern void WM_ERROR( const char * func, unsigned long int lne, int wmerno, const char * wmfor, int error);

#endif //__WM_ERROR_H
