        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDdeRespond:PROC
        PUBLIC  _WinDdeRespond
_WinDdeRespond PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinDdeRespond
    add    esp, 20
    pop    fs
    ret
_WinDdeRespond ENDP

CODE32          ENDS

                END
