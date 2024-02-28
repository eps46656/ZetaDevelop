#include "ELF.h"

#include "utils.h"

#define READ(length, dst)                              \
    {                                                  \
        if (data_end - data < length) { return NULL; } \
        dst = ReadFunc(data, length);                  \
        data += l;                                     \
    }                                                  \
    ZETA_StaticAssert(TRUE)

#define WRITE(length, val)                        \
    {                                             \
        if (dst_end - dst < length) { return 1; } \
        WriteFunc(dst, length, val);              \
        dst += length;                            \
    }                                             \
    ZETA_StaticAssert(TRUE)

byte_t const* Zeta_ELF_ReadHeader(Zeta_ELF_Header* dst, byte_t const* data,
                                  byte_t const* data_end) {
    size_t (*ReadFunc)(byte_t const* data, unsigned int length) =
        Zeta_ReadLittleEndian;

    if (data_end - data < 4) { return NULL; }

    dst->ei_mag[0] = data[0];
    dst->ei_mag[1] = data[1];
    dst->ei_mag[2] = data[2];
    dst->ei_mag[3] = data[3];
    data += 4;

    READ(1, dst->ei_class);
    ZETA_DebugAssert(dst->ei_class == 1 || dst->ei_class == 2);

    unsigned int length = dst->ei_class == 1 ? 4 : 8;

    READ(1, dst->ei_data);
    ZETA_DebugAssert(dst->ei_data == 1 || dst->ei_data == 2);

    if (dst->ei_data == 2) { ReadFunc = Zeta_ReadBigEndian; }

    READ(1, dst->ei_version);
    READ(1, dst->ei_osabi);
    READ(1, dst->ei_abiversion);

    if (data_end - data < 7) { return NULL; }
    data += 7;

    READ(2, dst->e_type);
    READ(2, dst->e_machine);
    READ(4, dst->e_version);

    READ(length, dst->e_entry);
    READ(length, dst->e_phoff);
    READ(length, dst->e_shoff);

    READ(4, dst->e_flags);

    READ(2, dst->e_ehsize);

    READ(2, dst->e_phentsize);
    READ(2, dst->e_phnum);

    READ(2, dst->e_shentsize);
    READ(2, dst->e_shnum);

    READ(2, dst->e_shstrndx);
}

byte_t* Zeta_ELF_WriteHeader(byte_t* dst, byte_t* dst_end,
                             Zeta_ELF_Header* header) {
    //
}

void Zeta_ELF_ReadProgramHeader(Zeta_ELF_ProgramHeader* dst, byte_t const* data,
                                byte_t const* data_end) {
    ZETA_DebugAssert(data <= data_end);

    size_t (*ReadFunc)(byte_t const* data, unsigned int length) =
        header->ei_data == 1 ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    unsigned int length = header->ei_class == 1 ? 4 : 8;

    READ(4, dst->p_type);

    if (length == 8) { READ(4, dst->p_flags); }

    READ(length, dst->p_offset);
    READ(length, dst->p_vaddr);
    READ(length, dst->p_paddr);
    READ(length, dst->p_filesz);
    READ(length, dst->p_memsz);

    if (length == 4) { READ(4, dst->p_flags); }

    READ(length, dst->p_align);
}

void Zeta_ELF_ReadSectionHeader(Zeta_ELF_SectionHeader* dst, byte_t const* data,
                                byte_t const* data_end) {
    ZETA_DebugAssert(data <= data_end);

    size_t (*ReadFunc)(byte_t const* data, unsigned int length) =
        header->ei_data == 1 ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    unsigned int length = header->ei_class == 1 ? 4 : 8;

    READ(4, dst->sh_name);
    READ(4, dst->sh_type);

    READ(length, dst->sh_flags);

    READ(length, dst->sh_addr);
    READ(length, dst->sh_offset);
    READ(length, dst->sh_size);

    READ(4, dst->sh_link);
    READ(4, dst->sh_info);

    READ(length, dst->sh_addralign);
    READ(length, dst->sh_entsize);
}

ZETA_extern_c_end;
