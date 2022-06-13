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

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
	char path[1024] = {0};
	strcat(path, source);
	strcat(path, target);
	printf("Path: %s\n", path);

	int fd = open(path, O_RDWR);
	if (fd == -1)
	{
		perror("mount create fd");
		return -1;
	}

	INode root_inode = {0, "/", sizeof(INode) * NUM_INODES, INODE_INIT_SIZE, -1, _DIRECTORY, _USED};
	add_inode(fd, &root_inode);
	for (size_t i = 1; i < NUM_INODES; i++)
	{
		INode inode = {i, "test", 0, 0, -1, _UNKNOWN, _UNUSED};
		add_inode(fd, &inode);
	}

	myDIR root_dir = {0, {0}, "root", 0};
	lseek(fd, root_inode.data_start, SEEK_SET);
	write(fd, &root_dir, sizeof(myDIR));
	close(fd);
	return fd;
}
