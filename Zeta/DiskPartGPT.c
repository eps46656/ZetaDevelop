#include "DiskPartGPT.h"
#include "utils.h"

ZETA_STATIC_ASSERT(4 <= sizeof(size_t))

ZETA_DECL_STRUCT(Zeta_DiskPart_GPTHeader) {
    char sign[8];

    size_t revision_num;
    size_t size_of_header;

    size_t crc32_of_header;

    size_t zero_reserved;

    size_t cur_lba;
    size_t backup_lba;

    size_t usable_lba_beg;
    size_t usable_lba_end;

    size_t guid;

    size_t lba_beg_of_part_entries;
    size_t num_of_part_entries;
    size_t size_of_part_entries;
    size_t crc32_of_part_entries;
};

const byte_t* Zeta_DiskPart_GPT_ReadHeader(Zeta_DiskPart_GPTHeader* dst,
                                           const byte_t* data) {
    for (int i = 0; i < 8; ++i, ++data) { dst->sign[i] = data[0]; }

    dst->revision_num = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->size_of_header = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->crc32_of_header = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->zero_reserved = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->cur_lba = Zeta_ReadLittleEndian(data, 8);
    data += 8;

    dst->backup_lba = Zeta_ReadLittleEndian(data, 8);
    data += 8;

    dst->usable_lba_beg = Zeta_ReadLittleEndian(data, 8);
    data += 8;

    dst->usable_lba_end = Zeta_ReadLittleEndian(data, 8);
    data += 8;

    dst->guid_l = Zeta_ReadLittleEndian(data, 8);
    data += 8;

    dst->lba_beg_of_part_entries = Zeta_ReadLittleEndian(data, 8);
    data += 8;

    dst->num_of_part_entries = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->size_of_part_entries = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->crc32_of_part_entries = Zeta_ReadLittleEndian(data, 4);
    data += 4;
}

const byte_t* Zeta_DiskPart_GPT_PartEntry(Zeta_DiskPart_PartEntry* dst,
                                          const byte_t* data) {
    //
}
