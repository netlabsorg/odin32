;; @file
; Global DLL data for KERNEL32.DLL.
;
; Note: sizes of data variables must be kept in sync with their C declarations!
;

.386
                NAME    globaldata

PAGE_SIZE               = 4096
MAX_HEAPSIZE            = (2048*1024)
MAX_HEAPPAGES		= (MAX_HEAPSIZE/PAGE_SIZE)

_GLOBALDATA     SEGMENT DWORD PUBLIC USE32 'DATA'

;
; initterm.cpp:
;
                PUBLIC _globLoadNr

; extern int globLoadNr; // = 0
_globLoadNr     DD 0

;
; heapshared.cpp:
;

                PUBLIC _sharedHeap
                PUBLIC _pSharedMem
                PUBLIC _pageBitmap
                PUBLIC _refCount

; extern Heap_t  sharedHeap; // = 0
_sharedHeap     DD 0
; extern PVOID   pSharedMem; // = NULL
_pSharedMem     DD 0
; extern BYTE    pageBitmap[MAX_HEAPPAGES]; // = {0}
_pageBitmap     DB MAX_HEAPPAGES DUP (0)
; extern ULONG   refCount; // = 0;
_refCount       DD 0

;
; mmap.h/mmap.cpp:
;
                PUBLIC _globalmapcritsect
                PUBLIC __ZN11Win32MemMap7memmapsE

; extern CRITICAL_SECTION_OS2 globalmapcritsect; // = {0}
_globalmapcritsect          DD 6 DUP (0)
; /*static*/ Win32MemMap *Win32MemMap::memmaps; // = NULL
__ZN11Win32MemMap7memmapsE  DD 0

_GLOBALDATA     ENDS

                END

