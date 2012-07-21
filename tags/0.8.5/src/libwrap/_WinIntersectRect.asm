        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIntersectRect:PROC
        PUBLIC  _WinIntersectRect
_WinIntersectRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinIntersectRect
    add    esp, 16
    pop    fs
    ret
_WinIntersectRect ENDP

CODE32          ENDS

                END
