        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiBitBlt:PROC
        PUBLIC  _GpiBitBlt
_GpiBitBlt PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiBitBlt
    add    esp, 24
    pop    fs
    ret
_GpiBitBlt ENDP

CODE32          ENDS

                END
