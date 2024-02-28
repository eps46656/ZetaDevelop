#pragma once

#include "define.h"

typedef struct Zeta_DiskInfo Zeta_DiskInfo;

struct Zeta_DiskInfo {
    size_t sec_size;
    size_t num_of_secs;

    size_t num_of_cylinders;
    size_t heads_per_cylinder;
    size_t secs_per_track;
};
