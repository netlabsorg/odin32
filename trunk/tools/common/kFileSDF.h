/* $Id: kFileSDF.h,v 1.1 2001-04-17 04:16:03 bird Exp $
 *
 * kFileSDF- Structure Defintion File class declaration.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



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



/**
 * Structure Definition File Class.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kFileSDF : public kFileFormatBase
{
private:
    PSDFHEADER  pHdr;
    PSDFSTRUCT* papStructs;
    PSDFTYPE    paTypes;

    void        parseSDFFile(void *pvFile) throw(int);
    PSDFTYPE    getType(const char *pszType);
    PSDFSTRUCT  getStruct(const char *pszStruct);

public:
    kFileSDF(kFile *pFile) throw(int);
    ~kFileSDF();

    BOOL        dump(kFile *pOut);
};
