/* $Id: pe2lx.cpp,v 1.1 1999-10-06 18:50:55 bird Exp $
 *
 * PE2LX - Ring-3 standard PE2LX using LXHeader.
 *
 * Copyright (c) 1998-1999 knut  St.  osmundsen
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"
#include "misc.h"
#include "options.h"
#include <versionos2.h>

#include <stdio.h>

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static char INFO_BANNER[] =
"Usage: PE2LX winfile \n\
       OR\n\
       PE2LX winfile os2file\n";

struct options options;


int main(int argc, char **argv)
{
    if (heapInit(HEAP_SIZE) == 0)
        cout << "error! no heap!" << endl;
    LXFile *lxfile;
    char szbuffer[196];
    int offset;
    HFILE hFile = 0;
    ULONG ulAction = 0;
    ULONG ulRead;
    int rc;
    char szFilename[CCHMAXPATH];

    /* dump to file */
    char szBuffer[1024];
    FILE *outfile;
    ULONG ulOffset;
    ULONG ulBytesRead;
    char *pBuffer;

    SET_OPTIONS_PE2LX(options);

    cout << "PE2LX Special Edition v0.0." << PE2LX_VERSION <<"alpha (for debugging ring 0 code in ring 3 debug)"<<endl<<endl;
    if (argc != 2 && argc != 3)
    {
        cout << INFO_BANNER;
        return -1;
    }



    rc = DosOpen(argv[1], &hFile, &ulAction, 0,
                     FILE_NORMAL,
                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                     OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                     NULL);
    if (rc == NO_ERROR)
    {
        rc = DosRead(hFile,szbuffer,0x40,&ulRead);
        if (rc == NO_ERROR)
        {
            offset = *(PULONG)(szbuffer+0x3c);
            rc = DosSetFilePtr(hFile, offset, FILE_BEGIN, &ulRead);
            rc = DosRead(hFile, szbuffer, 196, &ulRead);
            if (rc == NO_ERROR)
            {
                rc = -1;
                lxfile = new LXFile;
                lxfile->SetModuleName(argv[1]);
                if (lxfile->init(hFile, (PIMAGE_SIG_FILE_HEADER)szbuffer, offset))
                {
                    //ULONG  read(ULONG ulOffset, PVOID pBuffer, ULONG ulBytesToRead);
                    rc = NO_ERROR;
                    cout << "init success fully completed" << endl;

                    /* dump to file */
                    if (argc == 2)
                    {
                        strcpy(szFilename,argv[1]);
                        rc = strlen(szFilename);
                        szFilename[rc+4] = szFilename[rc-0];
                        szFilename[rc+3] = szFilename[rc-1];
                        szFilename[rc+2] = szFilename[rc-2];
                        szFilename[rc+1] = szFilename[rc-3];
                        szFilename[rc+0] = szFilename[rc-4];
                        szFilename[rc-1] = '_';
                        szFilename[rc-2] = 'o';
                        szFilename[rc-3] = 's';
                        szFilename[rc-4] = '2';
                        strcat(szFilename,"_os2.exe");
                    }
                    else
                        strcpy(szFilename,argv[2]);
                    outfile = fopen(szFilename, "wb");
                    if (outfile == NULL)
                    {
                        printf("error:could not open out file %s\n.",szFilename);
                        return -1;
                    }
#define blocksize 907
#define MAGIC   "Win32-OS/2"
                    strcpy(szBuffer,MAGIC);
                    pBuffer = szBuffer+strlen(MAGIC)+1;
                    strcpy(pBuffer+blocksize,MAGIC);

                    rc = NO_ERROR;
                    ulOffset = 0;
                    rc = lxfile->read(ulOffset,pBuffer,blocksize, &ulBytesRead,0);
                    while(rc == NO_ERROR)
                    {
                        if (strcmp(pBuffer+blocksize, MAGIC) != 0)
                            return printf("!wrote to much! +\n");
                        if (strcmp(szBuffer, MAGIC) != 0)
                            return printf("!wrote to much! -\n");
                        fwrite(pBuffer,1,ulBytesRead,outfile);
                        flushall();
                        ulOffset += ulBytesRead;
                        rc = lxfile->read(ulOffset,pBuffer,blocksize, &ulBytesRead,0);
                    }
                    if (rc == -2)
                        fwrite(pBuffer,1,ulBytesRead,outfile);
                    fclose(outfile);
                }
                else
                    cout << "warning: init failed!" << endl;
                delete lxfile;
            }
            else
                cout << "error: probably not a valid PE-file" << endl;

        }

    }
    else
        cout << "error: could not open file" << argv[1] << endl;

    return rc;
}
