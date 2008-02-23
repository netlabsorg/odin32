        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplDeleteDevice:PROC
        PUBLIC  _SplDeleteDevice
_SplDeleteDevice PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   SplDeleteDevice
    add    esp, 8
    pop    fs
    ret
_SplDeleteDevice ENDP

CODE32          ENDS

                END
