#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "peexe.h"

DWORD FAR PASCAL MyGetVersion();
UINT FAR _loadds PASCAL MyWinExec(LPCSTR lpszCmdLine, UINT fuShowCmd);

BOOL    FAR _loadds PASCAL MyPostMessage(HWND, UINT, WPARAM, LPARAM);
LRESULT FAR _loadds PASCAL MySendMessage(HWND, UINT, WPARAM, LPARAM);

BOOL GetPEFileHeader (LPVOID lpFile, PIMAGE_FILE_HEADER pHeader);
BOOL GetPEOptionalHeader (LPVOID lpFile, PIMAGE_OPTIONAL_HEADER pHeader);

typedef DWORD (FAR * PASCAL FUNC_GetVersion)(void);
typedef UINT  (FAR * PASCAL FUNC_WinExec)(LPCSTR, UINT);
#ifdef PATCH_POSTSENDMSG
typedef BOOL    (FAR * PASCAL FUNC_PostMessage)(HWND, UINT, WPARAM, LPARAM);
typedef LRESULT (FAR * PASCAL FUNC_SendMessage)(HWND, UINT, WPARAM, LPARAM);
#endif

BYTE  oldcodeEXEC;
DWORD olddataEXEC;
UINT  selEXEC;
BYTE FAR *jumpEXEC;

BYTE  oldcodeVER;
DWORD olddataVER;
UINT  selVER;
BYTE FAR *jumpVER;

#ifdef PATCH_POSTSENDMSG
BYTE  oldcodeSEND;
DWORD olddataSEND;
UINT  selSEND;
BYTE FAR *jumpSEND;

BYTE  oldcodePOST;
DWORD olddataPOST;
UINT  selPOST;
BYTE FAR *jumpPOST;
#endif

BOOL fUnloaded = FALSE;
BOOL fInit     = FALSE;

char szPEPath[256] = {0};

FUNC_GetVersion orggetver   = NULL;
FUNC_WinExec    orgwinexec  = NULL;
#ifdef PATCH_POSTSENDMSG
FUNC_SendMessage orgsendmsg = NULL;
FUNC_PostMessage orgpostmsg = NULL;
#endif

//*****************************************************************************************
//*****************************************************************************************
int FAR _loadds CALLBACK LibMain(HINSTANCE hinst, WORD wDataSeg, WORD cbHeap, LPSTR lpszCmdLine)
{
	DWORD FAR *addr;
	
    if(fInit == FALSE) 
    {
    	fInit = TRUE;

		GetProfileString("PELDR", "LdrPath", "PE.EXE", szPEPath, sizeof(szPEPath));
		
		orggetver   = (FUNC_GetVersion)GetVersion;
		selVER      = AllocSelector(0);
		PrestoChangoSelector(SELECTOROF(orggetver), selVER);

		jumpVER     = MAKELP(selVER, OFFSETOF(orggetver));
		addr        = (DWORD FAR *)(jumpVER+1);
		oldcodeVER  = *jumpVER;
		olddataVER  = *addr;
		*jumpVER    = 0xEA;	//jmp
		*addr       = (DWORD)MyGetVersion;

		orgwinexec  = (FUNC_WinExec)WinExec;	
		selEXEC     = AllocSelector(0);
		PrestoChangoSelector(SELECTOROF(orgwinexec), selEXEC);

		jumpEXEC    = MAKELP(selEXEC, OFFSETOF(orgwinexec));
		addr        = (DWORD FAR *)(jumpEXEC+1);
		oldcodeEXEC = *jumpEXEC;
		olddataEXEC = *addr;
		*jumpEXEC   = 0xEA;	//jmp
		*addr       = (DWORD)MyWinExec;

#ifdef PATCH_POSTSENDMSG
		orgsendmsg  = (FUNC_SendMessage)SendMessage;	
		selSEND     = AllocSelector(0);
		PrestoChangoSelector(SELECTOROF(orgsendmsg), selSEND);

		jumpSEND    = MAKELP(selSEND, OFFSETOF(orgsendmsg));
		addr        = (DWORD FAR *)(jumpSEND+1);
		oldcodeSEND = *jumpSEND;
		olddataSEND = *addr;
		*jumpSEND   = 0xEA;	//jmp
		*addr       = (DWORD)MySendMessage;

		orgpostmsg  = (FUNC_PostMessage)PostMessage;	
		selPOST     = AllocSelector(0);
		PrestoChangoSelector(SELECTOROF(orgpostmsg), selPOST);

		jumpPOST    = MAKELP(selPOST, OFFSETOF(orgpostmsg));
		addr        = (DWORD FAR *)(jumpPOST+1);
		oldcodePOST = *jumpPOST;
		olddataPOST = *addr;
		*jumpPOST   = 0xEA;	//jmp
		*addr       = (DWORD)MyPostMessage;
#endif		
    }
    return 1;
}
//*****************************************************************************************
//*****************************************************************************************
int FAR _loadds PASCAL WEP(int entry)
{
	DWORD FAR *addr;
	
    /* Your WEP functionality goes here */
    if(!fUnloaded) {
    	fUnloaded = TRUE;

	    //restore entrypoints
	    addr      = (DWORD FAR *)(jumpVER + 1);
	    *jumpVER  = oldcodeVER;
	    *addr     = olddataVER;

	    addr      = (DWORD FAR *)(jumpEXEC + 1);
	    *jumpEXEC = oldcodeEXEC;
	    *addr     = olddataEXEC;

#ifdef PATCH_POSTSENDMSG
    	addr        = (DWORD FAR *)(jumpSEND + 1);
    	*jumpSEND   = oldcodeSEND;
    	*addr       = olddataSEND;
                            
        addr        = (DWORD FAR *)(jumpPOST + 1);
        *jumpPOST   = oldcodePOST;
        *addr       = olddataPOST;
#endif        
    }
  	return 1;
}
//*****************************************************************************************
//*****************************************************************************************
DWORD FAR PASCAL MyGetVersion()
{
 	return 0x00005F0C;
}
#ifdef PATCH_POSTSENDMSG
//*****************************************************************************************
//*****************************************************************************************
BOOL FAR _loadds PASCAL MyPostMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    DWORD FAR *addr;
    BOOL       ret;
    char       buffer[64];

    addr        = (DWORD FAR *)(jumpPOST + 1);
    *jumpPOST   = oldcodePOST;
    *addr       = olddataPOST;

#ifdef DEBUG
    if(hwnd == HWND_BROADCAST) {
        sprintf(buffer, "Broadcast %x %lx %lx", Msg, wParam, lParam);
        MessageBox(0, "PostMessage", buffer, MB_OK);
    }
#endif
    ret = PostMessage(hwnd, Msg, wParam, lParam);

    *jumpPOST   = 0xEA;	//jmp
    *addr       = (DWORD)&MyPostMessage;

    return ret;
}
//*****************************************************************************************
//*****************************************************************************************
LRESULT FAR _loadds PASCAL MySendMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    DWORD FAR *addr;
    BOOL       ret;
    char       buffer[64];

    addr        = (DWORD FAR *)(jumpSEND + 1);
    *jumpSEND   = oldcodeSEND;
    *addr       = olddataSEND;

#ifdef DEBUG
    if(hwnd == HWND_BROADCAST) {
        sprintf(buffer, "Broadcast %x %lx %lx", Msg, wParam, lParam);
        MessageBox(0, "SendMessage", buffer, MB_OK);
    }
#endif
    ret = SendMessage(hwnd, Msg, wParam, lParam);

    *jumpSEND   = 0xEA;	//jmp
    *addr       = (DWORD)&MySendMessage;

    return ret;
}                       
#endif 
//*****************************************************************************************
//*****************************************************************************************
void FAR _loadds PASCAL SetPELdr(LPCSTR lpszPELdr)
{                        
	strcpy(szPEPath, lpszPELdr);
}
//*****************************************************************************************
//*****************************************************************************************
UINT FAR _loadds PASCAL MyWinExec(LPCSTR lpszCmdLine, UINT fuShowCmd)
{
	LPSTR cmdline, tmp;
	UINT ret;
	OFSTRUCT of = {0};
	HFILE hFile = 0;
	int hdrsize;
	IMAGE_DOS_HEADER doshdr;
 	IMAGE_FILE_HEADER     fh;
 	IMAGE_OPTIONAL_HEADER oh;
	DWORD FAR *addr;
	UINT bytesRead;
	char FAR *header;
	HGLOBAL hMem1 = 0, hMem2 = 0, hMem3 = 0, hMem4 = 0;
	BOOL fFail = TRUE;

    if(szPEPath[0] == 0) goto calloldfunc;
    
	of.cBytes = sizeof(OFSTRUCT);
	
	hMem1 = GlobalAlloc(GPTR, strlen(lpszCmdLine)+1);
	cmdline = GlobalLock(hMem1);
	if(cmdline == NULL) goto calloldfunc;
		
	strcpy(cmdline, lpszCmdLine);
	
	//isolate path of executable	
	while(*cmdline == ' ') cmdline++;
	tmp = cmdline;
	while(*tmp != ' ' && *tmp != 0) tmp++;
	*tmp = 0;
	
	//open it to check the executable type; we only care about PE (win32) executables 
	//we'll let the original WinExec function handle everything else
	hFile = OpenFile(cmdline, &of, OF_READ);
  	bytesRead = _lread(hFile, &doshdr, sizeof(doshdr));
	if(bytesRead != sizeof(doshdr)) 			goto calloldfunc;
  	if(doshdr.e_magic != IMAGE_DOS_SIGNATURE) 	goto calloldfunc;
  	hdrsize = doshdr.e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER);

	_llseek(hFile, 0, 0);
	hMem3 = GlobalAlloc(GPTR, hdrsize);
	header = GlobalLock(hMem3);
	if(header == NULL) goto calloldfunc;
	
	bytesRead = _lread(hFile, header, hdrsize);
	if(bytesRead != hdrsize) {
		goto calloldfunc;
	}	
  	if(GetPEFileHeader (header, &fh) == FALSE) {
		goto calloldfunc;
  	}
  	if(GetPEOptionalHeader (header, &oh) == FALSE) {
		goto calloldfunc;
  	}
	
  	if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
		goto calloldfunc;
  	}
  	if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
        goto calloldfunc;
  	}
  	//IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  	if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
        goto calloldfunc;
  	}

    //Ok, it's a PE executable. Use the PE loader to launch it
	tmp = cmdline;
	hMem4 = hMem1;
	
	hMem1 = GlobalAlloc(GPTR, strlen(tmp)+16+strlen(szPEPath));
	cmdline = GlobalLock(hMem1);
	if(cmdline == NULL) {
		goto calloldfunc;
	}

	strcpy(cmdline, szPEPath);
	strcat(cmdline, " ");
	strcat(cmdline, tmp);

	fFail = FALSE;
	
calloldfunc:			
	if(hMem4) {
		GlobalUnlock(hMem4);
		GlobalFree(hMem4);
	}		
	if(hMem3) {
		GlobalUnlock(hMem3);
		GlobalFree(hMem3);
	}		
	if(hMem2) {
		GlobalUnlock(hMem2);
		GlobalFree(hMem2);
	}		
	if(hFile)	
		_lclose(hFile);

    //restore original WinExec entrypoint
    addr      = (DWORD FAR *)(jumpEXEC + 1);
    *jumpEXEC = oldcodeEXEC;
    *addr     = olddataEXEC;
	
	//and call it	
	if(fFail) {
		//non-PE executable
		 ret = WinExec(lpszCmdLine, fuShowCmd);
	}
	else {
		//PE executable
		ret = WinExec(cmdline, fuShowCmd);
		if(ret >= 32) {
			DWORD tickcount1, tickcount2;
			
			//give PE a chance to load the exe file
			tickcount1 = GetTickCount();
			do {
				Yield();
				tickcount2 = GetTickCount();
			}
			while(tickcount2 - tickcount1 < 2000UL);
		}
	}
	
	//put back our WinExec override	                   
	*jumpEXEC   = 0xEA;	//jmp
	*addr       = (DWORD)&MyWinExec;
	
	if(hMem1) {	
		GlobalUnlock(hMem1);
		GlobalFree(hMem1);
	}
	return ret;
}
//******************************************************************************
//******************************************************************************
BOOL GetPEFileHeader (LPVOID lpFile, PIMAGE_FILE_HEADER pHeader)
{
    if(*(WORD *)lpFile == IMAGE_DOS_SIGNATURE &&
       *(DWORD *)PE_HEADER (lpFile) == IMAGE_NT_SIGNATURE)
    {
	 	 memcpy ((LPVOID)pHeader, PEHEADEROFF (lpFile), sizeof (IMAGE_FILE_HEADER));
	 	 return TRUE;
    }
    else return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL GetPEOptionalHeader (LPVOID lpFile, PIMAGE_OPTIONAL_HEADER pHeader)
{
    if(*(WORD *)lpFile == IMAGE_DOS_SIGNATURE &&
       *(DWORD *)PE_HEADER (lpFile) == IMAGE_NT_SIGNATURE)
    {
	 	 memcpy ((LPVOID)pHeader, OPTHEADEROFF (lpFile), sizeof (IMAGE_OPTIONAL_HEADER));
	 	 return TRUE;
    }
    else return FALSE;
}
//*****************************************************************************************
//*****************************************************************************************
