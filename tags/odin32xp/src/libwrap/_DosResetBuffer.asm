        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosResetBuffer:PROC
        PUBLIC  _DosResetBuffer
_DosResetBuffer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosResetBuffer
    add    esp, 4
    pop    fs
    ret
_DosResetBuffer ENDP

CODE32          ENDS

                END
