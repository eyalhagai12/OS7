#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#pragma once
#define MAXOPENFILES 10000
#define MAXNAMESIZE 256

int mymount(const char *source, const char *target,
const char *filesystemtype, unsigned long
mountflags, const void *data);

