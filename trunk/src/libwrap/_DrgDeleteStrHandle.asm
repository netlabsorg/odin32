        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgDeleteStrHandle:PROC
        PUBLIC  _DrgDeleteStrHandle
_DrgDeleteStrHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgDeleteStrHandle
    add    esp, 4
    pop    fs
    ret
_DrgDeleteStrHandle ENDP

CODE32          ENDS

                END
