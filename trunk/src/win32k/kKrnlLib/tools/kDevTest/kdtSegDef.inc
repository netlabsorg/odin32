; $Id: kdtSegDef.inc,v 1.1 2002-09-30 23:53:54 bird Exp $
;
; Segment definitions for kDevTest.
;
; Copyright (c) 2002 knut st. osmundsen <bird@anduin.net>
;
;
; This file is part of kKrnlLib.
;
; kKrnlLib is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; kKrnlLib is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with kKrnlLib; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;


DATA16 segment word public 'FAR_DATA' use16
DATA16 ends

DATA16_BSS segment word public 'FAR_DATA' use16
DATA16_BSS ends

DATA16_CONST segment word public 'FAR_DATA' use16
DATA16_CONST ends

DATA16_GLOBAL segment word public 'FAR_DATA' use16
DATA16_GLOBAL ends


DATA16_INIT segment word public 'FAR_DATA' use16
DATA16_INIT ends

DATA16_INIT_CONST segment word public 'FAR_DATA' use16
DATA16_INIT_CONST ends

DATA16_INIT_BSS segment word public 'FAR_DATA' use16
DATA16_INIT_BSS ends

;
;A bad trick to make all the 16-bit data segments one
;
DATA16_GROUP    group DATA16, DATA16_CONST, DATA16_BSS, DATA16_GLOBAL, DATA16_INIT, DATA16_INIT_CONST, DATA16_INIT_BSS


CODE16 segment word public 'CODE' use16
CODE16 ends

CODE16_INIT segment word public 'CODE' use16
CODE16_INIT ends

CODE16_IOSEG segment word public 'CODE' use16
CODE16_IOSEG ends


CODE32 segment dword public 'CODE' use32
CODE32 ends

EH_CODE segment dword public 'CODE' use32
EH_CODE ends


DGROUP group BSS32, DATA32, CONST32_RO, _VFT, c_common, DDE4_DATA32, CTOR_DTOR1, CTOR_DTOR2, CTOR_DTOR3, EH_DATA

DATA32  segment dword public 'DATA' use32
DATA32 ends

c_common segment dword public 'DATA' use32
c_common ends

DDE4_DATA32  segment dword public 'DATA' use32
DDE4_DATA32 ends

BSS32 segment dword public 'BSS' use32
BSS32 ends

CTOR_DTOR1 segment dword public 'DATA' use32
CTOR_DTOR1 ends

CTOR_DTOR2 segment dword public 'DATA' use32
CTOR_DTOR2 ends

CTOR_DTOR3 segment dword public 'DATA' use32
CTOR_DTOR3 ends

;CONSTGROUP group CONST32_RO, _VFT, EH_DATA

CONST32_RO segment dword public 'CONST' use32
CONST32_RO ends

_VFT segment dword public 'CONST' use32
_VFT ends

EH_DATA segment byte public 'CONST' use32
EH_DATA ends


