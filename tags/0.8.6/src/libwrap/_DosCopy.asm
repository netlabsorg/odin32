        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCopy:PROC
        PUBLIC  _DosCopy
_DosCopy PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosCopy
    add    esp, 12
    pop    fs
    ret
_DosCopy ENDP

CODE32          ENDS

                END
