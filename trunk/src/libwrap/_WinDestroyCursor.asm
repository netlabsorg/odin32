        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDestroyCursor:PROC
        PUBLIC  _WinDestroyCursor
_WinDestroyCursor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinDestroyCursor
    add    esp, 4
    pop    fs
    ret
_WinDestroyCursor ENDP

CODE32          ENDS

                END
