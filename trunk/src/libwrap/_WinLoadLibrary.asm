        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLoadLibrary:PROC
        PUBLIC  _WinLoadLibrary
_WinLoadLibrary PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinLoadLibrary
    add    esp, 8
    pop    fs
    ret
_WinLoadLibrary ENDP

CODE32          ENDS

                END
