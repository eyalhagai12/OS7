#include "myfs.h"

#include <string.h>

int main(int argc, char const *argv[])
{
	char buff[5];
	fd = open(ROOT_DIR, O_RDWR | O_CREAT);
	if (read(fd, buff, 4) == 0)
	{
		mymount_root("", ROOT_DIR, NULL, 0, NULL);
	}

	// fd = mymount("", "disk", "", 0, NULL);
	// fd = open("disk", O_RDWR);
	// INode *root = read_inode(fd, 0);
	// myDIR *root_dir = read_directory(fd, 0);
	// printf("Hello world!\n");
	return 0;
}
