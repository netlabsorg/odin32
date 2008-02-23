        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinMapWindowPoints:PROC
        PUBLIC  _WinMapWindowPoints
_WinMapWindowPoints PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinMapWindowPoints
    add    esp, 16
    pop    fs
    ret
_WinMapWindowPoints ENDP

CODE32          ENDS

                END
