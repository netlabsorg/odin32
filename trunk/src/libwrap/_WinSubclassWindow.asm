        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSubclassWindow:PROC
        PUBLIC  _WinSubclassWindow
_WinSubclassWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSubclassWindow
    add    esp, 8
    pop    fs
    ret
_WinSubclassWindow ENDP

CODE32          ENDS

                END
