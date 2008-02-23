        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQuerySysPointer:PROC
        PUBLIC  _WinQuerySysPointer
_WinQuerySysPointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQuerySysPointer
    add    esp, 12
    pop    fs
    ret
_WinQuerySysPointer ENDP

CODE32          ENDS

                END
