; $Id: dllinitstub.asm,v 1.1 2000-11-19 15:44:53 bird Exp $
;
;
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;

    .386

    extrn _DLL_InitTerm:far

DATA32 SEGMENT DWORD PUBLIC USE32 "DATA"
_DllEntry dd 0
DATA32 ENDS

END _DLL_InitTerm
