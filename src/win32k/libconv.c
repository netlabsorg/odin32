/* $Id: libconv.c,v 1.2.4.1 2000-07-16 22:43:19 bird Exp $
 *
 * Very simple OMF/LIB dumper.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <malloc.h>


#include "include\omf.h"

int fCodeToCode16 = 0;

/*@IntFunc**********************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int processFile(const char *pszFilename, const char *pszFilenameOut);
void *processRecord(void *pvRecord, void *pvBase, FILE *phNew);



int main(int argc, char **argv)
{
    int rc = 0;
    int argi = 1;

    if (argc != 3)
        printf("syntax error - requires two filename, in.lib and out.lib\n");

    rc = processFile(argv[argi], argv[argi+1]);

    return rc;
}




int processFile(const char *pszFilename, const char *pszFilenameOut)
{
    FILE *phFile;
    FILE *phNew;
    char *psz;
    int rc = 0;

    psz = strrchr(pszFilename, '\\');
    if (psz)
        fCodeToCode16 = stricmp(psz, "\\dhcalls.lib") == 0;

    phFile = fopen(pszFilename, "rb");
    phNew = fopen(pszFilenameOut, "wb");
    if (phFile != NULL && phNew != NULL)
    {
        int cbFile;

        /* get size of file */
        if (!fseek(phFile, 0, SEEK_END)
            && (cbFile = ftell(phFile) ) != -1
            && !fseek(phFile, 0, SEEK_SET)
            )
        {
            void *pvFile;

            /* allocater memory */
            pvFile = malloc(cbFile);
            if (pvFile != NULL)
            {
                /* read the whole file */
                if (fread(pvFile, cbFile, 1, phFile) == 1)
                {
                    void *pvNew = pvFile;

                    /*  main loop */
                    while (pvNew < (void*)(cbFile + (int)pvFile))
                        pvNew = processRecord(pvNew, pvFile, phNew);
                }
                else
                    rc = 4;
            }
            else
                rc = 3;
        }
        else
            rc = 2;
        fclose(phFile);
        fclose(phNew);
    }
    else
        rc = 1;

    return rc;
}



void *processRecord(void *pvRecord, void *pvBase, FILE *phNew)
{
    static char *apszLNames[256];
    static int   cpszLNames = 0;
    static int   iCode16 = 0;
    void *pvRet = pvRecord;
    int  cbRecord;
    int  i;
    int  fChanged = 0;

    switch (*(unsigned char*)pvRecord)
    {
       case LIBHDR:
            fChanged = 1;
            fwrite(pvRecord, 1, 3, phNew);
            if (*(unsigned short*)((int)pvRecord+1) < 6)
                fwrite((char*)pvRecord+3, 1, *(unsigned short*)((int)pvRecord+1), phNew);
            else
            {
                i = 0;
                fwrite(&i, 1, 2, phNew);
                fwrite(&i, 1, 2, phNew);
                fwrite(&i, 1, 2, phNew);
                fwrite((char*)pvRecord+3+6, 1, *(unsigned short*)((int)pvRecord+1) - 6, phNew);
            }
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;

        case 0x98: /* 16-bit segdef */
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            i = *(char*)((int)pvRecord + 6);
            if (cbRecord == 10 && i <= cpszLNames && strcmp(apszLNames[i-1], "CODE") == 0)
                {
                *(char*)((int)pvRecord + 6) = iCode16+1;
                }
            break;

        case MODEND:
        case MODEND2:
        {
            unsigned long ul = (unsigned long)pvRecord - (unsigned long)pvBase;

            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            /* it seems to be somthing funny here! - lets try aligning it to on a 16 bytes boundrary... */
            /* PS. I know this I have a wrong approch to the lib files, not using the directory... */
            fChanged = 1;
            fwrite(pvRecord, 1, cbRecord, phNew);
            ul += cbRecord;
            if (*((unsigned char*)((int)pvRecord+cbRecord)) == 0x00)
            {
                if ((ul % 16) > 0)
                    cbRecord += 16 - (ul % 16);
            }
            /*if (*((unsigned char*)pvRecord + cbRecord) != LIBEND)*/
            {
                ul = ftell(phNew);
                while (ul++ % 16 != 0)
                    fputc(0, phNew);
            }
            while (cpszLNames-- > 0)
                free(apszLNames[cpszLNames]);
        } break;

        case LNAMES:
            {
            char *apszConv[] =
            {
                "CODE",     "CODE",
                "_CODE",    "CODE16",
                "TEXT",     "CODE16",
                "_TEXT",    "CODE16",
                "_IOSEG",   "CODE16",
                "IOSEG_CODE", "CODE",
                "DATA",     "FAR_DATA",
                "_DATA",    "DATA16",
                "DGROUP",   "DATA16_GROUP",
                "_BSS",     "DATA16BSS",
                "BSS",      "DATA16BSS",
                "_CONST",   "DATA16CONST",
                "CONST",    "DATA16CONST",
                NULL, NULL
            };

            int     cb;
            int     cbNew; /* record size */
            char   *psz;
            iCode16 = -1;

            cpszLNames = 0;
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            cb = 3;
            cbNew = 1;
            psz = (char*)pvRecord + 3;
            while (cb < cbRecord-1)
            {
                int j = 0;
                char szName[256];

                strncpy(szName, psz+1, *psz);
                szName[*psz] = '\0';

                while (apszConv[j] != NULL && stricmp(szName, apszConv[j]) != 0)
                    j += 2;

                if (apszConv[j] == NULL)
                {
                    cbNew += *psz + 1;
                    if (*psz != 0)
                        printf("LNAMES: %.*s was not converted\n", *psz, psz+1);
                    apszLNames[cpszLNames++] = strdup(&szName[0]);
                }
                else
                {
                    cbNew += strlen(apszConv[j+1]) + 1;
                    if (strcmp(apszConv[j+1], "CODE16") == 0)
                        iCode16 = cpszLNames;
                    apszLNames[cpszLNames++] = strdup(apszConv[j+1]);
                }
                cb += *psz + 1;
                psz += 1 + *psz;
            }

            if (iCode16 == -1)
            {
                iCode16 = cpszLNames;
                apszLNames[cpszLNames++] = strdup("CODE16");
                cbNew += 1 + 6;
            }

            fChanged = 1;
            fputc(LNAMES, phNew);
            fwrite(&cbNew, 1, 2, phNew);

            for (i = 0; i < cpszLNames; i++)
            {
                fputc(strlen(apszLNames[i]), phNew);
                fwrite(apszLNames[i], 1, strlen(apszLNames[i]), phNew);
            }

            fputc(0, phNew);
            break;
            }

        case LIBEND:
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            fwrite(pvRecord, 1, cbRecord, phNew);
            return (void*)0xffffffff; /* FINE */
            break;

        default:
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;
    }

    if (!fChanged)
        fwrite(pvRet, 1, cbRecord, phNew);


    return (void*)((int)pvRet + cbRecord);
}


