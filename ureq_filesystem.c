#include <stdio.h>
#include "ureq_filesystem.h"

// TODO: move all defines to one file
#define UREQ_FS_START 0x12000

void memcpyAligned(char *dst, char *src, int len) {
    // This function comes from dht-esp8266 project
    // It was written by Jeroen Domburg <jeroen@spritesmods.com>
    int x;
    int w, b;
    for (x=0; x<len; x++) {
        b=((int)src&3);
        w=*((int *)(src-b));
        if (b==0) *dst=(w>>0);
        if (b==1) *dst=(w>>8);
        if (b==2) *dst=(w>>16);
        if (b==3) *dst=(w>>24);
        dst++; src++;
    }
}



static char *ureq_fs_read(int a, int s, char *buf) {
    char *pos = (char*) UREQ_FS_START + 0x40200000;
    pos += a;

    memset(buf, 0, 1024);
    memcpyAligned(buf, pos, s);

    return buf;
}

UreqFile ureq_fs_open(char *rf) {

    char *pos = (char*) UREQ_FS_START + 0x40200000;

    int32_t amount;

    char    name[16];
    int32_t size;
    int32_t address;

    UreqFile f;
    f.size = 0;
    f.address = 0;

    // Get number of files
    os_memcpy(&amount, pos, sizeof(amount));
    //os_printf("Number of files: %d\n", amount);

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
            f.size = size;
            f.address = address;
            return f;       
        } else {
            // Move to next file
            pos += sizeof(char) * 16;   // filename
            pos += sizeof(int32_t);     // size
            pos += sizeof(int32_t);     // address
        }

    }
    return f;
}

int ureq_fs_first_run(HttpRequest *r) {
        UreqFile f = ureq_fs_open(r->url + 1);
        if (f.address == 0) {
            // File was not found
            return ureq_set_404_response(r);
        }

        r->bigFile  =  1;
        r->complete = -2;

        if (r->response.code == 0)
            r->response.code = 200;

        r->file = f;

        r->response.data = ureq_generate_response_header(r);
        r->len = strlen(r->response.data);

        return r->complete;
}
