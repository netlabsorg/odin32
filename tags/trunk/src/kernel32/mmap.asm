	title	mmap.cpp
	.386
	.387
	includelib CPPOM30.LIB
	includelib OS2386.LIB
CODE32	segment dword use32 public 'CODE'
CODE32	ends
DATA32	segment dword use32 public 'DATA'
DATA32	ends
CONST32_RO	segment dword use32 public 'CONST'
CONST32_RO	ends
BSS32	segment dword use32 public 'BSS'
BSS32	ends
EH_CODE	segment dword use32 public 'CODE'
EH_CODE	ends
CTOR_DTOR1	segment dword use32 public 'DATA'
CTOR_DTOR1	ends
CTOR_DTOR2	segment dword use32 public 'DATA'
CTOR_DTOR2	ends
CTOR_DTOR3	segment dword use32 public 'DATA'
CTOR_DTOR3	ends
EH_DATA	segment para use32 public 'DATA'
EH_DATA	ends
_VFT	segment para use32 public 'DATA'
_VFT	ends
DGROUP	group BSS32, DATA32
	assume	cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT
	extrn	_dllentry:proc
	extrn	WriteLog:proc
	extrn	_fltused:dword
CONST32_RO	segment
@CBE1	db "CreateFileMappingA Not I"
db "mplemented!",0ah,0h
	align 04h
@CBE2	db "CreateFileMappingA handl"
db "e %d, dwMaximumSize %X%X"
db "!",0ah,0h
	align 04h
@CBE3	db "lpszMapName %s",0ah,0h
@CBE4	db "CreateFileMappingW Not I"
db "mplemented!",0ah,0h
	align 04h
@CBE5	db "MapViewOfFileEx Not Impl"
db "emented!",0ah,0h
	align 04h
@CBE6	db "UnmapViewOfFile Not Impl"
db "emented! - TRUE",0ah,0h
	align 04h
@CBE7	db "MapViewOfFile Not Implem"
db "ented!",0ah,0h
CONST32_RO	ends
DATA32	segment
	dd	_dllentry
DATA32	ends
CODE32	segment

; 12 HANDLE WIN32API CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpsa,
	align 04h

	public _CreateFileMappingA@24
_CreateFileMappingA@24	proc
	push	ebp
	mov	ebp,esp

; 16   dprintf(("CreateFileMappingA Not Implemented!\n"));
	push	offset FLAT:@CBE1
	call	WriteLog
	add	esp,04h

; 17   dprintf(("CreateFileMappingA handle %d, dwMaximumSize %X%X!\n", hFile, dwMaximumSizeHigh, dwMaximumSizeLow));
	push	dword ptr [ebp+018h];	dwMaximumSizeLow
	push	dword ptr [ebp+014h];	dwMaximumSizeHigh
	push	dword ptr [ebp+08h];	hFile
	push	offset FLAT:@CBE2
	call	WriteLog
	add	esp,010h

; 18   if(lpszMapName){
	cmp	dword ptr [ebp+01ch],0h;	lpszMapName
	je	@BLBL3

; 19     dprintf(("lpszMapName %s\n", lpszMapName));
	push	dword ptr [ebp+01ch];	lpszMapName
	push	offset FLAT:@CBE3
	call	WriteLog
	add	esp,08h

; 20   }
@BLBL3:

; 21   return(0);
	xor	eax,eax
	leave	
	ret	018h
_CreateFileMappingA@24	endp

; 25 HANDLE WIN32API CreateFileMappingW(HANDLE hFile, LPSECURITY_ATTRIBUTES lpsa,
	align 04h

	public _CreateFileMappingW@24
_CreateFileMappingW@24	proc
	push	ebp
	mov	ebp,esp

; 29   dprintf(("CreateFileMappingW Not Implemented!\n"));
	push	offset FLAT:@CBE4
	call	WriteLog
	add	esp,04h

; 30   return(0);
	xor	eax,eax
	leave	
	ret	018h
_CreateFileMappingW@24	endp

; 34 LPVOID WIN32API MapViewOfFileEx(HANDLE hMapObject, DWORD fdwAccess,
	align 04h

	public _MapViewOfFileEx@24
_MapViewOfFileEx@24	proc
	push	ebp
	mov	ebp,esp

; 38   dprintf(("MapViewOfFileEx Not Implemented!\n"));
	push	offset FLAT:@CBE5
	call	WriteLog
	add	esp,04h

; 39   return(NULL);
	xor	eax,eax
	leave	
	ret	018h
_MapViewOfFileEx@24	endp

; 43 BOOL WIN32API UnmapViewOfFile(LPVOID lpBaseAddress)
	align 04h

	public _UnmapViewOfFile@4
_UnmapViewOfFile@4	proc
	push	ebp
	mov	ebp,esp

; 45   dprintf(("UnmapViewOfFile Not Implemented! - TRUE\n"));
	push	offset FLAT:@CBE6
	call	WriteLog
	add	esp,04h

; 46   return(TRUE);
	mov	eax,01h
	leave	
	ret	04h
_UnmapViewOfFile@4	endp

; 50 LPVOID WIN32API MapViewOfFile(HANDLE hMapObject, DWORD fdwAccess,
	align 04h

	public _MapViewOfFile@20
_MapViewOfFile@20	proc
	push	ebp
	mov	ebp,esp

; 54   dprintf(("MapViewOfFile Not Implemented!\n"));
	push	offset FLAT:@CBE7
	call	WriteLog
	add	esp,04h

; 55   return(NULL);
	xor	eax,eax
	leave	
	ret	014h
_MapViewOfFile@20	endp
CODE32	ends
end
