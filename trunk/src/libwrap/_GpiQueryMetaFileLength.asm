        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryMetaFileLength:PROC
        PUBLIC  _GpiQueryMetaFileLength
_GpiQueryMetaFileLength PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryMetaFileLength
    add    esp, 4
    pop    fs
    ret
_GpiQueryMetaFileLength ENDP

CODE32          ENDS

                END
