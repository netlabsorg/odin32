        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryModFromEIP:PROC
        PUBLIC  _DosQueryModFromEIP
_DosQueryModFromEIP PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   DosQueryModFromEIP
    add    esp, 24
    pop    fs
    ret
_DosQueryModFromEIP ENDP

CODE32          ENDS

                END
