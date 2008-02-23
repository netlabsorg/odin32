        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosEnterCritSec:PROC
        PUBLIC  _DosEnterCritSec
_DosEnterCritSec PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   DosEnterCritSec
    pop    fs
    ret
_DosEnterCritSec ENDP

CODE32          ENDS

                END
