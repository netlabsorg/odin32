; $Id: DosA.asm,v 1.1 2001-09-17 01:41:12 bird Exp $
;
; 16-bits Dos calls overloader
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386


;
; Include files
;
    include devsegdf.inc
    include os2.inc
    include devsym.inc


;
; Exported symbols
;
    public  DOS16OPEN
    public  DOS16CLOSE
    public  DOS16DEVIOCTL


;
; extrns
;
    extrn   _strategyAsm0:FAR
    extrn   _DOS16OPEN:FAR     ;70
    extrn   _DOS16CLOSE:FAR    ;59

    extrn   ThunkStack16To32:PROC
    extrn   ThunkStack32To16:PROC
    extrn   TKSSBase:PROC

    extrn   _TKSSBase16:DWORD
    extrn   _R0FlatCS16:WORD
    extrn   _R0FlatDS16:WORD

DATA16 SEGMENT
DATA16 ENDS



CODE16 SEGMENT
    ASSUME cs:CODE16, ds:DATA16

hkKrnlHlp  EQU     0fE1Fh               ; Value of the kKrnlHlp handle

;;
; Fake opening of kKrnlHlp
;
; PASCAL convention: Parameters pushed left to right of C prototype.
;                    We'll have to do the cleanup.
;
; @cproto APIRET  APIENTRY  DosOpen(
;                               PSZ     pszFname,
;                               PHFILE  phfOpen,
;                               PUSHORT pusAction,
;                               ULONG   ulFSize,
;                               USHORT  usAttr,
;                               USHORT  fsOpenFlags,
;                               USHORT  fsOpenMode,
;                               ULONG   ulReserved);
;
; @returns
; @param     pszFname       bp +1c  dd
; @param     phfOpen        bp +18  dd
; @param     pusAction      bp +14  dd
; @param     ulFSize        bp +10  dd
; @param     usAttr         bp + e  dw
; @param     fsOpenFlags    bp + c  dw
; @param     fsOpenMode     bp + a  dw
; @param     ulReserved     bp + 6  dd
;
; @uses
; @equiv
; @time
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
DOS16OPEN PROC FAR
    ASSUME CS:CODE16, DS:NOTHING, ES:NOTHING, SS:NOTHING
    push    bp
    mov     bp, sp

    ;
    ; Is this an attempt to open the kKrnlHlp device driver?
    ;
    push    ds
    push    bx
    xor     ebx, ebx
    lds     bx, [bp + 1ch]
    mov     eax, ds:[ebx]
    cmp     eax, 'ved\'                 ;'\dev' assumes low-case!
    jne     do_notkKrnlHlp
    mov     eax, ds:[ebx+4]
    cmp     eax, 'rKk\'                 ;'\kKr'
    jne     do_notkKrnlHlp
    mov     eax, ds:[ebx+8]
    cmp     eax, 'lHln'                 ; 'nlHl'
    jnz     do_notkKrnlHlp
    mov     eax, ds:[ebx+8]
    cmp     ax, 'p'                     ; 'p\0'
    jnz     do_notkKrnlHlp

    ;
    ; found filename string equal to "\dev\kKrnlHlp".
    ;
    ; return phFile equal to kKrnlHlp handle and pusAction set to 1 (FILE EXISTED).
    ;
    lds     bx, [bp + 18h]
    mov     word ptr ds:[bx], hkKrnlHlp ; *phFile <- hkKrnlHlp
    lds     bx, [bp + 14h]
    mov     word ptr ds:[bx], 1         ; *pusAction <- File existed.

do_ret:
    ;
    ; restore bx and ds and return successfully.
    ;
    pop     bx
    pop     ds
    xor     eax, eax
    leave
    ret     01ah

do_notkKrnlHlp:
    push    dword ptr [bp +01ch]
    push    dword ptr [bp +018h]
    push    dword ptr [bp +014h]
    push    dword ptr [bp +010h]
    push    word  ptr [bp + 0eh]
    push    word  ptr [bp + 0ch]
    push    word  ptr [bp + 0ah]
    push    dword ptr [bp + 06h]
    call far ptr _DOS16OPEN
    pop     bx
    pop     ds
    leave
    ret     01ah
DOS16OPEN ENDP


;;
; Fake close of kKrnlHlp
; @cproto    APIRET  APIENTRY DosClose(HFILE hf);
; @returns   OS/2 return code. (NO_ERROR)
; @param     hFile  bp + 6
; @uses      EAX
; @status    completly implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
DOS16CLOSE PROC FAR
    ASSUME CS:CODE16, DS:NOTHING, ES:NOTHING, SS:NOTHING
    push    bp
    mov     bp, sp

    ;
    ; Is it the kKrnlHlp handle?
    ;
    mov     ax, [bp + 6]
    cmp     ax, hkKrnlHlp
    jne     dc_notkKrnlHlp
    xor     eax, eax
    leave
    ret     2

dc_notkKrnlHlp:
    push    word ptr [bp + 6]           ; can't jump to higher privilege
    call    _DOS16CLOSE
    leave
    ret     2
DOS16CLOSE ENDP



;;
;
; @cproto    APIRET  APIENTRY   DosDevIOCtl(
;                                   PVOID   pData,
;                                   PVOID   pParms,
;                                   USHORT  usFun,
;                                   USHORT  usCategory,
;                                   HFILE   hDev);
; @returns
; @param     pData       bp +10 dd
; @param     pParms      bp + c dd
; @param     usFun       bp + a dw
; @param     usCategory  bp + 8 dw
; @param     hDev        bp + 6 dw
; @param
; @uses      eax
; @sketch
; @status    completly implemented
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
DOS16DEVIOCTL PROC FAR
    ASSUME CS:CODE16, DS:NOTHING, ES:NOTHING, SS:NOTHING
    LOCAL   pkt:Packet

    push    bx
    push    es


    ;
    ; Check that it really is the kKrnlHlp handle.
    ;
    mov     ax, word ptr [bp + 6]
    cmp     word ptr [bp + 6], hkKrnlHlp
    jne     ddioc_invalidhandle

    ;
    ; Then we'll have to call _strategyAsm0. But first will have to
    ; build a request packet for this IOCtl
    ;
    mov     pkt.PktLen, LENGenIOCTL     ; PktLen          db      ?               ; length in bytes of packet
    mov     pkt.PktUnit, 0              ; PktUnit         db      ?               ; subunit number of block device
    mov     pkt.PktCmd, CMDGenIOCTL     ; PktCmd          db      ?               ; command code
    mov     pkt.PktStatus, 0            ; PktStatus       dw      ?               ; status word
    mov     pkt.PktFlag, 0              ; PktFlag         db      ?               ; disk driver internal flags
    mov     pkt.PktFlag+1, 0            ;                 db      3 dup(?)        ; reserved
    mov     pkt.PktFlag+2, 0            ;
    mov     pkt.PktFlag+3, 0            ;
    mov     pkt.PktDevLink, 0           ; PktDevLink      dd      ?               ; device multiple-request link
    mov     ax, [bp + 8]
    mov     byte  ptr pkt.PktData + 0, al   ; Category Code
    mov     ax, [bp + 0ah]
    mov     byte ptr pkt.PktData + 1, al    ; Function code
    mov     eax, [bp + 0ch]
    mov     dword ptr pkt.PktData + 2, eax  ; pointer to parameter packet
    mov     eax, [bp + 10h]
    mov     dword ptr pkt.PktData + 6, eax  ; pointer to data packet
    mov     word ptr  pkt.PktData + 10, 0   ; SFN of the driver.

    ; do the call - who cleans up the stack?
    mov     bx, ss
    mov     es, bx
    lea     bx, pkt
    ;call    far ptr _strategyAsm0      ; why did this make it into a 16:32 call?
    call    far ptr CODE16:_strategyAsm0

    ;
    ; We'll check if the status to see wether it failed or not.
    ;
    test    pkt.PktStatus, STERR
    jnz     ddioc_error
    test    pkt.PktStatus, STDON
    jz      ddioc_notdone
    test    pkt.PktStatus, STINTER
    jnz     ddioc_interchar
    test    pkt.PktStatus, STBUI
    jnz     ddioc_busy
    xor     eax, eax                    ; return NO_ERROR
    jmp ddioc_ret

ddioc_error:
    mov     ax, pkt.PktStatus
    and     ax, STECODE
    jmp ddioc_ret

ddioc_notdone:
    mov     eax, 31                     ; ERROR_GEN_FAILURE
    jmp ddioc_ret

ddioc_interchar:
    mov     eax, 31                     ; ERROR_GEN_FAILURE
    jmp ddioc_ret

ddioc_busy:
    mov     eax, 31                     ; ERROR_GEN_FAILURE
    jmp ddioc_ret

ddioc_invalidhandle:
    mov     eax, 6                      ; ERROR_INVALID_HANDLE
    jmp     ddioc_ret

ddioc_ret:
    pop     es
    pop     bx
    leave
    ret     0eh
DOS16DEVIOCTL ENDP



CODE16 ENDS

END


