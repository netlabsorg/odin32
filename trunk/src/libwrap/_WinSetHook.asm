        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetHook:PROC
        PUBLIC  _WinSetHook
_WinSetHook PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinSetHook
    add    esp, 20
    pop    fs
    ret
_WinSetHook ENDP

CODE32          ENDS

                END
