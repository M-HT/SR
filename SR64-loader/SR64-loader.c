/**
 *
 *  Copyright (C) 2022-2025 Roman Pauer
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

#include <sys/syscall.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include "syscall64.h"


#define MEM_1MB (UINT64_C(0x00100000))
#define MEM_2GB (UINT64_C(0x80000000))
#define MEM_END (UINT64_C(0x800000000000))

#define STACK_SIZE (1024 * 1024)

#define FAILED_MMAP(x) (((x) == MAP_FAILED) || ((int64_t)(x) < 0 && (int64_t)(x) > -4096))


static void *fini_func;
static int64_t argc, envc, auxc;
static char **argv, **envp;
static Elf64_auxv_t *auxv;

static int64_t page_size;
static int default_huge_page_size_shift;
static int min_huge_page_size_shift;
static int max_huge_page_size_shift;
static uint64_t huge_page_size_set;

static uint64_t *stack32, *stack32_top;
static void *program_entry;


extern void _start3(void *func, void *stack, void *entry);


// string functions

static int str_startswith(const char *str, const char *start)
{
    for (; *start != 0; str++,start++)
    {
        if (*str != *start) return 0;
    }

    return 1;
}

static uint64_t str_readuint(const char *str, int *read_length)
{
    uint64_t value;
    int len;

    for (value = 0, len = 0; str[len] >= '0' && str[len] <= '9'; len++)
    {
        value = value * 10 + (str[len] - '0');
    }
    if (read_length != 0) *read_length = len;
    return value;
}

static uint64_t str_readhex(const char *str, int *read_length)
{
    uint64_t value;
    int len;

    for (value = 0, len = 0; ; len++)
    {
        if (str[len] >= '0' && str[len] <= '9')
        {
            value = value * 16 + (str[len] - '0');
        }
        else if (str[len] >= 'a' && str[len] <= 'f')
        {
            value = value * 16 + (str[len] + 10 - 'a');
        }
        else if (str[len] >= 'A' && str[len] <= 'F')
        {
            value = value * 16 + (str[len] + 10 - 'A');
        }
        else break;
    }
    if (read_length != 0) *read_length = len;
    return value;
}

static int str_length(const char *str)
{
    int len;

    for (len = 0; str[len] != 0; len++);

    return len;
}

static void mem_copy(void *dst, const void *src, int len)
{
    int index;

    for (index = 0; index < len; index++)
    {
        ((uint8_t *)dst)[index] = ((uint8_t *)src)[index];
    }
}


// syscalls
// A value in the range between -4095 and -1 indicates an error, it is -errno.

static void sys_exit(int status)
{
    syscall1(SYS_exit_group, status);
}

static int sys_openat(int dirfd, const char *pathname, int flags)
{
    return syscall3(SYS_openat, (intptr_t)dirfd, (uintptr_t)pathname, flags);
}

static int sys_close(int fd)
{
    return syscall1(SYS_close, fd);
}

static long int sys_getdents64(int fd, void *dirp, unsigned long int count)
{
    return syscall3(SYS_getdents64, fd, (uintptr_t)dirp, count);
}

static long int sys_read(int fd, void *buf, unsigned long int count)
{
    return syscall3(SYS_read, fd, (uintptr_t)buf, count);
}

static long int sys_write(int fd, const void *buf, unsigned long int count)
{
    return syscall3(SYS_write, fd, (uintptr_t)buf, count);
}

static long int sys_lseek(int fd, long int offset, int whence)
{
    return syscall3(SYS_lseek, fd, offset, whence);
}

static void *sys_mmap(void *addr, unsigned long int length, int prot, int flags, int fd, unsigned long int offset)
{
    return (void *)syscall6(SYS_mmap, (uintptr_t)addr, length, prot, flags, (intptr_t)fd, offset);
}

static int sys_munmap(void *addr, unsigned long int length)
{
    return syscall2(SYS_munmap, (uintptr_t)addr, length);
}

static int sys_mprotect(void *addr, unsigned long int len, int prot)
{
    return syscall3(SYS_mprotect, (uintptr_t)addr, len, prot);
}

static int sys_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
    return syscall5(SYS_prctl, option, arg2, arg3, arg4, arg5);
}


// print functions

static void pr_print(const char *text)
{
    sys_write(1, text, str_length(text));
}

#if 0
static void pr_printhex(uint64_t num)
{
    static const char hexchars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    char text[16];
    int len;

    if (num == 0)
    {
        text[15] = '0';
        len = 1;
    }
    else
    {
        for (len = 0; len < 16; len++)
        {
            text[15 - len] = hexchars[num & 0x0f];
            num >>= 4;
        }

        for (len = 16; text[16 - len] == '0'; len--);
    }

    sys_write(1, &(text[16 - len]), len);
}
#endif

static void pr_errprint(const char *text)
{
    sys_write(2, text, str_length(text));
}


// other functions

static __attribute__ ((noinline)) void read_stack(uint64_t *stack)
{
    int64_t index;

    if (sizeof(void *) != 8)
    {
        pr_errprint("Pointer size isn't 64 bits\n");
        sys_exit(1);
    }

// https://articles.manugarg.com/aboutelfauxiliaryvectors.html

    argc = stack[0];

    if (argc < 2)
    {
        pr_errprint("No input file\n");
        sys_exit(2);
    }

    argv = (char **) &(stack[1]);
    envp = (char **) &(stack[argc + 2]);
    for (index = 0; envp[index] != 0; index++);
    envc = index;
    auxv = (Elf64_auxv_t *) &(stack[argc + envc + 3]);
    for (index = 0; auxv[index].a_type != AT_NULL; index++);
    auxc = index;
}


static void read_default_hugepagesize(void)
{
    int fd;
    char buf[256];
    int len1, len2, ofs1, ofs2, shift;
    uint64_t default_huge_page_size_kb;

    default_huge_page_size_kb = 0;

    fd = sys_openat(AT_FDCWD, "/proc/meminfo", O_RDONLY);
    if (fd < 0) return;

    len1 = 0;
    len2 = 1;
    while (len2)
    {
        len2 = sys_read(fd, &(buf[len1]), sizeof(buf) - len1);
        if (len2 < 0) break; // error

        len1 += len2;
        if (len1 != sizeof(buf) && len2) continue; // buffer not full and not end of file

        if (len1 == 0) break; // empty buffer

        ofs1 = 0;
        while (1)
        {
            // skip empty lines
            while (ofs1 < len1 && (buf[ofs1] == '\r' || buf[ofs1] == '\n')) ofs1++;
            if (ofs1 >= len1) // end of buffer
            {
                len1 = 0;
                // read more data
                break;
            }

            // find end of line
            ofs2 = ofs1;
            while (ofs2 < len1 && (buf[ofs2] != '\r' && buf[ofs2] != '\n')) ofs2++;
            if (ofs2 == len1) // end of buffer
            {
                if (ofs2 == sizeof(buf)) // end of full buffer
                {
                    len1 = 0;
                    if (ofs1 != 0) // ignore lines longer than buffer size (shouldn't happen)
                    {
                        for (; ofs1 < ofs2; ofs1++, len1++)
                        {
                            buf[len1] = buf[ofs1];
                        }
                    }
                    // read more data
                    break;
                }
            }

            buf[ofs2] = 0;

            if (str_startswith(&buf[ofs1], "Hugepagesize:"))
            {
                ofs1 += 13;
                while (buf[ofs1] == ' ') ofs1++;

                default_huge_page_size_kb = str_readuint(&(buf[ofs1]), 0);

                // don't read anymore data
                len2 = 0;
                break;
            }

            ofs1 = ofs2 + 1;
        }
    }

    sys_close(fd);

    if (default_huge_page_size_kb == 0) return;

    for (shift = 1; shift <= 53; shift++)
    {
        if ((1 << shift) == default_huge_page_size_kb)
        {
            default_huge_page_size_shift = shift + 10;
            break;
        }
        else if ((1 << shift) > default_huge_page_size_kb)
        {
            break;
        }
    }
}

static void read_hugepagesize_set(void)
{
    int fd;
    uint8_t buf[1024];
    struct linux_dirent64 {
        __ino64_t      d_ino;
        __off64_t      d_off;
        unsigned short d_reclen;
        unsigned char  d_type;
        char           d_name[];
    } *entry;
    int len, ofs, shift, huge_page_size_shift;
    uint64_t huge_page_size_kb;

    fd = sys_openat(AT_FDCWD, "/sys/kernel/mm/hugepages", O_RDONLY | O_DIRECTORY);
    if (fd < 0) return;

    while (1)
    {
        len = sys_getdents64(fd, buf, sizeof(buf));
        if (len <= 0) break;

        for (ofs = 0; ofs < len;)
        {
            entry = (struct linux_dirent64 *) (buf + ofs);
            ofs += entry->d_reclen;

            if (!str_startswith(entry->d_name, "hugepages-")) continue;

            huge_page_size_kb = str_readuint(&(entry->d_name[10]), 0);
            if (huge_page_size_kb == 0) continue;

            huge_page_size_shift = 0;
            for (shift = 1; shift <= 53; shift++)
            {
                if ((1 << shift) == huge_page_size_kb)
                {
                    huge_page_size_shift = shift + 10;
                    break;
                }
                else if ((1 << shift) > huge_page_size_kb)
                {
                    break;
                }
            }

            if (huge_page_size_shift == 0) continue;

            if (min_huge_page_size_shift > huge_page_size_shift)
            {
                min_huge_page_size_shift = huge_page_size_shift;
            }

            if (max_huge_page_size_shift < huge_page_size_shift)
            {
                max_huge_page_size_shift = huge_page_size_shift;
            }

            huge_page_size_set |= UINT64_C(1) << huge_page_size_shift;
        }
    }

    sys_close(fd);
}

static __attribute__ ((noinline)) void read_page_sizes(void)
{
    int64_t index;

    page_size = 0;
    for (index = 0; index < auxc; index++)
    {
        if (auxv[index].a_type == AT_PAGESZ)
        {
            page_size = auxv[index].a_un.a_val;
            break;
        }
    }
    if (page_size <= 0) page_size = 4096;

    default_huge_page_size_shift = 0;
    min_huge_page_size_shift = 64;
    max_huge_page_size_shift = 0;
    huge_page_size_set = 0;

    read_default_hugepagesize();
    read_hugepagesize_set();
}

static __attribute__ ((noinline)) void read_kernel_version(void)
{
    int fd;
    char buf[32];
    int len1, len2;
    uint64_t version, major;

    fd = sys_openat(AT_FDCWD, "/proc/sys/kernel/osrelease", O_RDONLY);
    if (fd < 0) return;

    len1 = 0;
    len2 = 1;
    while (len2)
    {
        len2 = sys_read(fd, &(buf[len1]), sizeof(buf) - len1);
        if (len2 < 0) break; // error

        len1 += len2;
        if (len1 != sizeof(buf) && len2) continue; // buffer not full and not end of file

        buf[31] = 0;

        version = str_readuint(buf, &len2);
        if (version == 0) break;

        while (buf[len2] != 0 && (buf[len2] < '0' || buf[len2] > '9')) len2++;

        major = str_readuint(&(buf[len2]), &len2);

        if ((version < 4) || ((version == 4) && (major < 19)))
        {
            pr_print("The kernel version is old.\nThe loader may not work correctly.\nRecommended kernel version is 5.0 or higher.");
        }

        break;
    }

    sys_close(fd);
}


static int reserve_memory_range(uint64_t start, uint64_t end);

static int reserve_huge_memory_range(uint64_t start, uint64_t end, int huge_page_size_shift)
{
    uint64_t huge_start, huge_len, huge_page_size;
    void *req;
    void *mem;
    int flags;

    huge_page_size = UINT64_C(1) << huge_page_size_shift;
    if (huge_page_size > (end - start)) return 0; // memory range to small

    huge_start = start;
    if (huge_start & (huge_page_size - 1))
    {
        huge_start = (huge_start + huge_page_size - 1) & ~(huge_page_size - 1);
    }

    if (huge_start >= end) return 0; // aligned memory range to small

    huge_len = (end - huge_start) & ~(huge_page_size - 1);

    if (huge_len < huge_page_size) return 0; // aligned memory range to small

    req = (void *)huge_start;
    flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE | MAP_NORESERVE | MAP_HUGETLB;
    if (huge_page_size_shift != default_huge_page_size_shift)
    {
        flags |= huge_page_size_shift << MAP_HUGE_SHIFT;
    }

    mem = sys_mmap(req, huge_len, PROT_NONE, flags, -1, 0);
    if (FAILED_MMAP(mem))
    {
        return 0;
    }

    if (mem != req)
    {
        pr_errprint("Error memory mmap moved\n");
        return -2;
    }

    // reserve memory range between start and aligned start
    if (start != huge_start)
    {
        if (reserve_memory_range(start, huge_start) < 0)
        {
            return -3;
        }
    }

    // reserve memory range between aligned end and end
    if (huge_start + huge_len != end)
    {
        if (reserve_memory_range(huge_start + huge_len, end) < 0)
        {
            return -4;
        }
    }

    return 1;
}

static int reserve_memory_range(uint64_t start, uint64_t end)
{
    void *req;
    void *mem;
    int res, shift;

    if (max_huge_page_size_shift != 0)
    {
        for (shift = max_huge_page_size_shift; shift >= min_huge_page_size_shift; shift--)
        {
            if ((huge_page_size_set & (UINT64_C(1) << shift)) == 0) continue;

            res = reserve_huge_memory_range(start, end, shift);
            if (res < 0)
            {
                return -5;
            }

            if (res > 0)
            {
                return 0;
            }
        }
    }
    else if (default_huge_page_size_shift != 0)
    {
        res = reserve_huge_memory_range(start, end, default_huge_page_size_shift);
        if (res < 0)
        {
            return -6;
        }

        if (res > 0)
        {
            return 0;
        }
    }

    req = (void *)start;
    mem = sys_mmap(req, end - start, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE | MAP_NORESERVE , -1, 0);
    if (FAILED_MMAP(mem))
    {
        pr_errprint("Error mmaping memory\n");
        return -1;
    }

    if (mem != req)
    {
        pr_errprint("Error memory mmap moved\n");
        return -2;
    }

    return 0;
}

static __attribute__ ((noinline)) void reserve_memory_space(void)
{
// 64 maps should be more then enough
#define MAX_MAPS 64
    int fd;
    uint64_t start, end;
    int len1, len2, ofs1, ofs2, len3, num_map, index;
    uint64_t maps[MAX_MAPS * 2];
    char buf[256 + PATH_MAX];

    fd = sys_openat(AT_FDCWD, "/proc/self/maps", O_RDONLY);
    if (fd < 0)
    {
        pr_errprint("Error opening maps\n");
        sys_exit(10);
    }

    num_map = 0;
    len1 = 0;
    len2 = 1;
    while (len2)
    {
        len2 = sys_read(fd, &(buf[len1]), sizeof(buf) - len1);
        if (len2 < 0) // error
        {
            pr_errprint("Error reading maps\n");
            sys_close(fd);
            sys_exit(11);
        }

        len1 += len2;
        if (len1 != sizeof(buf) && len2) continue; // buffer not full and not end of file

        if (len1 == 0) break; // empty buffer

        ofs1 = 0;
        while (1)
        {
            // skip empty lines
            while (ofs1 < len1 && (buf[ofs1] == '\r' || buf[ofs1] == '\n')) ofs1++;
            if (ofs1 >= len1) // end of buffer
            {
                len1 = 0;
                // read more data
                break;
            }

            // find end of line
            ofs2 = ofs1;
            while (ofs2 < len1 && (buf[ofs2] != '\r' && buf[ofs2] != '\n')) ofs2++;
            if (ofs2 == len1) // end of buffer
            {
                if (ofs2 == sizeof(buf)) // end of full buffer
                {
                    if (ofs1 == 0) // line longer than buffer size (shouldn't happen)
                    {
                        pr_errprint("Map with too long line\n");
                        sys_close(fd);
                        sys_exit(12);
                    }

                    len1 = 0;
                    for (; ofs1 < ofs2; ofs1++, len1++)
                    {
                        buf[len1] = buf[ofs1];
                    }
                    // read more data
                    break;
                }
            }

            buf[ofs2] = 0;

            start = str_readhex(&(buf[ofs1]), &len3);
            if (len3 == 0 || buf[ofs1 + len3] != '-')
            {
                pr_errprint("Map without start address\n");
                sys_close(fd);
                sys_exit(13);
            }

            ofs1 += len3 + 1;
            end = str_readhex(&(buf[ofs1]), &len3);
            if (len3 == 0 || buf[ofs1 + len3] != ' ')
            {
                pr_errprint("Map without end address\n");
                sys_close(fd);
                sys_exit(14);
            }


            if ((start & UINT64_C(0x8000000000000000)) == 0) // ignore maps in kernel address space
            {
                num_map++;
                if (num_map > MAX_MAPS)
                {
                    pr_errprint("Too many maps\n");
                    sys_close(fd);
                    sys_exit(15);
                }

                maps[2 * num_map - 2] = start;
                maps[2 * num_map - 1] = end;
            }


            ofs1 = ofs2 + 1;
        }
    }

    sys_close(fd);

    if (num_map == 0) return;

    // reserve memory below 1MB
    if (maps[0] >= MEM_1MB)
    {
        start = 65536;
        if (start & (page_size - 1))
        {
            start = (start + (page_size - 1)) & ~(page_size - 1);
        }

        if (reserve_memory_range(start, MEM_1MB) < 0)
        {
            // ignore error
            //sys_exit(16);
        }
    }

    // reserve memory above 2GB, below existing maps
    if (maps[0] >= MEM_2GB)
    {
        if (reserve_memory_range(MEM_2GB, maps[0]) < 0)
        {
            sys_exit(17);
        }
    }

    // reserve memory between existing maps
    for (index = 1; index < num_map; index++)
    {
        start = maps[2 * index - 1];
        end = maps[2 * index];

        if (end == start) continue; // no memory between maps
        if (end <= MEM_2GB) continue; // don't reserve memory bellow 2GB

        if (start < MEM_2GB) // don't reserve memory bellow 2GB
        {
            start = MEM_2GB;
        }

        if (reserve_memory_range(start, end) < 0)
        {
            sys_exit(18);
        }
    }

    // reserve memory above existing maps
    if (maps[2 * num_map - 1] <= (MEM_END - page_size))
    {
        uint64_t req;
        void *mem;

        start = maps[2 * num_map - 1];
        if (start < MEM_2GB) // don't reserve memory bellow 2GB
        {
            start = MEM_2GB;
        }

        // find end of mapable memory
        end = MEM_END;
        while (1)
        {
            req = end >> 1;
            if (req < start) break;

            mem = sys_mmap((void *)req, page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE | MAP_NORESERVE , -1, 0);
            if (FAILED_MMAP(mem))
            {
                end = req;
                continue;
            }

            if (mem != (void *)req)
            {
                pr_errprint("Error memory mmap moved\n");
                sys_exit(19);
            }

            sys_munmap((void *)req, page_size);
            break;
        }

        if (start <= (end - page_size))
        {
            // check if last page is mapable
            req = end - page_size;
            mem = sys_mmap((void *)req, page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE | MAP_NORESERVE , -1, 0);
            if (FAILED_MMAP(mem))
            {
                end -= page_size;
            }
            else if (mem != (void *)req)
            {
                pr_errprint("Error memory mmap moved\n");
                sys_exit(19);
            }
            else
            {
                sys_munmap((void *)req, page_size);
            }

            if (start < end)
            {
                if (reserve_memory_range(start, end) < 0)
                {
                    sys_exit(19);
                }
            }
        }
    }
#undef MAX_MAPS
}


static long int read2(int fd, void *buf, unsigned long int count)
{
    long int res;
    unsigned long int count2;

    count2 = 0;
    while (1)
    {
        res = sys_read(fd, buf, count);
        if (res < 0) return res; // error

        if (res == 0) return count2; // end of file

        count2 += res;
        count -= res;

        if (count == 0) return count2; // full buffer

        buf = (void *) (res + (uintptr_t)buf);
    }
}

static int load_elf_segments(int fd, Elf64_Phdr *program_headers, int num_headers, int dynamic, void **elf_addr)
{
    uint64_t min_addr, max_addr, start, length, page_offset, filesz;
    int index, prot;
    void *base_addr, *segment;

    max_addr = 0;
    min_addr = (int64_t)-1;
    for (index = 0; index < num_headers; index++)
    {
        if (program_headers[index].p_type != PT_LOAD) continue;

        if (program_headers[index].p_vaddr < min_addr)
        {
            min_addr = program_headers[index].p_vaddr;
        }
        if (program_headers[index].p_vaddr + program_headers[index].p_memsz > max_addr)
        {
            max_addr = program_headers[index].p_vaddr + program_headers[index].p_memsz;
        }
    }

    min_addr = min_addr & ~(page_size - 1);
    max_addr = (max_addr + (page_size - 1)) & ~(page_size - 1);

    base_addr = sys_mmap((void *) (dynamic ? 0 : min_addr), max_addr - min_addr, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | (dynamic ? 0 : MAP_FIXED_NOREPLACE), -1, 0);
    if (FAILED_MMAP(base_addr))
    {
        return -1;
    }

    if (!dynamic && (base_addr != (void *)min_addr))
    {
        return -2;
    }

    for (index = 0; index < num_headers; index++)
    {
        if (program_headers[index].p_type != PT_LOAD) continue;

        page_offset = program_headers[index].p_vaddr & (page_size - 1);
        start = ((program_headers[index].p_vaddr - min_addr) + (dynamic ? (uint64_t)base_addr : 0)) & ~(page_size - 1);
        length = (page_offset + program_headers[index].p_memsz + (page_size - 1)) & ~(page_size - 1);

        prot = PROT_NONE;
        if (program_headers[index].p_flags & PF_X) prot |= PROT_EXEC;
        if (program_headers[index].p_flags & PF_W) prot |= PROT_WRITE;
        if (program_headers[index].p_flags & PF_R) prot |= PROT_READ;

        if ((page_offset == 0) && (program_headers[index].p_filesz == program_headers[index].p_memsz) && !(program_headers[index].p_offset & (page_size - 1)))
        {
            segment = sys_mmap((void *)start, program_headers[index].p_filesz, prot, MAP_PRIVATE | MAP_FIXED, fd, program_headers[index].p_offset);
            if (FAILED_MMAP(segment))
            {
                return -3;
            }
        }
        else
        {
            segment = sys_mmap((void *)start, length, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
            if (FAILED_MMAP(segment))
            {
                return -3;
            }

            if (sys_lseek(fd, program_headers[index].p_offset, SEEK_SET) < 0)
            {
                return -4;
            }

            filesz = program_headers[index].p_filesz;
            if (program_headers[index].p_memsz < filesz) filesz = program_headers[index].p_memsz;
            if (read2(fd, (void *)(page_offset + (uint64_t)segment), filesz) != filesz)
            {
                return -5;
            }

            if (sys_mprotect(segment, length, prot) < 0)
            {
                return -6;
            }
        }
    }

    *elf_addr = base_addr;

    return 0;
}

static __attribute__ ((noinline)) void load_elf_executable(void)
{
    void *stack;
    Elf64_Ehdr elf_header[2];
    int index, fd, alloc_size, index2;
    Elf64_Phdr *program_headers;
    void *elf_addr[2], *elf_entry[2];
    char *elf_interpreter;

    stack = sys_mmap(0, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN | MAP_STACK, -1, 0);
    if (FAILED_MMAP(stack))
    {
        pr_errprint("Error mmaping stack\n");
        sys_exit(20);
    }

    stack32 = (uint64_t *)stack;

    elf_interpreter = 0;
    for (index = 0; index <= 1; index++)
    {
        fd = sys_openat(AT_FDCWD, (index == 0) ? argv[1] : elf_interpreter, O_RDONLY);
        if (fd < 0)
        {
            pr_errprint("Error opening file\n");
            sys_exit(21);
        }

        if (read2(fd, &(elf_header[index]), sizeof(elf_header[index])) != sizeof(elf_header[index]))
        {
            pr_errprint("Error reading elf header\n");
            sys_exit(22);
        }

        if ((elf_header[index].e_ident[EI_MAG0] != ELFMAG0) ||
            (elf_header[index].e_ident[EI_MAG1] != ELFMAG1) ||
            (elf_header[index].e_ident[EI_MAG2] != ELFMAG2) ||
            (elf_header[index].e_ident[EI_MAG3] != ELFMAG3) ||
            (elf_header[index].e_ident[EI_CLASS] != ELFCLASS64) ||
            (elf_header[index].e_ident[EI_VERSION] != EV_CURRENT) ||
            (elf_header[index].e_type != ET_EXEC && elf_header[index].e_type != ET_DYN) ||
            (elf_header[index].e_phnum == 0)
           )
        {
            pr_errprint("Unsupported elf file\n");
            sys_exit(23);
        }

        alloc_size = elf_header[index].e_phnum * sizeof(Elf64_Phdr);
        if ((elf_header[index].e_phnum == PN_XNUM) || ((alloc_size + 15) & ~15) > STACK_SIZE)
        {
            pr_errprint("Too many program headers\n");
            sys_exit(24);
        }

        program_headers = (Elf64_Phdr *) ((STACK_SIZE - ((alloc_size + 15) & ~15)) + (uintptr_t)stack);

        if (sys_lseek(fd, elf_header[index].e_phoff, SEEK_SET) < 0)
        {
            pr_errprint("Error seeking file\n");
            sys_exit(25);
        }

        if (read2(fd, program_headers, alloc_size) != alloc_size)
        {
            pr_errprint("Error reading program headers\n");
            sys_exit(26);
        }

        if (load_elf_segments(fd, program_headers, elf_header[index].e_phnum, elf_header[index].e_type == ET_DYN, &(elf_addr[index])) < 0)
        {
            pr_errprint("Error reading elf segments\n");
            sys_exit(27);
        }

        elf_entry[index] = (void *)(elf_header[index].e_entry + ((elf_header[index].e_type == ET_DYN) ? (uint64_t)elf_addr[index] : 0));

        if (index == 0)
        {
            for (index2 = 0; index2 < elf_header[index].e_phnum; index2++)
            {
                if (program_headers[index2].p_type != PT_INTERP) continue;

                alloc_size = program_headers[index2].p_filesz;
                if (((alloc_size + 1 + 15) & ~15) > STACK_SIZE)
                {
                    pr_errprint("Too large elf interpreter name\n");
                    sys_exit(28);
                }

                elf_interpreter = (char *) ((STACK_SIZE - ((alloc_size + 1 + 15) & ~15)) + (uintptr_t)stack);

                if (sys_lseek(fd, program_headers[index2].p_offset, SEEK_SET) < 0)
                {
                    pr_errprint("Error seeking file\n");
                    sys_exit(29);
                }

                if (read2(fd, elf_interpreter, alloc_size) != alloc_size)
                {
                    pr_errprint("Error reading elf interpreter name\n");
                    sys_exit(30);
                }

                elf_interpreter[alloc_size] = 0;

                break;
            }
        }

        sys_close(fd);

        if (elf_interpreter == 0) break;
    }

    program_entry = elf_entry[elf_interpreter ? 1 : 0];

    // replace auxiliary vectors
    for (index = 0; index < auxc; index++)
    {
        switch (auxv[index].a_type)
        {
            case AT_PHDR:
                auxv[index].a_un.a_val = elf_header[0].e_phoff + (uint64_t)elf_addr[0];
                break;
            case AT_PHENT:
                auxv[index].a_un.a_val = elf_header[0].e_phentsize;
                break;
            case AT_PHNUM:
                auxv[index].a_un.a_val = elf_header[0].e_phnum;
                break;
            case AT_BASE:
                if (elf_interpreter)
                {
                    auxv[index].a_un.a_val = (uint64_t)elf_addr[1];
                }
                break;
            case AT_ENTRY:
                auxv[index].a_un.a_val = (uint64_t)elf_entry[0];
                break;
            case AT_EXECFN:
                auxv[index].a_un.a_val = (uint64_t)argv[1];
                break;
            default:
                break;
        }
    }
}


static void check_stack32(void *top)
{
    if ((uintptr_t)top <= (uintptr_t)stack32)
    {
        pr_errprint("Not enough stack space\n");
        sys_exit(40);
    }
}

static __attribute__ ((noinline)) void prepare_stack32(void)
{
    uint8_t *stack_top;
    void *result;
    uint64_t *stack_top2;
    int64_t index;
    int len;

    stack_top = (uint8_t *) (STACK_SIZE + (uintptr_t)stack32);

    // set lowest stack page as inaccessible = guard page
    result = sys_mmap(stack32, page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_NORESERVE, -1, 0);
    if (!FAILED_MMAP(result))
    {
        stack32 = (uint64_t *)result;
    }

    for (index = envc - 1; index >= 0; index--)
    {
        len = str_length(envp[index]);
        stack_top -= len + 1;
        check_stack32(stack_top);
        mem_copy(stack_top, envp[index], len + 1);
        envp[index] = (char *)stack_top;
    }

    for (index = argc - 1; index >= 1; index--)
    {
        len = str_length(argv[index]);
        stack_top -= len + 1;
        check_stack32(stack_top);
        mem_copy(stack_top, argv[index], len + 1);
        argv[index] = (char *)stack_top;
    }

    // align to 8 bytes
    stack_top2 = (uint64_t *) (((uintptr_t)stack_top) & ~7);

    // final stack must be aligned to 16 bytes
    if ( (((argc + envc) & 1) == 0) != ((((uintptr_t)stack_top2) & 8) == 0) )
    {
        stack_top2--;
    }

    for (index = auxc; index >= 0; index--)
    {
        stack_top2 -= 2;
        check_stack32(stack_top2);

        // replace auxiliary vector
        if (auxv[index].a_type == AT_EXECFN)
        {
            auxv[index].a_un.a_val = (uint64_t)argv[1];
        }

        *(Elf64_auxv_t *)stack_top2 = auxv[index];
    }

    for (index = envc; index >= 0; index--)
    {
        stack_top2--;
        check_stack32(stack_top2);
        *(char **)stack_top2 = envp[index];
    }

    for (index = argc; index >= 1; index--)
    {
        stack_top2--;
        check_stack32(stack_top2);
        *(char **)stack_top2 = argv[index];
    }

    stack_top2--;
    check_stack32(stack_top2);
    *stack_top2 = argc - 1;

    stack32_top = stack_top2;
}


static __attribute__ ((noinline)) void rename_process(void)
{
    int index, name_offset;

    name_offset = 0;
    for (index = 0; argv[1][index] != 0; index++)
    {
        if (argv[1][index] == '/')
        {
            name_offset = index + 1;
        }
    }

    sys_prctl(PR_SET_NAME, (uintptr_t)&(argv[1][name_offset]), 0, 0, 0);
}


// start function

void _start2(void *func, uint64_t *stack)
{
    fini_func = func;
    read_stack(stack);
    read_page_sizes();
    read_kernel_version();
    reserve_memory_space();
    load_elf_executable();
    prepare_stack32();
    rename_process();
    _start3(fini_func, stack32_top, program_entry);
    sys_exit(0);
}

