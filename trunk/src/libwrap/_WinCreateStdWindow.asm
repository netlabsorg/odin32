        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateStdWindow:PROC
        PUBLIC  _WinCreateStdWindow
_WinCreateStdWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    call   WinCreateStdWindow
    add    esp, 36
    pop    fs
    ret
_WinCreateStdWindow ENDP

CODE32          ENDS

                END
