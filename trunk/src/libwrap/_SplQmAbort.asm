        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQmAbort:PROC
        PUBLIC  _SplQmAbort
_SplQmAbort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   SplQmAbort
    add    esp, 4
    pop    fs
    ret
_SplQmAbort ENDP

CODE32          ENDS

                END
