#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>




int main(int argc, char **argv)
{
    FILE *  phFile;
    void *  pvFile;
    int     cbFile;
    int     off;

    if (argc != 4)
        return 1;

    off = atoi(argv[3]);
    if (off == 0)
        return 2;

    phFile = fopen(argv[1], "rb");
    if (!phFile)
        return 3;
    fseek(phFile, 0, SEEK_END);
    cbFile = ftell(phFile);
    pvFile = calloc(cbFile+2, 1);
    if (cbFile <= 0 || !pvFile || cbFile < off)
        return 4;
    fseek(phFile, 0, SEEK_SET);
    if (!fread(pvFile, cbFile, 1, phFile))
        return 5;
    fclose(phFile);

    unlink(argv[2]);

    phFile = fopen(argv[2], "wb");
    if (!phFile)
        return 6;
    if (!fwrite((char*)pvFile + off, cbFile - off, 1, phFile))
        return 7;
    fclose(phFile);
    return 0;
}
