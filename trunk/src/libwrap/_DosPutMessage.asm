        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosPutMessage:PROC
        PUBLIC  _DosPutMessage
_DosPutMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosPutMessage
    add    esp, 12
    pop    fs
    ret
_DosPutMessage ENDP

CODE32          ENDS

                END
