#include "../Zeta/define.h"

typedef unsigned EX;
typedef unsigned long long RX;

typedef struct CpuInfo CpuInfo;

struct CpuInfo {
    EX max_func_num;

    char vender_id[16];

    EX type;

    EX family;
    EX ext_family;

    EX model;
    EX ext_model;

    EX stepping;

    RX mem_size;
};

extern void GetCpuInfo(CpuInfo* dst) __attribute__((sysv_abi));

extern RX TargetFunc() __attribute__((sysv_abi));

extern RX GetRIPA() __attribute__((sysv_abi));
extern RX GetRIPB() __attribute__((sysv_abi));

int main() {
    CpuInfo cpu_info;

    GetCpuInfo(&cpu_info);

    ZETA_PrintPos;

    ZETA_PrintVar(cpu_info.vender_id);
    ZETA_PrintVar(cpu_info.type);
    ZETA_PrintVar(cpu_info.family);
    ZETA_PrintVar(cpu_info.ext_family);
    ZETA_PrintVar(cpu_info.model);
    ZETA_PrintVar(cpu_info.ext_model);
    ZETA_PrintVar(cpu_info.stepping);

    return 0;
}
