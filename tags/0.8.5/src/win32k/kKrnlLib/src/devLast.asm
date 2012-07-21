; $Id: devLast.asm,v 1.2 2002-12-16 02:24:28 bird Exp $
;
; Segment endmarkers.
;
; NOTE: This file must be put into an library to get be linked in last.
;
; Copyright (c) 1999-2002 knut st. osmundsen <bird@anduin.net>
;
;
; This file is part of kLib.
;
; kLib is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; kLib is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with kLib; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
    public DATA16END
    public DATA16_BSSEND
    public DATA16_CONSTEND
    public DATA16_GLOBALEND
    public DATA16_ENDEND
    public CODE16END
    public CODE16_ENDEND
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

DATA16_END segment
db 'DATA16_ENDEND'
DATA16_ENDEND db 0
DATA16_END ends


CODE16 segment
db 'CODE16END'
CODE16END db 0
CODE16 ends

CODE16_END segment
db 'CODE16_ENDEND'
CODE16_ENDEND db 0
CODE16_END ends

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

