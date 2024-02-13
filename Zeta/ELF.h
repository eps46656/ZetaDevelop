#pragma once

#include "define.h"

ZETA_extern_c_beg;

#define ZETA_ELF_PT_NULL 0
#define ZETA_ELF_PT_LOAD 1
#define ZETA_ELF_PT_DYNAMIC 2
#define ZETA_ELF_PT_INTERP 3
#define ZETA_ELF_PT_NOTE 4
#define ZETA_ELF_PT_SHLIB 5
#define ZETA_ELF_PT_PHDR 6
#define ZETA_ELF_PT_LOSUNW 0x6ffffffa
#define ZETA_ELF_PT_SUNWBSS 0x6ffffffb
#define ZETA_ELF_PT_SUNWSTACK 0x6ffffffa
#define ZETA_ELF_PT_HISUNW 0x6fffffff
#define ZETA_ELF_PT_LOPROC 0x70000000
#define ZETA_ELF_PT_HIPROC 0x7fffffff

#define ZETA_ELF_SHT_NULL 0
#define ZETA_ELF_SHT_PROGBITS 1
#define ZETA_ELF_SHT_SYMTAB 2
#define ZETA_ELF_SHT_STRTAB 3
#define ZETA_ELF_SHT_RELA 4
#define ZETA_ELF_SHT_HASH 5
#define ZETA_ELF_SHT_DYNAMIC 6
#define ZETA_ELF_SHT_NOTE 7
#define ZETA_ELF_SHT_NOBITS 8
#define ZETA_ELF_SHT_REL 9
#define ZETA_ELF_SHT_SHLIB 10
#define ZETA_ELF_SHT_DYNSYM 11
#define ZETA_ELF_SHT_LOPROC 0x70000000
#define ZETA_ELF_SHT_HIPROC 0x7FFFFFFF
#define ZETA_ELF_SHT_LOUSER 0x80000000
#define ZETA_ELF_SHT_HIUSER 0xFFFFFFFF

typedef struct Zeta_ELF_Ret Zeta_ELF_Ret;

struct Zeta_ELF_Ret {
    int rc;
    byte_t* nxt_dst;
    const byte_t* nxt_src;
};

typedef struct Zeta_ELF_Header Zeta_ELF_Header;

struct Zeta_ELF_Header {
    char ei_mag[4];

    byte_t ei_class;
    byte_t ei_data;

    byte_t ei_version;
    byte_t ei_osabi;
    byte_t ei_abiversion;

    u16_t e_type;
    u16_t e_machine;

    u32_t e_version;
    u64_t e_entry;
    u64_t e_phoff;
    u64_t e_shoff;

    u32_t e_flags;

    u16_t e_ehsize;

    u16_t e_phentsize;
    u16_t e_phnum;

    u16_t e_shentsize;
    u16_t e_shnum;

    u16_t e_shstrndx;
};

typedef struct Zeta_ELF_ProgramHeader Zeta_ELF_ProgramHeader;

struct Zeta_ELF_ProgramHeader {
    u32_t p_type;

    u32_t p_flags;

    u64_t p_offset;
    u64_t p_vaddr;
    u64_t p_paddr;
    u64_t p_filesz;
    u64_t p_memsz;

    u64_t p_align;
};

typedef struct Zeta_ELF_SectionHeader Zeta_ELF_SectionHeader;

struct Zeta_ELF_SectionHeader {
    u32_t sh_name;

    u32_t sh_type;

    u64_t sh_flags;

    u64_t sh_addr;
    u64_t sh_offset;
    u64_t sh_size;

    u32_t sh_link;
    u32_t sh_info;

    u64_t sh_addralign;
    u64_t sh_entsize;
};

int Zeta_ELF_ReadHeader(Zeta_ELF_Header* dst, const byte_t* src,
                        const byte_t* src_end);

void Zeta_ELF_ReadProgramHeader(Zeta_ELF_ProgramHeader* dst, const byte_t* src,
                                const byte_t* src_end);

void Zeta_ELF_ReadSectionHeader(Zeta_ELF_SectionHeader* dst, const byte_t* src,
                                const byte_t* src_end);

ZETA_extern_c_end;
