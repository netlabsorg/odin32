/* $Id: mmf.cpp,v 1.2 2001-02-24 04:37:15 bird Exp $
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

The Ring-0 part will create pseudo MTEs for each file handle given in the
openmapping call. These MTEs will not be linked into the mte list, but will
be linked into a private MMF list. By doing it this way we'll be able to use
the LDRGetPage function without overloading it. We just have to feed the
loader with valid pagelists.<p>

We'll have to do cleanups of this internal MTEs by overloading LDRFreeTask.<p>

Objects are allocated by us self using ldrAllocObjects and ldrSetVMFlags will
be overloaded to set the correct flags.


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
    <li>MMFDuplicate - Duplicates a mapping handle.
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


