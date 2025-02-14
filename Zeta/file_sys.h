#pragma once

#include "assoc_cntr.h"
#include "datetime.h"
#include "seq_cntr.h"

ZETA_DeclareStruct(Zeta_FileSys_NodeAttr);
ZETA_DeclareStruct(Zeta_FileSys_LinkAttr);
ZETA_DeclareStruct(Zeta_FileSys_Manager);

struct Zeta_FileSys_NodeAttr {
    unsigned long long nid;

    Zeta_SeqCntr const* name;

    /**
     * @brief Indicate whether the node has directory attribute.
     */
    bool_t is_dir;

    /**
     * @brief Indicate whether the linked node has file attribute.
     */
    bool_t is_file;

    size_t linked_cnt;

    Zeta_DateTime crt_datetime;
    Zeta_DateTime acc_datetime;
    Zeta_DateTime wrt_datetime;

    unsigned long long uid;
    unsigned long long gid;

    unsigned char user_rwx;
    unsigned char group_rwx;
    unsigned char other_rwx;
};

struct Zeta_FileSys_LinkAttr {
    Zeta_SeqCntr const* name;

    /**
     * @brief Indicate whether the linked node has directory attribute.
     */
    bool_t is_dir;

    /**
     * @brief Indicate whether the linked node has file attribute.
     */
    bool_t is_file;

    Zeta_DateTime crt_datetime;
    Zeta_DateTime acc_datetime;
    Zeta_DateTime wrt_datetime;

    unsigned long long user;
    unsigned long long group;

    unsigned char user_rwx;
    unsigned char group_rwx;
    unsigned char other_rwx;
};

struct Zeta_FileSys_Manager {
    void* context;

    void* (*OpenSession)(void* context);
    void (*CloseSession)(void* context, void* sd);

    Zeta_SeqCntr* (*GetPath)(void* context, void* sd);

    void (*GetInfo)(void* context, void* sd, size_t li,
                    Zeta_FileSys_NodeAttr* dst);

    Zeta_AssocCntr const* (*GetLinks)(void* context, void* sd);

    void (*Select)(void* context, void* sd, void* li);

    void (*Deselect)(void* context, void* sd, void* li);

    void (*CreateNode)(void* context, void* sd,
                       Zeta_FileSys_NodeAttr const* node_attr, void* dst_li);

    void (*AddLink)(void* context, void* sd, void* src_sd);

    void (*RemoveLink)(void* context, void* sd, void* li);

    void (*UpdateNode)(void* context, void* sd,
                       Zeta_FileSys_NodeAttr const* node_attr);

    void (*UpdateLink)(void* context, void* sd, void* li,
                       Zeta_FileSys_LinkAttr const* link_attr);

    Zeta_SeqCntr* (*GetData)(void* context, void* sd);
};
