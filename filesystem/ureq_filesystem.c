#include <stdio.h>

// TODO: move all defines to one file
#define UREQ_FS_START 0x12000


int ureq_fs_open() {

    char *pos = (char*) UREQ_FS_START + 0x40200000;

    char    name[16];
    int32_t size;
    int32_t address;


    os_memcpy(name, pos, sizeof(name));
    os_printf("BUF: %s\n", name);

    pos += sizeof(char) * 16;

    os_memcpy(&size, pos, sizeof(size));
    os_printf("BUF: %d\n", size);

    pos += sizeof(int);

    os_memcpy(&address, pos, sizeof(address));
    os_printf("BUF: %d\n", address);

    char    name2[16];

    pos += sizeof(int);
    os_memcpy(name2, pos, sizeof(name));
    os_printf("BUF: %s\n", name2);

    return 0;
}