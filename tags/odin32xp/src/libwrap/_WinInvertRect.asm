        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinInvertRect:PROC
        PUBLIC  _WinInvertRect
_WinInvertRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinInvertRect
    add    esp, 8
    pop    fs
    ret
_WinInvertRect ENDP

CODE32          ENDS

                END
