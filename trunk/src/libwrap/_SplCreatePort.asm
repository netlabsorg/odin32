        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplCreatePort:PROC
        PUBLIC  _SplCreatePort
_SplCreatePort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   SplCreatePort
    add    esp, 24
    pop    fs
    ret
_SplCreatePort ENDP

CODE32          ENDS

                END
