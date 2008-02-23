        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplStdOpen:PROC
        PUBLIC  _SplStdOpen
_SplStdOpen PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   SplStdOpen
    add    esp, 4
    pop    fs
    ret
_SplStdOpen ENDP

CODE32          ENDS

                END
