; $Id: devlast.asm,v 1.5 2000-02-25 18:15:03 bird Exp $
;
; DevLast - the object file termintating the resident part of the objects.
; Code after the ???END labes and object files and which are linked in
; after this file is discarded after init.
;
; Copyright (c) 1999 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
       .model flat
       .386p

;
; Include files
;
    include devsegdf.inc


;
; Exported symbols
;
    public CODE16END
    public DATA16END
    public DATA16_BSSEND
    public DATA16_CONSTEND
    public CODE16END
    public CODE32END
    public DATA32END
    public BSS32END
    public CONST32_ROEND
    public _VFTEND
    public EH_DATAEND
    public _CallR0Init32
    public _CallVerifyImportTab32



;
; all segments have a <segmentname>END label at the end of the segment.
;
DATA16 segment
DATA16END db ?
DATA16 ends

DATA16_BSS segment
DATA16_BSSEND db ?
DATA16_BSS ends

DATA16_CONST segment
DATA16_CONSTEND db ?
DATA16_CONST ends


extrn R0INIT32:FAR
extrn VERIFYIMPORTTAB32:FAR

CODE16 segment
CODE16END db ?

;;
; Thunk procedure for R0Init32.
; @cproto    USHORT NEAR CallR0Init32(LIN pRpInit);
; @returns   Same as R0Init32.
; @param     pRpInit  32-bit pointer to request packet.
; @status    completely implemented.
; @author    knut st. osmundsen
_CallR0Init32 PROC NEAR
    ASSUME CS:CODE16
    push    ds
    push    word ptr [esp+6]            ; push high word.
    push    word ptr [esp+6]            ; push low word.
    call    far ptr FLAT:R0INIT32
    pop     ds
    retn
_CallR0Init32 ENDP


;;
; Thunk procedure for VerifyImportTab32.
; @cproto    USHORT NEAR CallVerifyImportTab32(void);
; @returns   Same as VerifyImportTab32.
; @status    completely implemented.
; @author    knut st. osmundsen
_CallVerifyImportTab32 PROC NEAR
    ASSUME CS:CODE16
    push    ds
    call    far ptr FLAT:VERIFYIMPORTTAB32
    pop     ds
    retn
_CallVerifyImportTab32 ENDP

CODE16 ends


CODE32 segment
CODE32END LABEL BYTE
CODE32 ends

DATA32 segment
DATA32END  LABEL BYTE
DATA32 ends

BSS32 segment
BSS32END  LABEL BYTE
BSS32 ends

CONST32_RO segment
CONST32_ROEND LABEL BYTE
CONST32_RO ends

_VFT segment
_VFTEND LABEL BYTE
_VFT ends

EH_DATA segment
EH_DATAEND LABEL BYTE
EH_DATA ends

END

