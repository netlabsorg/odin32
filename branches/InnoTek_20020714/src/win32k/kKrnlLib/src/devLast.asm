; $Id: devLast.asm,v 1.1 2002-03-10 02:45:54 bird Exp $
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
    public DATA16END
    public DATA16_BSSEND
    public DATA16_CONSTEND
    public DATA16_GLOBALEND
    public DATA16_INITEND
    public DATA16_INIT_BSSEND
    public DATA16_INIT_CONSTEND
    public CODE16END
    public CODE16_INITEND
    public CODE32END
    public DATA32END
    public BSS32END
    public CONST32_ROEND
    public _VFTEND
    public EH_DATAEND



;
; all segments have a <segmentname>END label at the end of the segment.
;
DATA16 segment
db 'DATA16END'
DATA16END db 0
DATA16 ends

DATA16_BSS segment
db 'DATA16_BSSEND'
DATA16_BSSEND db 0
DATA16_BSS ends

DATA16_CONST segment
db 'DATA16_CONSTEND'
DATA16_CONSTEND db 0
DATA16_CONST ends

DATA16_GLOBAL segment
db 'DATA16_GLOBALEND'
DATA16_GLOBALEND db 0
DATA16_GLOBAL ends

DATA16_INIT segment
db 'DATA16_INITEND'
DATA16_INITEND db 0
DATA16_INIT ends

DATA16_INIT_BSS segment
db 'DATA16_INIT_BSSEND'
DATA16_INIT_BSSEND db 0
DATA16_INIT_BSS ends

DATA16_INIT_CONST segment
db 'DATA16_CONSTEND'
DATA16_INIT_CONSTEND db 0
DATA16_INIT_CONST ends

CODE16 segment
db 'CODE16END'
CODE16END db 0
CODE16 ends

CODE16_INIT segment
db 'CODE16_INITEND'
CODE16_INITEND db 0
CODE16_INIT ends


CODE32 segment
db 'CODE32END'
CODE32END db 0
CODE32 ends

DATA32 segment
db 'DATA32END'
DATA32END  db 0
DATA32 ends

BSS32 segment
db 'BSS32END'
BSS32END  db 0
BSS32 ends

CONST32_RO segment
db 'CONST32_ROEND'
CONST32_ROEND db 0
CONST32_RO ends

_VFT segment
db '_VFTEND'
_VFTEND db 0
_VFT ends

EH_DATA segment
db 'EH_DATAEND'
EH_DATAEND db 0
EH_DATA ends


END

