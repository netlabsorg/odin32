        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSaveWindowPos:PROC
        PUBLIC  _WinSaveWindowPos
_WinSaveWindowPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSaveWindowPos
    add    esp, 12
    pop    fs
    ret
_WinSaveWindowPos ENDP

CODE32          ENDS

                END
