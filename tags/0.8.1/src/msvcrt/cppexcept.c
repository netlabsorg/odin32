/*
 * msvcrt C++ exception handling
 *
 * Copyright 2002 Alexandre Julliard
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
 *
 * NOTES
 * A good reference is the article "How a C++ compiler implements
 * exception handling" by Vishal Kochhar, available on
 * www.thecodeproject.com.
 */
#ifdef __WIN32OS2__
#include <emxheader.h>
#include <string.h>
#include "winbase.h"
#else
#include "config.h"
#endif

#include "wine/port.h"

#include "winternl.h"
#include "msvcrt.h"
#include "wine/exception.h"
#include "excpt.h"
#include "wine/debug.h"

#include "cppexcept.h"

WINE_DEFAULT_DEBUG_CHANNEL(seh);

#ifdef __i386__  /* CxxFrameHandler is not supported on non-i386 */

static DWORD cxx_frame_handler( PEXCEPTION_RECORD rec, cxx_exception_frame* frame,
                                PCONTEXT exc_context, EXCEPTION_FRAME** dispatch,
                                cxx_function_descr *descr, EXCEPTION_FRAME* nested_frame,
                                int nested_trylevel, CONTEXT86 *context );

/* call a function with a given ebp */
inline static void *call_ebp_func( void *func, void *ebp )
{
    void *ret;
    __asm__ __volatile__ ("pushl %%ebp; movl %2,%%ebp; call *%%eax; popl %%ebp" \
                          : "=a" (ret) : "0" (func), "g" (ebp) : "ecx", "edx", "memory" );
    return ret;
}

/* call a copy constructor */
inline static void call_copy_ctor( void *func, void *this, void *src, int has_vbase )
{
    TRACE( "calling copy ctor %p object %p src %p\n", func, this, src );
    if (has_vbase)
        /* in that case copy ctor takes an extra bool indicating whether to copy the base class */
        __asm__ __volatile__("pushl $1; pushl %2; call *%0"
                             : : "r" (func), "c" (this), "g" (src) : "eax", "edx", "memory" );
    else
        __asm__ __volatile__("pushl %2; call *%0"
                             : : "r" (func), "c" (this), "g" (src) : "eax", "edx", "memory" );
}

/* call the destructor of the exception object */
inline static void call_dtor( void *func, void *object )
{
    __asm__ __volatile__("call *%0" : : "r" (func), "c" (object) : "eax", "edx", "memory" );
}


static void dump_type( const cxx_type_info *type )
{
    DPRINTF( "flags %x type %p", type->flags, type->type_info );
    if (type->type_info) DPRINTF( " (%p %s)", type->type_info->name, type->type_info->mangled );
    DPRINTF( " offset %d vbase %d,%d size %d copy ctor %p\n", type->this_offset,
             type->vbase_descr, type->vbase_offset, type->size, type->copy_ctor );
}

static void dump_exception_type( const cxx_exception_type *type )
{
    UINT i;

    DPRINTF( "exception type:\n" );
    DPRINTF( "flags %x destr %p handler %p type info %p\n",
             type->flags, type->destructor, type->custom_handler, type->type_info_table );
    for (i = 0; i < type->type_info_table->count; i++)
    {
        DPRINTF( "    %d: ", i );
        dump_type( type->type_info_table->info[i] );
    }
}

static void dump_function_descr( const cxx_function_descr *descr, const cxx_exception_type *info )
{
    UINT i;
    int j;

    DPRINTF( "function descr:\n" );
    DPRINTF( "magic %x\n", descr->magic );
    DPRINTF( "unwind table: %p %d\n", descr->unwind_table, descr->unwind_count );
    for (i = 0; i < descr->unwind_count; i++)
    {
        DPRINTF( "    %d: prev %d func %p\n", i,
                 descr->unwind_table[i].prev, descr->unwind_table[i].handler );
    }
    DPRINTF( "try table: %p %d\n", descr->tryblock, descr->tryblock_count );
    for (i = 0; i < descr->tryblock_count; i++)
    {
        DPRINTF( "    %d: start %d end %d catchlevel %d catch %p %d\n", i,
                 descr->tryblock[i].start_level, descr->tryblock[i].end_level,
                 descr->tryblock[i].catch_level, descr->tryblock[i].catchblock,
                 descr->tryblock[i].catchblock_count );
        for (j = 0; j < descr->tryblock[i].catchblock_count; j++)
        {
            catchblock_info *ptr = &descr->tryblock[i].catchblock[j];
            DPRINTF( "        %d: flags %x offset %d handler %p type %p",
                     j, ptr->flags, ptr->offset, ptr->handler, ptr->type_info );
            if (ptr->type_info) DPRINTF( " (%p %s)", ptr->type_info->name, ptr->type_info->mangled );
            DPRINTF( "\n" );
        }
    }
}

/* compute the this pointer for a base class of a given type */
static void *get_this_pointer( const cxx_type_info *type, void *object )
{
    void *this_ptr;
    int *offset_ptr;

    if (!object) return NULL;
    this_ptr = (char *)object + type->this_offset;
    if (type->vbase_descr >= 0)
    {
        /* move this ptr to vbase descriptor */
        this_ptr = (char *)this_ptr + type->vbase_descr;
        /* and fetch additional offset from vbase descriptor */
        offset_ptr = (int *)(*(char **)this_ptr + type->vbase_offset);
        this_ptr = (char *)this_ptr + *offset_ptr;
    }
    return this_ptr;
}

/* check if the exception type is caught by a given catch block, and return the type that matched */
static const cxx_type_info *find_caught_type( cxx_exception_type *exc_type, catchblock_info *catchblock )
{
    UINT i;

    for (i = 0; i < exc_type->type_info_table->count; i++)
    {
        const cxx_type_info *type = exc_type->type_info_table->info[i];

        if (!catchblock->type_info) return type;   /* catch(...) matches any type */
        if (catchblock->type_info != type->type_info)
        {
            if (strcmp( catchblock->type_info->mangled, type->type_info->mangled )) continue;
        }
        /* type is the same, now check the flags */
        if ((exc_type->flags & TYPE_FLAG_CONST) &&
            !(catchblock->flags & TYPE_FLAG_CONST)) continue;
        if ((exc_type->flags & TYPE_FLAG_VOLATILE) &&
            !(catchblock->flags & TYPE_FLAG_VOLATILE)) continue;
        return type;  /* it matched */
    }
    return NULL;
}


/* copy the exception object where the catch block wants it */
static void copy_exception( void *object, cxx_exception_frame *frame,
                            catchblock_info *catchblock, const cxx_type_info *type )
{
    void **dest_ptr;

    if (!catchblock->type_info || !catchblock->type_info->mangled[0]) return;
    if (!catchblock->offset) return;
    dest_ptr = (void **)((char *)&frame->ebp + catchblock->offset);

    if (catchblock->flags & TYPE_FLAG_REFERENCE)
    {
        *dest_ptr = get_this_pointer( type, object );
    }
    else if (type->flags & CLASS_IS_SIMPLE_TYPE)
    {
        memmove( dest_ptr, object, type->size );
        /* if it is a pointer, adjust it */
        if (type->size == sizeof(void *)) *dest_ptr = get_this_pointer( type, *dest_ptr );
    }
    else  /* copy the object */
    {
        if (type->copy_ctor)
            call_copy_ctor( type->copy_ctor, dest_ptr, get_this_pointer(type,object),
                            (type->flags & CLASS_HAS_VIRTUAL_BASE_CLASS) );
        else
            memmove( dest_ptr, get_this_pointer(type,object), type->size );
    }
}

/* unwind the local function up to a given trylevel */
static void cxx_local_unwind( cxx_exception_frame* frame, cxx_function_descr *descr, int last_level)
{
    void (*handler)();
    int trylevel = frame->trylevel;

    while (trylevel != last_level)
    {
        if (trylevel < 0 || trylevel >= descr->unwind_count)
        {
            ERR( "invalid trylevel %d\n", trylevel );
            MSVCRT_terminate();
        }
        handler = descr->unwind_table[trylevel].handler;
        if (handler)
        {
            TRACE( "calling unwind handler %p trylevel %d last %d ebp %p\n",
                   handler, trylevel, last_level, &frame->ebp );
            call_ebp_func( handler, &frame->ebp );
        }
        trylevel = descr->unwind_table[trylevel].prev;
    }
    frame->trylevel = last_level;
}

/* exception frame for nested exceptions in catch block */
struct catch_func_nested_frame
{
    EXCEPTION_FRAME      frame;     /* standard exception frame */
    EXCEPTION_RECORD    *prev_rec;  /* previous record to restore in thread data */
    cxx_exception_frame *cxx_frame; /* frame of parent exception */
    cxx_function_descr  *descr;     /* descriptor of parent exception */
    int                  trylevel;  /* current try level */
};

/* handler for exceptions happening while calling a catch function */
static DWORD catch_function_nested_handler( EXCEPTION_RECORD *rec, EXCEPTION_FRAME *frame,
                                            CONTEXT *context, EXCEPTION_FRAME **dispatcher )
{
    struct catch_func_nested_frame *nested_frame = (struct catch_func_nested_frame *)frame;

    if (rec->ExceptionFlags & (EH_UNWINDING | EH_EXIT_UNWIND))
    {
        msvcrt_get_thread_data()->exc_record = nested_frame->prev_rec;
        return ExceptionContinueSearch;
    }
    else
    {
        TRACE( "got nested exception in catch function\n" );
        return cxx_frame_handler( rec, nested_frame->cxx_frame, context,
                                  NULL, nested_frame->descr, &nested_frame->frame,
                                  nested_frame->trylevel, context );
    }
}

/* find and call the appropriate catch block for an exception */
/* returns the address to continue execution to after the catch block was called */
inline static void *call_catch_block( PEXCEPTION_RECORD rec, cxx_exception_frame *frame,
                                      cxx_function_descr *descr, int nested_trylevel,
                                      cxx_exception_type *info )
{
    UINT i;
    int j;
    void *addr, *object = (void *)rec->ExceptionInformation[1];
    struct catch_func_nested_frame nested_frame;
    int trylevel = frame->trylevel;
    MSVCRT_thread_data *thread_data = msvcrt_get_thread_data();

    for (i = 0; i < descr->tryblock_count; i++)
    {
        tryblock_info *tryblock = &descr->tryblock[i];

        if (trylevel < tryblock->start_level) continue;
        if (trylevel > tryblock->end_level) continue;

        /* got a try block */
        for (j = 0; j < tryblock->catchblock_count; j++)
        {
            catchblock_info *catchblock = &tryblock->catchblock[j];
            const cxx_type_info *type = find_caught_type( info, catchblock );
            if (!type) continue;

            TRACE( "matched type %p in tryblock %d catchblock %d\n", type, i, j );

            /* copy the exception to its destination on the stack */
            copy_exception( object, frame, catchblock, type );

            /* unwind the stack */
            RtlUnwind( frame, 0, rec, 0 );
            cxx_local_unwind( frame, descr, tryblock->start_level );
            frame->trylevel = tryblock->end_level + 1;

            /* call the catch block */
            TRACE( "calling catch block %p for type %p addr %p ebp %p\n",
                   catchblock, type, catchblock->handler, &frame->ebp );

            /* setup an exception block for nested exceptions */

            nested_frame.frame.Handler = catch_function_nested_handler;
            nested_frame.prev_rec  = thread_data->exc_record;
            nested_frame.cxx_frame = frame;
            nested_frame.descr     = descr;
            nested_frame.trylevel  = nested_trylevel + 1;

            __wine_push_frame( &nested_frame.frame );
            thread_data->exc_record = rec;
            addr = call_ebp_func( catchblock->handler, &frame->ebp );
            thread_data->exc_record = nested_frame.prev_rec;
            __wine_pop_frame( &nested_frame.frame );

            if (info->destructor) call_dtor( info->destructor, object );
            TRACE( "done, continuing at %p\n", addr );
            return addr;
        }
    }
    return NULL;
}


/*********************************************************************
 *		cxx_frame_handler
 *
 * Implementation of __CxxFrameHandler.
 */
static DWORD cxx_frame_handler( PEXCEPTION_RECORD rec, cxx_exception_frame* frame,
                                PCONTEXT exc_context, EXCEPTION_FRAME** dispatch,
                                cxx_function_descr *descr, EXCEPTION_FRAME* nested_frame,
                                int nested_trylevel, CONTEXT86 *context )
{
    cxx_exception_type *exc_type;
    void *next_ip;

    if (descr->magic != CXX_FRAME_MAGIC)
    {
        ERR( "invalid frame magic %x\n", descr->magic );
        return ExceptionContinueSearch;
    }
    if (rec->ExceptionFlags & (EH_UNWINDING|EH_EXIT_UNWIND))
    {
        if (descr->unwind_count && !nested_trylevel) cxx_local_unwind( frame, descr, -1 );
        return ExceptionContinueSearch;
    }
    if (!descr->tryblock_count) return ExceptionContinueSearch;

    exc_type = (cxx_exception_type *)rec->ExceptionInformation[2];
    if (rec->ExceptionCode == CXX_EXCEPTION &&
        rec->ExceptionInformation[0] > CXX_FRAME_MAGIC &&
        exc_type->custom_handler)
    {
        return exc_type->custom_handler( rec, frame, exc_context, dispatch,
                                         descr, nested_trylevel, nested_frame, 0 );
    }

    if (!exc_type)  /* nested exception, fetch info from original exception */
    {
        rec = msvcrt_get_thread_data()->exc_record;
        exc_type = (cxx_exception_type *)rec->ExceptionInformation[2];
    }

    if (TRACE_ON(seh))
    {
        TRACE("handling C++ exception rec %p frame %p trylevel %d descr %p nested_frame %p\n",
              rec, frame, frame->trylevel, descr, nested_frame );
        dump_exception_type( exc_type );
        dump_function_descr( descr, exc_type );
    }

    next_ip = call_catch_block( rec, frame, descr, frame->trylevel, exc_type );

    if (!next_ip) return ExceptionContinueSearch;
    rec->ExceptionFlags &= ~EH_NONCONTINUABLE;
    context->Eip = (DWORD)next_ip;
    context->Ebp = (DWORD)&frame->ebp;
    context->Esp = ((DWORD*)frame)[-1];
    return ExceptionContinueExecution;
}

/*********************************************************************
 *		__CxxFrameHandler (MSVCRT.@)
 */

#ifdef __WIN32OS2__
DEFINE_REGS_ENTRYPOINT_4( __CxxFrameHandler, EXC__CxxFrameHandler, PEXCEPTION_RECORD,
                          EXCEPTION_FRAME*, PCONTEXT ,EXCEPTION_FRAME**);

void EXC__CxxFrameHandler( PEXCEPTION_RECORD rec, EXCEPTION_FRAME* frame,
#else
void __CxxFrameHandler( PEXCEPTION_RECORD rec, EXCEPTION_FRAME* frame,
#endif
                        PCONTEXT exc_context, EXCEPTION_FRAME** dispatch,
                        CONTEXT86 *context )
{
    cxx_function_descr *descr = (cxx_function_descr *)context->Eax;
    context->Eax = cxx_frame_handler( rec, (cxx_exception_frame *)frame,
                                      exc_context, dispatch, descr, NULL, 0, context );
}

#endif  /* __i386__ */

/*********************************************************************
 *		_CxxThrowException (MSVCRT.@)
 */
void _CxxThrowException( void *object, const cxx_exception_type *type )
{
    DWORD args[3];

    args[0] = CXX_FRAME_MAGIC;
    args[1] = (DWORD)object;
    args[2] = (DWORD)type;
    RaiseException( CXX_EXCEPTION, EH_NONCONTINUABLE, 3, args );
}

/*********************************************************************
 *		__CxxDetectRethrow (MSVCRT.@)
 */
BOOL __CxxDetectRethrow(PEXCEPTION_POINTERS ptrs)
{
  PEXCEPTION_RECORD rec;

  if (!ptrs)
    return FALSE;

  rec = ptrs->ExceptionRecord;

  if (rec->ExceptionCode == CXX_EXCEPTION &&
      rec->NumberParameters == 3 &&
      rec->ExceptionInformation[0] == CXX_FRAME_MAGIC &&
      rec->ExceptionInformation[2])
  {
    ptrs->ExceptionRecord = msvcrt_get_thread_data()->exc_record;
    return TRUE;
  }
  return (msvcrt_get_thread_data()->exc_record == rec);
}

/*********************************************************************
 *		__CxxQueryExceptionSize (MSVCRT.@)
 */
unsigned int __CxxQueryExceptionSize(void)
{
  return sizeof(cxx_exception_type);
}
