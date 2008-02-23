        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLockInput:PROC
        PUBLIC  _WinLockInput
_WinLockInput PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinLockInput
    add    esp, 8
    pop    fs
    ret
_WinLockInput ENDP

CODE32          ENDS

                END
