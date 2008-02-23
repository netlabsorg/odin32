        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCp:PROC
        PUBLIC  _GpiQueryCp
_GpiQueryCp PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryCp
    add    esp, 4
    pop    fs
    ret
_GpiQueryCp ENDP

CODE32          ENDS

                END
