        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowPtr:PROC
        PUBLIC  _WinQueryWindowPtr
_WinQueryWindowPtr PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryWindowPtr
    add    esp, 8
    pop    fs
    ret
_WinQueryWindowPtr ENDP

CODE32          ENDS

                END
