#include "myfs.h"
#include "mylibc.h"

#include <string.h>
#include <dirent.h>

void __attribute__((constructor)) calledFirst();
void __attribute__((destructor)) calledLast();

void calledFirst()
{
	global_id = -1;
	global_idx = -1;
	for (size_t i = 0; i < MAXOPENFILES; i++)
	{
		openfiles[i] = -1;
		openfiles_pos[i] = 0;
	}

	char buff[5];

	fd = open(ROOT_DIR, O_RDWR, 0777);
	if (fd == -1)
	{
		perror("fd open");
		exit(1);
	}

	read(fd, &super, sizeof(superblock));
	superblock s = super;
	if (read(fd, buff, 5) == 0)
	{
		mymount("", ROOT_DIR, NULL, 0, NULL);
	}
}

void calledLast()
{
	close(fd);
}

int main(int argc, char const *argv[])
{
	myDIR *dir1 = myopendir("root/eyal/yanir");
	myDIR *dir2 = myopendir("root/eyal/mor");
	myDIR *dir3 = myopendir("root/eyal/test");
	myDIR *dir4 = myopendir("root/eyal/charlie");
	myDIR *eyal = myopendir("root/eyal");
	myDIR *root = myopendir("root");
	mydirent *dir;
	while ((dir = myreaddir(eyal)) != NULL)
	{
		printf("%s\n", dir->d_name);
		free(dir);
	}

	myFILE *myfd = myfopen("root/eyal/blabla.txt", "r+");

	myfprintf(myfd, "%d %c %f", 55555555, 's', 7.98765);
	int n;
	char c;
	float fl;
	myfseek(myfd, 0, SEEK_SET);
	myfscanf(myfd, "%d %c %f", &n, &c, &fl);
	printf("%d %c %f\n", n, c, fl);
	myclosedir(dir1);
	myclosedir(dir2);
	myclosedir(dir3);
	myclosedir(dir4);
	myclosedir(eyal);
	myclosedir(root);
	myfclose(myfd);
	return 0;
}
