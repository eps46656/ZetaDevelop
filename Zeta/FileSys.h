
#pragma once

#include "SegVector.h"

/*

Structure of File System:

The general structure of file sytem is directional graph. Nodes are consist of
two types, directory node and file node.

Directory:
    A node which contains links which point to other nodes. The links, a
directory containins, has a certain order which is implementation defined. The
first link of a directory points to the directory itself.

    The size of a directory is the number of links it contains. The size of a
directory is at least one since any directory must contains a link points to
itself.

    The names of links, a directory containins, must be non-duplicated. In other
words, the number of links having same name in one directory must be 0 or 1.

File:
    A node is a leaf of the file system graph, only contains a self pointing
link and bytes.

    The size of a file is the number of bytes it contains.

A file system has only one root directory.

*/

typedef struct Zeta_FileSys_NodeInfo Zeta_FileSys_NodeInfo;

struct Zeta_FileSys_NodeInfo {
    bool_t type;

    const Zeta_SegVector name;

    size_t size;

    size_t links_num;

    size_t user;
    size_t group;

    unsigned char user_rwx;
    unsigned char group_rwx;
    unsigned char other_rwx;
};

typedef struct Zeta_FileSys_Manager Zeta_FileSys_Manager;

struct Zeta_FileSys_Manager {
    void* context;

    void* (*OpenSession)(void* context);
    void (*CloseSession)(void* context, void* sd);

    /**
     * @brief Get the info of child i the present working directory.
     *
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param dst The destination of file info.
     * @param idx The index of link in the present working directry.
     */
    void (*GetInfo)(void* context, void* sd, Zeta_FilSys_FileInfo* dst,
                    size_t idx);

    /**
     * @brief Directory mode only operation. Open the info of child of the
     * present working directory.
     *
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param idx The index of link in the present working directry.
     */
    bool_t (*Open)(void* context, void* sd, size_t idx);

    /**
     * @brief Directory mode only operation. Open the info of child of the
     * present working directory.
     *
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     */
    bool_t (*Close)(void* context, void* sd);

    // Directory/File mode
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     */
    void (*Update)(void* context, void* sd, size_t idx,
                   Zeta_FileSys_NodeInfo const* info);

    // File mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     */
    void (*Read)(void* context, void* sd, size_t idx, size_t size,
                 byte_t* dst_data);

    // File mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     */
    void (*Write)(void* context, void* sd, size_t idx, size_t size,
                  byte_t const* src_data);

    // File mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param idx The begin index of insertion.
     * @param size The size of insertion.
     */
    void (*Insert)(void* context, void* sd, size_t idx, size_t size);

    /**
     * FIle mode only
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param idx The begin index of erasion.
     * @param size The size of erasion.
     */
    void (*Erase)(void* context, void* sd, size_t idx, size_t size);

    // Directory mode only
    // Add a link points to the node of "the src_idx th link of src_sd's working
    // node". If src_sd is NULL, than create a new directory/file
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     *
     * @param info The info for new link.
     * @param src_sd The session descriptor which
     * @param src_idx
     */
    void (*Link)(void* context, void* sd, Zeta_FileSys const* info,
                 void* src_sd, size_t src_idx);

    // Directory mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param idx The link index in present working directory.
     */
    void (*Unlink)(void* context, void* sd, size_t idx);
};

/*

r_cnt, w_cnt are the read opening count and write opengin count of directory and
file.

r_cnt of a directory is meaningless, and should not be used.

w_cnt of a directory is the number of sessions opening the directory or a
descending directory.

r_cnt of a directory is the number of sessions opening the file with read mode.
w_cnt of a directory is the number of sessions opening the file with write mode.


example:
    a session is opening /abc/edf/ghi. Then abc, edf, ghi's w_cnt will inc by
1 respectively.


Trying to remove a directory/file whose w_cnt is greater than 0 will fail.




struct FileSys_FileInfo {
    bool_t is_file;

    Zeta_SegVector path;
};



The operations of File System Manager.

DirMode/FileMode:
    GetInfo:
        Return the info of working directory.

DirMode Only: When session is opening a directory.
    List:
        Query Operation.

        Get a list of directory/file under working directory.

    Open(name)
        Query Operation.

        Open a directory/file user working directory.
        If name is a file, switch mode to FileMode.

    Close()
        Query Operation.

        Close the working directory.
        If working directory is root, then do nothing and return false.

    Create(name, type):
        Modification Operation.

        Create a directory/file with name under working directory.

        Return
            Success     The directory/file has been created successfully.

            Existed     The directory/file has already existed.

            Locked      The working directory has been locked and refused
                        any modification operations.

    Remove(name):
        Modification Operation.

        Remove a direction/file with name under the working directory.

        Return
            Success     The directory/file has been removed successfully.

            NotFound    The directory/file is not found.

            Refered     Other session is opening any sub-directory as its
                        working directory.

            Locked      The working directory has been locked and refused
                        any modification operations.

FileMode Only: When session is opening a file.
    Extend(size, atomic):
        Modification Operation.

        Extend the length of working file to size.
        If the length of working is greater than or equal to size, do nothing.

        If atomic is set, operation only succesful when ref_cnt is 1.
        In other words, operation will fail when other session is also opening
        the working file.

    Truncate(size, atomic):
        Modification Operation.

        Truncate the length of working file to size.
        If the length of working is less than or equal to size, do nothing.

        If atomic is set, operation only succesful when ref_cnt is 1.
        In other words, operation will fail when other session is also opening
        the working file.

        Return:
            Success

            Nothing

            LockBlocked

            AtomicBlocked

    Read(idx, src, size):
        Query Operation.

        Read data.

    Write(idx, dst, size):
        Modification Operation.

        Write data.

    Close():
        Close the working file.
        Switch working mode to DirMode.
*/
