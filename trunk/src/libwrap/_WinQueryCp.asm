        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryCp:PROC
        PUBLIC  _WinQueryCp
_WinQueryCp PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryCp
    add    esp, 4
    pop    fs
    ret
_WinQueryCp ENDP

CODE32          ENDS

                END
