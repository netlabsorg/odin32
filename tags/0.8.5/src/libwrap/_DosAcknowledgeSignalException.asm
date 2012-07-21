        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosAcknowledgeSignalException:PROC
        PUBLIC  _DosAcknowledgeSignalException
_DosAcknowledgeSignalException PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosAcknowledgeSignalException
    add    esp, 4
    pop    fs
    ret
_DosAcknowledgeSignalException ENDP

CODE32          ENDS

                END
