        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDrawBitmap:PROC
        PUBLIC  _WinDrawBitmap
_WinDrawBitmap PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   WinDrawBitmap
    add    esp, 28
    pop    fs
    ret
_WinDrawBitmap ENDP

CODE32          ENDS

                END
