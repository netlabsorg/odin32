        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinStartApp:PROC
        PUBLIC  _WinStartApp
_WinStartApp PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinStartApp
    add    esp, 20
    pop    fs
    ret
_WinStartApp ENDP

CODE32          ENDS

                END
