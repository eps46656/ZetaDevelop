#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../Zeta/define.h"

typedef unsigned char byte_t;

typedef _BitInt(16) s16_t;
typedef _BitInt(32) s32_t;
typedef _BitInt(64) s64_t;
typedef _BitInt(128) s128_t;

typedef unsigned _BitInt(16) u16_t;
typedef unsigned _BitInt(32) u32_t;
typedef unsigned _BitInt(64) u64_t;
typedef unsigned _BitInt(128) u128_t;

typedef unsigned long long ull;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

size_t ReadLittleEndian(const byte_t* data, int length) {
    ZETA_DebugAssert(0 <= length);
    ZETA_DebugAssert((size_t)length <= sizeof(size_t));

    size_t ret = 0;

    for (int i = 0; i < length; ++i) { ret |= (size_t)data[i] << (8 * i); }

    return ret;
}

size_t ReadBigEndian(const byte_t* data, int length) {
    ZETA_DebugAssert(0 <= length);
    ZETA_DebugAssert((size_t)length <= sizeof(size_t));

    size_t ret = 0;

    for (int i = length; 0 < i--;) { ret |= (size_t)data[i] << (8 * i); }

    return ret;
}

#define READ(x, l)             \
    {                          \
        x = ReadFunc(data, l); \
        data += l;             \
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct ELF_Header {
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

struct ELF_ProgramHeader {
    u32_t p_type;

    u32_t p_flags;

    u64_t p_offset;
    u64_t p_vaddr;
    u64_t p_paddr;
    u64_t p_filesz;
    u64_t p_memsz;

    u64_t p_align;
};

struct ELF_SectionHeader {
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

struct ELF_Headers {
    //

    ELF_Header header;

    // std::vector<ELF_ProgramHeader>
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define TMP(dst, val, str) \
    case val: dst = str; break;

void ELF_PrintHeader(ELF_Header* header) {
    printf("ei_mag: %d %c(%d) %c(%d) %c(%d)\n", header->ei_mag[0],
           header->ei_mag[1], header->ei_mag[1], header->ei_mag[2],
           header->ei_mag[2], header->ei_mag[3], header->ei_mag[3]);

    printf("ei_class: %d bits\n", header->ei_class == 1 ? 32 : 64);

    printf("ei_data: %s\n",
           header->ei_data == 1 ? "little endian" : "big endian");

    printf("ei_version: %d\n", header->ei_version);

    const char* ei_osabi_str;

    switch (header->ei_version) {
        TMP(ei_osabi_str, 0x00, "System V");
        TMP(ei_osabi_str, 0x01, "HP-UX");
        TMP(ei_osabi_str, 0x02, "NetBSD");
        TMP(ei_osabi_str, 0x03, "Linux");
        TMP(ei_osabi_str, 0x04, "GNU Hurd");
        TMP(ei_osabi_str, 0x06, "Solaris");
        TMP(ei_osabi_str, 0x07, "AIX");
        TMP(ei_osabi_str, 0x08, "IRIX");
        TMP(ei_osabi_str, 0x09, "FreeBSD");
        TMP(ei_osabi_str, 0x0A, "Tru64");
        TMP(ei_osabi_str, 0x0B, "Movell Modesto");
        TMP(ei_osabi_str, 0x0C, "OpenBSD");
        TMP(ei_osabi_str, 0x0D, "OpenVMS");
        TMP(ei_osabi_str, 0x0E, "NonStop Kernel");
        TMP(ei_osabi_str, 0x0F, "AROS");
        TMP(ei_osabi_str, 0x10, "FenixOS");
        TMP(ei_osabi_str, 0x11, "Nuxi CloudABI");
        TMP(ei_osabi_str, 0x12, "Stratus Technologies OpenVOS");

        default: ei_osabi_str = "Unknown";
    }

    printf("ei_osabi: %d(%s)\n", header->ei_version, ei_osabi_str);
    printf("ei_abiversion: %d\n", header->ei_version);
    printf("\n");

    const char* e_type_str;

    switch (header->ei_version) {
        TMP(e_type_str, 0x00, "0x00 Unkown");
        TMP(e_type_str, 0x01, "Relocatable file");
        TMP(e_type_str, 0x02, "Executable file");
        TMP(e_type_str, 0x03, "Shared file");
        TMP(e_type_str, 0x04, "Core file");

        default: e_type_str = "Unkonwn"; break;
    }

    printf("e_type: %d(%s)\n", (int)header->e_type, e_type_str);
    printf("\n");

    const char* e_machine_str;

    switch (header->e_machine) {
        TMP(e_machine_str, 0x00, "No specific");
        TMP(e_machine_str, 0x01, "AT&T WE 32100");
        TMP(e_machine_str, 0x02, "SPARC");
        TMP(e_machine_str, 0x03, "x86");
        TMP(e_machine_str, 0x04, "Motorola 68000");
        TMP(e_machine_str, 0x05, "Motorola 88000");
        TMP(e_machine_str, 0x06, "Intel MCU");
        TMP(e_machine_str, 0x07, "Intel 8086");
        TMP(e_machine_str, 0x08, "MIPS");
        TMP(e_machine_str, 0x09, "IBM System/370");
        TMP(e_machine_str, 0x0A, "MIPS RS3000 Little-endian");

        TMP(e_machine_str, 0x14, "PowerPC");
        TMP(e_machine_str, 0x28, "Arm");
        TMP(e_machine_str, 0x32, "IA-64");
        TMP(e_machine_str, 0x3E, "AMD x86-64");

        TMP(e_machine_str, 0xF3, "RISC-V");

        default: e_machine_str = "Unknown"; break;
    }

    printf("e_machine: %d(0x%llX, %s)\n", (int)header->e_machine,
           (ull)header->e_machine, e_machine_str);

    printf("e_version: %lld\n", (ull)header->e_version);

    printf("\n");

    printf("e_entry: %llu(0x%llX)\n", (ull)header->e_entry,
           (ull)header->e_entry);

    printf("\n");

    printf("e_flags: 0x%llX\n", (ull)header->e_shoff);

    printf("e_ehsize: 0x%llX\n", (ull)header->e_ehsize);

    printf("\n");

    printf("e_phoff: %llu(0x%llX)\n", (ull)header->e_phoff,
           (ull)header->e_phoff);

    printf("e_phentsize: %llu(0x%llX)\n", (ull)header->e_phentsize,
           (ull)header->e_phentsize);

    printf("e_phnum: %llu\n", (ull)header->e_phnum);

    printf("\n");

    printf("e_shoff: %llu(0x%llX)\n", (ull)header->e_shoff,
           (ull)header->e_shoff);

    printf("e_shentsize: %llu(0x%llX)\n", (ull)header->e_shentsize,
           (ull)header->e_shentsize);

    printf("e_shnum: %llu\n", (ull)header->e_shnum);

    printf("\n");

    printf("e_shstrndx: %llu\n", (ull)header->e_shstrndx);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ELF_ReadHeader(ELF_Header* dst, const byte_t* data) {
    size_t (*ReadFunc)(const byte_t* data, int width) = ReadLittleEndian;

    dst->ei_mag[0] = data[0];
    dst->ei_mag[1] = data[1];
    dst->ei_mag[2] = data[2];
    dst->ei_mag[3] = data[3];
    data += 4;

    READ(dst->ei_class, 1);
    ZETA_DebugAssert(dst->ei_class == 1 || dst->ei_class == 2);

    int length = dst->ei_class == 1 ? 4 : 8;

    READ(dst->ei_data, 1);
    ZETA_DebugAssert(dst->ei_data == 1 || dst->ei_data == 2);

    if (dst->ei_data == 2) { ReadFunc = ReadBigEndian; }

    READ(dst->ei_version, 1);
    READ(dst->ei_osabi, 1);
    READ(dst->ei_abiversion, 1);

    data += 7;

    READ(dst->e_type, 2);
    READ(dst->e_machine, 2);
    READ(dst->e_version, 4);

    READ(dst->e_entry, length);
    READ(dst->e_phoff, length);
    READ(dst->e_shoff, length);

    READ(dst->e_flags, 4);

    READ(dst->e_ehsize, 2);

    READ(dst->e_phentsize, 2);
    READ(dst->e_phnum, 2);

    READ(dst->e_shentsize, 2);
    READ(dst->e_shnum, 2);

    READ(dst->e_shstrndx, 2);
}

void ELF_ReadProgramHeader(ELF_ProgramHeader* dst, ELF_Header* header,
                           const byte_t* data) {
    size_t (*ReadFunc)(const byte_t* data, int width) =
        header->ei_data == 0 ? ReadLittleEndian : ReadBigEndian;

    int length = header->ei_class == 0 ? 4 : 8;

    READ(dst->p_type, 4);

    if (length == 8) { READ(dst->p_flags, 4); }

    READ(dst->p_offset, length);
    READ(dst->p_vaddr, length);
    READ(dst->p_paddr, length);
    READ(dst->p_filesz, length);
    READ(dst->p_memsz, length);

    if (length == 4) { READ(dst->p_flags, 4); }

    READ(dst->p_align, length);
}

void ELF_ReadSectionHeader(ELF_SectionHeader* dst, ELF_Header* header,
                           const byte_t* data) {
    size_t (*ReadFunc)(const byte_t* data, int width) =
        header->ei_data == 0 ? ReadLittleEndian : ReadBigEndian;

    int length = header->ei_class == 0 ? 4 : 8;

    READ(dst->sh_name, 4);
    READ(dst->sh_type, 4);

    READ(dst->sh_flags, length);

    READ(dst->sh_addr, length);
    READ(dst->sh_offset, length);
    READ(dst->sh_size, length);

    READ(dst->sh_link, 4);
    READ(dst->sh_info, 4);

    READ(dst->sh_addralign, length);
    READ(dst->sh_entsize, length);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    std::fstream iofs{ "./test_2.elf", std::ios::in | std::ios::binary };

    std::vector<byte_t> data;
    while (iofs) { data.push_back((byte_t)iofs.get()); }

    ZETA_PrintVar("%zu\n", data.size());

    ELF_Header header;

    ELF_ReadHeader(&header, data.data());

    ELF_PrintHeader(&header);

    return 0;

    std::vector<ELF_ProgramHeader> prog_headers;
    prog_headers.resize(header.e_phnum);

    for (int i = 0; i < header.e_phnum; ++i) {
        ELF_ReadProgramHeader(
            &prog_headers[i], &header,
            data.data() + header.e_phoff + header.e_phentsize * i);

        ZETA_PrintVar("%llu", (ull)prog_headers[i].p_type);
    }

    printf("ok\n");

    return 0;
}
