#include "myfs.h"
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void print_inodes(INode *inode)
{
	printf("id: %d\n", inode->id);
	printf("name: %s\n", inode->name);
	printf("size: %ld\n", inode->size);
	printf("start at: %ld\n", inode->data_start);
}

// -----------------------------------------------
// inode functions
// -----------------------------------------------

void add_inode(int fd, INode *inode)
{
	write(fd, inode, sizeof(INode));
}

INode *read_inode(int fd, off_t id)
{
	INode *inode = (INode *)malloc(sizeof(INode));
	lseek(fd, id * sizeof(INode), SEEK_SET);
	read(fd, inode, sizeof(INode));
	print_inodes(inode);
	return inode;
}

void change_name(int fd, off_t id, const char *new_name)
{
	INode *inode = read_inode(fd, id);
	strcpy(inode->name, new_name);
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

void change_data_start(int fd, off_t id, size_t new_start)
{
	INode *inode = read_inode(fd, id);
	inode->data_start = new_start;
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

void change_size(int fd, off_t id, size_t new_size)
{
	INode *inode = read_inode(fd, id);
	inode->size = new_size;
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

void next_id(int fd, off_t id, int new_id)
{
	INode *inode = read_inode(fd, id);
	inode->next_id = new_id;
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

void change_type(int fd, off_t id, enum INodeType new_type)
{
	INode *inode = read_inode(fd, id);
	inode->type = new_type;
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

void change_status(int fd, off_t id, enum STATUS new_status)
{
	INode *inode = read_inode(fd, id);
	inode->status = new_status;
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

void change_path(int fd, off_t id, const char *new_path)
{
	INode *inode = read_inode(fd, id);
	strcpy(inode->real_path, new_path);
	lseek(fd, id * sizeof(INode), SEEK_SET);
	write(fd, inode, sizeof(INode));
}

// -----------------------------------------------
// directory functions
// -----------------------------------------------
myDIR *read_directory(int fd, int inode_id)
{
	INode *inode = read_inode(fd, inode_id);
	if (inode->type != _DIRECTORY)
	{
		return NULL;
	}
	lseek(fd, inode->data_start, SEEK_SET);
	myDIR *dir = malloc(sizeof(myDIR));
	read(fd, dir, sizeof(myDIR));
	return dir;
}

myDIR *myopendir(const char *name)
{
	for (size_t i = 0; i < NUM_INODES; i++)
	{
		INode *inode = read_inode(fd, i);
		if (inode->type == _DIRECTORY)
		{
			if (strcmp(inode->name, name) == 0)
			{
				return read_directory(fd, i);
			}
		}
	}
}

void mymkdir(int fd, const char *path)
{
}

// -----------------------------------------------
// main functions
// -----------------------------------------------
int mkfs(const char *name)
{
	int new_fs = open(name, O_CREAT);
	close(new_fs);

	for (size_t i = 0; i < NUM_INODES; i++)
	{
		INode *inode = read_inode(fd, i);
		if (inode->status == _UNUSED && inode->type != _FILE_SYSTEM)
		{
			change_path(fd, i, strcat("./", name));
			change_type(fd, i, _FILE_SYSTEM);
			return 0;
		}
	}

	return -1;
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
	char path[1024] = {0};
	strcat(path, "root");
	strcat(path, source);
	strcat(path, target);
	printf("Path: %s\n", path);

	INode *fs_inode;
	for (size_t i = 0; i < NUM_INODES; i++)
	{
		fs_inode = read_inode(fd, i);
		if (fs_inode->status == _UNUSED && fs_inode->type == _FILE_SYSTEM)
		{
			change_name(fd, i, path);
			change_status(fd, i, _USED);
			break;
		}
	}

	int new_fd = open(fs_inode->real_path, O_RDWR);

	INode root_inode = {0, path[0], path[0], sizeof(INode) * NUM_INODES, INODE_INIT_SIZE, -1, _DIRECTORY, _USED};
	add_inode(new_fd, &root_inode);
	for (size_t i = 1; i < NUM_INODES; i++)
	{
		INode inode = {i, "test", "", sizeof(INode) * NUM_INODES + (INODE_INIT_SIZE * i), INODE_INIT_SIZE, -1, _UNKNOWN, _UNUSED};
		add_inode(new_fd, &inode);
	}

	myDIR root_dir = {0, {0}, target, 0};
	lseek(new_fd, root_inode.data_start, SEEK_SET);
	write(new_fd, &root_dir, sizeof(myDIR));
	close(new_fd);
	return new_fd;
}

int mymount_root(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
	char path[1024] = {0};
	strcat(path, source);
	strcat(path, target);
	printf("Path: %s\n", path);

	INode root_inode = {0, path, "./root", sizeof(INode) * NUM_INODES, INODE_INIT_SIZE, -1, _DIRECTORY, _USED};
	add_inode(fd, &root_inode);
	for (size_t i = 1; i < NUM_INODES; i++)
	{
		INode inode = {i, "test", "", sizeof(INode) * NUM_INODES + (INODE_INIT_SIZE * i), INODE_INIT_SIZE, -1, _UNKNOWN, _UNUSED};
		add_inode(fd, &inode);
	}

	myDIR root_dir;
	root_dir.id = 0;
	strcpy(root_dir.name, target);
	root_dir.num_inodes = 0;

	lseek(fd, root_inode.data_start, SEEK_SET);
	write(fd, &root_dir, sizeof(myDIR));
	close(fd);
	return fd;
}