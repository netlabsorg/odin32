        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DdfMetafile:PROC
        PUBLIC  _DdfMetafile
_DdfMetafile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DdfMetafile
    add    esp, 12
    pop    fs
    ret
_DdfMetafile ENDP

CODE32          ENDS

                END
