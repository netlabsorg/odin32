/* $Id: libconv.c,v 1.1 2001-09-02 04:35:58 bird Exp $
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
int fRemoveExtra = 0;

/*@IntFunc**********************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int processFile(const char *pszFilename, const char *pszFilenameOut);
void *processRecord(void *pvRecord, void *pvBase, FILE *phNew, unsigned short *ausTrans);



int main(int argc, char **argv)
{
    int rc = 0;
    int argi = 1;

    if (argc == 4)
    {
        argi = 2;
        fRemoveExtra = 1;
    }
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
                    unsigned short * ausTrans = calloc(sizeof(unsigned short), cbFile / 16 ); /* Assumes page size of 16 FIXME! */
                    void *pvNew = pvFile;


                    /*  main loop */
                    while (pvNew < (void*)(cbFile + (int)pvFile))
                        pvNew = processRecord(pvNew, pvFile, phNew, ausTrans);
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



void *processRecord(void *pvRecord, void *pvBase, FILE *phNew, unsigned short *ausTrans)
{
    static char *apszLNames[256];
    static int   cpszLNames = 0;
    static int   iCode16 = 0;
    static int   cbPage = 16;       /* fixme todo TODO FIXME ASSUMES: pagesize = 16 */
    void *pvRet = pvRecord;
    int  cbRecord;
    int  i;
    int  fChanged = 0;

    switch (*(unsigned char*)pvRecord)
    {
       case LIBHDR:
            if (*(unsigned short*)((int)pvRecord+1) < 6)
                fprintf(stderr, "error: not supported LIBHDR\n");
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;

        case THEADR:
            /* Add entry to the translation table - used to translate block numbers in the dictionary */
            ausTrans[((char*)pvRecord - (char*)pvBase) / cbPage] = (unsigned short)(ftell(phNew) / cbPage);
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;

        case SEGDEF: /* 16-bit segdef */
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            *((char*)pvRecord + 9) =  0;
            i = *(char*)((int)pvRecord + 6);
            if (cbRecord == 10 && i <= cpszLNames && strcmp(apszLNames[i-1], "CODE") == 0)
            {
                *(char*)((int)pvRecord + 6) = iCode16+1;
            }
            i = *(char*)((int)pvRecord + 6);
            if (cbRecord == 10 &&  (*((char *)pvRecord + 3) & 0xE0) == 0x20 && i <= cpszLNames && strcmp(apszLNames[i-1], "CODE16_IOSEG") == 0)
            {   /* change alignment of this segment... */
                /* fprintf(stderr, "debug %s 0x%02x\n", apszLNames[*(char*)((int)pvRecord + 6) - 1], (*((char *)pvRecord + 3) & 0xFF)); */
                *((char*)pvRecord + 3) =  0x40 | (*((char*)pvRecord + 3) & 0x1F);
            }
            break;

        case MODEND:
        case MODEND2:
        {
            unsigned long ul = (unsigned long)pvRecord - (unsigned long)pvBase;

            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            /* it seems to be somthing funny here! - lets try aligning it to on a 16 bytes boundrary... */
            /* PS. I know this I have a wrong approch to the lib files, not using the directory... */
            /* Aug 13 2000 4:24am: The alignment of modules are found in the library header.
             *    It's the size of the library header.
             *    Not implemented yet. TODO FIXME BUGBUG ASSUMES
             */
            fChanged = 1;
            fwrite(pvRecord, 1, cbRecord, phNew);
            ul += cbRecord;
            if (*((unsigned char*)((int)pvRecord+cbRecord)) == 0x00)
            {
                if ((ul % cbPage) > 0)
                    cbRecord += cbPage - (ul % cbPage);
            }
            /*if (*((unsigned char*)pvRecord + cbRecord) != LIBEND)*/
            {
                ul = ftell(phNew);
                while (ul++ % cbPage != 0)
                    fputc(0, phNew);
            }
            while (cpszLNames-- > 0)
                free(apszLNames[cpszLNames]);
        } break;

        case PUBDEF:
        {
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            *((char*)pvRecord + cbRecord - 1) = 0;
            break;
        }

        case LNAMES:
        {
            char *apszConv[] =
            {
                /* classes */
                "CODE",         "CODE",
                "IOSEG_CODE",   "CODE",
                "DATA",         "FAR_DATA",
                "BSS",          "DATA16BSS",
                "CONST",        "DATA16CONST",
                /* segment names */
                "_TEXT",        "CODE16",
                "_IOSEG",       "CODE16_IOSEG",
                "_CODE",        "CODE16",           /* dhcalls */
                "_DATA",        "DATA16",
                "_BSS",         "DATA16BSS",
                "_CONST",       "DATA16CONST",
                /* groups */
                "DGROUP",       "DATA16_GROUP",
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
        {
            /* align dictionary at 512 byte (3 is min size of this record) */
            int cBlocks;
            unsigned short int cbSize  = (((ftell(phNew) + 3 + 0x1ff) & ~0x1ff) - ftell(phNew)) - 3;
            unsigned long int  offDict = ftell(phNew) + cbSize + 3;
            cbRecord = cbSize + 3;
            fwrite(pvRecord, 1, 1, phNew);
            fwrite(&cbSize, 1, sizeof(cbSize), phNew);
            while (cbSize-- > 0)
                fwrite("\0", 1, 1, phNew);

            /* update header record. */
            fseek(phNew, 3, SEEK_SET);
            fwrite(&offDict, 1, sizeof(offDict), phNew);
            fseek(phNew, offDict, SEEK_SET);

            /* Translate the dictionary */
            pvRecord = (void*)((char*)pvRecord + *(unsigned short*)((char*)pvRecord + 1) + 3);
            cBlocks = *(unsigned short *)((char*)pvBase + 7);
            for (i = 0; i < cBlocks; i++)
            {
                int j;
                char *pchBlock = (char*)pvRecord + i * 512;
                for (j = 0; j < 37; j++)
                {
                    if (pchBlock[j] > 19 && pchBlock[j] < 0xff)
                    {
                        unsigned short *pusBlock = (unsigned short*)&pchBlock[pchBlock[j] * 2];
                        pusBlock = (unsigned short *) ((char*)pusBlock + *(char*)pusBlock + 1);
                        if (ausTrans[*pusBlock] == 0)
                            fprintf(stderr, "error: dictionary entry don't point to a THEADR page!\n");
                        else
                            *pusBlock = ausTrans[*pusBlock];
                    }
                }

            }

            /* Write the dictionary */
            fwrite(pvRecord, 512, cBlocks, phNew);
            return (void*)0xffffffff; /* FINE */
        }

        case COMENT:
        {
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            switch (*((unsigned char*)((int)pvRecord+4)))
            {
                /* remove "Link Pass Separator" */
                case 0xA2:
                /* remove "LIBMOD" - The VAC 3.6.5 Linker don't like these! */
                case 0xA3:
                    fChanged = fRemoveExtra;
                    break;
            }
            break;
        }

        /*
         * Remove checksum
         */
        /*
        case GRPDEF:
        case FIXUPP:
        case FIXUPP2:
        case LEDATA:
        {
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            *((char*)pvRecord + cbRecord - 1) = 0;
            break;
        }
        */

        default:
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;
    }

    if (!fChanged)
        fwrite(pvRet, 1, cbRecord, phNew);


    return (void*)((int)pvRet + cbRecord);
}


