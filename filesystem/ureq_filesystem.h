typedef struct UreqFilesystem {
	char filename[16];
	int  size;
	int	 address;
} UreqFilesystem;

#ifdef ESP8266
	#define UREQ_FS_START 0x12000
#endif