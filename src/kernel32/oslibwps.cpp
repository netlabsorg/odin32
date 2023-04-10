#define INCL_WIN
#define INCL_PM
#define INCL_WPCLASS
#include <os2wrap.h>
#define __USE_EMX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <dbglog.h>
#include <winconst.h>
#include <win32api.h>
#include <wprocess.h>
#include <custombuild.h>

#include "oslibmisc.h"

#define WPS_SHELLLINK_MAGIC	"WPS_SHELLLINK:"
#define WPS_SHELLLINK_DESKTOP   "WPS_SHELLLINK_DESKTOP_"


static BOOL fDisableFolderShellLink = FALSE;

//******************************************************************************
//******************************************************************************
void OSLibStripFile(char *path)
{
  char *pszFilename;
  char *pszFilename1;

   pszFilename  = strrchr(path, '\\');                 /* find rightmost backslash */
   pszFilename1 = strrchr(path, '/');                  /* find rightmost slash */
   if(pszFilename > pszFilename1 && pszFilename != NULL)
     *pszFilename = 0;
   else
   if (pszFilename1 != NULL)
     *pszFilename1 = 0;
}

extern "C" {

//******************************************************************************
// ODIN_DisableFolderShellLink
//
// Disable object creation in Odin folder. Desktop shortcuts will still be
// created as WPS objects on the desktop.
//
//******************************************************************************
void WIN32API ODIN_DisableFolderShellLink()
{
    fDisableFolderShellLink = TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OSLibWinCreateObject(LPSTR pszPath, LPSTR pszArgs,
                                   LPSTR pszWorkDir, LPSTR pszLink,
                                   LPSTR pszDescription, LPSTR pszIcoPath,
                                   INT iIcoNdx, BOOL fDesktop)
{
   HOBJECT hObject = 0;
   LPSTR   pszName;
   LPSTR   pszSetupString;
   LPSTR   pszFolder;
   char    szSystemDir[256];
   char    temp[128];
   char    szWorkDir[256];
   char    szPEGUILoaderPath[256];
   BOOL    fWin32App;
   FILE   *lnkfile = NULL;

   if(fDisableFolderShellLink && !fDesktop) {
       return TRUE; //pretend success
   }

   if(pszLink) {
       lnkfile = fopen(pszLink, "wb");

       char *tmp;
       pszName = OSLibStripPath(pszLink);
       tmp = pszName;
       while(*tmp) {
           if(*tmp == '.') {
               *tmp = 0;
               break;
           }
           tmp++;
       }
   }
   else {
       dprintf(("OSLibWinCreateObject: pszLink == NULL!!"));
       goto fail;
   }
   dprintf(("OSLibWinCreateObject %s %s %s\n    %s %s %s %d %d", pszPath, pszArgs,
            pszWorkDir, pszName, pszDescription, pszIcoPath, iIcoNdx, fDesktop));
   dprintf(("Link path %s", pszLink));

   GetSystemDirectoryA(szSystemDir, sizeof(szSystemDir));
   if(pszWorkDir && *pszWorkDir) {
       strcpy(szWorkDir, pszWorkDir);
   }
   else {
       strcpy(szWorkDir, pszPath);
       OSLibStripFile(szWorkDir);
   }

   ODIN_QueryLoaders(NULL, 0, szPEGUILoaderPath, sizeof(szPEGUILoaderPath), NULL, 0);

   pszSetupString = (LPSTR)malloc(128 + strlen(pszPath) + strlen(pszName) +
                                  strlen(pszLink) + strlen(szSystemDir) +
                                  strlen(szWorkDir) + strlen(pszIcoPath) +
                                  strlen(szPEGUILoaderPath) +
                                  ((pszArgs) ? strlen(pszArgs) : 0) +
                                  ((pszWorkDir) ? strlen(pszWorkDir) : 0));
   if(pszSetupString == NULL) {
       DebugInt3();
       goto fail;
   }

   fWin32App = ODIN_IsWin32App(pszPath);
   if(!fWin32App)
   {//don't use the PE loader; use the program path directly
        sprintf(pszSetupString, "PROGTYPE=PM;OBJECTID=<%s%s>;EXENAME=%s;SET BEGINLIBPATH=%s;STARTUPDIR=%s;ICONFILE=%s;PARAMETERS=", (fDesktop) ? WPS_SHELLLINK_DESKTOP : "", pszName, pszPath, szSystemDir, szWorkDir, pszIcoPath);
   }
   else sprintf(pszSetupString, "PROGTYPE=PM;OBJECTID=<%s%s>;EXENAME=%s;SET BEGINLIBPATH=%s;STARTUPDIR=%s;ICONFILE=%s;PARAMETERS=\"%s\"", (fDesktop) ? WPS_SHELLLINK_DESKTOP : "", pszName, szPEGUILoaderPath, szSystemDir, szWorkDir, pszIcoPath, pszPath);
   if(pszArgs && *pszArgs) {
       strcat(pszSetupString, " ");
       strcat(pszSetupString, pszArgs);
   }
   strcat(pszSetupString, ";");

   if(fDesktop) {
       dprintf(("Name = %s", pszName));
       dprintf(("Setup string = %s", pszSetupString));

       //Use a different name for desktop objects
       hObject = WinCreateObject("WPProgram", pszName, pszSetupString,
                                 "<WP_DESKTOP>", CO_REPLACEIFEXISTS);
   }
   else {
       //e.g.: Link path k:\source\odin32\bin\win\Start Menu\Programs\Winamp\Winamp
       OSLibStripFile(pszLink);
       pszFolder = OSLibStripPath(pszLink);
       sprintf(temp, "<FOLDER_%s>", pszFolder);
       sprintf(szWorkDir, "OBJECTID=%s;", temp);
       hObject = WinCreateObject("WPFolder", pszFolder, szWorkDir,
                                 "<ODINFOLDER>", CO_UPDATEIFEXISTS);
       if(hObject) {
           hObject = WinCreateObject("WPProgram", pszName, pszSetupString,
                                     temp, CO_REPLACEIFEXISTS);
       }
       else {
           hObject = 1; //force silent failure
       }
   }

   if(!hObject) {
       dprintf(("ERROR: WinCreateObject failed!!"));
   }
   else {
       //write WPS object info to lnk file

       //write magic string
       fwrite(WPS_SHELLLINK_MAGIC, 1, sizeof(WPS_SHELLLINK_MAGIC)-1, lnkfile);
       //and WPS object id
       if(fDesktop) {
           fwrite(WPS_SHELLLINK_DESKTOP, 1, sizeof(WPS_SHELLLINK_DESKTOP)-1, lnkfile);
       }
       fwrite(pszName, 1, strlen(pszName), lnkfile);
   }
   free(pszSetupString);
   if(lnkfile) fclose(lnkfile);
   return hObject != 0;

fail:
   if(lnkfile) fclose(lnkfile);
   return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OSLibIsShellLink(LPSTR lpszLink)
{
    char szLinkFile[CCHMAXPATH];
    BOOL fIsShellLink = FALSE;

    strcpy(szLinkFile, lpszLink);
    strupr(szLinkFile);
    if(strstr(szLinkFile, ".LNK"))
    {//could be a shelllink file, check for magic string at start of the file
       FILE *lnkfile;

       lnkfile = fopen(lpszLink, "r");
       if(lnkfile == NULL) return FALSE;

       char  szMagic[sizeof(WPS_SHELLLINK_MAGIC)];

       memset(szMagic, 0, sizeof(szMagic));
       fread(szMagic, sizeof(szMagic)-1, 1, lnkfile);

       if(!strcmp(szMagic, WPS_SHELLLINK_MAGIC)) {
           fIsShellLink = TRUE;
       }
       fclose(lnkfile);
    }
    return fIsShellLink;
}
//******************************************************************************
// OSLibWinDeleteObject
//
// Delete object with object id stored in the file
//
// Parameters:
//
//     LPSTR lpszLink        - shelllink file
//
// Returns:
//     FALSE	- failure
//     TRUE     - success
//
//******************************************************************************
BOOL WIN32API OSLibWinDeleteObject(LPSTR lpszLink)
{
    char szObjectId[CCHMAXPATH];
    HOBJECT hObject;
    int wpsobjectidsize;
    FILE *lnkfile;

    lnkfile = fopen(lpszLink, "r");
    if(lnkfile == NULL) return FALSE;

    char  szMagic[sizeof(WPS_SHELLLINK_MAGIC)];

    memset(szMagic, 0, sizeof(szMagic));
    fread(szMagic, sizeof(szMagic)-1, 1, lnkfile);

    if(!strcmp(szMagic, WPS_SHELLLINK_MAGIC))
    {
        fseek(lnkfile, 0, SEEK_END);
        wpsobjectidsize = ftell(lnkfile) - sizeof(WPS_SHELLLINK_MAGIC) + 2;
        fseek(lnkfile, sizeof(WPS_SHELLLINK_MAGIC)-1, SEEK_SET);

        memset(szObjectId, 0, sizeof(szObjectId));
        szObjectId[0] = '<';
        fread(&szObjectId[1], min(wpsobjectidsize, sizeof(szObjectId)-1), 1, lnkfile);
        strcat(szObjectId, ">");

        hObject = WinQueryObject(szObjectId);
        if(hObject) {
            WinDestroyObject(hObject);
        }
    }
    fclose(lnkfile);
    return TRUE;
}
//******************************************************************************
//******************************************************************************

} // extern "C"
