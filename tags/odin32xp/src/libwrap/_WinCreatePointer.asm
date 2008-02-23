        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreatePointer:PROC
        PUBLIC  _WinCreatePointer
_WinCreatePointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinCreatePointer
    add    esp, 20
    pop    fs
    ret
_WinCreatePointer ENDP

CODE32          ENDS

                END
