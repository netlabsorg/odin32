        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DdfSetColor:PROC
        PUBLIC  _DdfSetColor
_DdfSetColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DdfSetColor
    add    esp, 12
    pop    fs
    ret
_DdfSetColor ENDP

CODE32          ENDS

                END
