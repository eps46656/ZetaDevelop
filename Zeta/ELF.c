#include "ELF.h"

#include "utils.h"

#define READ(x, l)                                \
    if (!(data + 1 <= data_end)) { return NULL; } \
    x = ReadFunc(data, l);                        \
    data += l;                                    \
    ZETA_StaticAssert(TRUE)

#define WRITE(x, l)                          \
    if (!(dst + l <= dst_end)) { return 1; } \
    WriteFunc(dst, x, l) dst += l;           \
    ZETA_StaticAssert(TRUE)

const byte_t* Zeta_ELF_ReadHeader(Zeta_ELF_Header* dst, const byte_t* src,
                                  const byte_t* src_end) {
    size_t (*ReadFunc)(const byte_t* data, int length) = Zeta_ReadLittleEndian;

    dst->ei_mag[0] = src[0];
    dst->ei_mag[1] = src[1];
    dst->ei_mag[2] = src[2];
    dst->ei_mag[3] = src[3];
    src += 4;
    if (!(src <= src_end)) { return NULL; }

    READ(dst->ei_class, 1);
    ZETA_DebugAssert(dst->ei_class == 1 || dst->ei_class == 2);

    int length = dst->ei_class == 1 ? 4 : 8;

    READ(dst->ei_data, 1);
    ZETA_DebugAssert(dst->ei_data == 1 || dst->ei_data == 2);

    if (dst->ei_data == 2) { ReadFunc = Zeta_ReadBigEndian; }

    READ(dst->ei_version, 1);
    READ(dst->ei_osabi, 1);
    READ(dst->ei_abiversion, 1);

    src += 7;
    ZETA_DebugAssert(src <= data_end);

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

byte_t* Zeta_ELF_WriteHeader(byte_t* dst, byte_t* dst_end,
                             Zeta_ELF_Header* header) {
    //
}

void Zeta_ELF_ReadProgramHeader(Zeta_ELF_ProgramHeader* dst, const byte_t* src,
                                const byte_t* data_end) {
    ZETA_DebugAssert(src <= data_end);

    size_t (*ReadFunc)(const byte_t* data, int width) =
        header->ei_data == 1 ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    int length = header->ei_class == 1 ? 4 : 8;

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

void Zeta_ELF_ReadSectionHeader(Zeta_ELF_SectionHeader* dst, const byte_t* src,
                                const byte_t* data_end) {
    ZETA_DebugAssert(src <= data_end);

    size_t (*ReadFunc)(const byte_t* data, int width) =
        header->ei_data == 1 ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    int length = header->ei_class == 1 ? 4 : 8;

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

ZETA_extern_c_end;
