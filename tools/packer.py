'''

typedef struct UreqFilesystem {
    char filename[16];
    int  size;
    int  address;
} UreqFilesystem;

#ifdef ESP8266
    #define UREQ_FS_START 0x12000
#endif

'''

source_dir = "test"
output = "output.image"

import os, numpy, sys
from numpy import int32

files = []

for f in os.listdir(source_dir):
    if f.startswith("."):
        continue

    fl = []

    fl.append( "{:<16}".format(f + '\0') )

    with open(source_dir + "/" + f, 'r') as fh:
        fl.append( int32( os.fstat(fh.fileno()).st_size ) )
        fl.append( 0 )
        fl.append( fh.read() )

    files.append(fl)

# 4 is size of int32_t containing number of files
header_size = 4

# Estimate header size
for h in files:
    #              1 char     2 ints
    header_size += (1 * 16) + (4 * 2)

address = header_size

for i, h in enumerate(files):

    if i > 0:
        address += files[i-1][1]

    h[2] = int32( address )

print("Adding files:")

with open(output, "w") as fh:
    # number of files
    int32( len(files) ).tofile(fh)
    for p in files:
        # filename
        fh.write( p[0] )
        # size
        p[1].tofile(fh)
        # address
        p[2].tofile(fh)
        print("At address %s, with size %s, added file %s" % (p[2], p[1], p[0]) )

    print("Header successfully saved to file!")

with open(output, "a") as fh:
    for p in files:
        # contents
        fh.write( p[3] )

    print("Contents successfully saved to file")

print("Image is ready! Saved to %s." % output)

if (len(sys.argv)) > 1:
    print("Uploading to device!")
    os.system("esptool.py --port " + sys.argv[1] + " write_flash 0x12000 " + output)