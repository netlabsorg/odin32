; $Id: myVR32AllocMem.asm,v 1.1 2001-01-19 02:43:26 bird Exp $
;
; VR32AllocMem over loader which adds the OBJ_ANY flag.
;
; Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Include files
;
    include devsegdf.inc



;
; Exported symbols
;
    public


;
; Externs
;
    extrn ApiApplyChange:PROC


CODE32 segment


myVR32AllocMem proc near

myVR32AllocMem endp

CODE32 ends

end
