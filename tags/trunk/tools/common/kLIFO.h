/* $Id: kLIFO.h,v 1.1 1999-09-05 02:09:17 bird Exp $ */
/*
 * Simple LIFO template class.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */
#ifndef _kLIFO_h_
#define _kLIFO_h_

    /**
     * Simple LIFO template class.
     * Node class must implement:
     *      kEntry  *getNext();
     *      void     setNext(kEntry*);
     *      BOOL operator ==(const char *psz) const;
     * @author      knut st. osmundsen
     */
    template <class kEntry>
    class kLIFO
    {
        protected:
            kEntry *pTop;

        public:
            kLIFO(void);
            virtual ~kLIFO(void);

            void            destroy(void);
            virtual void    push(kEntry *pNewEntry);
            virtual kEntry *pop(void);
            virtual kEntry *get(const kEntry *pGetEntry);
            virtual void    unwind(kEntry *pToEntry);
            virtual void    popPush(const kEntry *pToEntry, kLIFO<kEntry> &lifoTo);
            BOOL            isEmpty(void) const;

            virtual kEntry *find(const char *pszKey) const;
            virtual BOOL    exists(const kEntry *pEntry) const;
    };

#endif
