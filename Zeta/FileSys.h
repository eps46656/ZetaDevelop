
#pragma once

typedef struct Zeta_FileSys_FileInfo Zeta_FileSys_FileInfo;

struct Zeta_FileSys_FileInfo {
    int is_file;
};

typedef struct Zeta_FileSysManager Zeta_FileSysManager;

struct Zeta_FileSysManager {
    void* context;

    void* (*Open)(void* context);
    void (*Close)(void* context, void* sd);

    //
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






The operations of File System Manager.

DirMode/FileMode:
    GetInfo:
        Return the info of working directory.

    GetPath:
        Return the path of opening directory/file.

    GetRLockCnt:
        The working directory/file is called "locked" if and only if it's
        rlock_cnt is greater than zero.

        rlock_cnt is zero initially.

        Return rlock_cnt of the working directory/file.

    GetWLockCnt:


    RLock:
        Lock the state of the working directory/file.

        Increase the rlock_cnt of the working directory/file.

        After RLock and before Unlock, modification and WLock operator will be
        blocked.

        Still accept other RLock operatorions.

    WLock:
        Lock the state of the working directory/file.

        Increase the wlock_cnt of the working directory/file.

        After RLock and before Unlock, modification, RLock and WLock operator
        will be blocked.

    Unlock:
        Unlock the state of the working directory/file.

        Increase the lock_cnt of the working directory/file.

        Return if the working directory/file is locked.

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
