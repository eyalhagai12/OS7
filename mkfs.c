#include <stdio.h>
#include <string.h>

#include "myfs.h"

int main(int argc, char const *argv[])
{
	if (argc > 2)
	{
		if (strcmp(argv[1], "-s") == 0)
		{
			int size = atoi(argv[2]);
			super.num_inodes = size / (10 * sizeof(INode));
			super.inode_init_size = (size - super.num_inodes * sizeof(INode)) / super.num_inodes;
		}
	}
	else
	{
		printf("Too few arguments, using default\n");
		super.num_inodes = 1000;
		super.inode_init_size = 655376;
	}

	fd = open(ROOT_DIR, O_CREAT | O_RDWR, 0777);
	write(fd, &super, sizeof(superblock));
	close(fd);
	return 0;
}
