/*
	file_io.c

 	file handling

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

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef _WIN32
# include <pwd.h>
#endif

#include "wm_error.h"
#include "file_io.h"

unsigned char *
WM_BufferFile (const char *filename, unsigned long int *size) {
	int buffer_fd;
	unsigned char *data;
	struct stat buffer_stat;
#ifndef _WIN32
    char *home = NULL;
	struct passwd *pwd_ent;
	char buffer_dir[1024];
#endif

	char *buffer_file = malloc(strlen(filename) + 1);

	if (buffer_file == NULL) {
		WM_ERROR(__func__, __LINE__, WM_ERR_MEM, NULL, errno);
		WM_ERROR(__func__, __LINE__, WM_ERR_LOAD, filename, errno);
		return NULL;
	}

	strcpy (buffer_file, filename);
	if (buffer_file[0] == '|') {
		void *addr;
		char *addr_ptr, *size_ptr, *name_ptr;

		addr_ptr = &(buffer_file[1]);
		size_ptr = strchr(addr_ptr, '|');
		*size_ptr = 0;
		size_ptr++;
		name_ptr = strchr(size_ptr, '|');
		*name_ptr = 0;
		name_ptr++;

		addr = (void *)atoi(addr_ptr);
		*size = atoi(size_ptr);

		data = malloc(*size);
		if (data == NULL) {
			WM_ERROR(__func__, __LINE__, WM_ERR_MEM, NULL, errno);
			WM_ERROR(__func__, __LINE__, WM_ERR_LOAD, name_ptr, errno);
			free(buffer_file);
			return NULL;
		}

		memcpy(data, addr, *size);

		free(buffer_file);
		return data;
	}
#ifndef _WIN32
	if (strncmp(buffer_file,"~/",2) == 0) {
		if ((pwd_ent = getpwuid (getuid ()))) {
			home = pwd_ent->pw_dir;
		} else {
			home = getenv ("HOME");
		}
		if (home) {
			buffer_file = realloc(buffer_file,(strlen(buffer_file) + strlen(home) + 1));
			if (buffer_file == NULL) {
				WM_ERROR(__func__, __LINE__, WM_ERR_MEM, NULL, errno);
				WM_ERROR(__func__, __LINE__, WM_ERR_LOAD, filename, errno);
				free(buffer_file);
				return NULL;
			}
			memmove((buffer_file + strlen(home)), (buffer_file + 1), (strlen(buffer_file)));
			memcpy (buffer_file, home,strlen(home));
		}
	} else if (buffer_file[0] != '/') {
		char* cwdresult = getcwd(buffer_dir, 1024);
		if (buffer_dir[strlen(buffer_dir)-1] != '/') {
			buffer_dir[strlen(buffer_dir)+1] = '\0';
			buffer_dir[strlen(buffer_dir)] = '/';
		}
		buffer_file = realloc(buffer_file,(strlen(buffer_file) + strlen(buffer_dir) + 1));
		if (buffer_file == NULL || cwdresult == NULL) {
			WM_ERROR(__func__, __LINE__, WM_ERR_MEM, NULL, errno);
			WM_ERROR(__func__, __LINE__, WM_ERR_LOAD, filename, errno);
			free(buffer_file);
			return NULL;
		}
		memmove((buffer_file + strlen(buffer_dir)), buffer_file, strlen(buffer_file)+1);
		memcpy (buffer_file,buffer_dir,strlen(buffer_dir));
	}
#endif
	if (stat(buffer_file,&buffer_stat)) {
		WM_ERROR(__func__, __LINE__, WM_ERR_STAT, filename, errno);
		free(buffer_file);
		return NULL;
	}
	*size = buffer_stat.st_size;
	data = malloc(*size);
	if (data == NULL) {
		WM_ERROR(__func__, __LINE__, WM_ERR_MEM, NULL, errno);
		WM_ERROR(__func__, __LINE__, WM_ERR_LOAD, filename, errno);
		free(buffer_file);
		return NULL;
	}
#ifdef _WIN32
	if ((buffer_fd = open(buffer_file,(O_RDONLY | O_BINARY))) == -1) {
#else
	if ((buffer_fd = open(buffer_file,O_RDONLY)) == -1) {
#endif
		WM_ERROR(__func__, __LINE__, WM_ERR_OPEN, filename, errno);
		free(buffer_file);
		free(data);
		return NULL;
	}
	if (read(buffer_fd,data,*size) != buffer_stat.st_size) {
		WM_ERROR(__func__, __LINE__, WM_ERR_READ, filename, errno);
		free(buffer_file);
		free(data);
		close(buffer_fd);
		return NULL;
	}
	close(buffer_fd);
	free(buffer_file);
	return data;
}

