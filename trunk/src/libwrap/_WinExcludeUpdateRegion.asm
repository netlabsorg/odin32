        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinExcludeUpdateRegion:PROC
        PUBLIC  _WinExcludeUpdateRegion
_WinExcludeUpdateRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinExcludeUpdateRegion
    add    esp, 8
    pop    fs
    ret
_WinExcludeUpdateRegion ENDP

CODE32          ENDS

                END
