/*
 * Win32 definitions for Windows NT
 *
 * Copyright 1996 Alexandre Julliard
 */

#ifndef __WINE_WINNT_H
#define __WINE_WINNT_H

#include "windef.h"

#include "pshpack1.h"
/* Defines */

#define UNREFERENCED_PARAMETER(a)

/* Argument 1 passed to the DllEntryProc. */
#define	DLL_PROCESS_DETACH	0	/* detach process (unload library) */
#define	DLL_PROCESS_ATTACH	1	/* attach process (load library) */
#define	DLL_THREAD_ATTACH	2	/* attach new thread */
#define	DLL_THREAD_DETACH	3	/* detach thread */


/* u.x.wProcessorArchitecture (NT) */
#define PROCESSOR_ARCHITECTURE_INTEL	0
#define PROCESSOR_ARCHITECTURE_MIPS	1
#define PROCESSOR_ARCHITECTURE_ALPHA	2
#define PROCESSOR_ARCHITECTURE_PPC	3
#define PROCESSOR_ARCHITECTURE_SHX	4
#define PROCESSOR_ARCHITECTURE_ARM	5
#define PROCESSOR_ARCHITECTURE_UNKNOWN	0xFFFF

/* dwProcessorType */
#define PROCESSOR_INTEL_386      386
#define PROCESSOR_INTEL_486      486
#define PROCESSOR_INTEL_PENTIUM  586
#define PROCESSOR_INTEL_860      860
#define PROCESSOR_MIPS_R2000     2000
#define PROCESSOR_MIPS_R3000     3000
#define PROCESSOR_MIPS_R4000     4000
#define PROCESSOR_ALPHA_21064    21064
#define PROCESSOR_PPC_601        601
#define PROCESSOR_PPC_603        603
#define PROCESSOR_PPC_604        604
#define PROCESSOR_PPC_620        620
#define PROCESSOR_HITACHI_SH3    10003
#define PROCESSOR_HITACHI_SH3E   10004
#define PROCESSOR_HITACHI_SH4    10005
#define PROCESSOR_MOTOROLA_821   821
#define PROCESSOR_SHx_SH3        103
#define PROCESSOR_SHx_SH4        104
#define PROCESSOR_STRONGARM      2577
#define PROCESSOR_ARM720         1824    /* 0x720 */
#define PROCESSOR_ARM820         2080    /* 0x820 */
#define PROCESSOR_ARM920         2336    /* 0x920 */
#define PROCESSOR_ARM_7TDMI      70001

#define ANYSIZE_ARRAY 1

#define MINCHAR       0x80
#define MAXCHAR       0x7f
#define MINSHORT      0x8000
#define MAXSHORT      0x7fff
#define MINLONG       0x80000000
#define MAXLONG       0x7fffffff
#define MAXBYTE       0xff
#define MAXWORD       0xffff
#define MAXDWORD      0xffffffff

#define FIELD_OFFSET(type, field) \
  ((LONG)(INT)&(((type *)0)->field))

#define CONTAINING_RECORD(address, type, field) \
  ((type *)((PCHAR)(address) - (PCHAR)(&((type *)0)->field)))

/* Types */

/* TCHAR data types definitions for Winelib. */
/* These types are _not_ defined for the emulator, because they */
/* depend on the UNICODE macro that only exists in user's code. */

#if !defined(__WINE__) && !defined(__WINE_WINDEF_H)
# ifdef UNICODE
typedef LPWSTR LPTSTR;
typedef LPCWSTR LPCTSTR;
#define __TEXT(string) L##string /*probably wrong */
# else  /* UNICODE */
typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;
#define __TEXT(string) string
# endif /* UNICODE */
#endif   /* __WINE__ */
#define TEXT(quote) __TEXT(quote)

typedef BYTE     BOOLEAN;
typedef BOOLEAN *PBOOLEAN;
typedef DWORD		EXECUTION_STATE;

typedef struct _LIST_ENTRY {
  struct _LIST_ENTRY *Flink;
  struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

typedef struct _SINGLE_LIST_ENTRY {
  struct _SINGLE_LIST_ENTRY *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;

/* Heap flags */

#define HEAP_NO_SERIALIZE               0x00000001
#define HEAP_GROWABLE                   0x00000002
#define HEAP_GENERATE_EXCEPTIONS        0x00000004
#define HEAP_ZERO_MEMORY                0x00000008
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020
#define HEAP_FREE_CHECKING_ENABLED      0x00000040
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080
#define HEAP_CREATE_ALIGN_16            0x00010000
#define HEAP_CREATE_ENABLE_TRACING      0x00020000
#define HEAP_WINE_SEGPTR                0x01000000  /* Not a Win32 flag */
#define HEAP_WINE_CODESEG               0x02000000  /* Not a Win32 flag */
#define HEAP_WINE_CODE16SEG             0x04000000  /* Not a Win32 flag */

/* Processor feature flags.  */
#define PF_FLOATING_POINT_PRECISION_ERRATA	0
#define PF_FLOATING_POINT_EMULATED		1
#define PF_COMPARE_EXCHANGE_DOUBLE		2
#define PF_MMX_INSTRUCTIONS_AVAILABLE		3
#define PF_PPC_MOVEMEM_64BIT_OK			4
#define PF_ALPHA_BYTE_INSTRUCTIONS		5


/* The Win32 register context */

/* CONTEXT is the CPU-dependent context; it should be used        */
/* wherever a platform-specific context is needed (e.g. exception */
/* handling, Win32 register functions). */

/* CONTEXT86 is the i386-specific context; it should be used     */
/* wherever only a 386 context makes sense (e.g. DOS interrupts, */
/* Win16 register functions), so that this code can be compiled  */
/* on all platforms. */

#define SIZE_OF_80387_REGISTERS      80

typedef struct _FLOATING_SAVE_AREA
{
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;    
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;    
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT86
{
    DWORD   ContextFlags;

    /* These are selected by CONTEXT_DEBUG_REGISTERS */
    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    /* These are selected by CONTEXT_FLOATING_POINT */
    FLOATING_SAVE_AREA FloatSave;

    /* These are selected by CONTEXT_SEGMENTS */
    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;    

    /* These are selected by CONTEXT_INTEGER */
    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;    
    DWORD   Ecx;
    DWORD   Eax;

    /* These are selected by CONTEXT_CONTROL */
    DWORD   Ebp;    
    DWORD   Eip;
    DWORD   SegCs;
    DWORD   EFlags;
    DWORD   Esp;
    DWORD   SegSs;
} CONTEXT86;

#define CONTEXT_X86       0x00010000
#define CONTEXT_i386      CONTEXT_X86
#define CONTEXT_i486      CONTEXT_X86

#define CONTEXT86_CONTROL   (CONTEXT_i386 | 0x0001) /* SS:SP, CS:IP, FLAGS, BP */
#define CONTEXT86_INTEGER   (CONTEXT_i386 | 0x0002) /* AX, BX, CX, DX, SI, DI */
#define CONTEXT86_SEGMENTS  (CONTEXT_i386 | 0x0004) /* DS, ES, FS, GS */
#define CONTEXT86_FLOATING_POINT  (CONTEXT_i386 | 0x0008L) /* 387 state */
#define CONTEXT86_DEBUG_REGISTERS (CONTEXT_i386 | 0x0010L) /* DB 0-3,6,7 */
#define CONTEXT86_FULL (CONTEXT86_CONTROL | CONTEXT86_INTEGER | CONTEXT86_SEGMENTS)

/* i386 context definitions */
#ifdef __i386__

#define CONTEXT_CONTROL         CONTEXT86_CONTROL
#define CONTEXT_INTEGER         CONTEXT86_INTEGER
#define CONTEXT_SEGMENTS        CONTEXT86_SEGMENTS
#define CONTEXT_FLOATING_POINT  CONTEXT86_FLOATING_POINT
#define CONTEXT_DEBUG_REGISTERS CONTEXT86_DEBUG_REGISTERS
#define CONTEXT_FULL            CONTEXT86_FULL

typedef CONTEXT86 CONTEXT;

#endif  /* __i386__ */

/* Alpha context definitions */
#ifdef _ALPHA_

#define CONTEXT_ALPHA   0x00020000
 
#define CONTEXT_CONTROL		(CONTEXT_ALPHA | 0x00000001L)
#define CONTEXT_FLOATING_POINT	(CONTEXT_ALPHA | 0x00000002L)
#define CONTEXT_INTEGER		(CONTEXT_ALPHA | 0x00000004L)
#define CONTEXT_FULL  (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

typedef struct _CONTEXT
{
    /* selected by CONTEXT_FLOATING_POINT */
    ULONGLONG FltF0;
    ULONGLONG FltF1;
    ULONGLONG FltF2;
    ULONGLONG FltF3;
    ULONGLONG FltF4;
    ULONGLONG FltF5;
    ULONGLONG FltF6;
    ULONGLONG FltF7;
    ULONGLONG FltF8;
    ULONGLONG FltF9;
    ULONGLONG FltF10;
    ULONGLONG FltF11;
    ULONGLONG FltF12;
    ULONGLONG FltF13;
    ULONGLONG FltF14;
    ULONGLONG FltF15;
    ULONGLONG FltF16;
    ULONGLONG FltF17;
    ULONGLONG FltF18;
    ULONGLONG FltF19;
    ULONGLONG FltF20;
    ULONGLONG FltF21;
    ULONGLONG FltF22;
    ULONGLONG FltF23;
    ULONGLONG FltF24;
    ULONGLONG FltF25;
    ULONGLONG FltF26;
    ULONGLONG FltF27;
    ULONGLONG FltF28;
    ULONGLONG FltF29;
    ULONGLONG FltF30;
    ULONGLONG FltF31;

    /* selected by CONTEXT_INTEGER */
    ULONGLONG IntV0;
    ULONGLONG IntT0;
    ULONGLONG IntT1;
    ULONGLONG IntT2;
    ULONGLONG IntT3;
    ULONGLONG IntT4;
    ULONGLONG IntT5;
    ULONGLONG IntT6;
    ULONGLONG IntT7;
    ULONGLONG IntS0;
    ULONGLONG IntS1;
    ULONGLONG IntS2;
    ULONGLONG IntS3;
    ULONGLONG IntS4;
    ULONGLONG IntS5;
    ULONGLONG IntFp;
    ULONGLONG IntA0;
    ULONGLONG IntA1;
    ULONGLONG IntA2;
    ULONGLONG IntA3;
    ULONGLONG IntA4;
    ULONGLONG IntA5;
    ULONGLONG IntT8;
    ULONGLONG IntT9;
    ULONGLONG IntT10;
    ULONGLONG IntT11;
    ULONGLONG IntRa;
    ULONGLONG IntT12;
    ULONGLONG IntAt;
    ULONGLONG IntGp;
    ULONGLONG IntSp;
    ULONGLONG IntZero;

    /* selected by CONTEXT_FLOATING_POINT */
    ULONGLONG Fpcr;
    ULONGLONG SoftFpcr;

    /* selected by CONTEXT_CONTROL */
    ULONGLONG Fir;
    DWORD Psr;
    DWORD ContextFlags;
    DWORD Fill[4];
} CONTEXT;

#define _QUAD_PSR_OFFSET   HighSoftFpcr
#define _QUAD_FLAGS_OFFSET HighFir

#endif  /* _ALPHA_ */

/* Mips context definitions */
#ifdef _MIPS_

#define CONTEXT_R4000   0x00010000

#define CONTEXT_CONTROL         (CONTEXT_R4000 | 0x00000001)
#define CONTEXT_FLOATING_POINT  (CONTEXT_R4000 | 0x00000002)
#define CONTEXT_INTEGER         (CONTEXT_R4000 | 0x00000004)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

typedef struct _CONTEXT
{
    DWORD Argument[4];
    /* These are selected by CONTEXT_FLOATING_POINT */
    DWORD FltF0;
    DWORD FltF1;
    DWORD FltF2;
    DWORD FltF3;
    DWORD FltF4;
    DWORD FltF5;
    DWORD FltF6;
    DWORD FltF7;
    DWORD FltF8;
    DWORD FltF9;
    DWORD FltF10;
    DWORD FltF11;
    DWORD FltF12;
    DWORD FltF13;
    DWORD FltF14;
    DWORD FltF15;
    DWORD FltF16;
    DWORD FltF17;
    DWORD FltF18;
    DWORD FltF19;
    DWORD FltF20;
    DWORD FltF21;
    DWORD FltF22;
    DWORD FltF23;
    DWORD FltF24;
    DWORD FltF25;
    DWORD FltF26;
    DWORD FltF27;
    DWORD FltF28;
    DWORD FltF29;
    DWORD FltF30;
    DWORD FltF31;

    /* These are selected by CONTEXT_INTEGER */
    DWORD IntZero;
    DWORD IntAt;
    DWORD IntV0;
    DWORD IntV1;
    DWORD IntA0;
    DWORD IntA1;
    DWORD IntA2;
    DWORD IntA3;
    DWORD IntT0;
    DWORD IntT1;
    DWORD IntT2;
    DWORD IntT3;
    DWORD IntT4;
    DWORD IntT5;
    DWORD IntT6;
    DWORD IntT7;
    DWORD IntS0;
    DWORD IntS1;
    DWORD IntS2;
    DWORD IntS3;
    DWORD IntS4;
    DWORD IntS5;
    DWORD IntS6;
    DWORD IntS7;
    DWORD IntT8;
    DWORD IntT9;
    DWORD IntK0;
    DWORD IntK1;
    DWORD IntGp;
    DWORD IntSp;
    DWORD IntS8;
    DWORD IntRa;
    DWORD IntLo;
    DWORD IntHi;

    /* These are selected by CONTEXT_FLOATING_POINT */
    DWORD Fsr;

    /* These are selected by CONTEXT_CONTROL */
    DWORD Fir;
    DWORD Psr;

    DWORD ContextFlags;
    DWORD Fill[2];
} CONTEXT;

#endif  /* _MIPS_ */

/* PowerPC context definitions */
#ifdef __PPC__

#define CONTEXT_CONTROL         0x0001
#define CONTEXT_FLOATING_POINT  0x0002
#define CONTEXT_INTEGER         0x0004
#define CONTEXT_DEBUG_REGISTERS 0x0008
#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

typedef struct
{
    /* These are selected by CONTEXT_FLOATING_POINT */
    double Fpr0;
    double Fpr1;
    double Fpr2;
    double Fpr3;
    double Fpr4;
    double Fpr5;
    double Fpr6;
    double Fpr7;
    double Fpr8;
    double Fpr9;
    double Fpr10;
    double Fpr11;
    double Fpr12;
    double Fpr13;
    double Fpr14;
    double Fpr15;
    double Fpr16;
    double Fpr17;
    double Fpr18;
    double Fpr19;
    double Fpr20;
    double Fpr21;
    double Fpr22;
    double Fpr23;
    double Fpr24;
    double Fpr25;
    double Fpr26;
    double Fpr27;
    double Fpr28;
    double Fpr29;
    double Fpr30;
    double Fpr31;
    double Fpscr;

    /* These are selected by CONTEXT_INTEGER */
    DWORD Gpr0;
    DWORD Gpr1;
    DWORD Gpr2;
    DWORD Gpr3;
    DWORD Gpr4;
    DWORD Gpr5;
    DWORD Gpr6;
    DWORD Gpr7;
    DWORD Gpr8;
    DWORD Gpr9;
    DWORD Gpr10;
    DWORD Gpr11;
    DWORD Gpr12;
    DWORD Gpr13;
    DWORD Gpr14;
    DWORD Gpr15;
    DWORD Gpr16;
    DWORD Gpr17;
    DWORD Gpr18;
    DWORD Gpr19;
    DWORD Gpr20;
    DWORD Gpr21;
    DWORD Gpr22;
    DWORD Gpr23;
    DWORD Gpr24;
    DWORD Gpr25;
    DWORD Gpr26;
    DWORD Gpr27;
    DWORD Gpr28;
    DWORD Gpr29;
    DWORD Gpr30;
    DWORD Gpr31;

    DWORD Cr;
    DWORD Xer;

    /* These are selected by CONTEXT_CONTROL */
    DWORD Msr;
    DWORD Iar;
    DWORD Lr;
    DWORD Ctr;

    DWORD ContextFlags;
    DWORD Fill[3];

    /* These are selected by CONTEXT_DEBUG_REGISTERS */
    DWORD Dr0;
    DWORD Dr1;
    DWORD Dr2;
    DWORD Dr3;
    DWORD Dr4;
    DWORD Dr5;
    DWORD Dr6;
    DWORD Dr7;
} CONTEXT;

typedef struct _STACK_FRAME_HEADER
{
    DWORD BackChain;
    DWORD GlueSaved1;
    DWORD GlueSaved2;
    DWORD Reserved1;
    DWORD Spare1;
    DWORD Spare2;

    DWORD Parameter0;
    DWORD Parameter1;
    DWORD Parameter2;
    DWORD Parameter3;
    DWORD Parameter4;
    DWORD Parameter5;
    DWORD Parameter6;
    DWORD Parameter7;
} STACK_FRAME_HEADER,*PSTACK_FRAME_HEADER;

#endif  /* __PPC__ */

#ifdef __sparc__

/* 
 * FIXME:  
 *
 * There is no official CONTEXT structure defined for the SPARC 
 * architecture, so I just made one up.
 *
 * This structure is valid only for 32-bit SPARC architectures,
 * not for 64-bit SPARC.
 *
 * Note that this structure contains only the 'top-level' registers;
 * the rest of the register window chain is not visible.
 *
 * The layout follows the Solaris 'prgregset_t' structure.
 * 
 */ 

#define CONTEXT_SPARC            0x10000000

#define CONTEXT_CONTROL         (CONTEXT_SPARC | 0x00000001)
#define CONTEXT_FLOATING_POINT  (CONTEXT_SPARC | 0x00000002)
#define CONTEXT_INTEGER         (CONTEXT_SPARC | 0x00000004)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

typedef struct _CONTEXT
{
    DWORD ContextFlags;

    /* These are selected by CONTEXT_INTEGER */
    DWORD g0;
    DWORD g1;
    DWORD g2;
    DWORD g3;
    DWORD g4;
    DWORD g5;
    DWORD g6;
    DWORD g7;
    DWORD o0;
    DWORD o1;
    DWORD o2;
    DWORD o3;
    DWORD o4;
    DWORD o5;
    DWORD o6;
    DWORD o7;
    DWORD l0;
    DWORD l1;
    DWORD l2;
    DWORD l3;
    DWORD l4;
    DWORD l5;
    DWORD l6;
    DWORD l7;
    DWORD i0;
    DWORD i1;
    DWORD i2;
    DWORD i3;
    DWORD i4;
    DWORD i5;
    DWORD i6;
    DWORD i7;

    /* These are selected by CONTEXT_CONTROL */
    DWORD psr;
    DWORD pc;
    DWORD npc;
    DWORD y;
    DWORD wim;
    DWORD tbr;

    /* FIXME: floating point registers missing */

} CONTEXT;

#endif

#if !defined(CONTEXT_FULL) && !defined(RC_INVOKED)
#error You need to define a CONTEXT for your CPU
#endif

typedef CONTEXT *PCONTEXT;
typedef HANDLE *PHANDLE;

#ifdef __WINE__

/* Macros for easier access to i386 context registers */

#define EAX_reg(context)     ((context)->Eax)
#define EBX_reg(context)     ((context)->Ebx)
#define ECX_reg(context)     ((context)->Ecx)
#define EDX_reg(context)     ((context)->Edx)
#define ESI_reg(context)     ((context)->Esi)
#define EDI_reg(context)     ((context)->Edi)
#define EBP_reg(context)     ((context)->Ebp)

#define CS_reg(context)      ((context)->SegCs)
#define DS_reg(context)      ((context)->SegDs)
#define ES_reg(context)      ((context)->SegEs)
#define FS_reg(context)      ((context)->SegFs)
#define GS_reg(context)      ((context)->SegGs)
#define SS_reg(context)      ((context)->SegSs)

#define EFL_reg(context)     ((context)->EFlags)
#define EIP_reg(context)     ((context)->Eip)
#define ESP_reg(context)     ((context)->Esp)

#define AX_reg(context)      (*(WORD*)&EAX_reg(context))
#define BX_reg(context)      (*(WORD*)&EBX_reg(context))
#define CX_reg(context)      (*(WORD*)&ECX_reg(context))
#define DX_reg(context)      (*(WORD*)&EDX_reg(context))
#define SI_reg(context)      (*(WORD*)&ESI_reg(context))
#define DI_reg(context)      (*(WORD*)&EDI_reg(context))
#define BP_reg(context)      (*(WORD*)&EBP_reg(context))

#define AL_reg(context)      (*(BYTE*)&EAX_reg(context))
#define AH_reg(context)      (*((BYTE*)&EAX_reg(context)+1))
#define BL_reg(context)      (*(BYTE*)&EBX_reg(context))
#define BH_reg(context)      (*((BYTE*)&EBX_reg(context)+1))
#define CL_reg(context)      (*(BYTE*)&ECX_reg(context))
#define CH_reg(context)      (*((BYTE*)&ECX_reg(context)+1))
#define DL_reg(context)      (*(BYTE*)&EDX_reg(context))
#define DH_reg(context)      (*((BYTE*)&EDX_reg(context)+1))
                            
#define SET_CFLAG(context)   (EFL_reg(context) |= 0x0001)
#define RESET_CFLAG(context) (EFL_reg(context) &= ~0x0001)
#define SET_ZFLAG(context)   (EFL_reg(context) |= 0x0040)
#define RESET_ZFLAG(context) (EFL_reg(context) &= ~0x0040)

#define ISV86(context)       (EFL_reg(context) & 0x00020000)
#define V86BASE(context)     ((context)->Dr7) /* ugly */


/* Macros to retrieve the current context */

#ifdef __i386__
#define _DEFINE_REGS_ENTRYPOINT( name, fn, args ) \
  __asm__(".align 4\n\t"                         \
          ".globl " #name "\n\t"                 \
          ".type " #name ",@function\n\t"        \
          #name ":\n\t"                          \
          "call CALL32_Regs\n\t"                 \
          ".long " #fn "\n\t"                    \
          ".byte " #args ", " #args "\n\t");

#define DEFINE_REGS_ENTRYPOINT_0( name, fn ) \
  _DEFINE_REGS_ENTRYPOINT( name, fn, 0 )
#define DEFINE_REGS_ENTRYPOINT_1( name, fn, t1 ) \
  _DEFINE_REGS_ENTRYPOINT( name, fn, 4 )
#define DEFINE_REGS_ENTRYPOINT_2( name, fn, t1, t2 ) \
  _DEFINE_REGS_ENTRYPOINT( name, fn, 8 )
#define DEFINE_REGS_ENTRYPOINT_3( name, fn, t1, t2, t3 ) \
  _DEFINE_REGS_ENTRYPOINT( name, fn, 12 )
#define DEFINE_REGS_ENTRYPOINT_4( name, fn, t1, t2, t3, t4 ) \
  _DEFINE_REGS_ENTRYPOINT( name, fn, 16 )

#endif  /* __i386__ */

#ifdef __sparc__
/* FIXME: use getcontext() to retrieve full context */
#define _GET_CONTEXT \
    CONTEXT context;   \
    do { memset(&context, 0, sizeof(CONTEXT));            \
         context.ContextFlags = CONTEXT_CONTROL;          \
         context.pc = (DWORD)__builtin_return_address(0); \
       } while (0)

#define DEFINE_REGS_ENTRYPOINT_0( name, fn ) \
  void WINAPI name ( void ) \
  { _GET_CONTEXT; fn( &context ); }
#define DEFINE_REGS_ENTRYPOINT_1( name, fn, t1 ) \
  void WINAPI name ( t1 a1 ) \
  { _GET_CONTEXT; fn( a1, &context ); }
#define DEFINE_REGS_ENTRYPOINT_2( name, fn, t1, t2 ) \
  void WINAPI name ( t1 a1, t2 a2 ) \
  { _GET_CONTEXT; fn( a1, a2, &context ); }
#define DEFINE_REGS_ENTRYPOINT_3( name, fn, t1, t2, t3 ) \
  void WINAPI name ( t1 a1, t2 a2, t3 a3 ) \
  { _GET_CONTEXT; fn( a1, a2, a3, &context ); }
#define DEFINE_REGS_ENTRYPOINT_4( name, fn, t1, t2, t3, t4 ) \
  void WINAPI name ( t1 a1, t2 a2, t3 a3, t4 a4 ) \
  { _GET_CONTEXT; fn( a1, a2, a3, a4, &context ); }

#endif /* __sparc__ */

#ifndef DEFINE_REGS_ENTRYPOINT_0
#error You need to define DEFINE_REGS_ENTRYPOINT macros for your CPU
#endif

#ifdef __i386__
# define GET_IP(context) ((LPVOID)(context)->Eip)
#endif
#ifdef __sparc__
# define GET_IP(context) ((LPVOID)(context)->pc)
#endif
 
#if !defined(GET_IP) && !defined(RC_INVOKED)
# error You must define GET_IP for this CPU
#endif

#endif  /* __WINE__ */

/*
 * Exception codes
 */
 
#define STATUS_SUCCESS                   0x00000000
#define STATUS_WAIT_0                    0x00000000
#define STATUS_ABANDONED_WAIT_0          0x00000080
#define STATUS_USER_APC                  0x000000C0
#define STATUS_TIMEOUT                   0x00000102
#define STATUS_PENDING                   0x00000103

#define STATUS_GUARD_PAGE_VIOLATION      0x80000001    
#define STATUS_DATATYPE_MISALIGNMENT     0x80000002
#define STATUS_BREAKPOINT                0x80000003
#define STATUS_SINGLE_STEP               0x80000004
#define	STATUS_BUFFER_OVERFLOW           0x80000005
#define STATUS_NO_MORE_FILES             0x80000006
#define STATUS_WAKE_SYSTEM_DEBUGGER      0x80000007

#define STATUS_HANDLES_CLOSED            0x8000000A
#define STATUS_NO_INHERITANCE            0x8000000B
#define STATUS_GUID_SUBSTITUTION_MADE    0x8000000C
#define STATUS_PARTIAL_COPY              0x8000000D
#define STATUS_DEVICE_PAPER_EMPTY        0x8000000E
#define STATUS_DEVICE_POWERED_OFF        0x8000000F
#define STATUS_DEVICE_OFF_LINE           0x80000010
#define STATUS_DEVICE_BUSY               0x80000011
#define STATUS_NO_MORE_EAS               0x80000012
#define STATUS_INVALID_EA_NAME           0x80000013
#define STATUS_EA_LIST_INCONSISTENT      0x80000014
#define STATUS_INVALID_EA_FLAG           0x80000015
#define STATUS_VERIFY_REQUIRED           0x80000016
#define STATUS_EXTRANEOUS_INFORMATION    0x80000017
#define STATUS_RXACT_COMMIT_NECESSARY    0x80000018
#define STATUS_NO_MORE_ENTRIES           0x8000001A
#define STATUS_FILEMARK_DETECTED         0x8000001B
#define STATUS_MEDIA_CHANGED             0x8000001C
#define STATUS_BUS_RESET                 0x8000001D
#define STATUS_END_OF_MEDIA              0x8000001E
#define STATUS_BEGINNING_OF_MEDIA        0x8000001F
#define STATUS_MEDIA_CHECK               0x80000020
#define STATUS_SETMARK_DETECTED          0x80000021
#define STATUS_NO_DATA_DETECTED          0x80000022
#define STATUS_REDIRECTOR_HAS_OPEN_HANDLES 0x80000023
#define STATUS_SERVER_HAS_OPEN_HANDLES   0x80000024
#define STATUS_ALREADY_DISCONNECTED      0x80000025
#define STATUS_LONGJUMP                  0x80000026

#define STATUS_UNSUCCESSFUL              0xC0000001
#define STATUS_NOT_IMPLEMENTED           0xC0000002
#define STATUS_INVALID_INFO_CLASS        0xC0000003
#define STATUS_INFO_LENGTH_MISMATCH      0xC0000004
#define STATUS_ACCESS_VIOLATION          0xC0000005
#define STATUS_IN_PAGE_ERROR             0xC0000006
#define STATUS_PAGEFILE_QUOTA            0xC0000007
#define STATUS_INVALID_HANDLE            0xC0000008
#define STATUS_BAD_INITIAL_STACK         0xC0000009
#define STATUS_BAD_INITIAL_PC            0xC000000A
#define STATUS_INVALID_CID               0xC000000B
#define STATUS_TIMER_NOT_CANCELED        0xC000000C
#define STATUS_INVALID_PARAMETER         0xC000000D
#define STATUS_NO_SUCH_DEVICE            0xC000000E
#define STATUS_NO_SUCH_FILE              0xC000000F
#define STATUS_INVALID_DEVICE_REQUEST    0xC0000010
#define STATUS_END_OF_FILE               0xC0000011
#define STATUS_WRONG_VOLUME              0xC0000012
#define STATUS_NO_MEDIA_IN_DEVICE        0xC0000013
#define STATUS_UNRECOGNIZED_MEDIA        0xC0000014
#define STATUS_NONEXISTENT_SECTOR        0xC0000015
#define STATUS_MORE_PROCESSING_REQUIRED  0xC0000016
#define STATUS_NO_MEMORY                 0xC0000017
#define STATUS_CONFLICTING_ADDRESSES     0xC0000018
#define STATUS_NOT_MAPPED_VIEW           0xC0000019
#define STATUS_UNABLE_TO_FREE_VM         0xC000001A
#define STATUS_UNABLE_TO_DELETE_SECTION  0xC000001B
#define STATUS_INVALID_SYSTEM_SERVICE    0xC000001C
#define STATUS_ILLEGAL_INSTRUCTION       0xC000001D
#define STATUS_INVALID_LOCK_SEQUENCE     0xC000001E
#define STATUS_INVALID_VIEW_SIZE         0xC000001F
#define STATUS_INVALID_FILE_FOR_SECTION  0xC0000020
#define STATUS_ALREADY_COMMITTED         0xC0000021
#define STATUS_ACCESS_DENIED             0xC0000022
#define	STATUS_BUFFER_TOO_SMALL          0xC0000023
#define STATUS_OBJECT_TYPE_MISMATCH      0xC0000024
#define STATUS_NONCONTINUABLE_EXCEPTION  0xC0000025
#define STATUS_INVALID_DISPOSITION       0xC0000026
#define STATUS_UNWIND                    0xC0000027
#define STATUS_BAD_STACK                 0xC0000028
#define STATUS_INVALID_UNWIND_TARGET     0xC0000029
#define STATUS_NOT_LOCKED                0xC000002A
#define STATUS_PARITY_ERROR              0xC000002B
#define STATUS_UNABLE_TO_DECOMMIT_VM     0xC000002C
#define STATUS_NOT_COMMITTED             0xC000002D
#define STATUS_INVALID_PORT_ATTRIBUTES   0xC000002E
#define STATUS_PORT_MESSAGE_TOO_LONG     0xC000002F
#define STATUS_INVALID_PARAMETER_MIX     0xC0000030
#define STATUS_INVALID_QUOTA_LOWER       0xC0000031
#define STATUS_DISK_CORRUPT_ERROR        0xC0000032
#define STATUS_OBJECT_NAME_INVALID       0xC0000033
#define STATUS_OBJECT_NAME_NOT_FOUND     0xC0000034
#define STATUS_OBJECT_NAME_COLLISION     0xC0000035
#define STATUS_PORT_DISCONNECTED         0xC0000037
#define STATUS_DEVICE_ALREADY_ATTACHED   0xC0000038
#define STATUS_OBJECT_PATH_INVALID       0xC0000039
#define STATUS_OBJECT_PATH_NOT_FOUND     0xC000003A
#define STATUS_PATH_SYNTAX_BAD           0xC000003B
#define STATUS_DATA_OVERRUN              0xC000003C
#define STATUS_DATA_LATE_ERROR           0xC000003D
#define STATUS_DATA_ERROR                0xC000003E
#define STATUS_CRC_ERROR                 0xC000003F
#define STATUS_SECTION_TOO_BIG           0xC0000040
#define STATUS_PORT_CONNECTION_REFUSED   0xC0000041
#define STATUS_INVALID_PORT_HANDLE       0xC0000042
#define STATUS_SHARING_VIOLATION         0xC0000043
#define STATUS_QUOTA_EXCEEDED            0xC0000044
#define STATUS_INVALID_PAGE_PROTECTION   0xC0000045
#define STATUS_MUTANT_NOT_OWNED          0xC0000046
#define STATUS_SEMAPHORE_LIMIT_EXCEEDED  0xC0000047
#define STATUS_PORT_ALREADY_SET          0xC0000048
#define STATUS_SUSPEND_COUNT_EXCEEDED    0xC000004A
#define STATUS_LOCK_NOT_GRANTED          0xC0000054  /* FIXME: not sure */
#define STATUS_FILE_LOCK_CONFLICT        0xC0000055  /* FIXME: not sure */
#define	STATUS_UNKNOWN_REVISION          0xC0000058
#define	STATUS_INVALID_SECURITY_DESCR    0xC0000079
#define STATUS_DISK_FULL                 0xC000007F 
#define STATUS_SECTION_NOT_EXTENDED      0xC0000087
#define STATUS_ARRAY_BOUNDS_EXCEEDED     0xC000008C
#define STATUS_FLOAT_DENORMAL_OPERAND    0xC000008D
#define STATUS_FLOAT_DIVIDE_BY_ZERO      0xC000008E
#define STATUS_FLOAT_INEXACT_RESULT      0xC000008F
#define STATUS_FLOAT_INVALID_OPERATION   0xC0000090
#define STATUS_FLOAT_OVERFLOW            0xC0000091
#define STATUS_FLOAT_STACK_CHECK         0xC0000092
#define STATUS_FLOAT_UNDERFLOW           0xC0000093
#define STATUS_INTEGER_DIVIDE_BY_ZERO    0xC0000094
#define STATUS_INTEGER_OVERFLOW          0xC0000095
#define STATUS_PRIVILEGED_INSTRUCTION    0xC0000096
#define STATUS_MEDIA_WRITE_PROTECTED     0XC00000A2
#define	STATUS_INVALID_PARAMETER_2       0xC00000F0
#define STATUS_STACK_OVERFLOW            0xC00000FD
#define STATUS_DIRECTORY_NOT_EMPTY       0xC0000101
#define STATUS_TOO_MANY_OPENED_FILES     0xC000011F
#define STATUS_CONTROL_C_EXIT            0xC000013A
#define STATUS_PIPE_BROKEN               0xC000014B
#define STATUS_NOT_REGISTRY_FILE         0xC000015C
#define STATUS_PARTITION_FAILURE         0xC0000172
#define STATUS_INVALID_BLOCK_LENGTH      0xC0000173
#define STATUS_DEVICE_NOT_PARTITIONED    0xC0000174
#define STATUS_UNABLE_TO_LOCK_MEDIA      0xC0000175
#define STATUS_UNABLE_TO_UNLOAD_MEDIA    0xC0000176
#define STATUS_EOM_OVERFLOW              0xC0000177
#define STATUS_NO_MEDIA                  0xC0000178
#define STATUS_NO_SUCH_MEMBER            0xC000017A
#define STATUS_INVALID_MEMBER            0xC000017B
#define STATUS_KEY_DELETED               0xC000017C
#define STATUS_NO_LOG_SPACE              0xC000017D
#define STATUS_TOO_MANY_SIDS             0xC000017E
#define STATUS_LM_CROSS_ENCRYPTION_REQUIRED 0xC000017F
#define STATUS_KEY_HAS_CHILDREN          0xC0000180
#define STATUS_CHILD_MUST_BE_VOLATILE    0xC0000181
#define STATUS_DEVICE_CONFIGURATION_ERROR0xC0000182
#define STATUS_DRIVER_INTERNAL_ERROR     0xC0000183
#define STATUS_INVALID_DEVICE_STATE      0xC0000184
#define STATUS_IO_DEVICE_ERROR           0xC0000185
#define STATUS_DEVICE_PROTOCOL_ERROR     0xC0000186
#define STATUS_BACKUP_CONTROLLER         0xC0000187
#define STATUS_LOG_FILE_FULL             0xC0000188
#define STATUS_TOO_LATE                  0xC0000189
#define STATUS_NO_TRUST_LSA_SECRET       0xC000018A
#define STATUS_NO_TRUST_SAM_ACCOUNT      0xC000018B
#define STATUS_TRUSTED_DOMAIN_FAILURE    0xC000018C
#define STATUS_TRUSTED_RELATIONSHIP_FAILURE 0xC000018D
#define STATUS_EVENTLOG_FILE_CORRUPT     0xC000018E
#define STATUS_EVENTLOG_CANT_START       0xC000018F
#define STATUS_TRUST_FAILURE             0xC0000190
#define STATUS_MUTANT_LIMIT_EXCEEDED     0xC0000191
#define STATUS_NETLOGON_NOT_STARTED      0xC0000192
#define STATUS_ACCOUNT_EXPIRED           0xC0000193
#define STATUS_POSSIBLE_DEADLOCK         0xC0000194
#define STATUS_NETWORK_CREDENTIAL_CONFLICT 0xC0000195
#define STATUS_REMOTE_SESSION_LIMIT      0xC0000196
#define STATUS_EVENTLOG_FILE_CHANGED     0xC0000197
#define STATUS_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT 0xC0000198
#define STATUS_NOLOGON_WORKSTATION_TRUST_ACCOUNT 0xC0000199
#define STATUS_NOLOGON_SERVER_TRUST_ACCOUNT 0xC000019A
#define STATUS_DOMAIN_TRUST_INCONSISTENT 0xC000019B
#define STATUS_FS_DRIVER_REQUIRED        0xC000019C

#define STATUS_RESOURCE_LANG_NOT_FOUND   0xC0000204

#define MAXIMUM_WAIT_OBJECTS 64
#define MAXIMUM_SUSPEND_COUNT 127

#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT

/* Wine extension; Windows doesn't have a name for this code */
#define EXCEPTION_CRITICAL_SECTION_WAIT     0xc0000194


/*
 * Return values from the actual exception handlers
 */

#define ExceptionContinueExecution 0
#define ExceptionContinueSearch    1
#define ExceptionNestedException   2
#define ExceptionCollidedUnwind    3
 
/*
 * Return values from filters in except() and from UnhandledExceptionFilter
 */
 
#define EXCEPTION_EXECUTE_HANDLER        1
#define EXCEPTION_CONTINUE_SEARCH        0
#define EXCEPTION_CONTINUE_EXECUTION    -1

/*
 * From OS/2 2.0 exception handling
 * Win32 seems to use the same flags as ExceptionFlags in an EXCEPTION_RECORD
 */

#define EH_NONCONTINUABLE   0x01
#define EH_UNWINDING        0x02
#define EH_EXIT_UNWIND      0x04
#define EH_STACK_INVALID    0x08
#define EH_NESTED_CALL      0x10

#define EXCEPTION_CONTINUABLE        0
#define EXCEPTION_NONCONTINUABLE     EH_NONCONTINUABLE
 
/*
 * The exception record used by Win32 to give additional information 
 * about exception to exception handlers.
 */

#define EXCEPTION_MAXIMUM_PARAMETERS 15

typedef struct __EXCEPTION_RECORD
{
    DWORD    ExceptionCode;
    DWORD    ExceptionFlags;
    struct __EXCEPTION_RECORD *ExceptionRecord;

    LPVOID   ExceptionAddress;
    DWORD    NumberParameters;
    DWORD    ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

/*
 * The exception pointers structure passed to exception filters
 * in except() and the UnhandledExceptionFilter().
 */
 
typedef struct _EXCEPTION_POINTERS 
{
  PEXCEPTION_RECORD  ExceptionRecord;
  PCONTEXT           ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;


/*
 * The exception frame, used for registering exception handlers 
 * Win32 cares only about this, but compilers generally emit 
 * larger exception frames for their own use.
 */

struct __EXCEPTION_FRAME;

typedef DWORD (*PEXCEPTION_HANDLER)(PEXCEPTION_RECORD,struct __EXCEPTION_FRAME*,
                                    PCONTEXT,struct __EXCEPTION_FRAME **);

typedef struct __EXCEPTION_FRAME
{
  struct __EXCEPTION_FRAME *Prev;
  PEXCEPTION_HANDLER       Handler;
} EXCEPTION_FRAME, *PEXCEPTION_FRAME;

#include "poppack.h"

/*
 * function pointer to a exception filter
 */

typedef LONG (* CALLBACK PTOP_LEVEL_EXCEPTION_FILTER)(PEXCEPTION_POINTERS ExceptionInfo);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

DWORD WINAPI UnhandledExceptionFilter( PEXCEPTION_POINTERS epointers );
LPTOP_LEVEL_EXCEPTION_FILTER
WINAPI SetUnhandledExceptionFilter( LPTOP_LEVEL_EXCEPTION_FILTER filter );

/* status values for ContinueDebugEvent */
#define DBG_CONTINUE                0x00010002
#define DBG_TERMINATE_THREAD        0x40010003
#define DBG_TERMINATE_PROCESS       0x40010004
#define DBG_CONTROL_C               0x40010005
#define DBG_CONTROL_BREAK           0x40010008
#define DBG_EXCEPTION_NOT_HANDLED   0x80010001

struct _TEB;

#ifdef __WINE__

#ifndef __WIN32OS2__
#if defined(__i386__)
static inline struct _TEB * WINE_UNUSED __get_teb(void)
{
    struct _TEB *teb;
    __asm__(".byte 0x64\n\tmovl (0x18),%0" : "=r" (teb));
    return teb;
}
#elif defined(HAVE__LWP_CREATE)
extern void *_lwp_getprivate(void);
static inline struct _TEB * WINE_UNUSED __get_teb(void)
{
    return (struct _TEB *)_lwp_getprivate();
}
#else
#error NtCurrentTeb() not defined for this architecture!
#endif

#define NtCurrentTeb() __get_teb()

#else /* __WINE__ */
#define NtCurrentTeb GetThreadTEB
struct _TEB  *WINAPI GetThreadTEB();
#endif
#endif //__WIN32OS2__

/*
 * Here follows typedefs for security and tokens.
 */ 

/*
 * First a constant for the following typdefs.
 */

#define ANYSIZE_ARRAY   1

/* FIXME:  Orphan.  What does it point to? */
typedef PVOID PACCESS_TOKEN;

/*
 * TOKEN_INFORMATION_CLASS
 */

typedef enum _TOKEN_INFORMATION_CLASS {
  TokenUser = 1, 
  TokenGroups, 
  TokenPrivileges, 
  TokenOwner, 
  TokenPrimaryGroup, 
  TokenDefaultDacl, 
  TokenSource, 
  TokenType, 
  TokenImpersonationLevel, 
  TokenStatistics 
} TOKEN_INFORMATION_CLASS; 

#ifndef _SECURITY_DEFINED
#define _SECURITY_DEFINED

#include "pshpack1.h"

typedef DWORD ACCESS_MASK, *PACCESS_MASK;

typedef struct _GENERIC_MAPPING {
    ACCESS_MASK GenericRead;
    ACCESS_MASK GenericWrite;
    ACCESS_MASK GenericExecute;
    ACCESS_MASK GenericAll;
} GENERIC_MAPPING, *PGENERIC_MAPPING;

#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED
#define SID_IDENTIFIER_AUTHORITY_DEFINED
typedef struct {
    BYTE Value[6];
} SID_IDENTIFIER_AUTHORITY,*PSID_IDENTIFIER_AUTHORITY,*LPSID_IDENTIFIER_AUTHORITY;
#endif /* !defined(SID_IDENTIFIER_AUTHORITY_DEFINED) */

#ifndef SID_DEFINED
#define SID_DEFINED
typedef struct _SID {
    BYTE Revision;
    BYTE SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    DWORD SubAuthority[1];
} SID,*PSID;
#endif /* !defined(SID_DEFINED) */

#define	SID_REVISION			(1)	/* Current revision */
#define	SID_MAX_SUB_AUTHORITIES		(15)	/* current max subauths */
#define	SID_RECOMMENDED_SUB_AUTHORITIES	(1)	/* recommended subauths */


/* 
 * ACL 
 */

#define ACL_REVISION1 1
#define ACL_REVISION2 2
#define ACL_REVISION3 3
#define ACL_REVISION4 4

typedef enum _ACL_INFORMATION_CLASS {
    AclRevisionInformation = 1,
    AclSizeInformation
} ACL_INFORMATION_CLASS;

typedef struct _ACL_REVISION_INFORMATION {
    DWORD AclRevision;
} ACL_REVISION_INFORMATION;
typedef ACL_REVISION_INFORMATION *PACL_REVISION_INFORMATION;

typedef struct _ACL_SIZE_INFORMATION {
    DWORD AceCount;
    DWORD AclBytesInUse;
    DWORD AclBytesFree;
} ACL_SIZE_INFORMATION;
typedef ACL_SIZE_INFORMATION *PACL_SIZE_INFORMATION;

#define MIN_ACL_REVISION ACL_REVISION2
#define MAX_ACL_REVISION ACL_REVISION4

typedef struct _ACL {
    BYTE AclRevision;
    BYTE Sbz1;
    WORD AclSize;
    WORD AceCount;
    WORD Sbz2;
} ACL, *PACL;


/* SECURITY_DESCRIPTOR */
#define	SECURITY_DESCRIPTOR_REVISION	1
#define	SECURITY_DESCRIPTOR_REVISION1	1


#define	SE_OWNER_DEFAULTED	0x0001
#define	SE_GROUP_DEFAULTED	0x0002
#define	SE_DACL_PRESENT		0x0004
#define	SE_DACL_DEFAULTED	0x0008
#define	SE_SACL_PRESENT		0x0010
#define	SE_SACL_DEFAULTED	0x0020
#define	SE_SELF_RELATIVE	0x8000

typedef DWORD SECURITY_INFORMATION, *PSECURITY_INFORMATION;
typedef WORD SECURITY_DESCRIPTOR_CONTROL, *PSECURITY_DESCRIPTOR_CONTROL;

/* The security descriptor structure */
typedef struct {
    BYTE Revision;
    BYTE Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    DWORD Owner;
    DWORD Group;
    DWORD Sacl;
    DWORD Dacl;
} SECURITY_DESCRIPTOR_RELATIVE, *PISECURITY_DESCRIPTOR_RELATIVE;

typedef struct {
    BYTE Revision;
    BYTE Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    PSID Owner;
    PSID Group;
    PACL Sacl;
    PACL Dacl;
} SECURITY_DESCRIPTOR, *PSECURITY_DESCRIPTOR;

#define SECURITY_DESCRIPTOR_MIN_LENGTH   (sizeof(SECURITY_DESCRIPTOR)) 

#include "poppack.h"

#endif /* _SECURITY_DEFINED */

#include "pshpack1.h"

/* 
 * SID_AND_ATTRIBUTES
 */

typedef struct _SID_AND_ATTRIBUTES {
  PSID  Sid; 
  DWORD Attributes; 
} SID_AND_ATTRIBUTES ; 
 
/* security entities */
#define SECURITY_NULL_RID			(0x00000000L)
#define SECURITY_WORLD_RID			(0x00000000L)
#define SECURITY_LOCAL_RID			(0X00000000L)

#define SECURITY_NULL_SID_AUTHORITY		{0,0,0,0,0,0}

/* S-1-1 */
#define SECURITY_WORLD_SID_AUTHORITY		{0,0,0,0,0,1}

/* S-1-2 */
#define SECURITY_LOCAL_SID_AUTHORITY		{0,0,0,0,0,2}

/* S-1-3 */
#define SECURITY_CREATOR_SID_AUTHORITY		{0,0,0,0,0,3}
#define SECURITY_CREATOR_OWNER_RID		(0x00000000L) 
#define SECURITY_CREATOR_GROUP_RID		(0x00000001L)
#define SECURITY_CREATOR_OWNER_SERVER_RID	(0x00000002L)
#define SECURITY_CREATOR_GROUP_SERVER_RID	(0x00000003L)

/* S-1-4 */
#define SECURITY_NON_UNIQUE_AUTHORITY		{0,0,0,0,0,4}

/* S-1-5 */
#define SECURITY_NT_AUTHORITY			{0,0,0,0,0,5} 
#define SECURITY_DIALUP_RID                     0x00000001L
#define SECURITY_NETWORK_RID                    0x00000002L
#define SECURITY_BATCH_RID                      0x00000003L
#define SECURITY_INTERACTIVE_RID                0x00000004L
#define SECURITY_LOGON_IDS_RID                  0x00000005L
#define SECURITY_SERVICE_RID                    0x00000006L
#define SECURITY_ANONYMOUS_LOGON_RID            0x00000007L
#define SECURITY_PROXY_RID                      0x00000008L
#define SECURITY_ENTERPRISE_CONTROLLERS_RID     0x00000009L
#define SECURITY_PRINCIPAL_SELF_RID             0x0000000AL
#define SECURITY_AUTHENTICATED_USER_RID         0x0000000BL
#define SECURITY_RESTRICTED_CODE_RID            0x0000000CL
#define SECURITY_TERMINAL_SERVER_RID            0x0000000DL
#define SECURITY_LOCAL_SYSTEM_RID               0x00000012L
#define SECURITY_NT_NON_UNIQUE                  0x00000015L
#define SECURITY_BUILTIN_DOMAIN_RID             0x00000020L

#define DOMAIN_GROUP_RID_ADMINS                 0x00000200L
#define DOMAIN_GROUP_RID_USERS                  0x00000201L
#define DOMAIN_GROUP_RID_GUESTS                 0x00000202L

#define DOMAIN_ALIAS_RID_ADMINS                 0x00000220L
#define DOMAIN_ALIAS_RID_USERS                  0x00000221L
#define DOMAIN_ALIAS_RID_GUESTS                 0x00000222L

#define SECURITY_SERVER_LOGON_RID		SECURITY_ENTERPRISE_CONTROLLERS_RID

#define SECURITY_LOGON_IDS_RID_COUNT		(3L)
 
/*
 * TOKEN_USER
 */

typedef struct _TOKEN_USER {
  SID_AND_ATTRIBUTES User; 
} TOKEN_USER; 

/*
 * TOKEN_GROUPS
 */

typedef struct _TOKEN_GROUPS  {
  DWORD GroupCount; 
  SID_AND_ATTRIBUTES Groups[ANYSIZE_ARRAY]; 
} TOKEN_GROUPS, *PTOKEN_GROUPS; 

/*
 * LUID_AND_ATTRIBUTES
 */
#if defined(__WIN32OS2__) && !defined(WINE_LARGE_INTEGER)
typedef struct _LARGE_INTEGER {
       	ULONG LowPart;
       	LONG HighPart;
} LARGE_INTEGER, *LPLARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _ULARGE_INTEGER {
       	ULONG LowPart;
       	LONG HighPart;
} ULARGE_INTEGER, *LPULARGE_INTEGER, *PULARGE_INTEGER;
#else
typedef union _LARGE_INTEGER {
    struct {
        DWORD    LowPart;
        LONG     HighPart;
    } DUMMYSTRUCTNAME;
    LONGLONG QuadPart;
} LARGE_INTEGER, *LPLARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD    LowPart;
        DWORD    HighPart;
    } DUMMYSTRUCTNAME;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *LPULARGE_INTEGER, *PULARGE_INTEGER;
#endif
/*
 * Locally Unique Identifier
 */

typedef LARGE_INTEGER LUID,*PLUID;

typedef struct _LUID_AND_ATTRIBUTES {
  LUID   Luid; 
  DWORD  Attributes; 
} LUID_AND_ATTRIBUTES; 

/*
 * PRIVILEGE_SET
 */

typedef struct _PRIVILEGE_SET {
    DWORD PrivilegeCount;
    DWORD Control;
    LUID_AND_ATTRIBUTES Privilege[ANYSIZE_ARRAY];
} PRIVILEGE_SET, *PPRIVILEGE_SET;

/*
 * TOKEN_PRIVILEGES
 */

typedef struct _TOKEN_PRIVILEGES {
  DWORD PrivilegeCount; 
  LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY]; 
} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES; 

/*
 * TOKEN_OWNER
 */

typedef struct _TOKEN_OWNER {
  PSID Owner; 
} TOKEN_OWNER; 

/*
 * TOKEN_PRIMARY_GROUP
 */

typedef struct _TOKEN_PRIMARY_GROUP {
  PSID PrimaryGroup; 
} TOKEN_PRIMARY_GROUP; 


/*
 * TOKEN_DEFAULT_DACL
 */

typedef struct _TOKEN_DEFAULT_DACL { 
  PACL DefaultDacl; 
} TOKEN_DEFAULT_DACL; 

/*
 * TOKEN_SOURCEL
 */

typedef struct _TOKEN_SOURCE {
  char Sourcename[8]; 
  LUID SourceIdentifier; 
} TOKEN_SOURCE; 

/*
 * TOKEN_TYPE
 */

typedef enum tagTOKEN_TYPE {
  TokenPrimary = 1, 
  TokenImpersonation 
} TOKEN_TYPE; 

/*
 * SECURITY_IMPERSONATION_LEVEL
 */

typedef enum _SECURITY_IMPERSONATION_LEVEL {
  SecurityAnonymous, 
  SecurityIdentification, 
  SecurityImpersonation, 
  SecurityDelegation 
} SECURITY_IMPERSONATION_LEVEL, *PSECURITY_IMPERSONATION_LEVEL; 


typedef BOOLEAN SECURITY_CONTEXT_TRACKING_MODE,
	* PSECURITY_CONTEXT_TRACKING_MODE;
/*
 *	Quality of Service
 */

typedef struct _SECURITY_QUALITY_OF_SERVICE {
  DWORD				Length;
  SECURITY_IMPERSONATION_LEVEL	ImpersonationLevel;
  SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
  BOOL				EffectiveOnly;
} SECURITY_QUALITY_OF_SERVICE, *PSECURITY_QUALITY_OF_SERVICE;

/*
 * TOKEN_STATISTICS
 */

typedef struct _TOKEN_STATISTICS {
  LUID  TokenId; 
  LUID  AuthenticationId; 
  LARGE_INTEGER ExpirationTime; 
  TOKEN_TYPE    TokenType; 
  SECURITY_IMPERSONATION_LEVEL ImpersonationLevel; 
  DWORD DynamicCharged; 
  DWORD DynamicAvailable; 
  DWORD GroupCount; 
  DWORD PrivilegeCount; 
  LUID  ModifiedId; 
} TOKEN_STATISTICS; 

/* 
 *	ACLs of NT 
 */

#define	ACL_REVISION	2

#define	ACL_REVISION1	1
#define	ACL_REVISION2	2

/* ACEs, directly starting after an ACL */
typedef struct _ACE_HEADER {
	BYTE	AceType;
	BYTE	AceFlags;
	WORD	AceSize;
} ACE_HEADER,*PACE_HEADER;

/* AceType */
#define	ACCESS_ALLOWED_ACE_TYPE		0
#define	ACCESS_DENIED_ACE_TYPE		1
#define	SYSTEM_AUDIT_ACE_TYPE		2
#define	SYSTEM_ALARM_ACE_TYPE		3

/* inherit AceFlags */
#define	OBJECT_INHERIT_ACE		0x01
#define	CONTAINER_INHERIT_ACE		0x02
#define	NO_PROPAGATE_INHERIT_ACE	0x04
#define	INHERIT_ONLY_ACE		0x08
#define	VALID_INHERIT_FLAGS		0x0F

/* AceFlags mask for what events we (should) audit */
#define	SUCCESSFUL_ACCESS_ACE_FLAG	0x40
#define	FAILED_ACCESS_ACE_FLAG		0x80

/* different ACEs depending on AceType 
 * SidStart marks the begin of a SID
 * so the thing finally looks like this:
 * 0: ACE_HEADER
 * 4: ACCESS_MASK
 * 8... : SID
 */
typedef struct _ACCESS_ALLOWED_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} ACCESS_ALLOWED_ACE,*PACCESS_ALLOWED_ACE;

typedef struct _ACCESS_DENIED_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} ACCESS_DENIED_ACE,*PACCESS_DENIED_ACE;

typedef struct _SYSTEM_AUDIT_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} SYSTEM_AUDIT_ACE,*PSYSTEM_AUDIT_ACE;

typedef struct _SYSTEM_ALARM_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} SYSTEM_ALARM_ACE,*PSYSTEM_ALARM_ACE;

typedef enum tagSID_NAME_USE {
	SidTypeUser = 1,
	SidTypeGroup,
	SidTypeDomain,
	SidTypeAlias,
	SidTypeWellKnownGroup,
	SidTypeDeletedAccount,
	SidTypeInvalid,
	SidTypeUnknown
} SID_NAME_USE,*PSID_NAME_USE;

/* Access rights */

#define DELETE                     0x00010000
#define READ_CONTROL               0x00020000
#define WRITE_DAC                  0x00040000
#define WRITE_OWNER                0x00080000
#define SYNCHRONIZE                0x00100000
#define STANDARD_RIGHTS_REQUIRED   0x000f0000

#define STANDARD_RIGHTS_READ       READ_CONTROL
#define STANDARD_RIGHTS_WRITE      READ_CONTROL
#define STANDARD_RIGHTS_EXECUTE    READ_CONTROL

#define STANDARD_RIGHTS_ALL        0x001f0000

#define SPECIFIC_RIGHTS_ALL        0x0000ffff

#define GENERIC_READ               0x80000000
#define GENERIC_WRITE              0x40000000
#define GENERIC_EXECUTE            0x20000000
#define GENERIC_ALL                0x10000000

#define MAXIMUM_ALLOWED            0x02000000
#define ACCESS_SYSTEM_SECURITY     0x01000000

#define EVENT_MODIFY_STATE         0x0002
#define EVENT_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define SEMAPHORE_MODIFY_STATE     0x0002
#define SEMAPHORE_ALL_ACCESS       (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define MUTEX_MODIFY_STATE         0x0001
#define MUTEX_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x1)

#define PROCESS_TERMINATE          0x0001
#define PROCESS_CREATE_THREAD      0x0002
#define PROCESS_VM_OPERATION       0x0008
#define PROCESS_VM_READ            0x0010
#define PROCESS_VM_WRITE           0x0020
#define PROCESS_DUP_HANDLE         0x0040
#define PROCESS_CREATE_PROCESS     0x0080
#define PROCESS_SET_QUOTA          0x0100
#define PROCESS_SET_INFORMATION    0x0200
#define PROCESS_QUERY_INFORMATION  0x0400
#define PROCESS_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0xfff)

#define THREAD_TERMINATE           0x0001
#define THREAD_SUSPEND_RESUME      0x0002
#define THREAD_GET_CONTEXT         0x0008
#define THREAD_SET_CONTEXT         0x0010
#define THREAD_SET_INFORMATION     0x0020
#define THREAD_QUERY_INFORMATION   0x0040
#define THREAD_SET_THREAD_TOKEN    0x0080
#define THREAD_IMPERSONATE         0x0100
#define THREAD_DIRECT_IMPERSONATION 0x0200
#define THREAD_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3ff)

#define THREAD_BASE_PRIORITY_LOWRT  15 
#define THREAD_BASE_PRIORITY_MAX    2 
#define THREAD_BASE_PRIORITY_MIN   -2
#define THREAD_BASE_PRIORITY_IDLE  -15

#define FILE_READ_DATA            0x0001    /* file & pipe */
#define FILE_LIST_DIRECTORY       0x0001    /* directory */
#define FILE_WRITE_DATA           0x0002    /* file & pipe */
#define FILE_ADD_FILE             0x0002    /* directory */
#define FILE_APPEND_DATA          0x0004    /* file */
#define FILE_ADD_SUBDIRECTORY     0x0004    /* directory */
#define FILE_CREATE_PIPE_INSTANCE 0x0004    /* named pipe */
#define FILE_READ_EA              0x0008    /* file & directory */
#define FILE_READ_PROPERTIES      FILE_READ_EA
#define FILE_WRITE_EA             0x0010    /* file & directory */
#define FILE_WRITE_PROPERTIES     FILE_WRITE_EA
#define FILE_EXECUTE              0x0020    /* file */
#define FILE_TRAVERSE             0x0020    /* directory */
#define FILE_DELETE_CHILD         0x0040    /* directory */
#define FILE_READ_ATTRIBUTES      0x0080    /* all */
#define FILE_WRITE_ATTRIBUTES     0x0100    /* all */
#define FILE_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x1ff)

#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ | FILE_READ_DATA | \
                                   FILE_READ_ATTRIBUTES | FILE_READ_EA | \
                                   SYNCHRONIZE)
#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE | FILE_WRITE_DATA | \
                                   FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | \
                                   FILE_APPEND_DATA | SYNCHRONIZE)
#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE | FILE_EXECUTE | \
                                   FILE_READ_ATTRIBUTES | SYNCHRONIZE)


/* File attribute flags
 */
#define FILE_SHARE_READ			0x00000001L
#define FILE_SHARE_WRITE		0x00000002L
#define FILE_SHARE_DELETE		0x00000004L
#define FILE_ATTRIBUTE_READONLY         0x00000001L
#define FILE_ATTRIBUTE_HIDDEN           0x00000002L
#define FILE_ATTRIBUTE_SYSTEM           0x00000004L
#define FILE_ATTRIBUTE_LABEL            0x00000008L  /* Not in Windows API */
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010L
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020L
#define FILE_ATTRIBUTE_NORMAL           0x00000080L
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100L
#define FILE_ATTRIBUTE_ATOMIC_WRITE     0x00000200L
#define FILE_ATTRIBUTE_XACTION_WRITE    0x00000400L
#define FILE_ATTRIBUTE_COMPRESSED       0x00000800L
#define FILE_ATTRIBUTE_OFFLINE		0x00001000L
#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001L
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002L   
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004L   
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008L   
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010L   
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020L   
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040L   
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100L   
#define FILE_ACTION_ADDED               0x00000001L   
#define FILE_ACTION_REMOVED             0x00000002L   
#define FILE_ACTION_MODIFIED            0x00000003L   
#define FILE_ACTION_RENAMED_OLD_NAME    0x00000004L   
#define FILE_ACTION_RENAMED_NEW_NAME    0x00000005L   
#define MAILSLOT_NO_MESSAGE             ((DWORD)-1) 
#define MAILSLOT_WAIT_FOREVER           ((DWORD)-1) 

/* File alignments (NT) */
#define	FILE_BYTE_ALIGNMENT		0x00000000
#define	FILE_WORD_ALIGNMENT		0x00000001
#define	FILE_LONG_ALIGNMENT		0x00000003
#define	FILE_QUAD_ALIGNMENT		0x00000007
#define	FILE_OCTA_ALIGNMENT		0x0000000f
#define	FILE_32_BYTE_ALIGNMENT		0x0000001f
#define	FILE_64_BYTE_ALIGNMENT		0x0000003f
#define	FILE_128_BYTE_ALIGNMENT		0x0000007f
#define	FILE_256_BYTE_ALIGNMENT		0x000000ff
#define	FILE_512_BYTE_ALIGNMENT		0x000001ff

/* Registry security values */
#define OWNER_SECURITY_INFORMATION  0x00000001
#define GROUP_SECURITY_INFORMATION  0x00000002
#define DACL_SECURITY_INFORMATION   0x00000004
#define SACL_SECURITY_INFORMATION   0x00000008


#define EVENTLOG_SUCCESS                0x0000
#define EVENTLOG_ERROR_TYPE             0x0001
#define EVENTLOG_WARNING_TYPE           0x0002
#define EVENTLOG_INFORMATION_TYPE       0x0004
#define EVENTLOG_AUDIT_SUCCESS          0x0008
#define EVENTLOG_AUDIT_FAILURE          0x0010

#define SERVICE_BOOT_START   0x00000000
#define SERVICE_SYSTEM_START 0x00000001
#define SERVICE_AUTO_START   0x00000002
#define SERVICE_DEMAND_START 0x00000003
#define SERVICE_DISABLED     0x00000004

#define SERVICE_ERROR_IGNORE   0x00000000
#define SERVICE_ERROR_NORMAL   0x00000001
#define SERVICE_ERROR_SEVERE   0x00000002
#define SERVICE_ERROR_CRITICAL 0x00000003

/* Service types */
#define SERVICE_KERNEL_DRIVER      0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER 0x00000002
#define SERVICE_ADAPTER            0x00000004
#define SERVICE_RECOGNIZER_DRIVER  0x00000008

#define SERVICE_DRIVER ( SERVICE_KERNEL_DRIVER | SERVICE_FILE_SYSTEM_DRIVER | \
                         SERVICE_RECOGNIZER_DRIVER )

#define SERVICE_WIN32_OWN_PROCESS   0x00000010
#define SERVICE_WIN32_SHARE_PROCESS 0x00000020
#define SERVICE_WIN32  (SERVICE_WIN32_OWN_PROCESS | SERVICE_WIN32_SHARE_PROCESS)

#define SERVICE_INTERACTIVE_PROCESS 0x00000100

#define SERVICE_TYPE_ALL ( SERVICE_WIN32 | SERVICE_ADAPTER | \
                           SERVICE_DRIVER | SERVICE_INTERACTIVE_PROCESS )


typedef enum _CM_SERVICE_NODE_TYPE 
{
  DriverType               = SERVICE_KERNEL_DRIVER,
  FileSystemType           = SERVICE_FILE_SYSTEM_DRIVER,
  Win32ServiceOwnProcess   = SERVICE_WIN32_OWN_PROCESS,
  Win32ServiceShareProcess = SERVICE_WIN32_SHARE_PROCESS,
  AdapterType              = SERVICE_ADAPTER,
  RecognizerType           = SERVICE_RECOGNIZER_DRIVER
} SERVICE_NODE_TYPE;

typedef enum _CM_SERVICE_LOAD_TYPE 
{
  BootLoad    = SERVICE_BOOT_START,
  SystemLoad  = SERVICE_SYSTEM_START,
  AutoLoad    = SERVICE_AUTO_START,
  DemandLoad  = SERVICE_DEMAND_START,
  DisableLoad = SERVICE_DISABLED
} SERVICE_LOAD_TYPE;

typedef enum _CM_ERROR_CONTROL_TYPE 
{
  IgnoreError   = SERVICE_ERROR_IGNORE,
  NormalError   = SERVICE_ERROR_NORMAL,
  SevereError   = SERVICE_ERROR_SEVERE,
  CriticalError = SERVICE_ERROR_CRITICAL
} SERVICE_ERROR_TYPE;


#ifndef __WINE__
#define RtlEqualMemory(Destination, Source, Length) (!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination, Source, Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination, Source, Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination, Length, Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination, Length) memset((Destination),0,(Length))
#endif

//#include "guiddef.h"

typedef struct _RTL_CRITICAL_SECTION_DEBUG 
{
  WORD   Type;
  WORD   CreatorBackTraceIndex;
  struct _RTL_CRITICAL_SECTION *CriticalSection;
  LIST_ENTRY ProcessLocksList;
  DWORD EntryCount;
  DWORD ContentionCount;
  DWORD Spare[ 2 ];
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;
}  RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

#include <peexe.h>

#include "poppack.h"

#endif  /* __WINE_WINNT_H */
