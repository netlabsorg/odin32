        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLockPointerUpdate:PROC
        PUBLIC  _WinLockPointerUpdate
_WinLockPointerUpdate PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinLockPointerUpdate
    add    esp, 12
    pop    fs
    ret
_WinLockPointerUpdate ENDP

CODE32          ENDS

                END
