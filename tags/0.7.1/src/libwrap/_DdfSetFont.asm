        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DdfSetFont:PROC
        PUBLIC  _DdfSetFont
_DdfSetFont PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DdfSetFont
    add    esp, 16
    pop    fs
    ret
_DdfSetFont ENDP

CODE32          ENDS

                END
