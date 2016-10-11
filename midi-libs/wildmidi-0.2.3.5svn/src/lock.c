/*
    lock.c - data locking code for lib
    
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

#ifdef _WIN32
#include <windows.h>
#else
#define _GNU_SOURCE
#include <unistd.h>
#endif

#include "lock.h"

/*
    WL_Lock(wmlock)

	wm_lock = a pointer to a value

	returns nothing

	Attemptes to set a lock on the MDI tree so that
	only 1 library command may access it at any time.
	If lock fails the process retries untill successful.

*/

void
WM_Lock (int * wmlock) {
	LOCK_START:
	/*
		Check if lock is clear, if so set it
	*/
	if (__builtin_expect(((*wmlock) == 0),1)) {
		(*wmlock)++;
		/*
			Now that the lock is set, make sure we don't have a race condition.
			If so decrement the lock by one and retry
		*/
		if (__builtin_expect(((*wmlock) == 1), 1)) {
			/*
				Lock cleanly set
			*/
			return;
		}
		(*wmlock)--;
	}
#ifdef _WIN32
	Sleep(10);
#else
	usleep(500);
#endif
	goto LOCK_START;
}

/*
    WL_Unlock(wmlock)

	wm_lock = a pointer to a value

	returns nothing

	Removes a lock previously placed on the MDI tree.
*/


void
WM_Unlock (int *wmlock) {
	/*
		We don't want a -1 lock so just to make sure
	*/
	if ((*wmlock) != 0) {
		(*wmlock)--;
	}
}
