        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_PlayMetaFileRecord:PROC
        PUBLIC  O32_PlayMetaFileRecord
O32_PlayMetaFileRecord PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   _O32_PlayMetaFileRecord
    add    esp, 16
    pop    fs
    ret
O32_PlayMetaFileRecord ENDP

CODE32          ENDS

                END
