; $Id: iccio.asm,v 1.5 1999-12-31 13:55:12 sandervl Exp $

; **********************************************************************
; Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de)
; Use at your own risk! No Warranty! The author is not responsible for
; any damage or loss of data caused by proper or improper use of this
; device driver.
; **********************************************************************
;
; compile with ALP
;
    TITLE   ICCIO.ASM
    .386
    .387

DATA32  SEGMENT DWORD PUBLIC USE32 'DATA'
;        ASSUME  CS:FLAT ,DS:FLAT,SS:FLAT
        ASSUME  DS:FLAT,SS:FLAT
    EXTRN   ioentry:DWORD
    EXTRN   gdt:WORD
    EXTRN   devname:BYTE
DATA32  ENDS

CODE32  SEGMENT DWORD PUBLIC USE32 'CODE'
;        ASSUME  CS:FLAT ,DS:FLAT,SS:FLAT
        ASSUME DS:FLAT,SS:FLAT

    ALIGN 04H

; performs fast output of a byte to an I/O port
; this routine is intended to be called from icc C code
;
; Calling convention:
;   void _c_outb(short port,char data)
;
;
    PUBLIC  _c_outb
_c_outb PROC
    MOV EDX, DWORD PTR [ESP+4]  ; get port
    MOV AL, BYTE PTR [ESP+8]    ; get data
    PUSH    EBX         ; save register
    MOV EBX, 4          ; function code 4 = write byte
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX
    RET
_c_outb ENDP

; performs fast output of a word to an I/O port
; this routine is intended to be called from icc C code
;
; Calling convention:
;   void _c_outw(short port,short data)
;
;
    ALIGN   04H

    PUBLIC  _c_outw
_c_outw PROC
    MOV EDX, DWORD PTR [ESP+4]  ; get port
    MOV AX, WORD PTR [ESP+8]    ; get data
    PUSH    EBX         ; save register
    MOV EBX, 5          ; function code 5 = write word
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX
    RET
_c_outw ENDP

; performs fast output of a dword to an I/O port
; this routine is intended to be called from icc C code
;
; Calling convention:
;   void _c_outl(short port,long data)
;
;
    ALIGN   04H

    PUBLIC  _c_outl
_c_outl PROC
    MOV EDX, DWORD PTR [ESP+4]  ; get port
    MOV EAX, DWORD PTR [ESP+8]  ; get data
    PUSH    EBX         ; save register
    MOV EBX, 6          ; function code 6 = write dword
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX
    RET
_c_outl ENDP

; performs fast input of a byte from an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;   char _c_inb(short port)
;
;
    ALIGN   04H
    PUBLIC  _c_inb
_c_inb  PROC
    MOV EDX, DWORD PTR [ESP+4]  ; get port number
    PUSH    EBX         ; save register
    MOV EBX, 1          ; function code 1 = read byte
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    AND EAX, 000000ffH      ; mask out required byte
    POP EBX         ; restore register
    RET
_c_inb  ENDP

; performs fast input of a word from an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;   short _c_inw(short port)
;
;
    ALIGN   04H
    PUBLIC  _c_inw
_c_inw  PROC
    MOV EDX, DWORD PTR [ESP+4]  ; get port number
    PUSH    EBX         ; save register
    MOV EBX, 2          ; function code 2 = read short
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    AND EAX, 0000ffffH      ; mask out required byte
    POP EBX         ; restore register
    RET
_c_inw  ENDP

; performs fast input of a dword from an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;   long _c_inl(short port)
;
;
    ALIGN   04H
    PUBLIC  _c_inl
_c_inl  PROC
    MOV EDX, DWORD PTR [ESP+4]  ; get port number
    PUSH    EBX         ; save register
    MOV EBX, 3          ; function code 3 = read long
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
;   AND EAX, 000000ffH      ; mask out required byte
    POP EBX         ; restore register
    RET
_c_inl  ENDP

    ALIGN   4
    PUBLIC  _c_readmsr
_c_readmsr PROC NEAR
    PUSH    EBP
    MOV EBP, ESP
    PUSH    EAX
    PUSH    EBX
    PUSH    ECX
    PUSH    EDX         ; save register
    MOV ECX, DWORD PTR [EBP+8]  ; get msr reg
    MOV EBX, 7          ; function code 7 = read msr
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    MOV EBX, DWORD PTR [EBP+12] ; LONGLONG ptr
    MOV DWORD PTR [EBX+4], EDX
    MOV DWORD PTR [EBX], EAX
    POP EDX
    POP ECX
    POP EBX         ; restore register
    POP EAX
    POP EBP
    RET
_c_readmsr ENDP

;------------------------------------------------------------------------------

; performs fast output of a byte to an I/O port
; this routine is intended to be called from assembler code
; note there is no stack frame, however 8 byte stack space is required
;
; calling convention:
;   MOV EDX, portnr
;   MOV AL, data
;   CALL    a_outb
;
;
    ALIGN   04H
    PUBLIC  a_outb
a_outb  PROC
    PUSH    EBX         ; save register
    MOV EBX, 4          ; function code 4 = write byte
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX         ; restore bx
    RET
a_outb  ENDP

; performs fast output of a word to an I/O port
; this routine is intended to be called from assembler code
; note there is no stack frame, however 8 byte stack space is required
;
; calling convention:
;   MOV EDX, portnr
;   MOV AX, data
;   CALL    a_outw
;
;
    ALIGN   04H
    PUBLIC  a_outw
a_outw  PROC
    PUSH    EBX         ; save register
    MOV EBX, 5          ; function code 5 = write word
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX         ; restore bx
    RET
a_outw  ENDP

; performs fast output of a long to an I/O port
; this routine is intended to be called from assembler code
; note there is no stack frame, however 8 byte stack space is required
;
; calling convention:
;   MOV EDX, portnr
;   MOV EAX, data
;   CALL    a_outl
;
;
    ALIGN   04H
    PUBLIC  a_outl
a_outl  PROC
    PUSH    EBX         ; save register
    MOV EBX, 6          ; function code 6 = write long
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX         ; restore bx
    RET
a_outl  ENDP

; performs fast input of a byte from an I/O port
; this routine is intended to be called from assembler code
; note there is no stack frame, however 8 byte stack space is required
;
; calling convention:
;   MOV EDX, portnr
;   CALL    a_inb
;   ;data in AL
;
    ALIGN   04H
    PUBLIC  a_inb
a_inb   PROC
    PUSH    EBX         ; save register
    MOV EBX, 1          ; function code 1 = read byte
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    AND EAX, 000000FFh      ; mask byte
    POP EBX         ; restore register
    RET
a_inb   ENDP


; performs fast input of a word from an I/O port
; this routine is intended to be called from assembler code
; note there is no stack frame, however 8 byte stack space is required
;
; calling convention:
;   MOV EDX, portnr
;   CALL    a_inb
;   ;data in AX
;
    ALIGN   04H
    PUBLIC  a_inw
a_inw   PROC
    PUSH    EBX         ; save register
    MOV EBX, 2          ; function code 2 = read word
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    AND EAX, 0000FFFFh      ; mask byte
    POP EBX         ; restore register
    RET
a_inw   ENDP

; performs fast input of a dword from an I/O port
; this routine is intended to be called from assembler code
; note there is no stack frame, however 8 byte stack space is required
;
; calling convention:
;   MOV EDX, portnr
;   CALL    a_inl
;   ;data in EAX
;
    ALIGN   04H
    PUBLIC  a_inl
a_inl   PROC
    PUSH    EBX         ; save register
    MOV EBX, 3          ; function code 3 = read dword
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32
    POP EBX         ; restore register
    RET
a_inl   ENDP

CODE32  ENDS

;------------------------------------------------------------------------------

; Initialize I/O access via the driver.
; You *must* call this routine once for each executable that wants to do
; I/O.
;
; The routine is mainly equivalent to a C routine performing the
; following (but no need to add another file):
;   DosOpen("/dev/fastio$", read, nonexclusive)
;   DosDevIOCtl(device, XFREE86_IO, IO_GETSEL32)
;   selector -> ioentry+4
;   DosClose(device)
;
; Calling convention:
;   int io_init(void)
; Return:
;   0 if successful
;   standard APIRET return code if error
;

CODE32  SEGMENT
    PUBLIC  _io_init
    EXTRN   DosOpen:PROC
    EXTRN   DosClose:PROC
    EXTRN   DosDevIOCtl:PROC
_io_init    PROC
    PUSH    EBP
    MOV EBP, ESP    ; standard stack frame
    SUB ESP, 16     ; reserve memory
                ; -16 = len arg of DosDevIOCtl
                ; -12 = action arg of DosOpen
                ; -8 = fd arg of DosOpen
                ; -2 = short GDT selector arg
    PUSH    0       ; (PEAOP2)NULL
    PUSH    66      ; OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE
    PUSH    1       ; FILE_OPEN
    PUSH    0       ; FILE_NORMAL
    PUSH    0       ; initial size
    LEA EAX, [EBP-12]   ; Adress of 'action' arg
    PUSH    EAX
    LEA EAX, [EBP-8]    ; Address of 'fd' arg
    PUSH    EAX
    PUSH    OFFSET FLAT:devname
    CALL    DosOpen     ; call DosOpen
    ADD ESP, 32     ; cleanup stack frame
    CMP EAX, 0      ; is return code zero?
    JE  goon        ; yes, proceed
    LEAVE           ; no return error
    RET

    ALIGN   04H
goon:
    LEA EAX, [EBP-16]   ; address of 'len' arg of DosDevIOCtl
    PUSH    EAX
    PUSH    2       ; sizeof(short)
    LEA EAX, [EBP-2]    ; address to return the GDT selector
    PUSH    EAX
    PUSH    0       ; no parameter len
    PUSH    0       ; no parameter size
    PUSH    0       ; no parameter address
    PUSH    100     ; function code IO_GETSEL32
    PUSH    118     ; category code XFREE6_IO
    MOV EAX, [EBP-8]    ; file handle
    PUSH    EAX
    CALL    DosDevIOCtl ; perform ioctl
    ADD ESP, 36     ; cleanup stack
    CMP EAX, 0      ; is return code = 0?
    JE  ok      ; yes, proceed
    PUSH    EAX     ; was error, save error code
    MOV EAX, [EBP-8]    ; file handle
    PUSH    EAX
    CALL    DosClose    ; close device
    ADD ESP, 4      ; clean stack
    POP EAX     ; get error code
    LEAVE           ; return error
    RET

    ALIGN   04H
ok:
    MOV EAX,[EBP-8] ; file handle
    PUSH    EAX     ; do normal close
    CALL    DosClose
    ADD ESP, 4      ; clean stack

    MOV AX, WORD PTR [EBP-2]    ; load gdt selector
    MOV gdt, AX     ; store in ioentry address selector part
    XOR EAX, EAX    ; eax = 0
    MOV DWORD PTR [ioentry], EAX ; clear ioentry offset part
                ; return code = 0 (in %eax)
    LEAVE           ; clean stack frame
    RET         ; exit
_io_init    ENDP

; just for symmetry, does nothing

    ALIGN   04H
    PUBLIC  _io_exit
_io_exit    PROC
    XOR EAX,EAX
    RET
_io_exit    ENDP

    ALIGN   04H
    PUBLIC  int03

int03   PROC
    INT 3
    RET
int03   ENDP

;------------------------------------------------------------------------------

; Initialize I/O access via the driver.
; You *must* call this routine once for each *thread* that wants to do
; I/O.
;
; The routine is mainly equivalent to a C routine performing the
; following (but no need to add another file):
;   DosOpen("/dev/fastio$", read, nonexclusive)
;   DosDevIOCtl(device, XFREE86IO, IOGETSEL32)
;   selector -> ioentry+4
;   DosClose(device)
;
; Calling convention:
;   int io_init1(void)
; Return:
;   0 if successful
;   standard APIRET RETurn code if error
;

    PUBLIC  _io_init1
    ALIGN   04H
_io_init1   PROC
    PUSH    EBP
    MOV EBP, ESP    ; standard stack frame
    SUB ESP, 16     ; reserve memory
                ; -16 = len arg of DosDevIOCtl
                ; -12 = action arg of DosOpen
                ; -8 = fd arg of DosOpen
                ; -2 = short GDT selector arg
    PUSH    0       ; (PEAOP2)NULL
    PUSH    66      ; OPENACCESSREADWRITE|OPENSHAREDENYNONE
    PUSH    1       ; FILEOPEN
    PUSH    0       ; FILENORMAL
    PUSH    0       ; initial size
    LEA EAX, [EBP-12]   ; Adress of 'action' arg
    PUSH    EAX
    LEA EAX, [EBP-8]    ; Address of 'fd' arg
    PUSH    EAX
    PUSH    OFFSET FLAT:devname
    CALL    DosOpen     ; call DosOpen
    ADD ESP, 32     ; cleanup stack frame
    CMP EAX, 0      ; is return code zero?
    JE  goon1       ; yes, proceed
    LEAVE           ; no RETurn error
    RET
    ALIGN   04H
goon1:
    LEA EAX, [EBP-16]   ; address of 'len' arg of DosDevIOCtl
    PUSH    EAX
    PUSH    2       ; sizeof(short)
    LEA EAX, [EBP-2]    ; address to return the GDT selector
    PUSH    EAX
    PUSH    0       ; no parameter len
    PUSH    0       ; no parameter size
    PUSH    0       ; no parameter address
    PUSH    100     ; function code IOGETSEL32
    PUSH    118     ; category code XFREE6IO
    MOV EAX,[EBP-8] ; file handle
    PUSH    EAX
    CALL    DosDevIOCtl ; perform ioctl
    ADD ESP, 36     ; cleanup stack
    CMP EAX, 0      ; is return code = 0?
    JE  ok1     ; yes, proceed
    PUSH    EAX     ; was error, save error code
    MOV EAX, [EBP-8]    ; file handle
    PUSH    EAX
    CALL    DosClose    ; close device
    ADD ESP,4       ; clean stack
    POP EAX     ; get error code
    LEAVE           ; return error
    RET

    ALIGN   04H
ok1:
    MOV EAX, [EBP-8]    ; file handle
    PUSH    EAX     ; do normal close
    CALL    DosClose
    ADD ESP,4       ; clean stack

    MOV AX, [EBP-2] ; load gdt selector
    MOV gdt, AX     ; store in ioentry address selector part
    XOR EAX, EAX    ; EAX = 0
    MOV DWORD PTR [ioentry], EAX ; clear ioentry offset part
                ; return code = 0 (in EAX)

                ; now use this function to raise the IOPL
    MOV EBX,13      ; special function code
    CALL    FWORD PTR [ioentry] ; CALL intersegment indirect 16:32

    ; thread should now be running at IOPL=3

    XOR EAX, EAX    ; return code = 0
    LEAVE           ; clean stack frame
    RET         ; exit
_io_init1 ENDP

    PUBLIC  _io_exit1
    ALIGN   04H
_io_exit1   PROC
    push    EBP
    MOV EBP, ESP    ; stackframe, I am accustomed to this :-)

    MOV AX, gdt     ; check if ioinit was called once
    OR  AX, AX
    JZ  exerr       ; no gdt entry, so process cannot be at IOPL=3
                ; through this mechanism

    MOV EBX, 14     ; function code to disable iopl
    CALL    FWORD PTR [ioentry] ; call intersegment indirect 16:32

    ; process should now be at IOPL=3 again
    XOR EAX, EAX    ; ok, RETurn code = 0
    LEAVE
    RET
exerr:  XOR EAX, EAX    ; not ok, RETurn code = ffffffff
    DEC EAX
    LEAVE
    RET
_io_exit1   ENDP

; for diagnostic only

    PUBLIC  psw
    ALIGN   04H
psw PROC
    PUSHF           ; get the current PSW
    POP EAX     ; into EAX
    RET
psw ENDP

CODE32  ENDS
    END
