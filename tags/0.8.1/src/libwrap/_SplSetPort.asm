        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplSetPort:PROC
        PUBLIC  _SplSetPort
_SplSetPort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   SplSetPort
    add    esp, 24
    pop    fs
    ret
_SplSetPort ENDP

CODE32          ENDS

                END
