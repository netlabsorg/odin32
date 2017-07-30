/*
 * 32-bit spec files
 *
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Martin von Loewis
 * Copyright 1995, 1996, 1997 Alexandre Julliard
 * Copyright 1997 Eric Youngdale
 * Copyright 1999 Ulrich Weigand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"
#include "port.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "windef.h"
#include "winbase.h"
//#include "wine/exception.h"
#include "build.h"

static int string_compare( const void *ptr1, const void *ptr2 )
{
    const char * const *str1 = ptr1;
    const char * const *str2 = ptr2;
    return strcmp( *str1, *str2 );
}


/*******************************************************************
 *         make_internal_name
 *
 * Generate an internal name for an entry point. Used for stubs etc.
 */
static const char *make_internal_name( const ORDDEF *odp, const char *prefix )
{
    static char buffer[256];
    if (odp->name || odp->export_name)
    {
        char *p;
        sprintf( buffer, "__wine_%s_%s_%s", prefix, dll_file_name,
                 odp->name ? odp->name : odp->export_name );
        /* make sure name is a legal C identifier */
        for (p = buffer; *p; p++) if (!isalnum(*p) && *p != '_') break;
        if (!*p) return buffer;
    }
    sprintf( buffer, "__wine_%s_%s_%d", prefix, make_c_identifier(dll_file_name), odp->ordinal );
    return buffer;
}

/*******************************************************************
 *         AssignOrdinals
 *
 * Assign ordinals to all entry points.
 */
static void AssignOrdinals(void)
{
    int i, ordinal;

    if ( !nb_names ) return;

    /* start assigning from Base, or from 1 if no ordinal defined yet */
    if (Base == MAX_ORDINALS) Base = 1;
    for (i = 0, ordinal = Base; i < nb_names; i++)
    {
        if (Names[i]->ordinal != -1) continue;  /* already has an ordinal */
        while (Ordinals[ordinal]) ordinal++;
        if (ordinal >= MAX_ORDINALS)
        {
            current_line = Names[i]->lineno;
            fatal_error( "Too many functions defined (max %d)\n", MAX_ORDINALS );
        }
        Names[i]->ordinal = ordinal;
        Ordinals[ordinal] = Names[i];
    }
    if (ordinal > Limit) Limit = ordinal;
}




/*******************************************************************
 *         output_exports
 *
 * Output the export table for a Win32 module.
 */
static int output_exports( FILE *outfile, int nr_exports)
{
    int i, fwd_size = 0, total_size = 0;

    if (!nr_exports) return 0;

    /* export directory header */
    if(outfile) fprintf( outfile, "dll_file_name db \"%s\"\n\n", dll_file_name);
    if(outfile) fprintf( outfile, "__wine_spec_exports:\n");
    if(outfile) fprintf( outfile, "  dd  0    ; Characteristics\n" );                 /* Characteristics */
    if(outfile) fprintf( outfile, "  dd  0    ; TimeDateStamp\n" );                 /* TimeDateStamp */
    if(outfile) fprintf( outfile, "  dw  0    ; MajorVersion\n" );                 /* MajorVersion */
    if(outfile) fprintf( outfile, "  dw  0    ; MinorVersion\n" );                 /* MinorVersion */
    if(outfile) fprintf( outfile, "  dd  FLAT:dll_file_name   ; Name\n", dll_file_name); /* Name */
    if(outfile) fprintf( outfile, "  dd  %d   ; Base\n", Base );          /* Base */
    if(outfile) fprintf( outfile, "  dd  %d   ; NumberOfFunctions\n", nr_exports );    /* NumberOfFunctions */
    if(outfile) fprintf( outfile, "  dd  %d   ; NumberOfNames\n", nb_names );      /* NumberOfNames */
    if(outfile) fprintf( outfile, "  dd  FLAT:__wine_spec_exports_funcs    ;AddressOfFunctions\n" ); /* AddressOfFunctions */
    if (nb_names)
    {
        if(outfile) fprintf( outfile, "  dd  FLAT:__wine_spec_exp_name_ptrs    ; AddressOfNames\n" );     /* AddressOfNames */
        if(outfile) fprintf( outfile, "  dd  FLAT:__wine_spec_exp_ordinals     ; AddressOfNameOrdinals\n" );  /* AddressOfNameOrdinals */
    }
    else
    {
        if(outfile) fprintf( outfile, "  dd  0    ; AddressOfNames\n" );  /* AddressOfNames */
        if(outfile) fprintf( outfile, "  dd  0    ; AddressOfNameOrdinals\n" );  /* AddressOfNameOrdinals */
    }
    if(outfile) fprintf( outfile, "\n\n" );

    total_size += 10 * sizeof(int);

    /* output the function pointers */

    if(outfile) fprintf( outfile, "__wine_spec_exports_funcs:\n" );
    for (i = Base; i <= Limit; i++)
    {
        ORDDEF *odp = Ordinals[i];
        if (!odp || !odp->link_name) { if(outfile) fprintf( outfile, "    dd 0\n" ); }
        else switch(odp->type)
        {
        case TYPE_EXTERN:
        case TYPE_STDCALL:
        case TYPE_VARARGS:
        case TYPE_CDECL:
            if(outfile) fprintf( outfile, "    dd FLAT:%s\n", 
                     (odp->flags & FLAG_REGISTER) ? make_internal_name(odp,"regs") : odp->link_name );
            break;
        case TYPE_STUB:
//            if(outfile) fprintf( outfile, "    \"\\t.long " __ASM_NAME("%s") "\\n\"\n", make_internal_name( odp, "stub" ) ); 
            break; 
        default:
            assert(0);
        }
    }
    if(outfile) fprintf( outfile, "\n" );
    total_size += (Limit - Base + 1) * sizeof(int);

    if (nb_names)
    {
        /* output the function name pointers */

        int namepos = 0;

        if(outfile) fprintf( outfile, "__wine_spec_exp_name_ptrs:\n" );
        for (i = 0; i < nb_names; i++)
        {
            if(outfile) fprintf( outfile, "    dd FLAT:__wine_spec_exp_names+%d\n", namepos );
            namepos += strlen(Names[i]->name) + 1;
        }
        if(outfile) fprintf( outfile, "\n" );
        total_size += nb_names * sizeof(int);

        /* output the function names */

        if(outfile) fprintf( outfile, "__wine_spec_exp_names:\n" );
        for (i = 0; i < nb_names; i++)
            if(outfile) fprintf( outfile, "   db \"%s\", 0\n", Names[i]->name );
        if(outfile) fprintf( outfile, "\n" );


        /* output the function ordinals */
        if(outfile) fprintf( outfile, "__wine_spec_exp_ordinals: \n" );

        for (i = 0; i < nb_names; i++)
        {
            if(outfile) fprintf( outfile, "   dw %d\n", Names[i]->ordinal - Base );
        }
        total_size += nb_names * sizeof(short);
        if (nb_names % 2)
        {
            if(outfile) fprintf( outfile, "  dw  0\n" );
            total_size += sizeof(short);
        }
        if(outfile) fprintf( outfile, "\n" );
    }

#if 0
    /* output forward strings */

    if (fwd_size)
    {
        if(outfile) fprintf( outfile, "    \"__wine_spec_forwards:\\n\"\n" );
        for (i = Base; i <= Limit; i++)
        {
            ORDDEF *odp = Ordinals[i];
            if (odp && (odp->flags & FLAG_FORWARD))
                if(outfile) fprintf( outfile, "    \"\\t" __ASM_STRING " \\\"%s\\\"\\n\"\n", odp->link_name );
        }
        if(outfile) fprintf( outfile, "    \"\\t.align %d\\n\"\n", get_alignment(4) );
        total_size += (fwd_size + 3) & ~3;
    }

    /* output relays */

    if (debugging)
    {
        for (i = Base; i <= Limit; i++)
        {
            ORDDEF *odp = Ordinals[i];
            unsigned int j, args, mask = 0;
            const char *name;

            /* skip non-existent entry points */
            if (!odp) goto ignore;
            /* skip non-functions */
            if ((odp->type != TYPE_STDCALL) && (odp->type != TYPE_CDECL)) goto ignore;
            /* skip norelay and forward entry points */
            if (odp->flags & (FLAG_NORELAY|FLAG_FORWARD)) goto ignore;

            for (j = 0; odp->u.func.arg_types[j]; j++)
            {
                if (odp->u.func.arg_types[j] == 't') mask |= 1<< (j*2);
                if (odp->u.func.arg_types[j] == 'W') mask |= 2<< (j*2);
            }
            if ((odp->flags & FLAG_RET64) && (j < 16)) mask |= 0x80000000;

            name = odp->link_name;
            args = strlen(odp->u.func.arg_types) * sizeof(int);
            if (odp->flags & FLAG_REGISTER) name = make_internal_name( odp, "regs" );

            switch(odp->type)
            {
            case TYPE_STDCALL:
                fprintf( outfile, "    \"\\tjmp " __ASM_NAME("%s") "\\n\"\n", name );
                fprintf( outfile, "    \"\\tret $%d\\n\"\n", args );
                fprintf( outfile, "    \"\\t.long " __ASM_NAME("%s") ",0x%08x\\n\"\n", name, mask );
                break;
            case TYPE_CDECL:
                fprintf( outfile, "    \"\\tjmp " __ASM_NAME("%s") "\\n\"\n", name );
                fprintf( outfile, "    \"\\tret\\n\"\n" );
                fprintf( outfile, "    \"\\t" __ASM_SHORT " %d\\n\"\n", args );
                fprintf( outfile, "    \"\\t.long " __ASM_NAME("%s") ",0x%08x\\n\"\n", name, mask );
                break;
            default:
                assert(0);
            }
            continue;

        ignore:
            fprintf( outfile, "    \"\\t.long 0,0,0,0\\n\"\n" );
        }
    }

    fprintf( outfile, "    \"\\t.text\\n\"\n" );
    fprintf( outfile, "    \"\\t.align %d\\n\"\n", get_alignment(4) );
    fprintf( outfile, ");\n\n" );
#endif

    return total_size;
}




/*******************************************************************
 *         output_register_funcs
 *
 * Output the functions for register entry points
 */
static void output_register_funcs( FILE *outfile )
{
    const char *name;
    int i;

    for (i = 0; i < nb_entry_points; i++)
    {
        ORDDEF *odp = EntryPoints[i];
        if (odp->type != TYPE_STDCALL && odp->type != TYPE_CDECL) continue;

#if 1
        if(odp->link_name) 
            fprintf (outfile, "        extrn %s : near\n", odp->link_name);
#else
        if (!(odp->flags & FLAG_REGISTER)) continue;
        if (odp->flags & FLAG_FORWARD) continue;
        name = make_internal_name( odp, "regs" );
        fprintf( outfile,
                 "asm(\".align %d\\n\\t\"\n"
                 "    \"" __ASM_FUNC("%s") "\\n\\t\"\n"
                 "    \"" __ASM_NAME("%s") ":\\n\\t\"\n"
                 "    \"call " __ASM_NAME("__wine_call_from_32_regs") "\\n\\t\"\n"
                 "    \".long " __ASM_NAME("%s") "\\n\\t\"\n"
                 "    \".byte %d,%d\");\n",
                 get_alignment(4),
                 name, name, odp->link_name,
                 strlen(odp->u.func.arg_types) * sizeof(int),
                 (odp->type == TYPE_CDECL) ? 0 : (strlen(odp->u.func.arg_types) * sizeof(int)) );
#endif
    }
}


/*******************************************************************
 *         output_dll_init
 *
 * Output code for calling a dll constructor and destructor.
 */
void output_dll_init( FILE *outfile, const char *constructor, const char *destructor )
{
#if 0
    fprintf( outfile, "#ifndef __GNUC__\n" );
    fprintf( outfile, "static void __asm__dummy_dll_init(void) {\n" );
    fprintf( outfile, "#endif\n" );
#endif
#if defined(__i386__)
    if (constructor)
    {
        fprintf( outfile, "asm(\"\\t.section\\t\\\".init\\\" ,\\\"ax\\\"\\n\"\n" );
        fprintf( outfile, "    \"\\tcall " __ASM_NAME("%s") "\\n\"\n", constructor );
        fprintf( outfile, "    \"\\t.section\\t\\\".text\\\"\\n\");\n" );
    }
    if (destructor)
    {
        fprintf( outfile, "asm(\"\\t.section\\t\\\".fini\\\" ,\\\"ax\\\"\\n\"\n" );
        fprintf( outfile, "    \"\\tcall " __ASM_NAME("%s") "\\n\"\n", destructor );
        fprintf( outfile, "    \"\\t.section\\t\\\".text\\\"\\n\");\n" );
    }
#elif defined(__sparc__)
    if (constructor)
    {
        fprintf( outfile, "asm(\"\\t.section\\t\\\".init\\\" ,\\\"ax\\\"\\n\"\n" );
        fprintf( outfile, "    \"\\tcall " __ASM_NAME("%s") "\\n\"\n", constructor );
        fprintf( outfile, "    \"\\tnop\\n\"\n" );
        fprintf( outfile, "    \"\\t.section\\t\\\".text\\\"\\n\");\n" );
    }
    if (destructor)
    {
        fprintf( outfile, "asm(\"\\t.section\\t\\\".fini\\\" ,\\\"ax\\\"\\n\"\n" );
        fprintf( outfile, "    \"\\tcall " __ASM_NAME("%s") "\\n\"\n", destructor );
        fprintf( outfile, "    \"\\tnop\\n\"\n" );
        fprintf( outfile, "    \"\\t.section\\t\\\".text\\\"\\n\");\n" );
    }
#elif defined(__PPC__)
    if (constructor)
    {
        fprintf( outfile, "asm(\"\\t.section\\t\\\".init\\\" ,\\\"ax\\\"\\n\"\n" );
        fprintf( outfile, "    \"\\tbl " __ASM_NAME("%s") "\\n\"\n", constructor );
        fprintf( outfile, "    \"\\t.section\\t\\\".text\\\"\\n\");\n" );
    }
    if (destructor)
    {
        fprintf( outfile, "asm(\"\\t.section\\t\\\".fini\\\" ,\\\"ax\\\"\\n\"\n" );
        fprintf( outfile, "    \"\\tbl " __ASM_NAME("%s") "\\n\"\n", destructor );
        fprintf( outfile, "    \"\\t.section\\t\\\".text\\\"\\n\");\n" );
    }
#else
#error You need to define the DLL constructor for your architecture
#endif
#if 0
    fprintf( outfile, "#ifndef __GNUC__\n" );
    fprintf( outfile, "}\n" );
    fprintf( outfile, "#endif\n" );
#endif
}


/*******************************************************************
 *         BuildSpec32File
 *
 * Build a Win32 C file from a spec file.
 */
void BuildSpec32File( FILE *outfile )
{
    int exports_size = 0, i;
    int nr_exports, nr_imports, nr_resources;
    int characteristics, subsystem;
    DWORD page_size;
    char constructor[100];

#ifdef HAVE_GETPAGESIZE
    page_size = getpagesize();
#elif defined(__svr4__)
    page_size = sysconf(_SC_PAGESIZE);
#elif defined(_WINDOWS)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        page_size = si.dwPageSize;
    }
#else
#   error Cannot get the page size on this platform
#endif

    AssignOrdinals();
    nr_exports = Base <= Limit ? Limit - Base + 1 : 0;
    output_standard_file_header( outfile );

    fprintf( outfile, ".386p\n\n");
    fprintf( outfile, "DATA32	segment dword use32 public 'DATA'\n");
    fprintf( outfile, "DATA32	ends\n");
    fprintf( outfile, "CONST32_RO	segment dword use32 public 'CONST'\n");
    fprintf( outfile, "CONST32_RO	ends\n");
    fprintf( outfile, "BSS32	segment dword use32 public 'BSS'\n");
    fprintf( outfile, "BSS32	ends\n");
    fprintf( outfile, "DGROUP	group BSS32, DATA32\n");
    fprintf( outfile, "	assume	cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT\n\n");

    if (nr_exports)
    {
        /* Output the stub functions */
        /* Output code for all register functions */

        fprintf( outfile, "CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'\n");
        output_register_funcs( outfile );
        fprintf( outfile, "CODE32          ENDS\n\n");

        /* Calc size of the exports and relay entry points */
        exports_size = output_exports( NULL, nr_exports );

    }

    characteristics = subsystem = 0;
    characteristics = IMAGE_FILE_DLL;

    /* Output the NT header */
    fprintf( outfile, "DATA32	segment dword use32 public 'DATA'\n\n");

    /* this is the IMAGE_NT_HEADERS structure, but we cannot include winnt.h here */
    fprintf( outfile, ";;struct image_nt_headers\n;;{\n" );
    fprintf( outfile, ";;  char dosHeader[16*15];\n");
    fprintf( outfile, ";;  int Signature;\n" );
    fprintf( outfile, ";;  struct file_header {\n" );
    fprintf( outfile, ";;    short Machine;\n" );
    fprintf( outfile, ";;    short NumberOfSections;\n" );
    fprintf( outfile, ";;    int   TimeDateStamp;\n" );
    fprintf( outfile, ";;    void *PointerToSymbolTable;\n" );
    fprintf( outfile, ";;    int   NumberOfSymbols;\n" );
    fprintf( outfile, ";;    short SizeOfOptionalHeader;\n" );
    fprintf( outfile, ";;    short Characteristics;\n" );
    fprintf( outfile, ";;  } FileHeader;\n" );
    fprintf( outfile, ";;  struct opt_header {\n" );
    fprintf( outfile, ";;    short Magic;\n" );
    fprintf( outfile, ";;    char  MajorLinkerVersion, MinorLinkerVersion;\n" );
    fprintf( outfile, ";;    int   SizeOfCode;\n" );
    fprintf( outfile, ";;    int   SizeOfInitializedData;\n" );
    fprintf( outfile, ";;   int   SizeOfUninitializedData;\n" );
    fprintf( outfile, ";;    void *AddressOfEntryPoint;\n" );
    fprintf( outfile, ";;    void *BaseOfCode;\n" );
    fprintf( outfile, ";;    void *BaseOfData;\n" );
    fprintf( outfile, ";;    void *ImageBase;\n" );
    fprintf( outfile, ";;    int   SectionAlignment;\n" );
    fprintf( outfile, ";;    int   FileAlignment;\n" );
    fprintf( outfile, ";;    short MajorOperatingSystemVersion;\n" );
    fprintf( outfile, ";;    short MinorOperatingSystemVersion;\n" );
    fprintf( outfile, ";;   short MajorImageVersion;\n" );
    fprintf( outfile, ";;    short MinorImageVersion;\n" );
    fprintf( outfile, ";;    short MajorSubsystemVersion;\n" );
    fprintf( outfile, ";;    short MinorSubsystemVersion;\n" );
    fprintf( outfile, ";;   int   Win32VersionValue;\n" );
    fprintf( outfile, ";;    int   SizeOfImage;\n" );
    fprintf( outfile, ";;    int   SizeOfHeaders;\n" );
    fprintf( outfile, ";;    int   CheckSum;\n" );
    fprintf( outfile, ";;    short Subsystem;\n" );
    fprintf( outfile, ";;    short DllCharacteristics;\n" );
    fprintf( outfile, ";;    int   SizeOfStackReserve;\n" );
    fprintf( outfile, ";;    int   SizeOfStackCommit;\n" );
    fprintf( outfile, ";;   int   SizeOfHeapReserve;\n" );
    fprintf( outfile, ";;    int   SizeOfHeapCommit;\n" );
    fprintf( outfile, ";;    int   LoaderFlags;\n" );
    fprintf( outfile, ";;    int   NumberOfRvaAndSizes;\n" );
    fprintf( outfile, ";;    struct { const void *VirtualAddress; int Size; } DataDirectory[%d];\n",
             IMAGE_NUMBEROF_DIRECTORY_ENTRIES );
    fprintf( outfile, ";;  } OptionalHeader;\n" );
    fprintf( outfile, " PUBLIC nt_%s_header\n", dll_name);
    fprintf( outfile, " nt_%s_header:\n", dll_name);
    fprintf( outfile, "  db 04Dh, 05Ah, 000h, 000h, 001h, 000h, 000h, 000h, 004h, 000h, 000h, 000h, 0FFh, 0FFh, 00Bh, 000h\n");
    fprintf( outfile, "  db 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 040h, 000h, 000h, 000h, 001h, 000h, 000h, 000h\n");
    fprintf( outfile, "  db 06Ah, 072h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h\n");
    fprintf( outfile, "  db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0F0h, 000h, 000h, 000h\n");
    fprintf( outfile, "  db 0B4h, 030h, 0CDh, 021h, 086h, 0C4h, 03Dh, 00Ah, 014h, 072h, 042h, 0BEh, 080h, 000h, 08Ah, 01Ch\n");
    fprintf( outfile, "  db 032h, 0FFh, 046h, 088h, 038h, 02Eh, 08Ch, 01Eh, 06Eh, 000h, 08Eh, 006h, 02Ch, 000h, 033h, 0C0h\n");
    fprintf( outfile, "  db 08Bh, 0F8h, 0B9h, 000h, 080h, 0FCh, 0F2h, 0AEh, 075h, 023h, 049h, 078h, 020h, 0AEh, 075h, 0F6h\n");
    fprintf( outfile, "  db 047h, 047h, 00Eh, 01Fh, 02Eh, 089h, 03Eh, 068h, 000h, 02Eh, 08Ch, 006h, 06Ah, 000h, 0BEh, 05Ch\n");
    fprintf( outfile, "  db 000h, 0B9h, 06Ch, 063h, 0BBh, 025h, 000h, 0B4h, 064h, 0CDh, 021h, 073h, 00Bh, 0BAh, 07Ch, 000h\n");
    fprintf( outfile, "  db 00Eh, 01Fh, 0B4h, 009h, 0CDh, 021h, 0B0h, 001h, 0B4h, 04Ch, 0CDh, 021h, 020h, 000h, 001h, 000h\n");
    fprintf( outfile, "  db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 081h, 000h, 000h, 000h\n");
    fprintf( outfile, "  db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 003h, 000h, 054h, 068h, 069h, 073h\n");
    fprintf( outfile, "  db 020h, 070h, 072h, 06Fh, 067h, 072h, 061h, 06Dh, 020h, 06Dh, 075h, 073h, 074h, 020h, 062h, 065h\n");
    fprintf( outfile, "  db 020h, 072h, 075h, 06Eh, 020h, 075h, 06Eh, 064h, 065h, 072h, 020h, 057h, 069h, 06Eh, 033h, 032h\n");
    fprintf( outfile, "  db 02Eh, 00Dh, 00Ah, 024h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h\n");
    fprintf( outfile, "  dd %04xh\n", IMAGE_NT_SIGNATURE );   /* Signature */

    fprintf( outfile, "  dw %04xh\n", IMAGE_FILE_MACHINE_I386 );  /* Machine */
    fprintf( outfile, "  dw 1\n");		/* NumberOfSections */
    fprintf( outfile, "  dd 0, 0, 0\n" );
    fprintf( outfile, "  dw %d\n", sizeof(IMAGE_OPTIONAL_HEADER) ); /* SizeOfOptionalHeader */
    fprintf( outfile, "  dw %04xh\n", characteristics );        /* Characteristics */

    fprintf( outfile, "  dw %04xh\n", IMAGE_NT_OPTIONAL_HDR_MAGIC );  /* Magic */
    fprintf( outfile, "  db 0, 0\n" );                   /* Major/MinorLinkerVersion */
    fprintf( outfile, "  dd 0, 0, 0\n" );                /* SizeOfCode/Data */
    fprintf( outfile, "  dd 0\n" );  /* AddressOfEntryPoint */
    fprintf( outfile, "  dd 0, 0\n" );                   /* BaseOfCode/Data */
    fprintf( outfile, "  dd  FLAT:nt_%s_header\n" , dll_name);              /* ImageBase */
    fprintf( outfile, "  dd  %ld\n", page_size );         /* SectionAlignment */
    fprintf( outfile, "  dd  %ld\n", page_size );         /* FileAlignment */
    fprintf( outfile, "  dw  1, 0\n" );                   /* Major/MinorOperatingSystemVersion */
    fprintf( outfile, "  dw  0, 0\n" );                   /* Major/MinorImageVersion */
    fprintf( outfile, "  dw  4, 0\n" );                   /* Major/MinorSubsystemVersion */
    fprintf( outfile, "  dd  0\n" );                      /* Win32VersionValue */
    fprintf( outfile, "  dd  %ld\n", page_size );         /* SizeOfImage */
    fprintf( outfile, "  dd  %ld\n", page_size );         /* SizeOfHeaders */
    fprintf( outfile, "  dd  0\n" );                      /* CheckSum */
    fprintf( outfile, "  dw  %04xh\n", subsystem );      /* Subsystem */
    fprintf( outfile, "  dw  0\n" );                      /* DllCharacteristics */
    fprintf( outfile, "  dd  %d, 0\n", stack_size*1024 ); /* SizeOfStackReserve/Commit */
    fprintf( outfile, "  dd  %d, 0\n", DLLHeapSize*1024 );/* SizeOfHeapReserve/Commit */
    fprintf( outfile, "  dd  0\n" );                      /* LoaderFlags */
//    fprintf( outfile, " dd  %d\n", IMAGE_NUMBEROF_DIRECTORY_ENTRIES );  /* NumberOfRvaAndSizes */
    fprintf( outfile, "  dd  %d\n", 1 );  /* NumberOfRvaAndSizes */
    fprintf( outfile, "  dd  %s, %d \n",  /* IMAGE_DIRECTORY_ENTRY_EXPORT */
             exports_size ? "FLAT:__wine_spec_exports" : "0", exports_size );
    for(i=0;i<15;i++)
        fprintf( outfile, "  dd  0, 0\n");
    fprintf( outfile, "\n");

    if(exports_size) {
     fprintf( outfile, "  ;;#define	IMAGE_SIZEOF_SHORT_NAME	8\n");
     fprintf( outfile, "  ;;typedef struct _IMAGE_SECTION_HEADER {\n");
     fprintf( outfile, "  ;;	BYTE	Name[IMAGE_SIZEOF_SHORT_NAME];	\n");
     fprintf( outfile, "  ;;	union {\n");
     fprintf( outfile, "  ;;		DWORD	PhysicalAddress;	\n");
     fprintf( outfile, "  ;;		DWORD	VirtualSize;		\n");
     fprintf( outfile, "  ;;	} Misc;\n");
     fprintf( outfile, "  ;;	DWORD	VirtualAddress;			\n");
     fprintf( outfile, "  ;;	DWORD	SizeOfRawData;			\n");
     fprintf( outfile, "  ;;	DWORD	PointerToRawData;		\n");
     fprintf( outfile, "  ;;	DWORD	PointerToRelocations;		\n");
     fprintf( outfile, "  ;;	DWORD	PointerToLinenumbers;		\n");
     fprintf( outfile, "  ;;	WORD	NumberOfRelocations;		\n");
     fprintf( outfile, "  ;;	WORD	NumberOfLinenumbers;		\n");
     fprintf( outfile, "  ;;	DWORD	Characteristics;		\n");
     fprintf( outfile, "  ;;						\n");
     fprintf( outfile, "  ;;} IMAGE_SECTION_HEADER,*PIMAGE_SECTION_HEADER;\n");
     fprintf( outfile, "__wine_export_section:\n");
     fprintf( outfile, "  db \".edata\", 0, 0\n");
     fprintf( outfile, "  dd %d\n", exports_size);
     fprintf( outfile, "  dd FLAT:__wine_spec_exports\n");
     fprintf( outfile, "  dd 0\n");
     fprintf( outfile, "  dd 0\n");
     fprintf( outfile, "  dd 0\n");
     fprintf( outfile, "  dd 0\n");
     fprintf( outfile, "  dw 0\n");
     fprintf( outfile, "  dw 0\n");
     fprintf( outfile, "  dd 0\n");
    }

    if (nr_exports)
    {
        /* Output the exports and relay entry points */
        exports_size = output_exports( outfile, nr_exports );
    }

    fprintf( outfile, "DATA32	ends\n");
    fprintf( outfile, "            END\n");

}


