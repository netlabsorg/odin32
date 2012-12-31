/* $Id: sdf.c,v 1.2 2001-04-17 04:24:17 bird Exp $
 *
 * SDF dumper. SDF == Structur Definition Files.
 * SDF files are found with the PMDF utility.
 * Note: SDFs for Warp 45 have to be UNPACK.EXEed first.
 *
 * stdout: log
 * stderr: errors or "header" file.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _SDFType
{
    char            szName[33];
    char            szStruct[34];
    unsigned long   cb;
    unsigned long   ulPointerLevel;
    unsigned long   aul[8];
} SDFTYPE, *PSDFTYPE;

typedef struct _SDFMember
{
    char            szName[33];
    char            szType[33];
    char            szType2[34];
    unsigned long   cb;
    unsigned long   offset;
    unsigned char   ch1;
    unsigned char   ch2;
    unsigned char   ch3;
    unsigned char   chPointerLevel;
    unsigned long   aul[5];
} SDFMEMBER, *PSDFMEMBER;


typedef struct _SDFStruct
{
    char            szName[33];
    char            szDescription[35];
    unsigned long   cb;
    unsigned long   cMembers;
    unsigned long   ul1;
    unsigned short  us2a;
    unsigned short  us2b;
    unsigned long   ul3;
    unsigned long   ul4;
    unsigned long   ul5;
    unsigned long   ul6;
    SDFMEMBER aMembers[1];
} SDFSTRUCT, *PSDFSTRUCT;


typedef struct _SDFHeader
{
    unsigned long   cStructs;
    unsigned long   cTypes;
    unsigned long   ul[12];
} SDFHEADER, *PSDFHEADER;


/*@IntFunc**********************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void dumpSDF(void *pvFile, unsigned cbFile);
int makeName(char *pszName, PSDFMEMBER pMember, PSDFHEADER pHdr, PSDFTYPE paTypes);
void makeTypedefsForStruct(PSDFHEADER pHdr, PSDFTYPE paTypes, PSDFSTRUCT pStruct, FILE *phOut);
void makeTypedefsForTypedef(PSDFHEADER pHdr, PSDFTYPE paTypes, PSDFTYPE pTypedef, FILE *phOut);
PSDFTYPE sdfGetTypeArray(PSDFHEADER pHdr);
PSDFTYPE sdfGetType(PSDFHEADER pHdr, PSDFTYPE paTypes, const char *pszType);


int main(int argc, char **argv)
{
    FILE *phFile;
    char  szBuffer[512];
    char  szOutput[512];

    if (argc != 2)
    {
        fprintf(stderr, "invalid parameter\n");
        return 1;
    }

    phFile = fopen(argv[1], "r"); /*Note: not "rb"! */
    if (phFile != NULL)
    {
        unsigned  cbFile;
        void     *pvFile;

        fseek(phFile, 0, SEEK_END);
        cbFile = ftell(phFile);
        fseek(phFile, 0, SEEK_SET);

        pvFile = malloc(cbFile+20);
        if (pvFile != NULL)
        {
            if (fread(pvFile, 1, cbFile, phFile))
            {
                dumpSDF(pvFile, cbFile);
            }
            else
                fprintf(stderr, "fread failed\n");
        }
        else
            fprintf(stderr, "malloc(%d) failed\n", cbFile);
        fclose(phFile);
    }
    else
        fprintf(stderr, "unable to open input file, %s\n", argv[1]);

    return 0;
}




void dumpSDF(void *pvFile, unsigned cbFile)
{
    PSDFHEADER pHdr = (PSDFHEADER)pvFile;
    PSDFSTRUCT pStruct = (PSDFSTRUCT)(unsigned)(pHdr + 1);
    PSDFTYPE   paTypes = sdfGetTypeArray(pHdr);
    PSDFTYPE   pType;
    FILE *phOut = stderr;
    int i;

    printf("Header:\n"
           "-------\n"
           "cStructs=%d\n"
           "cTypes=%d\n",
           pHdr->cStructs,
           pHdr->cTypes);
    for (i = 0; i < sizeof(pHdr->ul) / sizeof(pHdr->ul[0]); i++)
        printf("ul[%#x] = 0x%08x\n",
               i, pHdr->ul[i]);

    printf("\nStructs:\n"
             "--------\n");
    for (i = 0; i < pHdr->cStructs; i++,
         pStruct = (PSDFSTRUCT)((unsigned)(pStruct+1) + sizeof(SDFMEMBER)*(pStruct->cMembers - 1)) )
    {
        int j;
        if (i > 0)
            printf("\n");
        printf("%3d struct: %-20s  #members: %3d  cb: 0x%04x  offset: %08x  description: %s\n",
               i,  pStruct->szName, pStruct->cMembers, pStruct->cb, (unsigned)pStruct - (unsigned)pvFile, pStruct->szDescription);
        printf("(ul1=%08x us2a=%04x us2b=%04x ul3=%08x ul4=%08x ul5=%08x ul6=%08x)\n",
               pStruct->ul1, pStruct->us2a, pStruct->us2b, pStruct->ul3, pStruct->ul4, pStruct->ul5, pStruct->ul6);
        fprintf(phOut, "struct %s /* #memb %d size %d (0x%03x) */\n{\n",
                pStruct->szName, pStruct->cMembers, pStruct->cb, pStruct->cb);

        for (j = 0; j < pStruct->cMembers; j++)
        {
            int k, cchName;
            char szName[80];
            printf("\tname: %-18s type: %-14s size: %04x  offset: %02x  chPointerLevel: %d ch1-3:",
                   pStruct->aMembers[j].szName,
                   pStruct->aMembers[j].szType,
                   pStruct->aMembers[j].cb,
                   pStruct->aMembers[j].offset,
                   pStruct->aMembers[j].chPointerLevel
                   );
            if (pStruct->aMembers[j].szType2[0] != '\0')
                printf("\n\tszType2=%.15s\n", pStruct->aMembers[j].szType2);

            makeName(szName, &pStruct->aMembers[j], pHdr, paTypes);
            cchName = 20 - strlen(szName);
            fprintf(phOut, "    %-20s %s; %*s/* off: %3d(%02x)  size: %2d(%02x) */\n",
                    pStruct->aMembers[j].szType,
                    szName,
                    cchName > 0 ? cchName : 0, "",
                    pStruct->aMembers[j].offset,
                    pStruct->aMembers[j].offset,
                    pStruct->aMembers[j].cb,
                    pStruct->aMembers[j].cb
                    );

            printf(" %02X %02X %02X aul:", pStruct->aMembers[i].ch1, pStruct->aMembers[i].ch2, pStruct->aMembers[i].ch3);
            for (k = 0; k < sizeof(pStruct->aMembers[j].aul) / sizeof(pStruct->aMembers[j].aul[0]); k++)
                printf(" %08X", pStruct->aMembers[j].aul[k]);
            printf("\n");
        }
        fprintf(phOut, "};\n");
        makeTypedefsForStruct(pHdr, paTypes, pStruct, phOut);
        fprintf(phOut, "\n\n");
    }

    fprintf(stderr, "\n/*debug: CurPos=0x%x  cbFile=0x%x */\n",
            (unsigned)pStruct - (unsigned)pvFile, cbFile);



    printf("\nTypes:\n"
             "------\n");
    pType = (PSDFTYPE)(unsigned)pStruct;
    assert(pType == paTypes);
    for (i = 0; i < pHdr->cTypes; i++, pType++)
    {
        int j;
        printf("%3d  Name: %-20s  Struct: %-20s  cb: %3d (0x%03x) PointerLevel: %d ",
               i,
               pType->szName,
               pType->szStruct[0] != '\0' ? pType->szStruct : "<none>",
               pType->cb,
               pType->cb,
               pType->ulPointerLevel
               );
        for (j = 0; j < sizeof(pType->aul) / sizeof(pType->aul[0]); j++)
            printf(" 0x%08x", pType->aul[j]);
        printf("\n");
    }

    fprintf(stderr, "\n/*debug: CurPos=0x%x  cbFile=0x%x */\n",
            (unsigned)pType - (unsigned)pvFile, cbFile);
}


int makeName(char *pszName, PSDFMEMBER pMember, PSDFHEADER pHdr, PSDFTYPE paTypes)
{
    char *    psz   = pszName;
    PSDFTYPE  pType = sdfGetType(pHdr, paTypes, pMember->szType);

    strcpy(pszName, "   ");

    if (pType != NULL && pMember->cb > pType->cb && (pType->cb > 0 || pMember->chPointerLevel > 0))
    {
        int i = 0;
        int c = pMember->chPointerLevel - pType->ulPointerLevel;
        for (i = 0; i < c; i++)
            pszName[i] = '*';
        pszName[i > 3 ? i : 3] = '\0';

        psz = pszName + strlen(pszName);
        if (c <= 0 && pType->cb > 0)
            sprintf(psz, "%s[%d]", pMember->szName, pMember->cb / pType->cb);
        else
            sprintf(psz, "%s", pMember->szName);
    }
    else
        strcat(pszName, pMember->szName);

    return strlen(pszName);
}

void makeTypedefsForStruct(PSDFHEADER pHdr, PSDFTYPE paTypes, PSDFSTRUCT pStruct, FILE *phOut)
{
    int i;
    for (i = 0; i < pHdr->cTypes; i++)
        if (strcmp(paTypes[i].szStruct, pStruct->szName) == 0)
        {
            int j;
            fprintf(phOut, "typedef struct %s ", pStruct->szName);
            for (j = 0; j < paTypes[i].ulPointerLevel; j++)
                fprintf(phOut, "*");
            fprintf(phOut, " %s;\n", paTypes[i].szName);
            makeTypedefsForTypedef(pHdr, paTypes, &paTypes[i], phOut);
        }
}

void makeTypedefsForTypedef(PSDFHEADER pHdr, PSDFTYPE paTypes, PSDFTYPE pTypedef, FILE *phOut)
{
    int i;
    for (i = 0; i < pHdr->cTypes; i++)
        if (&paTypes[i] != pTypedef && strcmp(paTypes[i].szStruct, pTypedef->szName) == 0)
        {
            int j;
            fprintf(phOut, "typedef %s ", pTypedef->szName);
            for (j = 0; j < paTypes[i].ulPointerLevel; j++)
                fprintf(phOut, "*");
            fprintf(phOut, " %s;\n", paTypes[i].szName);
            makeTypedefsForTypedef(pHdr, paTypes, &paTypes[i], phOut); /* a bit risky... */
        }
}




PSDFTYPE sdfGetTypeArray(PSDFHEADER pHdr)
{
    PSDFSTRUCT pStruct = (PSDFSTRUCT)(unsigned)(pHdr + 1);
    PSDFTYPE   pType;
    int i;

    for (i = 0; i < pHdr->cStructs; i++)
        pStruct = (PSDFSTRUCT)((unsigned)(pStruct+1) + sizeof(SDFMEMBER)*(pStruct->cMembers - 1));

    printf("typearray offset: 0x%08x\n\n", (unsigned)pStruct - (unsigned)pHdr);
    return (PSDFTYPE)(unsigned)pStruct;
}


PSDFTYPE sdfGetType(PSDFHEADER pHdr, PSDFTYPE paTypes, const char *pszType)
{
    if (*pszType != '\0')
    {
        int i;
        for (i = 0; i < pHdr->cTypes; i++)
            if (strcmp(paTypes[i].szName, pszType) == 0)
                return &paTypes[i];
    }

    return NULL;
}
