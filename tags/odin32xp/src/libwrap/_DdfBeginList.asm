        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DdfBeginList:PROC
        PUBLIC  _DdfBeginList
_DdfBeginList PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DdfBeginList
    add    esp, 16
    pop    fs
    ret
_DdfBeginList ENDP

CODE32          ENDS

                END
