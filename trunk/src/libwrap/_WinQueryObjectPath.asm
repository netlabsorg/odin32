        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryObjectPath:PROC
        PUBLIC  _WinQueryObjectPath
_WinQueryObjectPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQueryObjectPath
    add    esp, 12
    pop    fs
    ret
_WinQueryObjectPath ENDP

CODE32          ENDS

                END
