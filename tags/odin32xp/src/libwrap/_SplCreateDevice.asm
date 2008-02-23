        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplCreateDevice:PROC
        PUBLIC  _SplCreateDevice
_SplCreateDevice PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   SplCreateDevice
    add    esp, 16
    pop    fs
    ret
_SplCreateDevice ENDP

CODE32          ENDS

                END
