        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryMetaFileBits:PROC
        PUBLIC  _GpiQueryMetaFileBits
_GpiQueryMetaFileBits PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiQueryMetaFileBits
    add    esp, 16
    pop    fs
    ret
_GpiQueryMetaFileBits ENDP

CODE32          ENDS

                END
