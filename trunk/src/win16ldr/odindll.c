#include <windows.h>
#include <string.h>
#include "peexe.h"

DWORD FAR PASCAL MyGetVersion();
UINT FAR _loadds PASCAL MyWinExec(LPCSTR lpszCmdLine, UINT fuShowCmd);

BOOL GetPEFileHeader (LPVOID lpFile, PIMAGE_FILE_HEADER pHeader);
BOOL GetPEOptionalHeader (LPVOID lpFile, PIMAGE_OPTIONAL_HEADER pHeader);

typedef DWORD (FAR * WINAPI FUNC_GetVersion)(void);
typedef UINT (FAR *WINAPI FUNC_WinExec)(LPCSTR, UINT);

BYTE  oldcodeVER;
DWORD olddataVER;
UINT  selVER;
BYTE  oldcodeEXEC;
DWORD olddataEXEC;
UINT  selEXEC;
BYTE FAR *jumpVER;
BYTE FAR *jumpEXEC;

BOOL fUnloaded = FALSE;
BOOL fInit     = FALSE;

char szPEPath[256] = {0};

//*****************************************************************************************            
//*****************************************************************************************            
int FAR _loadds CALLBACK LibMain(HINSTANCE hinst, WORD wDataSeg, WORD cbHeap, LPSTR lpszCmdLine)
{
	FUNC_GetVersion getver;
	FUNC_WinExec winexec;
	DWORD FAR *addr;
	
    if (fInit == FALSE) {
    	fInit = TRUE;

		GetProfileString("Odin", "PEPath", "PE.EXE", szPEPath, sizeof(szPEPath));
		getver      = (FUNC_GetVersion)&GetVersion;
		selVER      = AllocSelector(0);
		PrestoChangoSelector(SELECTOROF(getver), selVER);

		jumpVER     = MAKELP(selVER, OFFSETOF(getver));
		addr        = (DWORD FAR *)(jumpVER+1);
		oldcodeVER  = *jumpVER;
		olddataVER  = *addr;
		*jumpVER    = 0xEA;	//jmp 
		*addr       = (DWORD)&MyGetVersion;

		winexec     = (FUNC_WinExec)&WinExec;	
		selEXEC     = AllocSelector(0);
		PrestoChangoSelector(SELECTOROF(winexec), selEXEC);

		jumpEXEC    = MAKELP(selEXEC, OFFSETOF(winexec));
		addr        = (DWORD FAR *)(jumpEXEC+1);
		oldcodeEXEC = *jumpEXEC;
		olddataEXEC = *addr;
		*jumpEXEC   = 0xEA;	//jmp 
		*addr       = (DWORD)&MyWinExec;
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
    }
  	return 1;
}
//*****************************************************************************************            
//*****************************************************************************************            
DWORD FAR PASCAL MyGetVersion()
{
 	return 0x00005F0C;
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
    
	of.cBytes = sizeof(OFSTRUCT);
	
	hMem1 = GlobalAlloc(GPTR, strlen(lpszCmdLine)+1);
	cmdline = GlobalLock(hMem1);
	if(cmdline == NULL) goto calloldfunc;
		
	strcpy(cmdline, lpszCmdLine);
	
	while(*cmdline == ' ') cmdline++;
	tmp = cmdline;
	while(*tmp != ' ' && *tmp != 0) tmp++;
	tmp++;
	*tmp = 0;
	
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

    addr      = (DWORD FAR *)(jumpEXEC + 1);
    *jumpEXEC = oldcodeEXEC;
    *addr     = olddataEXEC;
	
	if(fFail) {
		 ret = WinExec(lpszCmdLine, fuShowCmd);
	}
	else {
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
