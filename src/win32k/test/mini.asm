; $Id: mini.asm,v 1.1.2.11 2001-08-16 15:14:40 bird Exp $
;
; Haveing great fun making small executables...
;
; Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
;
; Project Odin Software License can be found in LICENSE.TXT
;


ifdef NORMAL  ;
;
;
;
; High-octane stock.
;
;
;
    .386

; config
CLIB EQU 1

ifdef CLIB
extrn vprintf:PROC                      ; optlink, 2+ parameters. second NULL.
else
extrn DosPutMessage:PROC                ; system, tree parameters.
endif


CODE32 segment byte stack use32 'STACK'
public minilx

;
; Data
;
ImReallySmall   db  "I'm really small!",0ah
ifdef CLIB
terminator db 0
endif

;
; Code
;
minilx:
ifdef CLIB
    ; method 1 - 4 bytes
    ;inc     eax
    ;shl     eax,16
    ; method 2 - 4 bytes
    ;bts     eax, 16
    ; method 3 - 4 bytes
    ;inc     ah
    ;mul     eax
    ; method 4 - 3 bytes YEAH!!!
    dec     ax
    inc     eax
    jmp     vprintf
else
    push    offset ImReallySmall
    push    18
    push    eax
    call    DosPutMessage
    add     esp, 12
    ret
endif
db 1000h-$ dup(?)
CODE32 ends


END minilx
endif



ifndef NORMAL
;
;
;
; Tiny edition - everything but the code is hardcoded!
;
;
;
    .386


ALL segment byte public

;
; Constants
;
impmod              EQU restab
;data                EQU ImReallySmall


;
; The LX header
;
            ;struct e32_exe                          /* New 32-bit .EXE header */
            ;{
lxhdr db 'LX';    unsigned char       e32_magic[2];   /* Magic number E32_MAGIC */
db  0       ;    unsigned char       e32_border;     /* The byte ordering for the .EXE */
db  0       ;    unsigned char       e32_worder;     /* The word ordering for the .EXE */
dd  0       ;    unsigned long       e32_level;      /* The EXE format level for now = 0 */
dw  2       ;    unsigned short      e32_cpu;        /* The CPU type */
dw  1       ;    unsigned short      e32_os;         /* The OS type */

; We can place the object table here I hope..
;   mpages is addjusted to hold the object flags.
;   reserved is 1 which is no problem it seems.
;
; Object table with one entry.
; Defines a
;
;dbg0 db 'objtab'    ;struct o32_obj                          /* Flat .EXE object table entry */
                    ;{
;objtab  dd 11000h   ;    unsigned long       o32_size;       /* Object virtual size */
;        dd 10000h   ;    unsigned long       o32_base;       /* Object base virtual address */
;                    ;    unsigned long       o32_flags;      /* Attribute flags */
;        dd (1h OR 2h OR 2000h OR 10h) ;=2013h
;        dd 1        ;    unsigned long       o32_pagemap;    /* Object page map index */
;        dd 1        ;    unsigned long       o32_mapsize;    /* Number of entries in object page map */
;        dd 0        ;    unsigned long       o32_reserved;   /* Reserved */
                     ;};
objtab:
dd  20000h  ;    unsigned long       e32_ver;        /* Module version */
            ;    unsigned long       e32_mflags;     /* Module flags */
dd  200h OR 00h
dd  2013h   ;    unsigned long       e32_mpages;     /* Module # pages */
dd  1       ;    unsigned long       e32_startobj;   /* Object # for instruction pointer */
dd  1       ;    unsigned long       e32_eip;        /* Extended instruction pointer */
dd  1       ;    unsigned long       e32_stackobj;   /* Object # for stack pointer */
dd  1000h   ;    unsigned long       e32_esp;        /* Extended stack pointer */
dd  1000h   ;    unsigned long       e32_pagesize;   /* .EXE page size */
dd  0       ;    unsigned long       e32_pageshift;  /* Page alignment shift in .EXE */
            ;    unsigned long       e32_fixupsize;  /* Fixup section size */
dd  offset fixrecend - offset fixpagetab ; allways used together with _ldrsize.

dd  0       ;    unsigned long       e32_fixupsum;   /* Fixup section checksum */
            ;    unsigned long       e32_ldrsize;    /* Loader section size */
dd  offset data - offset objtab
dd  0       ;    unsigned long       e32_ldrsum;     /* Loader section checksum */
            ;    unsigned long       e32_objtab;     /* Object table offset */
dd  offset objtab
dd  1       ;    unsigned long       e32_objcnt;     /* Number of objects in module */
            ;    unsigned long       e32_objmap;     /* Object page map offset */
dd  offset pagetab
            ;    unsigned long       e32_itermap;    /* Object iterated data map offset */  off=4ch
dd  offset data
dd  0       ;    unsigned long       e32_rsrctab;    /* Offset of Resource Table */
dd  0       ;    unsigned long       e32_rsrccnt;    /* Number of resource entries */
            ;    unsigned long       e32_restab;     /* Offset of resident name table */
dd  offset restab
dd  0       ;    unsigned long       e32_enttab;     /* Offset of Entry Table */
dd  0       ;    unsigned long       e32_dirtab;     /* Offset of Module Directive Table */                             available
dd  0       ;    unsigned long       e32_dircnt;     /* Number of module directives */                                  available
            ;    unsigned long       e32_fpagetab;   /* Offset of Fixup Page Table */
dd  offset fixpagetab
            ;    unsigned long       e32_frectab;    /* Offset of Fixup Record Table */
dd  offset fixrec
            ;    unsigned long       e32_impmod;     /* Offset of Import Module Name Table */
dd  offset impmod
dd  1       ;    unsigned long       e32_impmodcnt;  /* Number of entries in Import Module Name Table */
dd  0       ;    unsigned long       e32_impproc;    /* Offset of Import Procedure Name Table */ off = 78
;dd  offset impproc
dd  0       ;    unsigned long       e32_pagesum;    /* Offset of Per-Page Checksum Table */                            available
            ;    unsigned long       e32_datapage;   /* Offset of Enumerated Data Pages */
dd  offset data
; redefine these entries as obj tab stuff.
;red dd  0       ;    unsigned long       e32_preload;    /* Number of preload pages */                                      available!
;red dd  0       ;    unsigned long       e32_nrestab;    /* Offset of Non-resident Names Table */                           available?
;red dd  0       ;    unsigned long       e32_cbnrestab;  /* Size of Non-resident Name Table */                              available?
;red dd  0       ;    unsigned long       e32_nressum;    /* Non-resident Name Table Checksum */                             available?
;red dd  0       ;    unsigned long       e32_autodata;   /* Object # for automatic data object */                           available?
;red dd  0       ;    unsigned long       e32_debuginfo;  /* Offset of the debugging information */                          available?

;
;red dd  0       ;    unsigned long       e32_debuglen;   /* The length of the debugging info. in bytes */                   available?
;red dd  0       ;    unsigned long       e32_instpreload;/* Number of instance pages in preload section of .EXE file */     available?
;red dd  0       ;    unsigned long       e32_instdemand; /* Number of instance pages in demand load section of .EXE file */ available?
;red dd  0       ;    unsigned long       e32_heapsize;   /* Size of heap - for 16-bit apps */                               available?

;mv dd  1000h-32;    unsigned long       e32_stacksize;  /* Size of stack */

;dd 0,0,0,0

;
; fixup table.
;
;dbg2 db 'fixpagetab'
fixpagetab  dd 0                                ; offset of fixups for page 1.
;ntx high 0!dd (offset fixrecend - offset fixrec) ; end of fixup table - needed?
            dw (offset fixrecend - offset fixrec) ; end of fixup table - needed?

;
; Page entry for our one page.
;
;dbg1 db 'pagetab'
pagetab     dd  0                                   ; offset 0
            dw  offset dataend - offset data        ; data size,
            dw  0                                   ; flags. valid

;dw 0
;dd  1000h-36;     unsigned long       e32_stacksize;  /* Size of stack */
;dd  20 dup(0);   unsigned char       e32_res3[E32RESBYTES3];                                                            available!
            ;                                        /* Pad structure to 196 bytes */
            ;  };


;
; The fixup records.
;
; Make one single fixup record which imports the DosPutMessage API.
; That api have ordinal 5
; 32-bit offset fixup
;
;dbg3 db 'fixrec'
            ;struct r32_rlc                          /* Relocation item */
            ;{
fixrec db 08h ;  unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
            ;    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
db  (1h OR 80h)   ; imp. ord.; 8 bit ordinal
            ;    short               r32_soff;       /* Source offset */
dw  (offset fixup - offset data)
db  1h      ;    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
            ;
            ;    union targetid
            ;    {
            ;        offset             intref;      /* Internal fixup */
            ;
            ;        union extfixup
            ;        {
            ;            offset         proc;        /* Procedure name offset */
db 5h       ;            unsigned long  ord;         /* Procedure odrinal */
            ;        }
            ;                           extref;      /* External fixup */
            ;
            ;        struct addfixup
            ;        {
            ;            unsigned short entry;       /* Entry ordinal */
            ;            offset         addval;      /* Value added to the address */
            ;        }
            ;                           addfix;      /* Additive fixup */
            ;    }
            ;                        r32_target;     /* Target data */
            ;    unsigned short      r32_srccount;   /* Number of chained fixup records */
            ;    unsigned short      r32_chain;      /* Chain head */
            ;};
fixrecend:


;
; Imports the MSG dll.
;
restab      db 3,'MSG' ; now e32_stacksize
impproc:    db 0




assume CS:ALL
data:
    int 3
minilx:
;    int 3
    push    offset ImReallySmall - offset data + 10000h
    push    18
    push    eax                         ; 0 or 1 is just the same.
    ;call    DosPutMessage
    db 0e8h
fixup dd 0h
    add     esp, 12
    ret
ImReallySmall   db  "I'm really small!",0ah

db 0,0,0
;lxdump db 'lxdumplxdumplxdumplxdump'
;       db 'lxdumplxdumplxdumplxdump'
;       db 'lxdumplxdumplxdumplxdump'
dataend:

ALL ENDS

END
endif


