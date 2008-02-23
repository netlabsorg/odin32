        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetProcessCp:PROC
        PUBLIC  _DosSetProcessCp
_DosSetProcessCp PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosSetProcessCp
    add    esp, 4
    pop    fs
    ret
_DosSetProcessCp ENDP

CODE32          ENDS

                END
