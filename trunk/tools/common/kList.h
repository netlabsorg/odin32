/* $Id: kList.h,v 1.2 2000-03-25 23:17:47 bird Exp $ */
/*
 * Simple list and sorted list template class.
 * Note: simple list is not implemented yet, as it is not yet needed.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _lkList_h_
#define _lkList_h_

    /**
     * List entry parent function.
     * @purpose     Serve as a base class for list entries.
     * @author      knut st. osmundsen
     */
    class kListEntry
    {
        private:
            kListEntry *pNext;

        public:

            /**
             * Sets the nextpointer.
             * @param     pNewNext  New value of next pointer.
             */
            void setNext(kListEntry *pNewNext)
            {
                pNext = pNewNext;
            }


            /**
             * Get next pointer value.
             * @returns   Next pointer.
             */
            kListEntry *getNext(void)
            {
                return pNext;
            }

            #if 0
                /* !MUST IMPLEMENT! */
                BOOL operator==(const k... &entry) const;
                BOOL operator!=(const k... &entry) const;
                BOOL operator< (const k... &entry) const;
                BOOL operator<=(const k... &entry) const;
                BOOL operator> (const k... &entry) const;
                BOOL operator>=(const k... &entry) const;
            #endif
    };


    /**
     * Sorted List template.
     * @author      knut st. osmundsen
     */
    template <class kEntry>
    class kSortedList
    {
        private:
            kEntry        *pFirst;
            kEntry        *pLast;
            unsigned long  cEntries;

        public:
            kSortedList(void);
            ~kSortedList(void);

            void            destroy(void);
            void            insert(kEntry *pEntry);
            kEntry *        get(const kEntry &entry) const;
            kEntry *        getFirst(void) const;
            kEntry *        getLast(void) const;
            unsigned long   getCount(void) const;
    };


    /**
     * List template. Not Implemented yet.
     * @author      knut st. osmundsen
     */
    template <class kEntry>
    class kList
    {
        private:
            kEntry        *pFirst;
            kEntry        *pLast;
            unsigned long  cEntries;

        public:
            kList(void);
            ~kList(void);

            void            destroy(void);
            void            insert(kEntry *pEntry);
            kEntry *        getFirst(void) const;
            kEntry *        getLast(void) const;
            unsigned long   getCount(void) const;
    };

#endif


