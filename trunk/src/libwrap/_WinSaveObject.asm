        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSaveObject:PROC
        PUBLIC  _WinSaveObject
_WinSaveObject PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSaveObject
    add    esp, 8
    pop    fs
    ret
_WinSaveObject ENDP

CODE32          ENDS

                END
