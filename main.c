#include "myfs.h"

#include <string.h>

int main(int argc, char const *argv[])
{
	int fd = mymount("", "disk", "", 0, NULL);
	fd = open("disk", O_RDWR);
	INode *root = read_inode(fd, 0);
	return 0;
}
