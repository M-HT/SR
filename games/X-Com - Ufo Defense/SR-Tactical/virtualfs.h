/**
 *
 *  Copyright (C) 2016 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#if !defined(_VIRTUALFS_H_INCLUDED_)
#define _VIRTUALFS_H_INCLUDED_

typedef struct _file_entry_ {
    char dos_name[13], real_name[13];
    char *dos_fullname, *real_fullname;
    int attributes, dir_visited;
    struct _file_entry_ *parent, *next, *prev, *first_child;
} file_entry;

int vfs_init(int relative);
void vfs_delete_entry(file_entry *entry);
void vfs_visit_dir(file_entry *dir);
int vfs_get_real_name(const char *origdosname, char *buf, file_entry **realdir);
void vfs_add_file(file_entry *dir, const char *filepath);

#endif /* _VIRTUALFS_H_INCLUDED_ */
