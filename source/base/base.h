/* ./base/base.h
 * by kimht
 */
#ifndef _BASE_BASE_H_
#define _BASE_BASE_H_

///////////////////////////////////////////////////////////////////////////////
/// include
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <set>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/uio.h>

#include <capstone/capstone.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
/// define
///////////////////////////////////////////////////////////////////////////////

/*
 * HANDLE_ERROR - Handle error handling. Do perror() and return a value.
 */
#define HANDLE_ERROR(msg, ret_val) {  \
    perror(msg);  \
    return ret_val;  \
}

/// class
struct Line {
    uint64_t pc;
    string mnemonic;
    string op_str;
    bool is_branch;
    uint64_t level;
};

#endif

