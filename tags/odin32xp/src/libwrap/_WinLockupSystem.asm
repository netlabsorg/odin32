        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLockupSystem:PROC
        PUBLIC  _WinLockupSystem
_WinLockupSystem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinLockupSystem
    add    esp, 4
    pop    fs
    ret
_WinLockupSystem ENDP

CODE32          ENDS

                END
