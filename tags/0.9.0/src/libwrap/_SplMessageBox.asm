        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplMessageBox:PROC
        PUBLIC  _SplMessageBox
_SplMessageBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   SplMessageBox
    add    esp, 28
    pop    fs
    ret
_SplMessageBox ENDP

CODE32          ENDS

                END
