/* ./base/base.h
 * by kimht
 */
#ifndef _BASE_BASE_H_
#define _BASE_BASE_H_

/* include */
#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>

using namespace std;

/* function declaration */
void PrintErrorMessage(const char *msg);

#endif

