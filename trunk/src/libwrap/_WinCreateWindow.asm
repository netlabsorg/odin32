        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateWindow:PROC
        PUBLIC  _WinCreateWindow
_WinCreateWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    call   WinCreateWindow
    add    esp, 52
    pop    fs
    ret
_WinCreateWindow ENDP

CODE32          ENDS

                END
