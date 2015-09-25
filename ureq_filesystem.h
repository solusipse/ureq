#include "ureq_filesystem.c"

typedef struct UreqFilesystem {
	char filename[16];
	int  size;
	int	 address;
} UreqFilesystem;