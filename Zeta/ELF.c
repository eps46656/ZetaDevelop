#include "ELF.h"

#include "utils.h"

#define ASSERT_RET(cond)           \
    if (!(cond)) { goto ERR_RET; } \
    ZETA_StaticAssert(TRUE)

#define READ(dst, length)                  \
    ASSERT_RET(length <= data_end - data); \
    dst = ReadFunc(data, length);          \
    data += length;                        \
    ZETA_StaticAssert(TRUE)

#define WRITE(val, length)               \
    ASSERT_RET(length <= dst_end - dst); \
    WriteFunc(dst, val, length);         \
    dst += length;                       \
    ZETA_StaticAssert(TRUE)

byte_t const* Zeta_ELF_ReadHeader(Zeta_ELF_Header* dst, byte_t const* data,
                                  byte_t const* data_end) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    u128_t (*ReadFunc)(byte_t const* data, unsigned int length) =
        Zeta_ReadLittleEndian;

    ASSERT_RET(4 <= data_end - data && data[0] == 0x7F && data[1] == 'E' &&
               data[2] == 'L' && data[3] == 'F');

    data += 4;

    READ(dst->ei_class, 1);
    ASSERT_RET(dst->ei_class == 1 || dst->ei_class == 2);

    unsigned int length = dst->ei_class == 1 ? 4 : 8;

    READ(dst->ei_data, 1);
    ASSERT_RET(dst->ei_data == 1 || dst->ei_data == 2);

    if (dst->ei_data == 2) { ReadFunc = Zeta_ReadBigEndian; }

    READ(dst->ei_version, 1);
    READ(dst->ei_osabi, 1);
    READ(dst->ei_abiversion, 1);

    ASSERT_RET(7 <= data_end - data);
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

    return data;

ERR_RET:
    return NULL;
}

byte_t* Zeta_ELF_WriteHeader(byte_t* dst, byte_t* dst_end,
                             Zeta_ELF_Header* header) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);
    ZETA_DebugAssert(header != NULL);

    ASSERT_RET(4 <= dst_end - dst);

    u128_t (*WriteFunc)(byte_t* dst, u128_t val, unsigned int length) =
        Zeta_WriteLittleEndian;

    WRITE(0x7F, 1);
    WRITE('E', 1);
    WRITE('L', 1);
    WRITE('F', 1);

    unsigned int length;

    switch (header->ei_class) {
        case 1: length = 4; break;
        case 2: length = 8; break;
        default: goto ERR_RET;
    }

    WRITE(header->ei_class, 1);

    switch (header->ei_data) {
        case 1: break;
        case 2: WriteFunc = Zeta_WriteBigEndian; break;
        default: goto ERR_RET;
    }

    WRITE(header->ei_version, 1);
    WRITE(header->ei_osabi, 1);
    WRITE(header->ei_abiversion, 1);

    ASSERT_RET(7 <= dst_end - dst);
    dst += 7;

    WRITE(header->e_type, 2);
    WRITE(header->e_machine, 2);
    WRITE(header->e_version, 4);

    WRITE(header->e_entry, length);
    WRITE(header->e_phoff, length);
    WRITE(header->e_shoff, length);

    WRITE(header->e_flags, 4);

    WRITE(header->e_ehsize, 2);

    WRITE(header->e_phentsize, 2);
    WRITE(header->e_phnum, 2);

    WRITE(header->e_shentsize, 2);
    WRITE(header->e_shnum, 2);

    WRITE(header->e_shstrndx, 2);

    return dst;

ERR_RET:
    return NULL;
}

void Zeta_ELF_ReadProgramHeader(Zeta_ELF_ProgramHeader* dst,
                                Zeta_ELF_Header* header, byte_t const* data,
                                byte_t const* data_end) {
    ZETA_DebugAssert(data <= data_end);

    u128_t (*ReadFunc)(byte_t const* data, unsigned int length) =
        header->ei_data == 1 ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    unsigned int length = header->ei_class == 1 ? 4 : 8;

    READ(dst->p_type, 4);

    if (length == 8) { READ(dst->p_flags, 4); }

    READ(dst->p_offset, length);
    READ(dst->p_vaddr, length);
    READ(dst->p_paddr, length);
    READ(dst->p_filesz, length);
    READ(dst->p_memsz, length);

    if (length == 4) { READ(dst->p_flags, 4); }

    READ(dst->p_align, length);

ERR_RET:
    return;
}

void Zeta_ELF_ReadSectionHeader(Zeta_ELF_SectionHeader* dst,
                                Zeta_ELF_Header* header, byte_t const* data,
                                byte_t const* data_end) {
    ZETA_DebugAssert(data <= data_end);

    u128_t (*ReadFunc)(byte_t const* data, unsigned int length) =
        header->ei_data == 1 ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    unsigned int length = header->ei_class == 1 ? 4 : 8;

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

ERR_RET:
    return;
}

ZETA_extern_c_end;
