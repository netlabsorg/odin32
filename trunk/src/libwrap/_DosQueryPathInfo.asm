        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryPathInfo:PROC
        PUBLIC  _DosQueryPathInfo
_DosQueryPathInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryPathInfo
    add    esp, 16
    pop    fs
    ret
_DosQueryPathInfo ENDP

CODE32          ENDS

                END
