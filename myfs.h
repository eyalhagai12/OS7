#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ROOT_DIR "root"

#pragma once

#define MAXOPENFILES 10000
#define MAXNAMESIZE 1024
#define NUM_INODES 1000
#define INODE_INIT_SIZE 1024 

int fd; // global file descriptor

// --------------------------------------------------------------------
// enums
// --------------------------------------------------------------------
enum INodeType
{
	_FILE,
	_DIRECTORY,
	_FILE_SYSTEM,
	_UNKNOWN
};

enum STATUS
{
	_UNUSED,
	_USED
};

// --------------------------------------------------------------------
// structs
// --------------------------------------------------------------------
typedef struct inode_t
{
	int id;
	char name[MAXNAMESIZE];
	char real_path[MAXNAMESIZE];
	size_t data_start;
	size_t size;
	int next_id;
	enum INodeType type;
	enum STATUS status;
} INode;

typedef struct mydir_t
{
	int id;
	int inodes[100];
	char name[256];
	size_t num_inodes;
} myDIR;

// --------------------------------------------------------------------
// inode functions
// --------------------------------------------------------------------
int mymount(const char *source, const char *target,
			const char *filesystemtype, unsigned long mountflags, const void *data);

int mymount_root(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);

INode *read_inode(int fd, off_t id);

void change_name(int fd, off_t id, const char *new_name);

void change_data_start(int fd, off_t id, size_t new_start);

void change_size(int fd, off_t id, size_t new_size);

void next_id(int fd, off_t id, int new_id);

void change_type(int fd, off_t id, enum INodeType new_type);

void change_status(int fd, off_t id, enum STATUS new_status);

// --------------------------------------------------------------------
// directory functions
// --------------------------------------------------------------------
myDIR *read_directory(int fd, int inode_id);
