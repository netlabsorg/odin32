        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgAddStrHandle:PROC
        PUBLIC  _DrgAddStrHandle
_DrgAddStrHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgAddStrHandle
    add    esp, 4
    pop    fs
    ret
_DrgAddStrHandle ENDP

CODE32          ENDS

                END
