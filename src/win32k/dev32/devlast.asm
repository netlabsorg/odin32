; $Id: devlast.asm,v 1.1 1999-09-06 02:19:56 bird Exp $
; DevLast - the last object file which is resident all the time.
; Object files which are linked in after this is discarded after init.
;
; Copyright (c) 1999 knut st. osmundsen
;
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
    public _CallR0Init32




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

CODE16 segment
    ASSUME CS:CODE16
CODE16END db ?


_CallR0Init32 PROC NEAR
    push    ds
    push    word ptr [esp+4]
    push    word ptr [esp+8]
    call    far ptr FLAT:R0INIT32
    pop     ds
    retn
_CallR0Init32 ENDP
CODE16 ends

CODE32 segment
CODE32END db ?
CODE32 ends

DATA32 segment
DATA32END  db ?
DATA32 ends

BSS32 segment
BSS32END  LABEL BYTE
BSS32 ends

CONST32_RO segment
CONST32_ROEND db ?
CONST32_RO ends
END

