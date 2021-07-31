#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("** ramdisk3 maker [alpha]\n");
        printf("%s <in> <out>\n", argv[0]);
        return 0;
    }

    char *infile = argv[1];
    char *outfile = argv[2];
    size_t sz;
    FILE *fd = fopen(infile, "r");
    if (!fd)
    {
        printf("error opening %s\n", infile);
        return -1;
    }
    fseek(fd, 0, SEEK_END);
    sz = ftell(fd);
    assert(sz == 0x80000);
    fseek(fd, 0, SEEK_SET);
    unsigned char *rdsk = malloc(sz);
    if (!rdsk)
    {
        printf("error allocating file buffer\n");
        fclose(fd);
        return -1;
    }
    fread(rdsk, sz, 1, fd);
    fclose(fd);
    //ghidra pseudocode comes in clutch
    unsigned int new_block;
    int i = 0;
    while ((i < 0x58 &&
            (new_block = (i + 0x44) * 0x200, *(int *)(rdsk + ((i + 0x45) * 0x200 - 4)) != 0)))
    {
        i++;
    }
    unsigned int catalog_stuff = new_block - 0x8800;
    printf("new block: 0x%x\n", new_block);
    printf("writing \"3\"\n");
    rdsk[0x880F] = 3;
    printf("remap\n");
    rdsk[0x8813] = (new_block - 0x8800) / 0x200;
    printf("memcpy: 0x%x -> 0x%x\n", 0x8800, new_block);
    memcpy(rdsk + new_block, rdsk + 0x8E00, 0x200);
    printf("writing \"3\"\n");
    rdsk[new_block + 9] = 3;
    if (*(rdsk + new_block) != 0xDD)
    {
        for (i = 0x23; i < 0x1DD; i += 0xC)
        {
            catalog_stuff = i + new_block;
            rdsk[catalog_stuff] = 3;
            if (!rdsk[catalog_stuff + 0xC])
                break;
        }
        printf("setting trigger\n");
        rdsk[catalog_stuff - 2] = 1;
    }

    FILE *out = fopen(outfile, "w");
    if (!out)
    {
        printf("error opening %s\n", outfile);
        return -1;
    }
    fwrite(rdsk, sz, 1, out);
    fflush(out);
    fclose(out);
    return 0;
}