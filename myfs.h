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
#define NUM_INODES super.num_inodes
#define INODE_INIT_SIZE super.inode_init_size

int fd; // global file descriptor

int global_idx; // for myreaddir
int global_id;

int openfiles[MAXOPENFILES];
int openfiles_pos[MAXOPENFILES];

// --------------------------------------------------------------------
// enums
// --------------------------------------------------------------------
enum INodeType
{
	_FILE,
	_DIRECTORY,
	_SUBFILE,
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
	size_t data_start;
	size_t size;
	int next_id;
	enum INodeType type;
	enum STATUS status;
	size_t length;
} INode;

typedef struct mydir_t
{
	int id;
	int inodes[100];
	char name[256];
	size_t num_inodes;
} myDIR;

typedef struct mydirent
{
	char d_name[256];
} mydirent;

typedef struct superblock_t
{
	size_t num_inodes;
	size_t inode_init_size;
} superblock;

superblock super;

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

myDIR *myopendir(const char *name);

struct mydirent *myreaddir(myDIR *dirp);

int myclosedir(myDIR *dirp);

int myopen(const char *pathname, int flags);

int myclose(int myfd);

ssize_t myread(int myfd, void *buf, size_t count);

ssize_t mywrite(int myfd, const void *buf, size_t count);

off_t mylseek(int myfd, off_t offset, int whence);