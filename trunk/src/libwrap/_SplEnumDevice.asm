        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplEnumDevice:PROC
        PUBLIC  _SplEnumDevice
_SplEnumDevice PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   SplEnumDevice
    add    esp, 32
    pop    fs
    ret
_SplEnumDevice ENDP

CODE32          ENDS

                END
