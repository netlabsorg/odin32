        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsChild:PROC
        PUBLIC  _WinIsChild
_WinIsChild PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinIsChild
    add    esp, 8
    pop    fs
    ret
_WinIsChild ENDP

CODE32          ENDS

                END
