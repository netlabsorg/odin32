        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRestartSOMDD:PROC
        PUBLIC  _WinRestartSOMDD
_WinRestartSOMDD PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinRestartSOMDD
    add    esp, 4
    pop    fs
    ret
_WinRestartSOMDD ENDP

CODE32          ENDS

                END
