        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinTrackRect:PROC
        PUBLIC  _WinTrackRect
_WinTrackRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinTrackRect
    add    esp, 12
    pop    fs
    ret
_WinTrackRect ENDP

CODE32          ENDS

                END
