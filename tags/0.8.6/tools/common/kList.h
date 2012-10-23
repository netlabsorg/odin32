/* $Id: kList.h,v 1.4 2002-02-24 02:47:28 bird Exp $ */
/*
 * Simple list and sorted list template class.
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


    /**
     * List entry parent function.
     * @purpose     Serve as a base class for list sorted entries.
     * @author      knut st. osmundsen
     */
    class kSortedListEntry : public kListEntry
    {
        public:
            #if 0 //MUST BE IMPLEMENTED!
            virtual KBOOL operator==(const k..Entry &entry) const = 0;
            virtual KBOOL operator!=(const k..Entry &entry) const = 0;
            virtual KBOOL operator< (const k..Entry &entry) const = 0;
            virtual KBOOL operator<=(const k..Entry &entry) const = 0;
            virtual KBOOL operator> (const k..Entry &entry) const = 0;
            virtual KBOOL operator>=(const k..Entry &entry) const = 0;
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


#endif


