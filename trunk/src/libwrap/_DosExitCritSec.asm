        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosExitCritSec:PROC
        PUBLIC  _DosExitCritSec
_DosExitCritSec PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   DosExitCritSec
    pop    fs
    ret
_DosExitCritSec ENDP

CODE32          ENDS

                END
