        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinShutdownSystem:PROC
        PUBLIC  _WinShutdownSystem
_WinShutdownSystem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinShutdownSystem
    add    esp, 8
    pop    fs
    ret
_WinShutdownSystem ENDP

CODE32          ENDS

                END
