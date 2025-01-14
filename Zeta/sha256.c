#include "sha256.h"

#include "debugger.h"
#include "utils.h"

typedef Zeta_SHA256Hasher_word_t word_t;

static word_t const k_table[] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1,
    0x923F82A4, 0xAB1C5ED5, 0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174, 0xE49B69C1, 0xEFBE4786,
    0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147,
    0x06CA6351, 0x14292967, 0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85, 0xA2BFE8A1, 0xA81A664B,
    0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A,
    0x5B9CCA4F, 0x682E6FF3, 0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

void Zeta_SHA256_Hash(byte_t* dst, byte_t const* src, size_t cnt) {
    Zeta_SHA256Hasher hasher;
    Zeta_SHA256Hasher_Init(&hasher);

    Zeta_SHA256Hasher_Rotate(&hasher, src, cnt);

    Zeta_SHA256Hasher_GetResult(&hasher, dst);
}

void Zeta_SHA256Hasher_Init(void* hasher_) {
    Zeta_SHA256Hasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);

    hasher->hs[0] = 0x6A09E667;
    hasher->hs[1] = 0xBB67AE85;
    hasher->hs[2] = 0x3C6EF372;
    hasher->hs[3] = 0xA54FF53A;
    hasher->hs[4] = 0x510E527F;
    hasher->hs[5] = 0x9B05688C;
    hasher->hs[6] = 0x1F83D9AB;
    hasher->hs[7] = 0x5BE0CD19;

    hasher->size = 0;
}

void Zeta_SHA256Hasher_Reset(void* hasher) { Zeta_SHA256Hasher_Init(hasher); }

size_t Zeta_SHA256Hasher_GetSize(void* hasher_) {
    Zeta_SHA256Hasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);
    return hasher->size;
}

size_t Zeta_SHA256Hasher_GetResultWidth(void* hasher_) {
    Zeta_SHA256Hasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);
    return 32;
}

static void HashChunk_(word_t* hs, byte_t const* data) {
    word_t w[64];

    for (int i = 0; i < 16; ++i) { w[i] = ZETA_ReadBigEndian4(data + 4 * i); }

    for (int i = 16; i < 64; ++i) {
        word_t s0 = __builtin_rotateright32(w[i - 15], 7) ^
                    __builtin_rotateright32(w[i - 15], 18) ^ (w[i - 15] >> 3);
        word_t s1 = __builtin_rotateright32(w[i - 2], 17) ^
                    __builtin_rotateright32(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    word_t a = hs[0];
    word_t b = hs[1];
    word_t c = hs[2];
    word_t d = hs[3];
    word_t e = hs[4];
    word_t f = hs[5];
    word_t g = hs[6];
    word_t h = hs[7];

    for (int i = 0; i < 64; ++i) {
        word_t s0 = __builtin_rotateright32(a, 2) ^
                    __builtin_rotateright32(a, 13) ^
                    __builtin_rotateright32(a, 22);
        word_t s1 = __builtin_rotateright32(e, 6) ^
                    __builtin_rotateright32(e, 11) ^
                    __builtin_rotateright32(e, 25);

        word_t maj = (a & b) ^ (a & c) ^ (b & c);
        word_t ch = (e & f) ^ (~e & g);

        word_t t1 = h + s1 + ch + k_table[i] + w[i];
        word_t t2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    hs[0] += a;
    hs[1] += b;
    hs[2] += c;
    hs[3] += d;
    hs[4] += e;
    hs[5] += f;
    hs[6] += g;
    hs[7] += h;
}

void Zeta_SHA256Hasher_GetResult(void* hasher_, byte_t* dst) {
    Zeta_SHA256Hasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);

    ZETA_DebugAssert(dst != NULL);

    size_t cur_size = hasher->size;

    Zeta_SHA256Hasher tmp_hasher;
    Zeta_MemCopy(&tmp_hasher, hasher, sizeof(Zeta_SHA256Hasher));

    byte_t buffer[64 + 8];
    buffer[0] = 0x80;

    size_t buffer_i = 1;

    for (; (cur_size + buffer_i) % 64 != 56; ++buffer_i) {
        buffer[buffer_i] = 0;
    }

    ZETA_UIntToBytes(8 * cur_size, buffer + buffer_i, 8, ZETA_BigEndian);
    buffer_i += 8;

    Zeta_SHA256Hasher_Rotate(&tmp_hasher, buffer, buffer_i);

    ZETA_DebugAssert(tmp_hasher.size % 64 == 0);

    for (int i = 0; i < 8; ++i) {
        ZETA_UIntToBytes(tmp_hasher.hs[i], dst + 4 * i, 4, ZETA_BigEndian);
    }
}

void Zeta_SHA256Hasher_Rotate(void* hasher_, byte_t const* src, size_t cnt) {
    Zeta_SHA256Hasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);

    ZETA_DebugAssert(src != NULL);

    if (cnt) { return; }

    size_t cur_size = hasher->size;

    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(cnt, 64 - cur_size % 64);
        cnt -= cur_cnt;

        Zeta_MemMove(hasher->last_chunk + cur_size % 64, src,
                     sizeof(byte_t) * cur_cnt);

        cur_size += cur_cnt;
        src += cur_cnt;

        if (cur_size % 64 == 0) { HashChunk_(hasher->hs, hasher->last_chunk); }
    }

    hasher->size = cur_size;
}
