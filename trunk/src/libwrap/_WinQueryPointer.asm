        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryPointer:PROC
        PUBLIC  _WinQueryPointer
_WinQueryPointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryPointer
    add    esp, 4
    pop    fs
    ret
_WinQueryPointer ENDP

CODE32          ENDS

                END
