        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplStdStart:PROC
        PUBLIC  _SplStdStart
_SplStdStart PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   SplStdStart
    add    esp, 4
    pop    fs
    ret
_SplStdStart ENDP

CODE32          ENDS

                END
