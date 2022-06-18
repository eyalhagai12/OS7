#include "myfs.h"
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	read(fd, inode, sizeof(INode));
	// print_inodes(inode);
	return inode;
}

void change_name(int fd, off_t id, const char *new_name)
{
	INode *inode = read_inode(fd, id);
	strcpy(inode->name, new_name);
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	write(fd, inode, sizeof(INode));
	free(inode);
}

void change_data_start(int fd, off_t id, size_t new_start)
{
	INode *inode = read_inode(fd, id);
	inode->data_start = new_start;
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	write(fd, inode, sizeof(INode));
	free(inode);
}

void change_size(int fd, off_t id, size_t new_size)
{
	INode *inode = read_inode(fd, id);
	inode->size = new_size;
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	write(fd, inode, sizeof(INode));
	free(inode);
}

void next_id(int fd, off_t id, int new_id)
{
	INode *inode = read_inode(fd, id);
	inode->next_id = new_id;
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	write(fd, inode, sizeof(INode));
	free(inode);
}

void change_type(int fd, off_t id, enum INodeType new_type)
{
	INode *inode = read_inode(fd, id);
	inode->type = new_type;
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	write(fd, inode, sizeof(INode));
	free(inode);
}

void change_status(int fd, off_t id, enum STATUS new_status)
{
	INode *inode = read_inode(fd, id);
	inode->status = new_status;
	lseek(fd, sizeof(superblock) + (id * sizeof(INode)), SEEK_SET);
	write(fd, inode, sizeof(INode));
	free(inode);
}

INode create_Inode(int id, char *path, size_t data_start, size_t size, int next_id, enum INodeType type, enum STATUS status)
{
	INode root_inode;
	root_inode.id = id;
	strcpy(root_inode.name, path);
	root_inode.data_start = data_start;
	root_inode.size = size;
	root_inode.next_id = next_id;
	root_inode.type = type;
	root_inode.status = status;
	root_inode.length = 0;
	return root_inode;
}

// -----------------------------------------------
// directory functions
// -----------------------------------------------

myDIR create_dir(int id, const char *dir_name, size_t num_inodes)
{
	myDIR dir;
	dir.id = id;
	strcpy(dir.name, dir_name);
	dir.num_inodes = num_inodes;

	for (size_t i = 0; i < num_inodes; i++)
	{
		dir.inodes[i] = 0;
	}

	return dir;
}

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
	free(inode);
	return dir;
}

void insert_inode(int fd, myDIR *dir)
{
	INode *inode = read_inode(fd, dir->id);
	lseek(fd, inode->data_start, SEEK_SET);
	write(fd, dir, sizeof(myDIR));
	free(inode);
}

myDIR *myopendir(const char *name)
{
	char buff[1024];
	bzero(buff, 1024);
	if (name[0] == '/')
	{
		strcat(buff, "root");
	}
	strcat(buff, name);

	for (size_t i = 0; i < NUM_INODES; i++)
	{
		INode *inode = read_inode(fd, i);
		if (inode->type == _DIRECTORY)
		{
			if (strcmp(inode->name, buff) == 0)
			{
				free(inode);
				return read_directory(fd, i);
			}
		}
		free(inode);
	}

	myDIR *new_dir = malloc(sizeof(myDIR));
	for (size_t i = 0; i < NUM_INODES; i++)
	{
		INode *inode = read_inode(fd, i);
		if (inode->status == _UNUSED)
		{
			change_status(fd, i, _USED);
			change_type(fd, i, _DIRECTORY);
			change_name(fd, i, buff);

			*new_dir = create_dir(i, buff, 100);
			lseek(fd, inode->data_start, SEEK_SET);
			write(fd, new_dir, sizeof(myDIR));

			char parent_name[1024];
			bzero(parent_name, 1024);
			for (size_t k = strlen(buff) - 1; k >= 0; --k)
			{
				int counter = 0;
				if (buff[k] == '/')
				{
					strncpy(parent_name, buff, k);
					break;
				}
			}

			if (parent_name[0] == 'r')
			{
				myDIR *parent_dir = myopendir(parent_name);

				for (size_t j = 0; j < parent_dir->num_inodes; j++)
				{
					if (parent_dir->inodes[j] <= 0)
					{
						parent_dir->inodes[j] = i;
						insert_inode(fd, parent_dir);
						break;
					}
				}

				free(parent_dir);
			}
			free(inode);
			break;
		}
		free(inode);
	}

	return new_dir;
}

// -----------------------------------------------
// main functions
// -----------------------------------------------

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
	char path[1024] = {0};
	strcat(path, source);
	strcat(path, target);
	printf("Path: %s\n", path);

	INode root_inode = create_Inode(0, path, sizeof(superblock) + (sizeof(INode) * NUM_INODES), INODE_INIT_SIZE, -1, _DIRECTORY, _USED);
	add_inode(fd, &root_inode);
	for (size_t i = 1; i < NUM_INODES; i++)
	{
		INode inode = create_Inode(i, "test", sizeof(superblock) + (sizeof(INode) * NUM_INODES + (INODE_INIT_SIZE * i)), INODE_INIT_SIZE, -1, _UNKNOWN, _UNUSED);
		add_inode(fd, &inode);
	}

	myDIR root_dir = create_dir(0, "root", 100);

	lseek(fd, root_inode.data_start, SEEK_SET);
	write(fd, &root_dir, sizeof(myDIR));
	return fd;
}

struct mydirent *myreaddir(myDIR *dirp)
{
	if (global_id == dirp->id)
	{
		if (dirp->inodes[global_idx] != 0)
		{
			INode *dir = read_inode(fd, dirp->inodes[global_idx]);
			mydirent *ent = malloc(sizeof(mydirent));
			strcpy(ent->d_name, dir->name);
			free(dir);
			global_idx++;
			return ent;
		}
		return NULL;
	}
	else
	{
		global_id = dirp->id;
		global_idx = 0;
		if (dirp->inodes[global_idx] != 0)
		{
			myDIR *dir = read_directory(fd, dirp->inodes[global_idx]);
			mydirent *ent = malloc(sizeof(mydirent));
			strcpy(ent->d_name, dir->name);
			free(dir);
			global_idx++;
			return ent;
		}
		return NULL;
	}
}

int myclosedir(myDIR *dirp)
{
	global_idx = -1;
	global_id = -1;
	free(dirp);
	return 0;
}

int myopen(const char *pathname, int flags)
{
	char buff[1024];
	bzero(buff, 1024);
	if (pathname[0] == '/')
	{
		strcat(buff, "root");
	}
	strcat(buff, pathname);

	INode *inode;
	for (size_t i = 0; i < NUM_INODES; i++)
	{
		inode = read_inode(fd, i);
		if (inode->type == _FILE)
		{
			if (strcmp(inode->name, buff) == 0)
			{
				for (size_t i = 0; i < MAXOPENFILES; i++)
				{
					if (openfiles[i] == -1)
					{
						openfiles[i] = inode->id;
						free(inode);
						return i;
					}
				}
			}
		}
		free(inode);
	}

	if (flags & O_CREAT)
	{
		if (inode == NULL)
		{
			return -1;
		}

		myDIR *new_file = myopendir(pathname);
		INode *inode = read_inode(fd, new_file->id);
		change_name(fd, inode->id, buff);
		change_status(fd, inode->id, _USED);
		change_type(fd, inode->id, _FILE);
		free(new_file);
		char empty[INODE_INIT_SIZE];
		for (size_t i = 0; i < INODE_INIT_SIZE; i++)
		{
			empty[i] = '\0';
		}

		lseek(fd, inode->data_start, SEEK_SET);
		write(fd, empty, sizeof(empty));

		for (size_t i = 0; i < MAXOPENFILES; i++)
		{
			if (openfiles[i] == -1)
			{
				openfiles[i] = inode->id;
				free(inode);
				return i;
			}
		}
		free(inode);
	}
	return -1;
}

int myclose(int myfd)
{
	if (openfiles[myfd] == -1)
	{
		return -1;
	}
	openfiles[myfd] = -1;

	return 0;
}

ssize_t myread(int myfd, void *buf, size_t count)
{
	if (openfiles[myfd] == -1)
	{
		return -1;
	}

	INode *inode = read_inode(fd, openfiles[myfd]);
	lseek(fd, inode->data_start + openfiles_pos[myfd], SEEK_SET);
	ssize_t size = read(fd, buf, count);
	openfiles_pos[myfd] += size;
	free(inode);
	return size;
}

ssize_t mywrite(int myfd, const void *buf, size_t count)
{
	if (openfiles[myfd] == -1)
	{
		return -1;
	}

	INode *inode = read_inode(fd, openfiles[myfd]);
	lseek(fd, inode->data_start + openfiles_pos[myfd], SEEK_SET);
	ssize_t size = write(fd, buf, count);
	openfiles_pos[myfd] += size;
	if (openfiles_pos[myfd] > inode->length)
	{
		inode->length = openfiles_pos[myfd];
	}
	free(inode);
	return size;
}

off_t mylseek(int myfd, off_t offset, int whence)
{
	switch (whence)
	{
	case SEEK_CUR:
		openfiles_pos[myfd] += offset;
		break;

	case SEEK_SET:
		openfiles_pos[myfd] = offset;
		break;

	case SEEK_END:
	{
		INode *inode = read_inode(fd, openfiles[myfd]);
		openfiles_pos[myfd] = inode->length;
		free(inode);
		break;
	}

	default:
		printf("mylseek: no such option!\n");
		break;
	}
}