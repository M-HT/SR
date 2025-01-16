/**
 *
 *  Copyright (C) 2019-2025 Roman Pauer
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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "SR_defs.h"
#include "SR_vars.h"

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

#define _DEBUG_PE 1

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) __IMAGE_FILE_HEADER {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} _IMAGE_FILE_HEADER, *P_IMAGE_FILE_HEADER;

typedef struct __attribute__ ((__packed__)) __IMAGE_DATA_DIRECTORY {
    uint32_t VirtualAddress;
    uint32_t Size;
} _IMAGE_DATA_DIRECTORY, *P_IMAGE_DATA_DIRECTORY;

typedef struct __attribute__ ((__packed__)) __IMAGE_OPTIONAL_HEADER {
    uint16_t Magic;
    uint8_t  MajorLinkerVersion;
    uint8_t  MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint32_t BaseOfData;
    uint32_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    _IMAGE_DATA_DIRECTORY ExportTable;
    _IMAGE_DATA_DIRECTORY ImportTable;
    _IMAGE_DATA_DIRECTORY ResourceTable;
    _IMAGE_DATA_DIRECTORY ExceptionTable;
    _IMAGE_DATA_DIRECTORY CertificateTable;
    _IMAGE_DATA_DIRECTORY BaseRelocationTable;
    _IMAGE_DATA_DIRECTORY Debug;
    _IMAGE_DATA_DIRECTORY Architecture;
    _IMAGE_DATA_DIRECTORY GlobalPtr;
    _IMAGE_DATA_DIRECTORY TLSTable;
    _IMAGE_DATA_DIRECTORY LoadConfigTable;
    _IMAGE_DATA_DIRECTORY BoundImport;
    _IMAGE_DATA_DIRECTORY IAT;
    _IMAGE_DATA_DIRECTORY DelayImportDescriptor;
    _IMAGE_DATA_DIRECTORY CLRRuntimeHeader;
    _IMAGE_DATA_DIRECTORY Reserved;
} _IMAGE_OPTIONAL_HEADER, *P_IMAGE_OPTIONAL_HEADER;


typedef struct __attribute__ ((__packed__)) __IMAGE_NT_HEADERS {
    uint32_t Signature;
    struct __IMAGE_FILE_HEADER FileHeader;
    struct __IMAGE_OPTIONAL_HEADER OptionalHeader;
} _IMAGE_NT_HEADERS, *P_IMAGE_NT_HEADERS;

typedef struct __attribute__ ((__packed__)) __IMAGE_SECTION_HEADER {
    uint8_t  Name[8];
    uint32_t VirtualSize;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
} _IMAGE_SECTION_HEADER, *P_IMAGE_SECTION_HEADER;

typedef struct __attribute__ ((__packed__)) __IMAGE_IMPORT_BY_NAME {
    uint16_t Hint;
    uint8_t  Name[];
} _IMAGE_IMPORT_BY_NAME, *P_IMAGE_IMPORT_BY_NAME;

typedef struct __attribute__ ((__packed__)) __IMAGE_IMPORT_DESCRIPTOR {
    uint32_t ImportLookupTable;
    uint32_t TimeDateStamp;
    uint32_t ForwarderChain;
    uint32_t Name;
    uint32_t ImportAddressTable;
} _IMAGE_IMPORT_DESCRIPTOR, *P_IMAGE_IMPORT_DESCRIPTOR;

typedef struct __attribute__ ((__packed__)) __IMAGE_EXPORT_DIRECTORY {
    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Name;
    uint32_t Base;
    uint32_t NumberOfFunctions;
    uint32_t NumberOfNames;
    uint32_t AddressOfFunctions;
    uint32_t AddressOfNames;
    uint32_t AddressOfNameOrdinals;
} _IMAGE_EXPORT_DIRECTORY, *P_IMAGE_EXPORT_DIRECTORY;

#pragma pack()

typedef struct _Import_Check_ {
    import_data *result;
    const char *name;
} Import_Check;



typedef struct _memory_file_ {
    int64_t len;
    void *mem;
#ifdef WIN32
    HANDLE file, fmap;
#else
    int fd;
#endif
    int mem_allocated;
} memory_file;

static int load_file(memory_file *mem_file, const char *fname)
{
    FILE *mf;

    if (mem_file == NULL)
    {
        return -1;
    }

    mem_file->mem_allocated = 0;
    mem_file->mem = NULL;
#ifdef WIN32

    mem_file->fmap = INVALID_HANDLE_VALUE;

    mem_file->file = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mem_file->file == INVALID_HANDLE_VALUE) goto LOAD_FILE_MAP_ERROR;

    {
        DWORD loword, hiword;

        loword = GetFileSize(mem_file->file, &hiword);
        if (loword == INVALID_FILE_SIZE) goto LOAD_FILE_MAP_ERROR;

        mem_file->len = loword | ( ((int64_t) hiword) << 32 );
    }

    if (mem_file->len > (1LL << ( (sizeof(void *) * 8) - 2) ) ) goto LOAD_FILE_MAP_ERROR;

    mem_file->fmap = CreateFileMapping(mem_file->file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mem_file->fmap == INVALID_HANDLE_VALUE) goto LOAD_FILE_MAP_ERROR;

    mem_file->mem = MapViewOfFile(mem_file->fmap, FILE_MAP_READ, 0, 0, 0);
    if (mem_file->mem == NULL) goto LOAD_FILE_MAP_ERROR;

    return 0;

LOAD_FILE_MAP_ERROR:
    if (mem_file->fmap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mem_file->fmap);
        mem_file->fmap = INVALID_HANDLE_VALUE;
    }

    if (mem_file->file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mem_file->file);
        mem_file->file = INVALID_HANDLE_VALUE;
    }
#else
    mem_file->fd = open(fname, O_RDONLY);
    if (mem_file->fd < 0) goto LOAD_FILE_MAP_ERROR;

    {
        off_t len2;

        len2 = lseek(mem_file->fd, 0, SEEK_END);

        if (len2 < 0) goto LOAD_FILE_MAP_ERROR;

        if (lseek(mem_file->fd, 0, SEEK_SET) == (off_t)-1) goto LOAD_FILE_MAP_ERROR;

        mem_file->len = (int64_t) len2;
    }

    mem_file->mem = mmap(NULL, mem_file->len, PROT_READ, MAP_PRIVATE, mem_file->fd, 0);
    if(mem_file->mem == MAP_FAILED) goto LOAD_FILE_MAP_ERROR;

    return 0;

    LOAD_FILE_MAP_ERROR:
    mem_file->mem = NULL;

    if (mem_file->fd > 0)
    {
        close(mem_file->fd);
        mem_file->fd = 0;
    }
#endif

    mf = fopen(fname, "rb");
    if (mf == NULL) goto LOAD_FILE_READ_ERROR;

    {
        long len2;

        if (fseek(mf, 0, SEEK_END) == -1) goto LOAD_FILE_READ_ERROR;

        len2 = ftell(mf);

        if (len2 < 0) goto LOAD_FILE_READ_ERROR;

        if (fseek(mf, 0, SEEK_SET) == -1) goto LOAD_FILE_READ_ERROR;

        mem_file->len = (int64_t) len2;
    }

    mem_file->mem = malloc(mem_file->len);
    if (mem_file->mem == NULL) goto LOAD_FILE_READ_ERROR;

    mem_file->mem_allocated = 0;

    if (fread(mem_file->mem, 1, mem_file->len, mf) != mem_file->len) goto LOAD_FILE_READ_ERROR;

    fclose(mf);

    return 0;

LOAD_FILE_READ_ERROR:
    if (mem_file->mem_allocated)
    {
        mem_file->mem_allocated = 0;
        free(mem_file->mem);
    }

    if (mf != NULL)
    {
        fclose(mf);
    }

    return -2;
}

static void unload_file(memory_file *mem_file)
{
    if (mem_file == NULL) return;

    if (mem_file->mem_allocated)
    {
        mem_file->mem_allocated = 0;
        if (mem_file->mem != NULL)
        {
            free(mem_file->mem);
            mem_file->mem = NULL;
        }
    }
#ifdef WIN32
    else
    {
        if (mem_file->mem != NULL)
        {
            UnmapViewOfFile(mem_file->mem);
            mem_file->mem = NULL;
        }

        if (mem_file->fmap != INVALID_HANDLE_VALUE)
        {
            CloseHandle(mem_file->fmap);
            mem_file->fmap = INVALID_HANDLE_VALUE;
        }

        if (mem_file->file != INVALID_HANDLE_VALUE)
        {
            CloseHandle(mem_file->file);
            mem_file->file = INVALID_HANDLE_VALUE;
        }
    }
#else
    else
    {
        if (mem_file->mem != NULL)
        {
            munmap(mem_file->mem, mem_file->len);
            mem_file->mem = NULL;
        }

        if (mem_file->fd > 0)
        {
            close(mem_file->fd);
            mem_file->fd = 0;
        }
    }
#endif
}


static void ImportNameCheck(import_data *item, void *data)
{
#define DATA ((Import_Check *) data)

    if ( !(DATA->result) )
    {
        if ( !strcmp(item->ProcName, DATA->name) )
        {
            DATA->result = item;
        }
    }

#undef DATA
}

static import_data *FindImportName(const char *ImportName)
{
    Import_Check IC;

    IC.result = NULL;
    IC.name = ImportName;
    import_list_ForEach(&ImportNameCheck, &IC);

    return IC.result;
}

static int LoadBssBorder(void)
{
    char buf[8192];
    char *str1;
    FILE *file;
    uint_fast32_t BssObject, BssOffset;
    int items, length, BssAlignMinus;
    unsigned int BssAddress;

    file = fopen("bssborder.csv", "rt");
    if (file == NULL) return 0;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8192[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 != NULL)
        {
            *str1 = 0;
            str1++;

            sscanf(str1, "%i", &BssAlignMinus);
        }
        else
        {
            BssAlignMinus = 0;
        }

        sscanf(buf, "%i", &BssAddress);

        if (!SR_get_section_reladr(BssAddress, &BssObject, &BssOffset))
        {
            fprintf(stderr, "Error: wrong bss address: 0x%x\n", BssAddress);
            return 1;
        }

        if (section[BssObject].type != ST_DATA)
        {
            fprintf(stderr, "Error: wrong bss section type: 0x%x - %i\n", BssAddress, section[BssObject].type);
            return 2;
        }

        section[num_sections] = section[BssObject];
        section[num_sections].size -= BssOffset;
        section[num_sections].size_in_file = 0;
        section[num_sections].start += BssOffset;
        section[num_sections].virtaddr += BssOffset;
        section[num_sections].adr += BssOffset;
        section[num_sections].type = ST_UDATA;
        strcpy(section[num_sections].name, ".bss");
#ifdef _DEBUG_PE
        fprintf(stderr, "splitting bss section from end of %s (seg%.2i) to %s (seg%.2i)\n", section[BssObject].name, (unsigned int)(BssObject + 1), section[num_sections].name, num_sections + 1);
        fprintf(stderr, "\toriginal section changed size from %i to %i\n", (unsigned int)section[BssObject].size, (unsigned int)(BssOffset + BssAlignMinus));
        fprintf(stderr, "\tnew section start=0x%x and size=%i and type=%i\n", (unsigned int)section[num_sections].start, (unsigned int)section[num_sections].size, section[num_sections].type);
#endif
        num_sections++;

        section[BssObject].size = BssOffset + BssAlignMinus;
        if (section[BssObject].size_in_file > BssOffset + BssAlignMinus)
        {
            section[BssObject].size_in_file = BssOffset + BssAlignMinus;
        }
    }

    return 0;
}

static int LoadRelocations(P_IMAGE_NT_HEADERS Header)
{
    char buf[8192];
    char *str1, *str2;
    FILE *file;
    uint_fast32_t SourceObject, SourceOffset, TargetObject, TargetOffset;
    int items, length;
    unsigned int FixupAddress, TargetAddress;
    fixup_type FixupType;
    fixup_data *fixup;

    file = fopen("relocations.csv", "rt");
    if (file == NULL)
    {
        fprintf(stderr, "Error: error opening external relocations file: %s\n", "relocations.csv");
        return -1;
    }

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8192[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');
        if (str2 != NULL)
        {
            *str2 = 0;
            str2++;

            while (*str2 == ' ') str2++;
            if (*str2 == 0) str2 = NULL;
        }

        sscanf(buf, "%i", &FixupAddress);
        sscanf(str1, "%i", &TargetAddress);
        FixupType = (str2 != NULL && *str2 == 'i')?FT_IMAGEBASE:FT_NORMAL;

        if (!SR_get_section_reladr(FixupAddress, &SourceObject, &SourceOffset))
        {
            fprintf(stderr, "Error: wrong relocation source address: 0x%x - 0x%x\n", FixupAddress, TargetAddress);
            return 1;
        }

        if (TargetAddress != *((uint32_t *)&(section[SourceObject].adr[SourceOffset])))
        {
            fprintf(stderr, "Error: inconsistent relocation target address: 0x%x - 0x%x\n", FixupAddress, TargetAddress);
            return 2;
        }

        if (FixupType == FT_IMAGEBASE)
        {
            TargetObject = Header->OptionalHeader.ImageBase;
            TargetOffset = TargetAddress - Header->OptionalHeader.ImageBase;
        }
        else
        {
            if (!SR_get_section_reladr(TargetAddress, &TargetObject, &TargetOffset))
            {
                fprintf(stderr, "Error: wrong relocation target address: 0x%x - 0x%x\n", FixupAddress, TargetAddress);
                return 3;
            }
        }


        fixup = section_fixup_list_FindEntryEqual(SourceObject, SourceOffset);
        if (fixup == NULL)
        {
            fixup = section_fixup_list_Insert(SourceObject, SourceOffset);
            if (fixup == NULL)
            {
                fprintf(stderr, "Error: error adding relocation: 0x%x - 0x%x\n", FixupAddress, TargetAddress);
                return 5;
            }

            fixup->tofs = TargetOffset;
            fixup->tsec = TargetObject;
            fixup->type = FixupType;
        }
        else
        {
            if (fixup->tofs != TargetOffset ||
                fixup->tsec != TargetObject ||
                fixup->type != FixupType)
            {
                fprintf(stderr, "Error: error adding relocation twice: 0x%x - 0x%x\n", FixupAddress, TargetAddress);
                return 4;
            }
        }
    }

    return 0;
}

int SRW_LoadFile(const char *fname)
{
    memory_file mf;
    int64_t SR_FileSize;
    P_IMAGE_NT_HEADERS Header;
    int8_t *MZfile, *PEfile;
    P_IMAGE_SECTION_HEADER SectionHeader;
    uint32_t SR_CodeSize;
    uint8_t *SR_CodeRun;
    int need_external_relocations;


    // load file
    if ( load_file(&mf, fname) )
    {
        fprintf(stderr, "Error: Unable to load file: %s\n", fname);
        return -1;
    }

    MZfile = mf.mem;
    SR_FileSize = mf.len;

    // check if file is big enough to contain MZ EXE header
    if (SR_FileSize <= 0x40)
    {
        unload_file(&mf);
        fprintf(stderr, "Error: File too small\n");
        return -1;
    }

    // check MZ signature
    if (*((uint16_t *)MZfile) != 0x5a4d)
    {
        unload_file(&mf);
        fprintf(stderr, "Error: not an EXE file\n");
        return -1;
    }

    {
        uint_fast32_t HeaderOffset;

        // get PE header offset
        HeaderOffset = *((uint32_t *)&(MZfile[0x3c]));
        PEfile = &(MZfile[HeaderOffset]);
        Header = (P_IMAGE_NT_HEADERS) PEfile;

        // check if file is big enough to contain PE EXE header
        if (SR_FileSize <= HeaderOffset + sizeof(_IMAGE_NT_HEADERS))
        {
            unload_file(&mf);
            fprintf(stderr, "Error: File too small\n");
            return -1;
        }
    }

    // check PE signature
    if ( Header->Signature != 0x00004550 )
    {
        unload_file(&mf);
        fprintf(stderr, "Error: Invalid EXE file\n");
        return -1;
    }

    // check PE header
    if ( Header->FileHeader.Machine != 0x014c || // 386 or later
         Header->FileHeader.SizeOfOptionalHeader < sizeof(_IMAGE_OPTIONAL_HEADER) || // optional header size
         Header->OptionalHeader.Magic != 0x10b || // normal (PE32) executable
         (Header->OptionalHeader.Subsystem != 2 && Header->OptionalHeader.Subsystem != 3) || // Windows GUI/CUI subsystem
         Header->OptionalHeader.NumberOfRvaAndSizes < 16 // optional header contents
        )
    {
        unload_file(&mf);
        fprintf(stderr, "Error: Wrong PE header\n");
        return -1;
    }

#ifdef _DEBUG_PE
    {
        int i;
        P_IMAGE_DATA_DIRECTORY pp;


        pp = &(Header->OptionalHeader.ExportTable);
        for (i = 0; i < 16; i++)
        {
            if ((pp[i].VirtualAddress == 0) && (pp[i].Size == 0)) continue;

            switch (i)
            {
                case  0: printf("%s:\n", "Export Table (-> .edata Section)"); break;
                case  1: printf("%s:\n", "Import Table (-> .idata Section)"); break;
                case  2: printf("%s:\n", "Resource Table (-> .rsrc Section)"); break;
                case  3: printf("%s:\n", "Exception Table (-> .pdata Section)"); break;
                case  4: printf("%s:\n", "Certificate Table"); break;
                case  5: printf("%s:\n", "Base Relocation Table (-> .reloc Section)"); break;
                case  6: printf("%s:\n", "Debug (-> .debug Section)"); break;
                case  7: printf("%s:\n", "Architecture (must be 0)"); break;
                case  8: printf("%s:\n", "Global Ptr (size must be 0)"); break;
                case  9: printf("%s:\n", "TLS Table (-> .tls Section)"); break;
                case 10: printf("%s:\n", "Load Config Table"); break;
                case 11: printf("%s:\n", "Bound Import"); break;
                case 12: printf("%s:\n", "IAT"); break;
                case 13: printf("%s:\n", "Delay Import Descriptor"); break;
                case 14: printf("%s:\n", "CLR Runtime Header (-> .cormeta Section)"); break;
                case 15: printf("%s:\n", "Reserved (must be 0)"); break;
            }
            printf("Directory[%i].VirtualAddress=0x%x\n", i, pp[i].VirtualAddress);
            printf("Directory[%i].Size=%i\n", i, pp[i].Size);
        }
        printf("\n");
    }
#endif

    // check relocations
    need_external_relocations = 0;
    if (Header->FileHeader.Characteristics & 1)
    {
        /*unload_file(&mf);
        fprintf(stderr, "Error: Relocations were stripped from file :(\n");
        return -1;*/
        need_external_relocations = 1;
        fprintf(stderr, "Relocations were stripped from file - need external relocations\n");
    }

    num_sections = Header->FileHeader.NumberOfSections;
    SectionHeader = (P_IMAGE_SECTION_HEADER) &(PEfile[4 + sizeof(_IMAGE_FILE_HEADER) + Header->FileHeader.SizeOfOptionalHeader]);

    if (num_sections > MAX_SECTIONS ||
        num_sections == 0)
    {
        unload_file(&mf);
        fprintf(stderr, "Error: Too many or too few object table entries\n");
        return -1;
    }

    // allocate memory for the code
    {
        unsigned int Entry;
        uint_fast32_t size, section_size;
        uintptr_t address;

        SR_CodeSize = 0;
        for (Entry = 0; Entry < num_sections; Entry ++)
        {
            section_size = (SectionHeader[Entry].VirtualSize)?SectionHeader[Entry].VirtualSize:SectionHeader[Entry].SizeOfRawData;

            size = section_size / Header->OptionalHeader.SectionAlignment;
            size *= Header->OptionalHeader.SectionAlignment;
            if (size != section_size) size += Header->OptionalHeader.SectionAlignment;

            section[Entry].size = SR_CodeSize;
            SR_CodeSize += size;
        }

        SR_CodeBase = (uint8_t *) malloc(SR_CodeSize + Header->OptionalHeader.SectionAlignment - 1);
        if (SR_CodeBase == NULL)
        {
            unload_file(&mf);
            fprintf(stderr, "Error: Not enough memory\n");
            return -1;

        }

        address = (uintptr_t)SR_CodeBase / Header->OptionalHeader.SectionAlignment;
        address *= Header->OptionalHeader.SectionAlignment;
        SR_CodeRun = (address == (uintptr_t)SR_CodeBase)?((uint8_t *) address):((uint8_t *) address + Header->OptionalHeader.SectionAlignment);

        memset(SR_CodeRun, 0, SR_CodeSize);
    }

    // read pages into allocated memory
    {
        unsigned int Entry;

        import_list = NULL;

        for (Entry = 0; Entry < num_sections; Entry ++)
        {
            section[Entry].adr          = &(SR_CodeRun[section[Entry].size]);
            section[Entry].size         = (SectionHeader[Entry].VirtualSize)?SectionHeader[Entry].VirtualSize:SectionHeader[Entry].SizeOfRawData;
            section[Entry].size_in_file = SectionHeader[Entry].SizeOfRawData;
            section[Entry].virtaddr     = SectionHeader[Entry].VirtualAddress;
            section[Entry].start        = Header->OptionalHeader.ImageBase + SectionHeader[Entry].VirtualAddress;
            section[Entry].flags        = SectionHeader[Entry].Characteristics;
            section[Entry].fixup_list   = NULL;
            section[Entry].output_list  = NULL;
            section[Entry].entry_list   = NULL;
            section[Entry].code_list    = NULL;
            section[Entry].label_list   = NULL;
            section[Entry].noret_list   = NULL;
            section[Entry].extrn_list   = NULL;
            section[Entry].alias_list   = NULL;
            section[Entry].ignored_list  = NULL;
            section[Entry].replace_list = NULL;
            section[Entry].bound_list   = NULL;
            section[Entry].region_list  = NULL;
            section[Entry].nocode_list  = NULL;
            section[Entry].iflags_list  = NULL;
            section[Entry].code16_list  = NULL;
            section[Entry].ua_ebp_list  = NULL;
            section[Entry].ua_esp_list  = NULL;
            section[Entry].code2data_list = NULL;
            section[Entry].data2code_list = NULL;
            section[Entry].export_list  = NULL;

            section[Entry].name[8] = 0;
            memcpy(section[Entry].name, SectionHeader[Entry].Name, 8);

            if (SectionHeader[Entry].Characteristics & 0x20)
            {
                // code segment
                section[Entry].type = ST_CODE;
            }
            else if (SectionHeader[Entry].Characteristics & 0x80)
            {
                // uninitialized data segment
                section[Entry].type = ST_UDATA;
            }
            else
            {
                // data segment
                if (SectionHeader[Entry].Characteristics & 0x80000000)
                {
                    section[Entry].type = ST_DATA;
                }
                else
                {
                    section[Entry].type = ST_RDATA;
                }

                if ((Header->OptionalHeader.ExportTable.VirtualAddress != 0) && (Header->OptionalHeader.ExportTable.Size != 0))
                {
                    if ((Header->OptionalHeader.ExportTable.VirtualAddress == SectionHeader[Entry].VirtualAddress) &&
                        ((Header->OptionalHeader.ExportTable.Size == SectionHeader[Entry].VirtualSize) || (Header->OptionalHeader.ExportTable.Size == SectionHeader[Entry].SizeOfRawData))
                       )
                    {
                        section[Entry].type = ST_EDATA;
                    }
                }

                if ((Header->OptionalHeader.ImportTable.VirtualAddress != 0) && (Header->OptionalHeader.ImportTable.Size != 0))
                {
                    if ((Header->OptionalHeader.ImportTable.VirtualAddress >= SectionHeader[Entry].VirtualAddress) &&
                        (Header->OptionalHeader.ImportTable.VirtualAddress < SectionHeader[Entry].VirtualAddress + SectionHeader[Entry].VirtualSize)
                       )
                    {
                        if ((strcmp(section[Entry].name, ".idata") == 0) || (strcmp(section[Entry].name, "imports") == 0))
                        {
                            section[Entry].type = ST_IDATA;
                        }
                    }
                }

                if ((Header->OptionalHeader.IAT.VirtualAddress != 0) && (Header->OptionalHeader.IAT.Size != 0))
                {
                    if ((Header->OptionalHeader.IAT.VirtualAddress >= SectionHeader[Entry].VirtualAddress) &&
                        (Header->OptionalHeader.IAT.VirtualAddress < SectionHeader[Entry].VirtualAddress + SectionHeader[Entry].VirtualSize)
                       )
                    {
                        if ((strcmp(section[Entry].name, ".idata") == 0) || (strcmp(section[Entry].name, "imports") == 0))
                        {
                            section[Entry].type = ST_IDATA;
                        }
                    }
                }

                if ((Header->OptionalHeader.BaseRelocationTable.VirtualAddress != 0) && (Header->OptionalHeader.BaseRelocationTable.Size != 0))
                {
                    if ((Header->OptionalHeader.BaseRelocationTable.VirtualAddress >= SectionHeader[Entry].VirtualAddress) &&
                        (Header->OptionalHeader.BaseRelocationTable.VirtualAddress < SectionHeader[Entry].VirtualAddress + SectionHeader[Entry].VirtualSize)
                       )
                    {
                        section[Entry].type = ST_RELOC;
                    }
                }

                if ((Header->OptionalHeader.ResourceTable.VirtualAddress != 0) && (Header->OptionalHeader.ResourceTable.Size != 0))
                {
                    if (Header->OptionalHeader.ResourceTable.VirtualAddress == SectionHeader[Entry].VirtualAddress)
                    {
                        section[Entry].type = ST_RSRC;
                    }
                }
            }

#ifdef _DEBUG_PE
            {
                fprintf(stderr, "name: %s (seg%.2i)\n", section[Entry].name, Entry + 1);
                fprintf(stderr, "VirtualSize: %i\n", SectionHeader[Entry].VirtualSize);
                fprintf(stderr, "SizeOfRawData: %i\n", SectionHeader[Entry].SizeOfRawData);
                fprintf(stderr, "VirtualAddress: 0x%x\n", SectionHeader[Entry].VirtualAddress);
                fprintf(stderr, "PointerToRawData: 0x%x\n", SectionHeader[Entry].PointerToRawData);
                fprintf(stderr, "Characteristics: 0x%x\n", SectionHeader[Entry].Characteristics);
            }
#endif

            if (section[Entry].size_in_file != 0 &&
                SectionHeader[Entry].PointerToRawData != 0)
            {
                memcpy(section[Entry].adr, &(MZfile[SectionHeader[Entry].PointerToRawData]), section[Entry].size_in_file);
            }

        }
    }

    // try to split uninitialized data into bss section
    if (LoadBssBorder())
    {
        unload_file(&mf);
        fprintf(stderr, "Error: error loading bss border\n");
        return -1;
    }

    // split export and import tables to separate sections
    {
        unsigned int Entry, old_num_sections;
        int ending, c1, c2, curdir;
        P_IMAGE_DATA_DIRECTORY pp;


        pp = &(Header->OptionalHeader.ExportTable);


        old_num_sections = num_sections;
        for (Entry = 0; Entry < old_num_sections; Entry ++)
        {
            if ((section[Entry].type != ST_DATA) && (section[Entry].type != ST_RDATA)) continue;

            for (ending = 0; ending <= 2; ending++)
            {
                for (c1 = 0; c1 < 3; c1++)
                {
                    for (c2 = 0; c2 < 3; c2++)
                    {
                        curdir = (c2 == 2)?12:c2;

                        if ((pp[curdir].VirtualAddress == 0) || (pp[curdir].Size == 0)) continue;

                        if (ending == 0)
                        {
                            if (pp[curdir].VirtualAddress != section[Entry].virtaddr) continue;

                            if (pp[curdir].Size == section[Entry].size)
                            {
#ifdef _DEBUG_PE
                                fprintf(stderr, "changing section type: %s (seg%.2i) from %i to %i\n", section[Entry].name, Entry + 1, section[Entry].type, (c2 == 0)?ST_EDATA:ST_IDATA);
#endif
                                section[Entry].type = (c2 == 0)?ST_EDATA:ST_IDATA;
                            }
                            else
                            {
                                section[num_sections] = section[Entry];
                                section[num_sections].size = pp[curdir].Size;
                                if (section[num_sections].size_in_file > pp[curdir].Size)
                                {
                                    section[num_sections].size_in_file = pp[curdir].Size;
                                }
                                section[num_sections].type = (c2 == 0)?ST_EDATA:ST_IDATA;
#ifdef _DEBUG_PE
                                fprintf(stderr, "splitting section from start of %s (seg%.2i) to %s (seg%.2i)\n", section[Entry].name, Entry + 1, section[num_sections].name, num_sections + 1);
                                fprintf(stderr, "\toriginal section changed start from 0x%x to 0x%x and size from %i to %i\n", (unsigned int)section[Entry].start, (unsigned int)(section[Entry].start + pp[curdir].Size), (unsigned int)section[Entry].size, (unsigned int)(section[Entry].size - pp[curdir].Size));
                                fprintf(stderr, "\tnew section start=0x%x and size=%i and type=%i\n", (unsigned int)section[num_sections].start, (unsigned int)section[num_sections].size, section[num_sections].type);
#endif
                                num_sections++;

                                section[Entry].size -= pp[curdir].Size;
                                section[Entry].size_in_file = (section[Entry].size_in_file > pp[curdir].Size)?(section[Entry].size_in_file - pp[curdir].Size):0;
                                section[Entry].start += pp[curdir].Size;
                                section[Entry].virtaddr += pp[curdir].Size;
                                section[Entry].adr += pp[curdir].Size;
                            }
                        }
                        else
                        {
                            int Entry_size;

                            if (ending == 1)
                            {
                                if ((pp[curdir].VirtualAddress + pp[curdir].Size) != (section[Entry].virtaddr + section[Entry].size)) continue;

                                Entry_size = section[Entry].size - pp[curdir].Size;
                            }
                            else
                            {
                                if (pp[curdir].VirtualAddress < section[Entry].virtaddr) continue;
                                if (pp[curdir].VirtualAddress >= (section[Entry].virtaddr + section[Entry].size)) continue;

                                Entry_size = pp[curdir].VirtualAddress - section[Entry].virtaddr;
                            }

                            section[num_sections] = section[Entry];
                            section[num_sections].size -= Entry_size;
                            section[num_sections].size_in_file = (section[Entry].size_in_file > Entry_size)?(section[Entry].size_in_file - Entry_size):0;
                            section[num_sections].start += Entry_size;
                            section[num_sections].virtaddr += Entry_size;
                            section[num_sections].adr += Entry_size;
                            section[num_sections].type = (c2 == 0)?ST_EDATA:ST_IDATA;
#ifdef _DEBUG_PE
                            fprintf(stderr, "splitting section (%i) from end of %s (seg%.2i) to %s (seg%.2i)\n", ending, section[Entry].name, Entry + 1, section[num_sections].name, num_sections + 1);
                            fprintf(stderr, "\toriginal section changed size from %i to %i\n", (unsigned int)section[Entry].size, Entry_size);
                            fprintf(stderr, "\tnew section start=0x%x and size=%i and type=%i\n", (unsigned int)section[num_sections].start, (unsigned int)section[num_sections].size, section[num_sections].type);
#endif
                            num_sections++;

                            section[Entry].size = Entry_size;
                            if (section[Entry].size_in_file > Entry_size)
                            {
                                section[Entry].size_in_file = Entry_size;
                            }
                        }
                    }
                }
            }
        }
    }


    // get start address
    SR_get_section_reladr(Header->OptionalHeader.ImageBase + Header->OptionalHeader.AddressOfEntryPoint, &EIPObjectNum, &EIP);

    // create fixup table
    if (need_external_relocations)
    {
        if (LoadRelocations(Header))
        {
            unload_file(&mf);
            fprintf(stderr, "Error: error loading external relocations\n");
            return -1;
        }
    }
    else
    {
        unsigned int FixupOffset, FixupSize, PageFixupSize, PageFixupOffset, FixupType;
        uint_fast32_t PageAddress, FixupField, FixupAddress;
        uint_fast32_t SourceObject, SourceOffset, TargetObject, TargetOffset;
        uint8_t *Fixups;
        fixup_data *fixup;


        SR_get_section_reladr(Header->OptionalHeader.ImageBase + Header->OptionalHeader.BaseRelocationTable.VirtualAddress, &SourceObject, &SourceOffset);
        Fixups = &(section[SourceObject].adr[SourceOffset]);
        FixupOffset = 0;
        FixupSize = Header->OptionalHeader.BaseRelocationTable.Size;

        while (FixupOffset < FixupSize)
        {
            PageAddress = *((uint32_t *) &(Fixups[FixupOffset])) + Header->OptionalHeader.ImageBase;
            PageFixupSize = *((uint32_t *) &(Fixups[FixupOffset + 4]));
            FixupOffset += 8;
            PageFixupOffset = 8;
            while (PageFixupOffset < PageFixupSize)
            {
                FixupField = *((uint16_t *) &(Fixups[FixupOffset]));
                FixupOffset += 2;
                PageFixupOffset += 2;
                FixupAddress = PageAddress + (FixupField & 0x0fff);
                FixupType = (FixupField & 0xf000) >> 12;

                if (FixupType == 3)
                {
                    SR_get_section_reladr(FixupAddress, &SourceObject, &SourceOffset);

                    SR_get_section_reladr( *((uint32_t *)&(section[SourceObject].adr[SourceOffset])) , &TargetObject, &TargetOffset);


                    fixup = section_fixup_list_FindEntryEqual(SourceObject, SourceOffset);
                    if (fixup == NULL)
                    {
                        fixup = section_fixup_list_Insert(SourceObject, SourceOffset);
                        if (fixup == NULL)
                        {
                            unload_file(&mf);
                            fprintf(stderr, "Error: error adding fixup - %i\n", (unsigned int)SourceOffset);
                            return -2;
                        }

                        fixup->tofs = TargetOffset;
                        fixup->tsec = TargetObject;
                        fixup->type = FT_NORMAL;
                    }
                    else
                    {
                        if (fixup->tofs != TargetOffset ||
                            fixup->tsec != TargetObject)
                        {
                            unload_file(&mf);
                            fprintf(stderr, "Error: error adding fixup twice - %i\n", (unsigned int)SourceOffset);
                            return -3;
                        }
                    }

                }
                else if (FixupType != 0)
                {
                    unload_file(&mf);
                    fprintf(stderr, "Error: Unknown fixup type: %i\n", FixupType);
                    return -1;
                }
            }
        }

    }

    // read imports
    if ((Header->OptionalHeader.ImportTable.VirtualAddress != 0) && (Header->OptionalHeader.ImportTable.Size != 0))
    {
        P_IMAGE_IMPORT_DESCRIPTOR Directory;
        P_IMAGE_IMPORT_BY_NAME HintTable;
        uint32_t *LookupTable;
        const char *DllName, *ProcName;
        char *ProcAdrName;
        extrn_data *extrn;
        import_data *import;
        uint_fast32_t Entry, RelAdr, ImportObject, ImportAddress;
        uint32_t ImportNum;

        SR_get_section_reladr(Header->OptionalHeader.ImageBase + Header->OptionalHeader.ImportTable.VirtualAddress, &Entry, &RelAdr);
        Directory = (P_IMAGE_IMPORT_DESCRIPTOR) &(section[Entry].adr[RelAdr]);

        ImportNum = 0;

        while (Directory->ImportLookupTable != 0)
        {
            SR_get_section_reladr(Header->OptionalHeader.ImageBase + Directory->Name, &Entry, &RelAdr);
            DllName = (const char *) &(section[Entry].adr[RelAdr]);

            fprintf(stderr, "\t\tImported DLL: %s\n", DllName);

            SR_get_section_reladr(Header->OptionalHeader.ImageBase + Directory->ImportLookupTable, &Entry, &RelAdr);
            LookupTable	= (uint32_t *) &(section[Entry].adr[RelAdr]);

            SR_get_section_reladr(Header->OptionalHeader.ImageBase + Directory->ImportAddressTable, &ImportObject, &ImportAddress);

            while (*LookupTable != 0)
            {
                if (*LookupTable & 0x80000000)
                {
                    ProcName = NULL;

                    if (((*LookupTable & 0xffff) == 17) && (strcasecmp(DllName, "comctl32.dll") == 0))
                    {
                        ProcName = "InitCommonControls";
                    }

                    if (strcasecmp(DllName, "dsound.dll") == 0)
                    {
                        switch(*LookupTable & 0xffff)
                        {
                            case 1: ProcName = "DirectSoundCreate"; break;
                            case 2: ProcName = "DirectSoundEnumerateA"; break;
                        }
                    }

                    if (strcasecmp(DllName, "WSOCK32.dll") == 0)
                    {
                        switch(*LookupTable & 0xffff)
                        {
                            case   1: ProcName = "ws2_32.accept"; break;
                            case   2: ProcName = "ws2_32.bind"; break;
                            case   3: ProcName = "ws2_32.closesocket"; break;
                            case   4: ProcName = "ws2_32.connect"; break;
                            case   6: ProcName = "ws2_32.getsockname"; break;
                            case   7: ProcName = "getsockopt"; break;
                            case   8: ProcName = "ws2_32.htonl"; break;
                            case   9: ProcName = "ws2_32.htons"; break;
                            case  10: ProcName = "ws2_32.inet_addr"; break;
                            case  12: ProcName = "ws2_32.ioctlsocket"; break;
                            case  13: ProcName = "ws2_32.listen"; break;
                            case  14: ProcName = "ws2_32.ntohl"; break;
                            case  15: ProcName = "ws2_32.ntohs"; break;
                            case  16: ProcName = "recv"; break;
                            case  17: ProcName = "recvfrom"; break;
                            case  18: ProcName = "ws2_32.select"; break;
                            case  19: ProcName = "ws2_32.send"; break;
                            case  20: ProcName = "ws2_32.sendto"; break;
                            case  21: ProcName = "setsockopt"; break;
                            case  22: ProcName = "ws2_32.shutdown"; break;
                            case  23: ProcName = "ws2_32.socket"; break;
                            case  52: ProcName = "ws2_32.gethostbyname"; break;
                            case  57: ProcName = "ws2_32.gethostname"; break;
                            case 111: ProcName = "ws2_32.WSAGetLastError"; break;
                            case 115: ProcName = "ws2_32.WSAStartup"; break;
                        }
                    }

                    if (ProcName == NULL)
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: Import by ordinal not supported\n");
                        return -1;
                    }
                }
                else
                {
                    SR_get_section_reladr(Header->OptionalHeader.ImageBase + *LookupTable, &Entry, &RelAdr);
                    HintTable = (P_IMAGE_IMPORT_BY_NAME) &(section[Entry].adr[RelAdr]);

                    ProcName = (const char *)HintTable->Name;
                }

#if (defined(_DEBUG_PE) && (_DEBUG_PE >= 2))
                {
                    fprintf(stderr, "%s - %i - 0x%x\n", ProcName, ImportObject, ImportAddress);
                }
#endif

                ProcAdrName = (char *) malloc(strlen(ProcName) + 6);
                if (ProcAdrName == NULL)
                {
                    unload_file(&mf);
                    fprintf(stderr, "Error: error allocation import memory - %i\n", (unsigned int)ImportAddress);
                    return -4;
                }
                strcpy(ProcAdrName, "_adr_");
                strcat(ProcAdrName, ProcName);

                import = FindImportName(ProcName);
                if (import)
                {
                    if (0 != strcmp(import->DllName, DllName))
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: import name conflict - %i\n", ImportNum);
                        return -2;
                    }
                }
                else
                {
                    import = import_list_Insert(ImportNum);
                    if ( import == NULL )
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: error adding import - %i\n", ImportNum);
                        return -2;
                    }
                    import->DllName = DllName;
                    import->ProcAdrName = ProcAdrName;
                    import->ProcName = ProcName;
                }

                ImportNum++;


                extrn = section_extrn_list_FindEntryEqual(ImportObject, ImportAddress);
                if (extrn == NULL)
                {
                    extrn = section_extrn_list_Insert(ImportObject, ImportAddress, ProcName, NULL);
                    if (extrn == NULL)
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: error adding import extrn - %i\n", (unsigned int)ImportAddress);
                        return -2;
                    }

                    extrn->is_import = 1;
                }
                else
                {
                    if ( strcmp(extrn->proc, ProcName) )
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: error adding import extrn twice - %i\n", (unsigned int)ImportAddress);
                        return -3;
                    }
                }

                LookupTable++;
                ImportAddress+=4;
            }

            Directory++;
        }

    }

    // read exports
    if ((Header->OptionalHeader.ExportTable.VirtualAddress != 0) && (Header->OptionalHeader.ExportTable.Size != 0))
    {
        P_IMAGE_EXPORT_DIRECTORY Directory;
        uint_fast32_t Entry, RelAdr, index;
        uint32_t *AddressTable;
        export_data *export;


        SR_get_section_reladr(Header->OptionalHeader.ImageBase + Header->OptionalHeader.ExportTable.VirtualAddress, &Entry, &RelAdr);
        Directory = (P_IMAGE_EXPORT_DIRECTORY) &(section[Entry].adr[RelAdr]);

        SR_get_section_reladr(Header->OptionalHeader.ImageBase + Directory->AddressOfFunctions, &Entry, &RelAdr);
        AddressTable = (uint32_t *) &(section[Entry].adr[RelAdr]);

#ifdef _DEBUG_PE
        {
            fprintf(stderr, "Export Ordinal Base: %i\n", Directory->Base);
        }
#endif

        // exports by name
        if (Directory->NumberOfNames)
        {
            uint32_t *NameTable;
            uint16_t *OrdinalTable;
            char *ExportName;

            SR_get_section_reladr(Header->OptionalHeader.ImageBase + Directory->AddressOfNames, &Entry, &RelAdr);
            NameTable = (uint32_t *) &(section[Entry].adr[RelAdr]);

            SR_get_section_reladr(Header->OptionalHeader.ImageBase + Directory->AddressOfNameOrdinals, &Entry, &RelAdr);
            OrdinalTable = (uint16_t *) &(section[Entry].adr[RelAdr]);

            for (index = 0; index < Directory->NumberOfNames; index++)
            {
                SR_get_section_reladr(Header->OptionalHeader.ImageBase + NameTable[index], &Entry, &RelAdr);
                ExportName = (char *) &(section[Entry].adr[RelAdr]);

                SR_get_section_reladr(Header->OptionalHeader.ImageBase + AddressTable[OrdinalTable[index]], &Entry, &RelAdr);

                export = section_export_list_FindEntryEqual2(Entry, RelAdr, OrdinalTable[index] + Directory->Base);
                if (export == NULL)
                {
                    export = section_export_list_Insert(Entry, RelAdr, OrdinalTable[index] + Directory->Base, ExportName);
                    if (export == NULL)
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: error adding export - %s\n", ExportName);
                        return -2;
                    }
                    else
                    {
                        if (ExportName[0] == '_')
                        {
                            char *at;

                            at = strchr(ExportName, '@');
                            if ((at != NULL) && (at[1] >= '0') && (at[1] <= '9'))
                            {
                                uint_fast32_t internal_length;
                                internal_length = (at - ExportName) - 1;
                                export->internal = (char *) malloc(internal_length + 1);
                                if (export->internal == NULL)
                                {
                                    unload_file(&mf);
                                    fprintf(stderr, "Error: error allocation export memory - %s\n", ExportName);
                                    return -4;
                                }

                                memcpy(export->internal, ExportName + 1, internal_length);
                                export->internal[internal_length] = 0;
                            }
                        }
                    }
                }
                else
                {
                    if (strcmp(export->name, ExportName))
                    {
                        unload_file(&mf);
                        fprintf(stderr, "Error: error adding export twice - %s\n", ExportName);
                        return -3;
                    }
                }

            }
        }

        // exports by ordinal
        for (index = 0; index < Directory->NumberOfFunctions; index++)
        {
            SR_get_section_reladr(Header->OptionalHeader.ImageBase + AddressTable[index], &Entry, &RelAdr);

            export = section_export_list_FindEntryEqual2(Entry, RelAdr, index + Directory->Base);
            if (export == NULL)
            {
                export = section_export_list_Insert(Entry, RelAdr, index + Directory->Base, NULL);
                if (export == NULL)
                {
                    unload_file(&mf);
                    fprintf(stderr, "Error: error adding export - %i\n", (unsigned int)index);
                    return -2;
                }
            }
        }

    }

    unload_file(&mf);

    return 0;
}
