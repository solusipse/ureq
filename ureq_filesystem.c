#include <stdio.h>

// TODO: move all defines to one file
#define UREQ_FS_START 0x12000

static char *ureq_fs_read(int a, int s) {
    char *pos = (char*) UREQ_FS_START + 0x40200000;
    pos += a;

    char buf[256];
    os_memcpy(buf, pos, sizeof(buf));

    int i;
    for (i=0;i<s;i++) {
        printf("%c", buf[i]);
    }
    printf("\n");

    // TODO: rebuild this
    // This will of course crash in most cases
    // it should be called every from http response mechanism
    // so it can split file into multiple sends
    // For text files there should be a method for getting
    // them as chars (and use in page functions)
    
    return "ok";
}

char *ureq_fs_open(char *rf) {

    char *pos = (char*) UREQ_FS_START + 0x40200000;

    int32_t amount;

    char    name[16];
    int32_t size;
    int32_t address;

    // Get number of files
    os_memcpy(&amount, pos, sizeof(amount));
    os_printf("Number of files: %d\n", amount);

    // Move to the filesystem header
    pos += sizeof(int32_t);

    int i;
    for (i = 0; i < amount; i++) {
        memset(name, 0, sizeof(name));
        os_memcpy(name, pos, sizeof(name));

        if (strcmp(name, rf) == 0) {
            // Requested file was found
            pos += sizeof(char) * 16;
            int size, address;
            os_memcpy(&size, pos, sizeof(int32_t));
            pos += sizeof(int32_t);
            os_memcpy(&address, pos, sizeof(int32_t));

            return ureq_fs_read(address, size);

        } else {
            // Move to next file
            pos += sizeof(char) * 16;   // filename
            pos += sizeof(int32_t);     // size
            pos += sizeof(int32_t);     // address
        }

    }

    return 0;
}