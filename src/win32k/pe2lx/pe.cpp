/* $Id: pe.cpp,v 1.1 1999-09-06 02:20:07 bird Exp $ */

/*
 * PE2LX PE image interpreter
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#undef RING0
#define RING3

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"  //KSO Tue 2nd June 1998


#include <versionos2.h>
#include "menu.h"
#include "dialog.h"
#include "icon.h"
#include "icongrp.h"
#include "rcdata.h"
#include "strings.h"
#include "bitmap.h"
#include "accelerator.h"
#include "cursor.h"
#include "cursorgrp.h"

#include "options.h" //KSO Aug 15 1998: options from Win32k. Neede for TIBObject
struct options options;


//#define FORWARDERS

#ifndef max /*PLF Sat  97-06-21 22:25:44*/
    #define max(a,b)  (((a)>(b)) ? (a) : (b))
#endif

/*heximal(decimal) KSO Sun 24.05.1998*/
char szHexBuffer[30];

char *hex(ULONG num)
{
    sprintf(szHexBuffer, "0x%+08x (%lu)",num,num);
    return szHexBuffer;
}

BOOL fUseCodePage = FALSE;
int  WinCodePage;

char INFO_BANNER[] =
"Usage: PE2LX winfile [os2file] [-cp]\n\
        OR\n\
        PE2LX winfile os2file [-cp]\n";
#define MAX_RES 17
char *ResTypes[MAX_RES] =
{"niks", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG", "STRING",
    "FONTDIR", "FONT", "ACCELERATOR", "RCDATA",  "MESSAGETABLE",
    "GROUP_CURSOR", "niks", "GROUP_ICON", "niks", "VERSION"};
/**/
static void ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level, PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress, int type, int id);
static void StoreIdResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level, PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress, int type, int id);
static BOOL ProcessImports(void *pFile);

int main(int argc, char *argv[])
{
    HFILE  win32handle;
    ULONG  ulAction       = 0;        /* Action taken by DosOpen */
    ULONG  ulLocal        = 0;        /* File pointer position after DosSetFilePtr */
    APIRET rc             = NO_ERROR;               /* Return code */
    ULONG  filesize, ulRead;
    LPVOID win32file     = NULL;
    IMAGE_SECTION_HEADER    sh;
    IMAGE_OPTIONAL_HEADER      oh;
    IMAGE_FILE_HEADER    fh;
    PIMAGE_BASE_RELOCATION prel;
    PIMAGE_RESOURCE_DIRECTORY       prdRoot, prdType;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
    PIMAGE_RESOURCE_DATA_ENTRY      pData;
    PIMAGE_EXPORT_DIRECTORY         ped;
    char *page;
    int i, j, count, id;
    int *ptrNames, *ptrAddress;
    USHORT *ptrOrd;
    PIMAGE_SECTION_HEADER    psh;
    int  nSections;
char *winfile=NULL, *os2file=NULL;

    if(argc < 2 || argc > 4)
    {
        cout << "pe2lx v0.0." << PE2LX_VERSION << "alpha"<< endl;
        cout << INFO_BANNER << endl;
        return (0);
    }

  for(i=1;i<argc;i++)
  {
      if(!stricmp(argv[i], "/CP") || !stricmp(argv[i], "-CP"))
        fUseCodePage = TRUE;
      else if(winfile == NULL)
        winfile = argv[i];
      else if(os2file == NULL)
        os2file = argv[i];
    }


    //KSO Aug 15 1998: Options - needed for TIBFixObject
    #define OUTPUT_COM2 0
    SET_OPTIONS_PE2LX(options);

    rc = DosOpen(argv[1],                               /* File path name */
                     &win32handle,                          /* File handle */
                     &ulAction,                             /* Action taken */
                     0L,                                        /* File primary allocation */
                     0L,                          /* File attribute */
                     OPEN_ACTION_FAIL_IF_NEW |
                     OPEN_ACTION_OPEN_IF_EXISTS,        /* Open function type */
                     OPEN_FLAGS_NOINHERIT |
                     OPEN_SHARE_DENYNONE  |
                     OPEN_ACCESS_READONLY,              /* Open mode of the file */
                     0L);                                       /* No extended attribute */

    if (rc != NO_ERROR)
    {
        cout << "DosOpen returned " << rc << endl;
        return 1;
    }

    /* Move the file pointer back to the beginning of the file */
    DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);
    DosSetFilePtr(win32handle, 0L, FILE_END, &filesize);
    DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

    win32file = malloc(filesize);
    if (win32file == NULL)
    {
        cout << "Error allocating " << filesize << " bytes" << endl;
//SvL: Probably a huge installation file where the installation data is
//     simply appended
        filesize = 1024*1024;
    }
    win32file = malloc(filesize);
    if (win32file == NULL)
    {
        cout << "Error allocating memory" << endl;
        DosClose(win32handle);                    /* Close the file */
        return (1);
    }

    //KSO Aug 15 1998: LXHdr need to have the filebase address.
    OS2Exe.SetFileBase((char*)win32file);

    rc = DosRead(win32handle, win32file, filesize, &ulRead);
    if (rc != NO_ERROR)
    {
        cout << "DosRead returned " << rc << endl;
        DosClose(win32handle);                    /* Close the file */
        return (1);
    }

    if (GetPEFileHeader (win32file, &fh) == FALSE)
    {
        cout << "Not a valid PE file (probably a 16 bits windows exe/dll)!" << endl;
        DosClose(win32handle);                    /* Close the file */
        return (1);
    }

    if (!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
    {//not valid
        cout << "Not a valid PE file!" << endl;
        DosClose(win32handle);                    /* Close the file */
        return (1);
    }
    if (fh.Machine != IMAGE_FILE_MACHINE_I386)
    {
        cout << "You need a REAL CPU to run this code" << endl;
        DosClose(win32handle);                    /* Close the file */
        return (1);
    }
    //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
    if (fh.Characteristics & IMAGE_FILE_SYSTEM)
    {
        cout << "Can't convert system files" << endl;
        DosClose(win32handle);                    /* Close the file */
        return (1);
    }
    if (os2file == NULL)
    {
        //ok, it's a PE file, so we can safely make a backup copy
        char *newfile = (char *)malloc(strlen(winfile)+1);
        strcpy(newfile, winfile);
        newfile[strlen(newfile)-1]++;
        //save copy of win32 exe/dll (exe->exf, dll->dlk)
        rc = DosMove(winfile, newfile);
        if (rc)
        {
            cout << "Unable to save original win32 file to " << newfile << "(" << rc << ")" << endl;
            free(newfile);
            return (1);
        }
        free(newfile);
    }
    if (fh.Characteristics & IMAGE_FILE_DLL)
        OS2Exe.SetExeType(FALSE);
    else    OS2Exe.SetExeType(TRUE);

    if (fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
    {
        cout << "No fixups, might not run!" << endl;
        OS2Exe.SetNoFixups();
    }

    GetPEOptionalHeader (win32file, &oh);
    cout << "PE Optional header: " << endl;
    cout << "Preferred address : " << hex(oh.ImageBase) << endl;
    cout << "Base Of Code      : " << hex(oh.BaseOfCode) << endl;
    cout << "CodeSize          : " << hex(oh.SizeOfCode) << endl;
    cout << "Base Of Data      : " << hex(oh.BaseOfData) << endl;
    cout << "Data Size (uninit): " << hex(oh.SizeOfUninitializedData) << endl;
    cout << "Data Size (init)  : " << hex(oh.SizeOfInitializedData) << endl;
    cout << "Entry Point       : " << hex(oh.AddressOfEntryPoint) << endl;
    cout << "Section Alignment : " << hex(oh.SectionAlignment) << endl;
    cout << "Stack Reserve size: " << hex(oh.SizeOfStackReserve) << endl;
    cout << "Stack Commit size : " << hex(oh.SizeOfStackCommit) << endl;
    cout << "SizeOfHeapReserve : " << hex(oh.SizeOfHeapReserve) << endl;
    cout << "SizeOfHeapCommit  : " << hex(oh.SizeOfHeapCommit) << endl;
    cout << "FileAlignment     : " << hex(oh.FileAlignment) << endl;

    //SetExeType must have been called before
    if (oh.Subsystem != IMAGE_SUBSYSTEM_WINDOWS_GUI)
    {
        OS2Exe.SetModuleType(SYSTEM_CHARACTER);
        cout << "Console app" << endl;
    }
    else    OS2Exe.SetModuleType(SYSTEM_GUI);

    OS2Exe.SetStackSize(max(oh.SizeOfStackCommit, oh.SizeOfStackReserve));
    if (os2file == NULL)
        OS2Exe.SetModuleName(winfile);
    else  OS2Exe.SetModuleName(os2file);

    nSections = NR_SECTIONS(win32file);
    OS2Exe.SetEntryPoint(oh.AddressOfEntryPoint+oh.ImageBase);
    //KSO Aug 15 1998: dyn PESections
    ltassert( OS2Exe.SetNrSections(nSections) );

    /*KSO Aug 15 1998: Create TIBSection! */
    ltassert(OS2Exe.CreateTIBSection(&oh));

    if ((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (win32file)) != NULL)
    {
        cout << endl;
        for (i=0; i<nSections; i++)
        {
            cout << "Section Name:        " << psh[i].Name << endl;
            cout << "Raw data size:       " << hex(psh[i].SizeOfRawData) << endl;
            cout << "Virtual Address:     " << hex(psh[i].VirtualAddress) << endl;
            cout << "Virtual Size:        " << hex(psh[i].Misc.VirtualSize) << endl;
            cout << "Pointer to raw data: " << hex(psh[i].PointerToRawData) << endl;
            cout << "Section flags:       " << hex(psh[i].Characteristics) << endl;
            if (strcmp(psh[i].Name, ".reloc") == 0)
            {
                cout << ".reloc" << endl << endl;
                continue;
            }
            if (strcmp(psh[i].Name, ".edata") == 0)
            {
                cout << ".edata" << endl << endl;
                continue;
            }
            if (strcmp(psh[i].Name, ".pdata") == 0)
            {
                cout << ".pdata" << endl << endl;
                continue;
            }
            if (strcmp(psh[i].Name, ".rsrc") == 0)
            {
                cout << ".rsrc" << endl << endl;
                continue;
            }
            if (strcmp(psh[i].Name, ".debug") == 0)
            {
                cout << ".rdebug" << endl << endl;
                continue;
            }
            if (IsImportSection(win32file, &psh[i]))
            {
                int type = SECTION_IMPORT;
                cout << "Import Data Section" << endl << endl;
                if (psh[i].Characteristics & IMAGE_SCN_CNT_CODE)
                {
                    cout << "Also Code Section" << endl << endl;
                    OS2Exe.SetEntryAddress(oh.AddressOfEntryPoint-psh[i].VirtualAddress);
                    type |= SECTION_CODE;
                }
                OS2Exe.StoreSection((char *)win32file+psh[i].PointerToRawData,
                                          psh[i].SizeOfRawData, psh[i].Misc.VirtualSize,
                                          psh[i].VirtualAddress + oh.ImageBase,
                                          type);
                continue;
            }
            if (strcmp(psh[i].Name, ".tls") == 0)
            {
                IMAGE_TLS_DIRECTORY *tlsDir;

                tlsDir = (IMAGE_TLS_DIRECTORY *)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_TLS);
                if (tlsDir)
                {
                    cout << "TLS Directory" << endl;
                    cout << "TLS Address of Index     " << hex((ULONG)tlsDir->AddressOfIndex) << endl;
                    cout << "TLS Address of Callbacks " << hex((ULONG)tlsDir->AddressOfCallBacks) << endl;
                    cout << "TLS SizeOfZeroFill       " << hex(tlsDir->SizeOfZeroFill) << endl;
                    cout << "TLS Characteristics      " << hex(tlsDir->Characteristics) << endl;
                    OS2Exe.SetTLSAddress(tlsDir->StartAddressOfRawData);
                    OS2Exe.SetTLSIndexAddress((ULONG)tlsDir->AddressOfIndex);
                    OS2Exe.SetTLSInitSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData);
                    OS2Exe.SetTLSTotalSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData + tlsDir->SizeOfZeroFill);
                    OS2Exe.SetTLSCallBackAddr((ULONG)tlsDir->AddressOfCallBacks);
                }
            }

            if ((psh[i].Characteristics & IMAGE_SCN_CNT_CODE)
                 //KSO Sun 1998-08-09: Borland does not alway set the CODE flag for its "CODE" section (TIB fix need to have the first section marked as code)
                 || (psh[i].Characteristics & IMAGE_SCN_MEM_EXECUTE &&
                      !(psh[i].Characteristics & (IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_CNT_INITIALIZED_DATA)) //KSO: make sure its not marked as a datasection
                     )
                )
            {
                cout << "Code Section" << endl << endl;
                if (oh.AddressOfEntryPoint < psh[i].VirtualAddress)
                    OS2Exe.SetEntryAddress(oh.AddressOfEntryPoint - oh.BaseOfCode);
                else    OS2Exe.SetEntryAddress(oh.AddressOfEntryPoint-psh[i].VirtualAddress);

                OS2Exe.StoreSection((char *)win32file+psh[i].PointerToRawData,
                                          psh[i].SizeOfRawData, psh[i].Misc.VirtualSize,
                                          psh[i].VirtualAddress + oh.ImageBase,
                                          SECTION_CODE);
                continue;
            }
            if (!(psh[i].Characteristics & IMAGE_SCN_MEM_WRITE))
            {//read only data section
                cout << "Read Only Data Section" << endl << endl;
                OS2Exe.StoreSection((char *)win32file+psh[i].PointerToRawData,
                                          psh[i].SizeOfRawData, psh[i].Misc.VirtualSize,
                                          psh[i].VirtualAddress + oh.ImageBase,
                                          SECTION_READONLYDATA);
                continue;
            }
            if (psh[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
            {
                cout << "Uninitialized Data Section" << endl << endl;
                OS2Exe.StoreSection((char *)win32file+psh[i].PointerToRawData,
                                          psh[i].SizeOfRawData, psh[i].Misc.VirtualSize,
                                          psh[i].VirtualAddress + oh.ImageBase,
                                          SECTION_UNINITDATA);
                continue;
            }
            if (psh[i].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
            {
                cout << "Initialized Data Section" << endl << endl;
                OS2Exe.StoreSection((char *)win32file+psh[i].PointerToRawData,
                                          psh[i].SizeOfRawData, psh[i].Misc.VirtualSize,
                                          psh[i].VirtualAddress + oh.ImageBase,
                                          SECTION_INITDATA);
                continue;
            }
            if (psh[i].Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ))
            {
                cout << "Other Section, stored as read/write uninit data" << endl << endl;
                OS2Exe.StoreSection((char *)win32file+psh[i].PointerToRawData,
                                          psh[i].SizeOfRawData, psh[i].Misc.VirtualSize,
                                          psh[i].VirtualAddress + oh.ImageBase,
                                          SECTION_UNINITDATA);
                continue;
            }
        }
    }
    //SvL: Align and/or concatenate code & data sections
    OS2Exe.AlignSections(&oh);

    prel = (PIMAGE_BASE_RELOCATION) ImageDirectoryOffset (win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    OS2Exe.SetNrOff32Fixups((prel) ? oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size/2 : 0);

    //SvL: Add EntryPoint & TLS fixups (if required)
    OS2Exe.AddExtraFixups();

    if (prel)
    {
        j = 1;
        while (prel->VirtualAddress)
        {
            page = (char *)((char *)prel + (int)prel->VirtualAddress);
            count  = (prel->SizeOfBlock - 8)/2;
            cout.setf(ios::hex, ios::basefield);
            cout << "Page " << j << " Address " << (ULONG)prel->VirtualAddress << " Count " << count << endl;
            cout.setf(ios::dec, ios::basefield);
            j++;
            for (i=0;i<count;i++)
            {
                int type   = prel->TypeOffset[i] >> 12;
                int offset = prel->TypeOffset[i] & 0xFFF;
                switch (type)
                {
                    case IMAGE_REL_BASED_ABSOLUTE:
                        //cout << "absolute fixup; unused" << endl;
                        break;  //skip
                    case IMAGE_REL_BASED_HIGHLOW:
                        //cout << "address " << hex(offset) << " type " << type; // << endl;
                        OS2Exe.AddOff32Fixup(oh.ImageBase +
                                                    prel->VirtualAddress +
                                                    offset);
                        //cout << endl;
                        break;
                    case IMAGE_REL_BASED_HIGH:
                    case IMAGE_REL_BASED_LOW:
                    case IMAGE_REL_BASED_HIGHADJ:
                    case IMAGE_REL_BASED_MIPS_JMPADDR:
                    default:
                        cout << "Unknown/unsupported fixup type!" << endl;
                        break;
                }
            }
            prel = (PIMAGE_BASE_RELOCATION)((char*)prel + prel->SizeOfBlock);
        }//while
    }
    else
    {
        cout << "No fixups, might not run!" << endl;
        OS2Exe.SetNoFixups();
    }


    /* get section header and pointer to data directory for .edata section */
    if ((ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryOffset
          (win32file, IMAGE_DIRECTORY_ENTRY_EXPORT)) != NULL &&
         /*(fh.Characteristics & IMAGE_FILE_DLL) &&*/ //KSO Aug 16 1998: EXE have exports!
         GetSectionHdrByImageDir(win32file, IMAGE_DIRECTORY_ENTRY_EXPORT, &sh))
    {

        cout << "Exported Functions: " << endl;
        ptrOrd     = (USHORT *)((int)ped->AddressOfNameOrdinals -
                                        (int)sh.VirtualAddress +
                                        (int)sh.PointerToRawData + (int)win32file);
        ptrNames   = (int *)((int)ped->AddressOfNames -
                                    (int)sh.VirtualAddress +
                                    (int)sh.PointerToRawData + (int)win32file);
        ptrAddress = (int *)((int)ped->AddressOfFunctions -
                                    (int)sh.VirtualAddress +
                                    (int)sh.PointerToRawData + (int)win32file);
        OS2Exe.SetNrExtFixups(max(ped->NumberOfNames,ped->NumberOfFunctions));
        int   ord, RVAExport;
        char *name;
        for (i=0;i<ped->NumberOfNames;i++)
        {
            ord       = ptrOrd[i] + ped->Base;
            name      = (char *)((int)ptrNames[i] - (int)sh.VirtualAddress +
                                        (int)sh.PointerToRawData + (int)win32file);
            RVAExport = ptrAddress[ptrOrd[i]];
//      RVAExport = ptrAddress[i];
#ifdef FORWARDERS
            if (RVAExport < sh.VirtualAddress || RVAExport > sh.VirtualAddress + sh.SizeOfRawData)
            {
#endif
                //points to code (virtual address relative to oh.ImageBase
                cout << "address 0x";
                cout.setf(ios::hex, ios::basefield);
                cout << RVAExport;
                cout.setf(ios::dec, ios::basefield);
                cout << " " << name << "@" << ord << endl;
                OS2Exe.AddNameExport(oh.ImageBase + RVAExport, name, ord);
#ifdef FORWARDERS

            }
            else
            {//forwarder
                char *forward = (char *)((int)RVAExport -
                                                 (int)sh.VirtualAddress +
                                                 (int)sh.PointerToRawData +
                                                 (int)win32file);
                cout << RVAExport << " " << name << " @" << ord << " is forwarder to " << (int)forward << endl;
//          OS2Exe.AddForwarder(name, ord, forward);
            }
#endif
        }
        for (i=0;i<max(ped->NumberOfNames,ped->NumberOfFunctions);i++)
        {
            ord = ped->Base + i;     //Correct??
            RVAExport = ptrAddress[i];
#ifdef FORWARDERS
            if (RVAExport < sh.VirtualAddress || RVAExport > sh.VirtualAddress + sh.SizeOfRawData)
            {
#endif
                if (RVAExport)
                {
                    //points to code (virtual address relative to oh.ImageBase
                    cout << "ord " << ord << " at 0x";
                    cout.setf(ios::hex, ios::basefield);
                    cout << RVAExport  << endl;
                    cout.setf(ios::dec, ios::basefield);
                    OS2Exe.AddOrdExport(oh.ImageBase + RVAExport, ord);
                }
#ifdef FORWARDERS
            }
            else
            {//forwarder or empty
                char *forward = (char *)((int)RVAExport -
                                                 (int)sh.VirtualAddress +
                                                 (int)sh.PointerToRawData +
                                                 (int)win32file);
                cout << "ord " << ord << " at 0x";
                cout.setf(ios::hex, ios::basefield);
                cout << RVAExport << " is forwarder to 0x" << (int)forward << endl;
                cout.setf(ios::dec, ios::basefield);
//          OS2Exe.AddForwarder(NULL, ord, forward);
            }
#endif
        }
    }

    {
        if (fh.Characteristics & IMAGE_FILE_DLL)
        {
            OS2Exe.SetNrExtFixups(0);       //adds module name to resident table
        }
    }
    //imported stuff
    if (!ProcessImports(win32file))
    {
        cout << "Fatal: failed to process imports"  << endl;
        return -1;
    }


    //resource stuff
    if (GetSectionHdrByName (win32file, &sh, ".rsrc"))
    {
        cout << "Resource section: " << endl;
        cout << "Raw data size:       " << hex(sh.SizeOfRawData) << endl;
        cout << "Virtual Address:     " << hex(sh.VirtualAddress) << endl;
        cout << "Pointer to raw data: " << hex(sh.PointerToRawData) << endl;
        cout << "Section flags:       " << hex(sh.Characteristics) << endl << endl;
        /* get root directory of resource tree */
        if (NULL != (prdRoot = (PIMAGE_RESOURCE_DIRECTORY)ImageDirectoryOffset
                         (win32file, IMAGE_DIRECTORY_ENTRY_RESOURCE)))
        {
            cout << "Resources: " << endl;
            cout << "pdrRoot:   " << hex((ULONG)prdRoot - (ULONG)win32file) << endl;

            OS2Exe.SetResourceSize(max(sh.Misc.VirtualSize, sh.SizeOfRawData));
            OS2Exe.SetNrResources(GetNumberOfResources(win32file));

            /* set pointer to first resource type entry */
            prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));

            //SvL: Store all resource ids so we can safely allocate unique ids for name ids
            for (i=0; i<prdRoot->NumberOfNamedEntries+prdRoot->NumberOfIdEntries; i++)
            {
                /* locate directory or each resource type */
                prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (int)prde->u2.OffsetToData);

                if (i<prdRoot->NumberOfNamedEntries)
                {
                    //SvL: 30-10-'97, high bit is set, so clear to get real offset
                    prde->u1.Name &= ~0x80000000;
                    char *resname = UnicodeToAscii(*(WCHAR *)((int)prdRoot + (int)prde->u1.Name), (WCHAR *)((int)prdRoot + (int)prde->u1.Name + sizeof(WCHAR)));  // first word = string length

                    for (j=0;j<MAX_RES;j++)
                    {
                        if (strcmp(resname, ResTypes[j]) == 0)
                            break;
                    }
                    if (j == MAX_RES)
                    {
                        id = NTRT_RCDATA;
                    }
                    else      id = j;
                }
                else
                {
                    id = prde->u1.Id;
                }

                if ((int)prdType & 0x80000000)
                {//subdirectory?
                    StoreIdResSubDir(prdType, 1, prdRoot, sh.VirtualAddress, id, 0);
                }
                else    assert(FALSE);

                /* increment to next entry */
                prde++;
            }

            /* set pointer to first resource type entry */
            prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));

            /* loop through all resource directory entry types */
//eerste level -> types
//tweede level -> names
//derde  level -> language
            cout << "Name entries: " << endl;
            cout << "ID entries: " << endl;
            for (i=0; i<prdRoot->NumberOfNamedEntries+prdRoot->NumberOfIdEntries; i++)
            {
                /* locate directory or each resource type */
                prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (int)prde->u2.OffsetToData);

                if (i<prdRoot->NumberOfNamedEntries)
                {
                    //SvL: 30-10-'97, high bit is set, so clear to get real offset
                    prde->u1.Name &= ~0x80000000;
                    char *resname = UnicodeToAscii(*(WCHAR *)((int)prdRoot + (int)prde->u1.Name), (WCHAR *)((int)prdRoot + (int)prde->u1.Name + sizeof(WCHAR)));  // first word = string length

                    cout << "Type " << resname << endl;
                    for (j=0;j<MAX_RES;j++)
                    {
                        if (strcmp(resname, ResTypes[j]) == 0)
                            break;
                    }
                    if (j == MAX_RES)
                    {
                        id = NTRT_RCDATA;
                    }
                    else      id = j;
                }
                else
                {
                    if (prde->u1.Id < MAX_RES)
                        cout << "Type " << ResTypes[prde->u1.Id] << endl;
                    else    cout << "Custom Type " << prde->u1.Id << endl;
                    id = prde->u1.Id;
                }

                if ((int)prdType & 0x80000000)
                {//subdirectory?
                    ProcessResSubDir(prdType, 1, prdRoot, sh.VirtualAddress, id, 0);
                }
                else
                {
                    pData = (PIMAGE_RESOURCE_DATA_ENTRY)prdType;
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    cout << "Resource Data entry of size " << hex(pData->Size) << endl;
                    cout << "Resource Data RVA " << hex(pData->OffsetToData) << endl;
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    assert(FALSE);
                }

                /* increment to next entry */
                prde++;
            }
        }
        OS2Icon::DestroyAll();
        OS2Cursor::DestroyAll();
    }
    OS2Exe.SaveConvertedNames();
    if (os2file == NULL)
        OS2Exe.SaveNewExeFile(winfile);
    else  OS2Exe.SaveNewExeFile(os2file);

    return (0);
}
//******************************************************************************
//No support for multiple languages; just select the first one present and
//ignore the rest
//******************************************************************************

static void ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level,
                             PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress,
                             int type, int id)
{
    PIMAGE_RESOURCE_DIRECTORY       prdType2;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
    PIMAGE_RESOURCE_DIR_STRING_U    pstring;
    PIMAGE_RESOURCE_DATA_ENTRY      pData;
    int i;

    switch (level)
    {
        case 1:
            cout << "Names.." << endl;
            break;
        case 2:
            cout << "Language.." << endl;
            break;
    }
    prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)prdType & ~0x80000000);
    cout << "Subdir with " << prdType->NumberOfNamedEntries << " Name entries" << endl;
    cout << "Subdir with " << prdType->NumberOfIdEntries << " id entries" << endl;

    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdType + sizeof (IMAGE_RESOURCE_DIRECTORY));
    for (i=0;i<prdType->NumberOfNamedEntries+prdType->NumberOfIdEntries;i++)
    {
        /* locate directory or each resource type */
        prdType2 = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (int)prde->u2.OffsetToData);

        if (i < prdType->NumberOfNamedEntries)
        {//name or id entry?
            if (prde->u1.s.NameIsString) //unicode directory string /*PLF Sat  97-06-21 22:30:35*/
                prde->u1.Name &= ~0x80000000;
            pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((int)prdRoot + (int)prde->u1.Name);
            cout << "Name = " << UnicodeToAscii(pstring->Length, pstring->NameString) << endl;
            if (level == 1)
            {
                id = OS2Exe.ConvertNametoId(UnicodeToAscii(pstring->Length, pstring->NameString));
            }
        }
        else
        {
            cout << "Id " << prde->u1.Id << endl;
            if (level == 1)  id = prde->u1.Id;
        }

        if ((int)prdType2 & 0x80000000)
        {//subdirectory?
            ProcessResSubDir(prdType2, 2, prdRoot, VirtualAddress, type, id);
        }
        else
        {
            pData = (PIMAGE_RESOURCE_DATA_ENTRY)prdType2;
            cout << "Resource Data entry of size " << hex(pData->Size) << endl;
            cout << "Resource Data RVA " << hex(pData->OffsetToData - VirtualAddress) << endl;
            cout << "Resource Data RVA " << hex(pData->OffsetToData) << endl;
            cout << "Resource Data VA  " << hex(VirtualAddress) << endl;
            if (fUseCodePage == TRUE)
                WinCodePage = pData->CodePage;
            else    WinCodePage = 0;

            if (pData->Size)
            {//winamp17 winzip archive has resource with size 0
                switch (type)
                {
                    case NTRT_MENU:
                        ShowMenu(OS2Exe, id, (MenuHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size, WinCodePage);
                        break;
                    case NTRT_ICON:
                        new OS2Icon(id, (WINBITMAPINFOHEADER *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_BITMAP:
                        ShowBitmap(OS2Exe, id, (WINBITMAPINFOHEADER *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_GROUP_ICON:
                        ShowGroupIcon(OS2Exe, id, (IconHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_CURSOR:
                        new OS2Cursor(id, (CursorComponent *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_GROUP_CURSOR:
                        ShowGroupCursor(OS2Exe, id, (CursorHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_DIALOG:
                        ShowDialog(OS2Exe, id, (DialogBoxHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size, WinCodePage);
                        break;
                    case NTRT_VERSION:
                        //Store version resource as OS/2 RT_RCDATA resource
                        //to retrieve the original for win32's version apis
                        //TODO: Only supports one version resource (who would want to use more??)
                        OS2Exe.SetVersionResourceId(id);
                    case NTRT_RCDATA:
                        ShowRCData(OS2Exe, id, (char *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_STRING:
                        //String format: tables of 16 strings stored as one resource
                        //upper 12 bits of resource id passed by user determines block (res id)
                        //lower 4 bits are an index into the string table
                        //Best solution is to split the strings up and store them as RCDATA
                        ShowStrings(OS2Exe, id, (char *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size, WinCodePage);
                        break;
                    case NTRT_ACCELERATORS:
                        ShowAccelerator(OS2Exe, id, (WINACCEL *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    default:
                        cout << "!!Unsupported resource type!! : " << type << endl;
                        //Just save it as rcdata
                        ShowRCData(OS2Exe, id, (char *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                }//switch
            }//if size > 0
        }
        if (level == 2) break;   //only support one language!

        prde++;
        cout << endl;
    }
}
//******************************************************************************
//******************************************************************************
void StoreIdResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level,
                             PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress,
                             int type, int id)
{
    PIMAGE_RESOURCE_DIRECTORY       prdType2;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
    int i, j;

    prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)prdType & ~0x80000000);

    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdType + sizeof (IMAGE_RESOURCE_DIRECTORY));
    for (i=0;i<prdType->NumberOfNamedEntries+prdType->NumberOfIdEntries;i++)
    {
        /* locate directory or each resource type */
        prdType2 = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (int)prde->u2.OffsetToData);

        if (i >= prdType->NumberOfNamedEntries)
        {//name or id entry?
            if (level == 1)  id = prde->u1.Id;
        }
        else
        {
            prde++;
            continue;   //skip name ids
        }

        if ((int)prdType2 & 0x80000000)
        {//subdirectory?
            StoreIdResSubDir(prdType2, 2, prdRoot, VirtualAddress, type, id);
        }
        else
        {
            if (type == NTRT_STRING)
            {
                for (j=0;j<16;j++)
                {
                    OS2Exe.StoreResourceId((id-1)*16+j);
                }
            }
            else    OS2Exe.StoreResourceId(id);
        }
        if (level == 2) break;   //only support one language!

        prde++;
    }
}
//******************************************************************************
//******************************************************************************



/**
 * Process Imports in a PE file.
 * @param  pFile  Pointer to start of PE-file.
 *  @return True on success. False on failiure - object is then unusable.
 * @remark Should now detect most Borland styled files including the GifCon32.exe and
 *         loader32 from SoftIce. (Stupid Borland!!!)
 *         KSO [Jul 22 1998 2:44am]
 *        <I>This function has a sister function in LXFile::ProcessImports - update always both</I>
 */
BOOL ProcessImports(void *pFile)
{
    PIMAGE_IMPORT_DESCRIPTOR pID;
    IMAGE_SECTION_HEADER     shID;
    IMAGE_SECTION_HEADER     shExtra = {0};
    PIMAGE_OPTIONAL_HEADER   pOH;

    int    i,j;
    BOOL   fBorland = 0;
    int    cModules;
    char  *pszModules;
    char  *pszCurModule;
    char  *pszTmp;
    ULONG *pulImport;
    ULONG  ulCurFixup;
    int    Size;

    /* "algorithm:"
     *      1) get module names and store them
     *          a) check dwRVAModuleName is within .idata seg - if not find section
     *      2) iterate thru functions of each module
     *          a) check OriginalFirstThunk is not 0 and that it points to a RVA.
     *          b) if not a) borland-styled PE-file - ARG!!!
     *              check FirstThunk
     *          c) check OriginalFirstThunk/FirstThunk ok RVAs and find right section
     *          d) store ordinal/name import
     *      3) finished
     */

    /* 1) get module names */
    pID = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryOffset(pFile, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (pID == NULL)
    {
        OS2Exe.StoreImportModules("KERNEL32.DLL", 1);
        OS2Exe.SetNoNameImports();
        return TRUE;    //SvL: yes, there are dlls without imports!
    }
    if (!GetSectionHdrByImageDir(pFile, IMAGE_DIRECTORY_ENTRY_IMPORT, &shID))
    {
        OS2Exe.StoreImportModules("KERNEL32.DLL", 1);
        OS2Exe.SetNoNameImports();
        return TRUE;    //SvL: yes, there are dlls without imports!
    }
    //calc size of module list
    i = Size = cModules = 0;
    while (pID[i].Name != 0)
    {
        //test RVA inside ID-Section
        if (pID[i].Name >= shID.VirtualAddress && pID[i].Name < shID.VirtualAddress + max(shID.Misc.VirtualSize, shID.SizeOfRawData))
            pszTmp = (char*)(pID[i].Name- shID.VirtualAddress + shID.PointerToRawData + (ULONG)pFile);
        else
        {
            //is the "Extra"-section already found or do we have to find it?
            if (pID[i].Name < shExtra.VirtualAddress || pID[i].Name >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
                if (!GetSectionHdrByRVA(pFile, &shExtra, pID[i].Name))
                    return FALSE;
            pszTmp = (char*)(pID[i].Name- shExtra.VirtualAddress + shExtra.PointerToRawData + (ULONG)pFile);
        }
        Size += strlen(pszTmp) + 1;
        i++;
        cModules++;
    }

    pszModules = (char*)malloc(Size);
    assert(pszModules != NULL);
    j = 0;
    for (i = 0; i < cModules; i++)
    {
        //test RVA inside ID-Section
        if (pID[i].Name >= shID.VirtualAddress && pID[i].Name < shID.VirtualAddress + max(shID.Misc.VirtualSize, shID.SizeOfRawData))
            pszTmp = (char*)(pID[i].Name- shID.VirtualAddress + shID.PointerToRawData + (ULONG)pFile);
        else
        {
            fBorland = TRUE;
            //is the "Extra"-section already found or do we have to find it?
            if (pID[i].Name < shExtra.VirtualAddress || pID[i].Name >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
                if (GetSectionHdrByRVA(pFile, &shExtra, pID[i].Name))
                {
                    free(pszModules);
                    return FALSE;
                }
            pszTmp = (char*)(pID[i].Name- shExtra.VirtualAddress + shExtra.PointerToRawData + (ULONG)pFile);
        }
        strcpy(pszModules+j, pszTmp);
        j += strlen(pszTmp) + 1;
    }
    cout << endl;
    if (fBorland)
        cout << "Borland-styled PE-File." << endl;
    //Store modules
    cout << cModules << " imported Modules: " << endl;
    OS2Exe.StoreImportModules(pszModules, cModules);


    /* 2) functions */
    pszCurModule = pszModules;
    pOH = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF(pFile);
    for (i = 0; i < cModules; i++)
    {
        cout << "Module " << pszCurModule << endl;
        //  a) check that OriginalFirstThunk not is 0 and look for Borland-styled PE
        if (i == 0)
        {
            //heavy borland-style test - assume array of thunks is within that style does not change
            if ((ULONG)pID[i].u.OriginalFirstThunk == 0
                ////|| (ULONG)pID[i].u.OriginalFirstThunk < pOH->SizeOfImage
                || (ULONG)pID[i].u.OriginalFirstThunk < shID.VirtualAddress
                || (ULONG)pID[i].u.OriginalFirstThunk >= shID.VirtualAddress + max(shID.Misc.VirtualSize, shID.SizeOfRawData)
                || (ULONG)pID[i].u.OriginalFirstThunk >= pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                && (ULONG)pID[i].u.OriginalFirstThunk < sizeof(*pID)*cModules + pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                //   && (ULONG)pID[i].u.OriginalFirstThunk < pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size + pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                )
                fBorland = TRUE;
        }

        //light borland-style test
        if (pID[i].u.OriginalFirstThunk == 0 || fBorland)
            pulImport = (ULONG*)pID[i].FirstThunk;
        else
            pulImport = (ULONG*)pID[i].u.OriginalFirstThunk;

        //  b) check if RVA ok
        if (!(pulImport > 0 && (ULONG)pulImport < pOH->SizeOfImage))
        {
            cout << "Invalid RVA " << hex((ULONG)pulImport) << endl;
            break;
        }
        // check section
        if ((ULONG)pulImport < shExtra.VirtualAddress || (ULONG)pulImport >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
        {
            if (!GetSectionHdrByRVA(pFile, &shExtra, (ULONG)pulImport))
            {
                cout << "warning: could not find section for Thunk RVA " << hex((ULONG)pulImport) << endl;
                break;
            }
        }

        pulImport  = (PULONG)((ULONG)pulImport - shExtra.VirtualAddress + (ULONG)pFile + shExtra.PointerToRawData);
        j          = 0;
        ulCurFixup = (ULONG)pID[i].FirstThunk + pOH->ImageBase;
        while (pulImport[j] != 0)
        {
            if (pulImport[j] & IMAGE_ORDINAL_FLAG)
            {   //ordinal
                cout.setf(ios::hex, ios::basefield);
                cout << "0x" << ulCurFixup << " Imported function " << pszCurModule << "@" << (pulImport[j] & ~IMAGE_ORDINAL_FLAG) << endl;
                cout.setf(ios::dec, ios::basefield);
                OS2Exe.StoreImportByOrd(i, pulImport[j] & ~IMAGE_ORDINAL_FLAG, ulCurFixup);
            }
            else
            {   //name
                //check
                if (pulImport[j] < shExtra.VirtualAddress || pulImport[j] >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
                    if (!GetSectionHdrByRVA(pFile, &shExtra, pulImport[j]))
                    {
                        cout << "warning: could not find section for Import Name RVA " << hex(pulImport[j]) << endl;
                        break;
                    }
                    //KSO - Aug 6 1998 1:15am:this eases comparing...
                char *pszFunctionName = (char*)(pulImport[j] + (ULONG)pFile + shExtra.PointerToRawData - shExtra.VirtualAddress + 2);
                cout.setf(ios::hex, ios::basefield);
                cout << "0x" << ulCurFixup << " Imported function " << pszFunctionName << endl;
                cout.setf(ios::dec, ios::basefield);
                OS2Exe.StoreImportByName(pszCurModule, i, pszFunctionName, ulCurFixup);
            }
            ulCurFixup += sizeof(IMAGE_THUNK_DATA);
            j++;
        }

        pszCurModule += strlen(pszCurModule) + 1;
        cout << endl;
    }//for (i = 0; i < cModules; i++)

    free(pszModules);
    return TRUE;
}

