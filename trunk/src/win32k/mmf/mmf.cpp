/* $Id: mmf.cpp,v 1.1 2000-10-31 17:35:00 bird Exp $
 *
 * Memory Mapped Files.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/** @design     Memory Mapped Files - Ring 0

Support for Memory Mapped Files (MMF) is an excellent feature missing in OS/2.
Several Ring-3 implementations exists. Most of them have problems when calling
OS/2 APIs. APIs like DosRead, DosWrite will at Ring-0 check if the buffer
passed in is valid (all pages commited), and will fail if the pages aren't
commited. And AFAIK the Ring-3 MMF implementations exploits the commit/decommit
feature of DosSetMem.<p>

So, the only way to get this right and fast is to implement it at Ring-0 level.
This is what I (knut) aim to do some day. These are my current thoughts on
the subject. (Oct 31 2000 5:39pm)<p>

What I am think about is to create a Ring-0 class for MMF which maintains all
of the MMF handling. There will be a Ring-3 DLL which provides APIs which
uses IOCtls to communicate with the Ring-0 class. These APIs will be presented
in these forms:
<ul>
    <li>My own "native" APIs.
    <li>UNIX mmap, munmap, msync APIs
    <li>Win32 styled APIs. (if needed)
</ul><p>


@subsection     Loader Exploits (Overloads)

The Ring-0 part will overload the handling of the Ring-3 DLL, that's the
reason why I will insist on having a separate MMF DLL. When the first call to
the MMF is issued we'll find the MMF DLL and "hook" it's handle. This
"hooking" will enable us to make speciall processing in LDRGetPage,
LDRFreeTask and maybe ldrAllocateObjects. All of these LDR functions will have
to be overloaded with my own functions - LDRGetPage will be written in
assembly to minimize latency.<p>


@subsubsection  LDRGetPage

As you will see somewhere else (when this text is completed) we'll allocate
objects my self and give the hMTE of the MMF DLL. The block number will be
relative to the start of each mapping if there is a filebacking of the
object.<p>

So, what we'll actually do is 1st to check that the call isn't for the real
LX objects of the MMF DLL. Since we're overloading the processing of the
MMF DLL we will have to distiguish between calls related to the processing
of the DLL and the processing of our memorymapped objects. Then, if this
is a request for a page in the one of the objects of the MMF DLL we'll simply
call(/jump) to the original LDRGetPage.<p>

If this is a request for a page in one of the memory mapped objects, we'll
have to get that page. Starts out by finding the mapping this is for. If it's
a mapping without filebacking we'll simply return a zero'ed page. Else, we'll
have to read it from the disk. And that's about all we'll have to do.


@subsubsection  LDRFreeTask

Here we'll clean up all our resources associated with this process. I am not
quite sure what this will be; flushing mappings, decreasing usage counts,
eventually releasing objects, closing files.


@subsubsection  ldrAllocObjects

Here we could attach inherited mappings. Not really needed.



@subsection     Interface Ring-0 to Ring-3

An interface between Ring-0 and Ring-3 will have to be as simple as possible,
and yet the Ring-0 would not trust the Ring-3 very much since someone may
call the DosIOCtls them selves. These are the proposed interfaces:
<ul>
    <li>MMFCreating  - Create a mapping handle.
    <li>MMFDuplicate - Duplicates an mapping handle.
    <li>MMFOpen      - Open an existing mapping.
    <li>MMFViewMap   - Creates a view for a part of the file.
    <li>MMFViewUnmap - Flushes and destroys a view.
    <li>MMFViewSync  - Flush a view.
</ul>
This will roughly be the exported "native" APIs too.

@subsubsection  MMFCreating
@subsubsection  MMFDuplicate
@subsubsection  MMFOpen
@subsubsection  MMFViewMap
@subsubsection  MMFViewUnmap
@subsubsection  MMFViewSync


@subsection     Innerworkings of the Ring-0 MMF class(es)

To be written some other day.

 */
