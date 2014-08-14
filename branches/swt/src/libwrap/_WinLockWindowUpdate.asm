        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLockWindowUpdate:PROC
        PUBLIC  _WinLockWindowUpdate
_WinLockWindowUpdate PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinLockWindowUpdate
    add    esp, 8
    pop    fs
    ret
_WinLockWindowUpdate ENDP

CODE32          ENDS

                END
