        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEnablePhysInput:PROC
        PUBLIC  _WinEnablePhysInput
_WinEnablePhysInput PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinEnablePhysInput
    add    esp, 8
    pop    fs
    ret
_WinEnablePhysInput ENDP

CODE32          ENDS

                END
