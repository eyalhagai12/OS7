#include "myfs.h"

int main(int argc, char const *argv[])
{
    create_disk("new_disk", 1024);
    int op = myopen("new_disk", 0);
    printf("op: %d\n", op);
    printf("disk fd: %d\n", disk.fd);
    printf("disk n_blocks: %ld\n", disk.n_blocks);
    myclose(op);
    return 0;
}
