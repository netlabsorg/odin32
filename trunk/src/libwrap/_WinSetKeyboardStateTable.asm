        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetKeyboardStateTable:PROC
        PUBLIC  _WinSetKeyboardStateTable
_WinSetKeyboardStateTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetKeyboardStateTable
    add    esp, 12
    pop    fs
    ret
_WinSetKeyboardStateTable ENDP

CODE32          ENDS

                END
