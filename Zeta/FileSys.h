
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

ZETA_DeclareStruct(Zeta_FileSys_NodeInfo);

struct Zeta_FileSys_NodeInfo {
    bool_t type;

    Zeta_SegVector const name;

    /**
     * @brief Whether the node has directory attribute, to link other nodes.
     */
    bool_t is_dir;

    /**
     * @brief Whether the node has file attribute, to store binary data.
     */
    bool_t is_file;

    /**
     * @brief The number of links pointing to the this node.
     */
    size_t linked_cnt;

    /**
     * @brief The number of links pointing from the this node to other nodes.
     */
    size_t links_cnt;

    /**
     * @brief The number of bytes stored in this node.
     */
    size_t bytes_cnt;

    size_t user;
    size_t group;

    unsigned char user_rwx;
    unsigned char group_rwx;
    unsigned char other_rwx;
};

#define ZETA_FileSys_LinkIndicator_SelfLink ((size_t)(-1))

#define ZETA_FileSys_LinkIndicator_BaskLink ((size_t)(-2))

/*

Concept:
    Node:
        In a graph structure file system, a node indicates a directory or file
        instance.

    Link:
        In a graph structure file system, a link indicate a relation, connection
        between two nodes.

    Directory Node:
        Directory node is a kind of node which represents a directory instance
        in file system. It maintains a set of links, user can find, insert or
        erase links in the set.

    File Node:
        File node is a kind of node which represents a file instance in file
        system. It maintains a sequence of bytes, user can modify, insert or
        erase bytes in the sequence.

    Real Link:
        Real link is a kind of link which links a node to another node. Is
        indicates a meaningful relation among file managment.

    Dummy Link:
        Dummy link is a kind of link which links a node to another node.

        Self Link:
            Self link is a dummy link. It may not be really stored in file
            system. It can be used to indicate the present working directory of
            a session itself.

        Back Link:
            Back link is a dummy link. It may not be really stored in file
            system. The node linked by back link may be dependent on the explore
            path of session. It can be used to indicate the previous present
            working directory node.

    Link indicator:
        An



    Session: A session is a explore path to
    Session Descriptor: A descriptor to identify, store session context.



Link:

Size(sd):
    The size of the present node of sd.

    If the present node is a directory node, the size of the node is the number
of links it contains.

    If the present node is a file node. the size of the node is the number of
bytes it contains.

Size(sd, li):
    pass

Node(sd, li):
    sd: session descriptor
    li: link indicator

    The node pointed by the link, indicated by li, in the present working node
of session descriptor, sd.

Link(sd, li)
    sd: session descriptor
    li: link indicator

    The link, indicated by li, in the present working node of session
descriptor, sd.

*/

ZETA_DeclareStruct(Zeta_FileSys_Manager);

struct Zeta_FileSys_Manager {
    void* context;

    void* (*OpenSession)(void* context);
    void (*CloseSession)(void* context, void* sd);

    /**
     * @brief Get the info of node, Node(sd, li).
     *
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param li The link indicator of target link.
     * @param dst The destination of node info.
     */
    void (*GetInfo)(void* context, void* sd, size_t li,
                    Zeta_FileSys_NodeInfo* dst);

    /**
     * @brief Open the node, Node(sd, li).
     *
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param li The link indicator of target node, Node(sd, li).
     */
    bool_t (*Open)(void* context, void* sd, size_t li);

    /**
     * @brief Update the node info of target node.
     *
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param li The link indicator of link pointing to target node.
     * @param info The source node info.
     */
    void (*Update)(void* context, void* sd, size_t li,
                   Zeta_FileSys_NodeInfo const* info);

    // Directory mode only
    // Add a link points to the node of "the src_idx th link of src_sd's working
    // node". If src_sd is NULL, than create a new directory/file
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param src_sd The source session descriptor
     * @param src_li The source link indicator pointing to Node(src_sd, src_li).
     * @param info The info for created link. Not all attributes will be adopted
     * to new link which is based on actual type of file system and operation
     * arguments.
     */
    bool_t (*Link)(void* context, void* sd, size_t li, void* src_sd,
                   size_t src_li, Zeta_FileSys_NodeInfo const* info);

    // Directory mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     * @param li The link index in present working directory.
     */
    void (*Unlink)(void* context, void* sd, size_t li);

    // File mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     */
    void (*Read)(void* context, void* sd, size_t li, size_t size,
                 byte_t* dst_data);

    // File mode only
    /**
     * @param context The context of this file sys manager.
     * @param sd The session descriptor of present working session.
     */
    void (*Write)(void* context, void* sd, size_t li, size_t size,
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
