/* $Id: winres.h,v 1.7 1999-08-31 14:39:16 sandervl Exp $ */

/*
 * Win32 resource class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINRES_H__
#define __WINRES_H__

class Win32Image;

//Use to distinguish between converted OS/2 resources in an image (pe2lx'ed) or
//resources that are loaded from the original win32 image (pe loader)
#define RSRC_PELOADER        0
#define RSRC_PE2LX	     1
#define RSRC_CUSTOMNODATA    2  //resources created in runtime without data (i.e. CreateMenu)
#define RSRC_CUSTOMINDIRECT  3  //resources indirectly created in runtime (i.e. CreateMenuIndirect)

class Win32Resource
{
public:
                   // Constructors and destructors
                   Win32Resource(); //custum resource (i.e. created by app in runtime)
                   Win32Resource(Win32Image *module, HRSRC hRes, ULONG id, ULONG type);
                   Win32Resource(Win32Image *module, ULONG id, ULONG type,
	                         ULONG size, char *resdata);
    virtual       ~Win32Resource();

    virtual  PVOID lockResource();	//get original win32 resource
    virtual  PVOID lockOS2Resource();	//get converted OS/2 resource

             ULONG getSize() 		{ return ressize; };

  	     ULONG getOS2Handle()   		{ return OS2ResHandle; };
	      void setOS2Handle(ULONG handle) 	{ OS2ResHandle = handle; };

    static    void destroyAll(Win32Image *module);

protected:

             PVOID convertOS2Bitmap(void *bmpdata);

	     PVOID convertResource(void *win32res);

       Win32Image *module;

             HRSRC hres;
             ULONG type;
	     ULONG orgos2type;
             ULONG id;

             PVOID os2resdata;
             PVOID winresdata;

	     ULONG OS2ResHandle;

             ULONG ressize;
	       int resType;

                               // Linked list management
  Win32Resource*   next;               // Next Resource in module

private:
    friend    class Win32Image;
};


    #define    NTRT_NEWRESOURCE      0x2000
    #define    NTRT_ERROR            0x7fff
    #define    NTRT_CURSOR           1
    #define    NTRT_BITMAP           2
    #define    NTRT_ICON             3
    #define    NTRT_MENU             4
    #define    NTRT_DIALOG           5
    #define    NTRT_STRING           6
    #define    NTRT_FONTDIR          7
    #define    NTRT_FONT             8
    #define    NTRT_ACCELERATORS     9
    #define    NTRT_RCDATA           10
    #define    NTRT_MESSAGETABLE     11
    #define    NTRT_GROUP_CURSOR     12
    #define    NTRT_GROUP_ICON       14
    #define    NTRT_VERSION          16
    #define    NTRT_NEWBITMAP        (NTRT_BITMAP|NTRT_NEWRESOURCE)
    #define    NTRT_NEWMENU          (NTRT_MENU|NTRT_NEWRESOURCE)
    #define    NTRT_NEWDIALOG        (NTRT_DIALOG|NTRT_NEWRESOURCE)


#endif
