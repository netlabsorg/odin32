/* $Id: omfdumper.c,v 1.2 2000-08-19 01:03:45 bird Exp $ */
/*
 * Very simple OMF/LIB dumper.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */


/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <malloc.h>


#include "omf.h"


/*@IntFunc**********************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int   processFile(const char *pszFilename);
void *processRecord(void *pvRecord, void *pvBase, FILE *phDef);



int main(int argc, char **argv)
{
    int rc = 0;
    int argi = 1;

    while (argi < argc)
    {
        processFile(argv[argi]);

        /* next */
        argi++;
    }


    return rc;
}




int processFile(const char *pszFilename)
{
    FILE *phFile;
    FILE *phDef;
    char  szDefName[260];
    char *psz;
    int rc = 0;

    strcpy(szDefName, pszFilename);
    psz = strrchr(szDefName, '.');
    if (psz != NULL)
        *psz = '\0';
    strcat(szDefName, ".def");

    phFile = fopen(pszFilename, "rb");
    phDef = fopen(szDefName, "w");
    if (phFile != NULL && phDef != NULL)
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
                        pvNew = processRecord(pvNew, pvFile, phDef);
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
        fclose(phDef);
    }
    else
        rc = 1;

    return rc;
}



void *processRecord(void *pvRecord, void *pvBase, FILE *phDef)
{
    void *pvRet = pvRecord;
    int  cbRecord;
    int  i;

    switch (*(unsigned char*)pvRecord)
    {
        case LIBHDR:
            printf("LIBHDR: len=%d;", *((unsigned short*)((int)pvRecord+1)));
            printf(" offDict=0x%04x;", *((unsigned long*)((int)pvRecord+3)));
            printf(" cbDict=%d;", *((unsigned short*)((int)pvRecord+7)));
            printf(" flag=0x%0x(%s)", *((unsigned char*)((int)pvRecord+9)), *((unsigned char*)((int)pvRecord+9)) == 1 ? "insens" : "sens");
            printf("\n");
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;

        case THEADR:
            printf("THEADR: len=%d;", *((unsigned short*)((int)pvRecord+1)));
            printf(" name=%*s\n", *((unsigned char*)((int)pvRecord+3)), (char*)((int)pvRecord+4));
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            break;

        case MODEND:
        case MODEND2:
        {
            unsigned long ul = (unsigned long)pvRecord - (unsigned long)pvBase;

            printf("MODEND: \n");
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            /* it seems to be somthing funny here! - lets try aligning it to on a 16 bytes boundrary... */
            /* PS. I know this I have a wrong approch to the lib files, not using the directory... */
            ul += cbRecord;
            if ((ul % 16) > 0 && *((unsigned char*)((int)pvRecord+cbRecord)) == 0x00)
                cbRecord += 16 - (ul % 16);
            else
                ul = 0;
        } break;

        case COMENT:
            printf("COMENT: len=%d;", *((unsigned short*)((int)pvRecord+1)));
            printf(" type: 0x%02x; ", *((unsigned char*)((int)pvRecord+3)));
            printf(" class: 0x%02x; ", *((unsigned char*)((int)pvRecord+4)));
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;

            /* class switch */
            switch (*((unsigned char*)((int)pvRecord+4)))
            {
                case 0xA0:
                    printf(" subtype: 0x%02x;", *((unsigned char*)((int)pvRecord+5)));
                    /* sub type switch */
                    switch (*((unsigned char*)((int)pvRecord+5)))
                    {
                        case IMPDEF:
                        {
                        #if 1
                            int             fOrd = *((unsigned char*)((int)pvRecord + 6));
                            unsigned char   cchIntName  = *((unsigned char*)((int)pvRecord + 7));
                            char           *pachIntName = (char*)((int)pvRecord + 8);
                            unsigned char   cchDllName  = *((unsigned char*)((int)pvRecord + 8 + cchIntName));
                            char           *pachDllName = (char*)((int)pvRecord + 9 + cchIntName);
                            unsigned char   cchExtName  = *((unsigned char*)((int)pvRecord + 9 + cchIntName + cchDllName));
                            char           *pachExtName = (char*)((int)pvRecord +10 + cchIntName + cchDllName);
                            unsigned short  usOrdinal   = *((unsigned short*)((int)pvRecord + 9 + cchIntName + cchDllName));

                            printf("\n\tIMPDEF: fOrd=%d; intName=%.*s; dllName=%.*s; ",
                                   fOrd, cchIntName, pachIntName, cchDllName, pachDllName);
                            if (fOrd)
                                printf("%d\n", usOrdinal);
                            else
                                printf("%.*s\n", cchExtName, pachExtName);

                            /* write to the definition file */
                            if (fOrd)
                                fprintf(phDef, "   %.*s @ %d\n", cchIntName, pachIntName, usOrdinal);
                            else
                                if (cchExtName == 0)
                                    fprintf(phDef, "   %.*s\n", cchIntName, pachIntName);
                                else
                                    fprintf(phDef, "   %.*s = %.*s\n", cchExtName, pachExtName, cchIntName, pachIntName);

                        #else
                            printf(" fOrd=%d; ", *((unsigned char*)((int)pvRecord+6)));
                            i = *((unsigned char*)((int)pvRecord+7));
                            printf(" intName=%.*s; ", i, (char*)((int)pvRecord+8));
                            i += 8;
                            printf(" dllName=%.*s; ", *((unsigned char*)((int)pvRecord+i)), (char*)((int)pvRecord+i+1));
                            i += 1 + *((unsigned char*)((int)pvRecord+i));
                            if (*((unsigned char*)((int)pvRecord+6)))
                                printf(" ordinal=0x%d ", *((unsigned short*)((int)pvRecord+i)));
                            else
                                printf(" extName=%.*s; ", *((unsigned char*)((int)pvRecord+i)), (char*)((int)pvRecord+i+1));
                        #endif
                        } break;

                        default:
                            printf(" this subtype is not implemented yet");
                    }
                    break;

                default:
                    printf(" class is not implemented yet");

            }
            printf("\n");
            break;

        case LIBEND:
            printf("LIBEND: len=%d\n", *((unsigned short*)((int)pvRecord+1)));
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            return (void*)0xffffffff; /* FINE */
            break;

        default:
            cbRecord = *((unsigned short*)((int)pvRecord+1)) + 3;
            printf("oops: Record type not implemented yet. 0x%02x %d\n",
                   *(unsigned char*)pvRecord,
                   cbRecord);
            break;
    }

    return (void*)((int)pvRet + cbRecord);
}


