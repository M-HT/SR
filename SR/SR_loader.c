/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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

#pragma pack(1)
typedef struct __attribute__ ((__packed__)) _LEHeader_ {
	uint16_t Signature;
	uint8_t  ByteOrdering;
	uint8_t  WordOrdering;
	uint32_t FormatLevel;
	uint16_t CPUType;
	uint16_t OSType;
	uint32_t ModuleVersion;
	uint32_t ModuleFlags;
	uint32_t MouduleNumOfPages;
	uint32_t EIPObjectNum;
	uint32_t EIP;
	uint32_t ESPObjectNum;
	uint32_t ESP;
	uint32_t PageSize;
	uint32_t BytesOnLastPage;
	uint32_t FixupSectionSize;
	uint32_t FixupSectionChecksum;
	uint32_t LoaderSectionSize;
	uint32_t LoaderSectionChecksum;
	uint32_t ObjectTableOffset;
	uint32_t NumObjectsInModule;
	uint32_t ObjectPageMapTableOffset;
	uint32_t ObjectIteratedPagesOffset;
	uint32_t ResourceTableOffset;
	uint32_t NumResourceTableEntries;
	uint32_t ResidentNameTableOffset;
	uint32_t EntryTableOffset;
	uint32_t ModuleDirectivesOffset;
	uint32_t NumModuleDirectives;
	uint32_t FixupPageTableOffset;
	uint32_t FixupRecordTableOffset;
	uint32_t ImportModuleTableOffset;
	uint32_t NumImportModuleEntries;
	uint32_t ImportProcedureTableOffset;
	uint32_t PerPageChecksumOffset;
	uint32_t DataPagesOffset;
	uint32_t NumPreloadPages;
	uint32_t NonResidentNameTableOffset;
	uint32_t NonResidentNameTableLength;
	uint32_t NonResidentNameTableChecksum;
	uint32_t AutoDSObjectNum;
	uint32_t DebugInfoOffset;
	uint32_t DebugInfoLength;
	uint32_t NumInstancePreload;
	uint32_t NumInstanceDemand;
	uint32_t HeapSize;
} LEHeader;

typedef struct __attribute__ ((__packed__)) _ObjectTableEntry_ {
	uint32_t VirtualSize;
	uint32_t RelocationBaseAddress;
	uint32_t ObjectFlags;
	uint32_t PageTableIndex;
	uint32_t NumPageTableEntries;
	uint32_t Reserved;
} ObjectTableEntry;

typedef struct __attribute__ ((__packed__)) _ObjectPageMapTableEntry_ {
	uint8_t  Unknown;
	uint8_t  FixupTableIndex_HB; // high byte
	uint8_t  FixupTableIndex_LB; // low byte
	uint8_t  Type;
} ObjectPageMapTableEntry;

#pragma pack()

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

	mem_file->mem_allocated = 1;

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


#if ((OUTPUT_TYPE != OUT_ORIG) && (OUTPUT_TYPE != OUT_DOS))
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

        if ((section[BssObject].type != ST_DATA) && (section[BssObject].type != ST_STACK))
        {
            fprintf(stderr, "Error: wrong bss section type: 0x%x - %i\n", BssAddress, section[BssObject].type);
            return 2;
        }

        section[num_sections] = section[BssObject];
        section[num_sections].size -= BssOffset;
        section[num_sections].size_in_file = 0;
        section[num_sections].start += BssOffset;
        section[num_sections].adr += BssOffset;
        section[num_sections].type = ST_UDATA;
#if (OUTPUT_TYPE == OUT_ARM_LINUX)
        strcpy(section[num_sections].name, ".bss");
#else
        sprintf(section[num_sections].name, "useg%.2i", num_sections + 1);
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
#endif

static int add_fixup(unsigned int Entry, uint_fast32_t SourceOffset, uint_fast32_t TargetObject, int_fast32_t TargetOffset, fixup_type FixupType)
{
    fixup_data *fixup;

    fixup = section_fixup_list_FindEntryEqual(Entry, SourceOffset);
    if (fixup == NULL)
    {
        fixup = section_fixup_list_Insert(Entry, SourceOffset);
        if (fixup == NULL)
        {
            fprintf(stderr, "Error: error adding fixup - %i\n", (unsigned int)SourceOffset);
            return -2;
        }

        fixup->tofs = TargetOffset;
        fixup->tsec = TargetObject - 1;
        fixup->type = FixupType;
    }
    else
    {
        if (fixup->tofs != TargetOffset ||
            fixup->tsec != TargetObject - 1 ||
            fixup->type != FixupType)
        {
            fprintf(stderr, "Error: error adding fixup twice - %i\n", (unsigned int)SourceOffset);
            return -3;
        }
    }

    return 0;
}

int SR_LoadFile(const char *fname)
{
	memory_file mf;
	int64_t SR_FileSize;
	uint8_t *MZfile, *LEfile;
	LEHeader *Header;
	ObjectTableEntry *ObjectTable;
	uint8_t *SR_CodeRun;


	// load file
	if ( load_file(&mf, fname) )
	{
		fprintf(stderr, "Error: Unable to load file: %s\n", fname);
		return -1;
	}

	MZfile = (uint8_t *) mf.mem;
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

	if ((*((uint16_t *)&(MZfile[0x18])) != 0x40) || (*((uint16_t *)&(MZfile[0x3c])) == 0))
	{
		// search for next MZ header
		uint_fast32_t HeaderOffset;

		while (1)
		{
			HeaderOffset = *((uint16_t *)&(MZfile[0x02])) + 512 * *((uint16_t *)&(MZfile[0x04]));

			if (*((uint16_t *)MZfile) == 0x5a4d) // MZ
			{
				HeaderOffset -= 512;
			}
			else if (*((uint16_t *)MZfile) != 0x5742) // BW
			{
				unload_file(&mf);
				fprintf(stderr, "Error: Invalid EXE file\n");
				return -1;
			}

			// check if file is big enough to contain MZ EXE header
			if (SR_FileSize <= HeaderOffset + 0x40)
			{
				unload_file(&mf);
				fprintf(stderr, "Error: File too small\n");
				return -1;
			}

			MZfile += HeaderOffset;

			if ((*((uint16_t *)MZfile) == 0x5a4d) && (*((uint16_t *)&(MZfile[0x18])) == 0x40) && (*((uint16_t *)&(MZfile[0x3c])) != 0))
			{
				break;
			}
		};
	}

	{
		uint_fast32_t HeaderOffset;

		// get PE header offset
		HeaderOffset = *((uint32_t *)&(MZfile[0x3c]));
		LEfile = &(MZfile[HeaderOffset]);
		Header = (LEHeader *) LEfile;

		// check if file is big enough to contain LE EXE header
		if (SR_FileSize <= HeaderOffset + sizeof(LEHeader))
		{
			unload_file(&mf);
			fprintf(stderr, "Error: File too small\n");
			return -1;
		}
	}

	// check LE signature
	if (*((uint32_t *)LEfile) != 0x0000454c)
	{
		unload_file(&mf);
		fprintf(stderr, "Error: Invalid EXE file\n");
		return -1;
	}

	EIPObjectNum = Header->EIPObjectNum - 1;
	EIP = Header->EIP;
	ESPObjectNum = Header->ESPObjectNum - 1;
	ESP = Header->ESP;

	num_sections = Header->NumObjectsInModule;

	if (num_sections > MAX_SECTIONS ||
		num_sections == 0)
	{
		unload_file(&mf);
		fprintf(stderr, "Error: Too many or too few object table entries\n");
		return -1;
	}

	ObjectTable = (ObjectTableEntry *) &(LEfile[Header->ObjectTableOffset]);

	// allocate memory for the code
	{
		unsigned int Entry;
		uint_fast32_t size, SR_CodeSize;
		uintptr_t address;

		SR_CodeSize = 0;
		for (Entry = 0; Entry < Header->NumObjectsInModule; Entry ++)
		{

			size = ObjectTable[Entry].VirtualSize / Header->PageSize;
			size *= Header->PageSize;
			if (size != ObjectTable[Entry].VirtualSize) size += Header->PageSize;

			section[Entry].size = SR_CodeSize;
			SR_CodeSize += size;
		}

		SR_CodeBase = (uint8_t *) malloc(SR_CodeSize + Header->PageSize - 1);
		if (SR_CodeBase == NULL)
		{
			unload_file(&mf);
			fprintf(stderr, "Error: Not enough memory\n");
			return -1;

		}

		address = (uintptr_t)SR_CodeBase / Header->PageSize;
		address *= Header->PageSize;
		SR_CodeRun = (address == (uintptr_t)SR_CodeBase)?((uint8_t *) address):((uint8_t *) address + Header->PageSize);

		memset(SR_CodeRun, 0, SR_CodeSize);
	}


	// read pages into allocated memory
	{
		unsigned int Entry, CurPage, RemainingPages;
		uint_fast32_t size;
		uint8_t *DstOffset, *SrcOffset;
		ObjectPageMapTableEntry *ObjectPageMapTable;

		ObjectPageMapTable = (ObjectPageMapTableEntry *) &(LEfile[Header->ObjectPageMapTableOffset]);
		SrcOffset = &(MZfile[Header->DataPagesOffset]);

		for (Entry = 0; Entry < Header->NumObjectsInModule; Entry ++)
		{
			section[Entry].adr          = &(SR_CodeRun[section[Entry].size]);
			section[Entry].size         = ObjectTable[Entry].VirtualSize;
			section[Entry].size_in_file = ObjectTable[Entry].NumPageTableEntries * Header->PageSize;
			section[Entry].start        = ObjectTable[Entry].RelocationBaseAddress;
			section[Entry].flags        = ObjectTable[Entry].ObjectFlags;
			section[Entry].fixup_list   = NULL;
			section[Entry].output_list  = NULL;
			section[Entry].entry_list   = NULL;
			section[Entry].code_list    = NULL;
			section[Entry].label_list   = NULL;
			section[Entry].noret_list   = NULL;
			section[Entry].extrn_list   = NULL;
			section[Entry].alias_list   = NULL;
			section[Entry].replace_list = NULL;
			section[Entry]._chk_list    = NULL;
			section[Entry].bound_list   = NULL;
			section[Entry].region_list  = NULL;
			section[Entry].nocode_list  = NULL;
			section[Entry].iflags_list  = NULL;
			section[Entry].code16_list  = NULL;
			section[Entry].ua_ebp_list  = NULL;
			section[Entry].ua_esp_list  = NULL;
			section[Entry].code2data_list = NULL;
			section[Entry].data2code_list = NULL;

			if ( (Header->ESPObjectNum - 1) == Entry )
			{
				// stack segment
				section[Entry].type = ST_STACK;
			}
			else if (ObjectTable[Entry].ObjectFlags & 4)
			{
				// code segment
				section[Entry].type = ST_CODE;
			}
			else
			{
				// data segment
				section[Entry].type = ST_DATA;
			}

			if (section[Entry].type == ST_CODE)
			{
#if (OUTPUT_TYPE == OUT_ARM_LINUX)
				sprintf(section[Entry].name, ".text");
#else
				sprintf(section[Entry].name, "cseg%.2i", Entry + 1);
#endif
			}
			else
			{
#if (OUTPUT_TYPE == OUT_ARM_LINUX)
				sprintf(section[Entry].name, ".data");
#else
				sprintf(section[Entry].name, "dseg%.2i", Entry + 1);
#endif
			}

			DstOffset = section[Entry].adr;
			CurPage = ObjectTable[Entry].PageTableIndex - 1;
			for (RemainingPages = ObjectTable[Entry].NumPageTableEntries; RemainingPages != 0; RemainingPages--)
			{
				if (ObjectPageMapTable[CurPage].Type != 0)
				{
					unload_file(&mf);
					fprintf(stderr, "Error: Unknown page type\n");
					return -1;
				}

				size = (Entry == Header->NumObjectsInModule - 1 && RemainingPages == 1)?(Header->BytesOnLastPage):(Header->PageSize);
				memcpy(DstOffset, SrcOffset, size);

				DstOffset += size;
				SrcOffset += size;
				CurPage++;
			}
		}
	}

#if ((OUTPUT_TYPE != OUT_ORIG) && (OUTPUT_TYPE != OUT_DOS))
    // try to split uninitialized data into bss section
    if (LoadBssBorder())
    {
        unload_file(&mf);
        fprintf(stderr, "Error: error loading bss border\n");
        return -1;
    }

    if (ESP >= section[ESPObjectNum].size)
    {
        uint_fast32_t SecNum, RelAdr;
        if (SR_get_section_reladr(section[ESPObjectNum].start + ESP, &SecNum, &RelAdr))
        {
            ESPObjectNum = SecNum;
            ESP = RelAdr;
        }
    }
#endif

	// create fixup table
	{
		unsigned int Entry, CurPage, RemainingPages, TargetIs32Bit;
		int ret;
		uint_fast32_t CurIndex, NextIndex, TargetObject, SourceOffset;
		int_fast32_t TargetOffset;
		uint32_t *FixupPageTable;
		uint8_t *FixupRecordTable;
		fixup_type FixupType;

		FixupPageTable = (uint32_t *) &(LEfile[Header->FixupPageTableOffset]);
		FixupRecordTable = &(LEfile[Header->FixupRecordTableOffset]);

		for (Entry = 0; Entry < Header->NumObjectsInModule; Entry ++)
		{
			CurPage = ObjectTable[Entry].PageTableIndex - 1;
			for (RemainingPages = ObjectTable[Entry].NumPageTableEntries; RemainingPages != 0; RemainingPages--)
			{
				CurIndex = FixupPageTable[CurPage];
				NextIndex = FixupPageTable[CurPage + 1];

				while (CurIndex < NextIndex)
				{
					switch (FixupRecordTable[CurIndex])
					{
						case 2:
							FixupType = FT_SEGMENT;
							break;
						case 5:
							FixupType = FT_16BITOFS;
							break;
						case 6:
							FixupType = FT_SEGOFS32;
							break;
						case 7:
							FixupType = FT_NORMAL;
							break;
						case 8:
							FixupType = FT_SELFREL;
							break;
						case 19:
							FixupType = FT_SEGOFS16;
							break;
						default:
							fprintf(stderr, "Error: Unknown fixup source type: %i - %i (%i: %i)\n", FixupRecordTable[CurIndex], FixupRecordTable[CurIndex + 1], Entry, (ObjectTable[Entry].NumPageTableEntries - RemainingPages) * Header->PageSize + *((int16_t *) &(FixupRecordTable[CurIndex + 2])));
							unload_file(&mf);
							return -1;
					}

					switch (FixupRecordTable[CurIndex + 1])
					{
						case 0:
							TargetIs32Bit = 0;
							break;
						case 16:
							TargetIs32Bit = 1;
							break;
						default:
							fprintf(stderr, "Error: Unknown fixup target type: %i\n", FixupRecordTable[CurIndex + 1]);
							unload_file(&mf);
							return -1;
					}

					SourceOffset = (ObjectTable[Entry].NumPageTableEntries - RemainingPages) * Header->PageSize
								 + *((int16_t *) &(FixupRecordTable[CurIndex + 2]));

					TargetObject = *((uint8_t *) &(FixupRecordTable[CurIndex + 4]));
					if (FixupType == FT_SEGMENT)
					{
						TargetOffset = 0x80000000;
						CurIndex += 5;
					}
					else if (TargetIs32Bit)
					{
						TargetOffset = *((int32_t *) &(FixupRecordTable[CurIndex + 5]));
						CurIndex += 9;
					}
					else
					{
						TargetOffset = *((uint16_t *) &(FixupRecordTable[CurIndex + 5]));
						CurIndex += 7;
					}

                    if (TargetOffset != 0x80000000)
                    {
                        if (TargetOffset >= section[TargetObject - 1].size)
                        {
                            uint_fast32_t SecNum, RelAdr;
                            if (SR_get_section_reladr(section[TargetObject - 1].start + TargetOffset, &SecNum, &RelAdr))
                            {
                                TargetObject = SecNum + 1;
                                TargetOffset = RelAdr;
                            }
                        }
                    }

                    if (FixupType == FT_SEGOFS32)
                    {
                        ret = add_fixup(Entry, SourceOffset + 4, TargetObject, 0x80000000, FT_SEGMENT);

                        if (ret)
                        {
                            unload_file(&mf);
                            return ret;
                        }

                        FixupType = FT_NORMAL;
                    }
                    else if (FixupType == FT_SEGOFS16)
                    {
                        ret = add_fixup(Entry, SourceOffset + 2, TargetObject, 0x80000000, FT_SEGMENT);

                        if (ret)
                        {
                            unload_file(&mf);
                            return ret;
                        }

                        FixupType = FT_16BITOFS;
                    }

					ret = add_fixup(Entry, SourceOffset, TargetObject, TargetOffset, FixupType);

					if (ret)
					{
                        unload_file(&mf);
                        return ret;
					}

				}

				CurPage++;
			}
		}
	}

	unload_file(&mf);
	return 0;
}
