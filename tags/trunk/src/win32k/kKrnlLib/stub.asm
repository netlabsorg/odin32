; $Id: stub.asm,v 1.1 2002-03-10 05:22:32 bird Exp $
;
; stub - make sure everything is included.
;
; Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p


;
; Include files
;
ifdef KKRNLLIB
    include devsegdf.inc
endif


;
; extrns
;
ifdef KKRNLLIB
    extrn _Device_Help:dword
    extrn callTab:dword                 ; dev32/calltaba.asm
    extrn _dev0Init:far                 ; dev16/d16init.c
    extrn aKrnlSymDB32:dword            ; dev32/SymDB32.asm
    extrn _aDevHdrs:dword               ; dev16/d16strat.c
endif
ifdef EXTRACT
    extrn _main:far
endif


CODE32 segment
    assume cs:CODE32, ds:flat, ss:nothing, es:nothing
    .386p

CODE32 ends


END
