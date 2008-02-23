        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsPhysInputEnabled:PROC
        PUBLIC  _WinIsPhysInputEnabled
_WinIsPhysInputEnabled PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinIsPhysInputEnabled
    add    esp, 4
    pop    fs
    ret
_WinIsPhysInputEnabled ENDP

CODE32          ENDS

                END
