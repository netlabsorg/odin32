/* $Id: kInterfaces.h,v 1.1 2000-12-04 08:49:00 bird Exp $
 *
 * This headerfile contains interfaces for the common tools classes.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _kInterfaces_h_
#define _kInterfaces_h_

/**
 * Interface class (ie. virtual) which defines the interface for
 * executables (resources and object too perhaps) files used to
 * get and put pages.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kPageI
{
public:
    /** @cat Get and put page methods. */

    /**
     * Get a code, data or resource page from the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      will receive the page data on the specified address.
     * @param   ulAddress   Page address. This will be page aligned.
     */
    virtual BOOL    getPage(char *pachPage, ULONG ulAddress) = 0;

    /**
     * Get a code, data or resource page from the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      will receive the page data on the specified address.
     * @param   iObject     Object number.  (0-based)
     * @param   offObject   Offset into the object.
     * @remark  Object = Section.
     */
    virtual BOOL    getPage(char *pachPage, int iObject, int offObject) = 0;

    /**
     * Updates or adds a code, data, or resource page to the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      holds the page data.
     * @param   ulAddress   Page address. This will be page aligned.
     */
    virtual BOOL    putPage(const char *pachPage, ULONG ulAddress) = 0;

    /**
     * Updates or adds a code, data, or resource page to the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      holds the page data.
     * @param   iObject     Object number. (0-based)
     * @param   offObject   Offset into the object.
     */
    virtual BOOL    putPage(const char *pachPage, int iObject, int offObject) = 0;

    /**
     * Get pagesize for the file.
     * @returns Pagesize in bytes.
     */
    virtual int     getPageSize() = 0;
};

#endif
