/*
 * oList.h
 * -------
 * Simple double linked list class
 */

#ifndef OLIST_INCLUDED
#define OLIST_INCLUDED

class _oList;		// Base list
class _oListElem;	// Element in list
class _oListIter;	// Iterator of list

class _oListElem
{
    friend class _oList;
    friend class _oListIter;

private:
    _oListElem *	m_pNext;
    _oListElem *	m_pPrev;
    _oList *	m_pList;

public:

    ~_oListElem()
	{ Remove(); }

protected:
    _oListElem() : m_pNext(0), m_pPrev(0), m_pList(0)
	{}

    void InsertBefore(_oList * pList, _oListElem * pSibling);

    void InsertAfter(_oList * pList, _oListElem * pSibling);

    void Remove();

    _oListElem * Next() const
	{ return m_pNext; }

    _oListElem * Prev() const
	{ return m_pPrev; }

};

template <class T>
class oListElem : public _oListElem
{
private:
    T		m_Data;

public:
    oListElem(T Data) : _oListElem(), m_Data(Data)
	{}
    virtual ~oListElem(void )
	{}

    oListElem<T> * Next() const
	{ return  (oListElem<T> *)_oListElem::Next(); }

    oListElem<T> * Prev() const
	{ return  (oListElem<T> *)_oListElem::Prev(); }

    T Data(void ) const
	{ return m_Data; }

};

class _oList
{
    friend class _oListElem;
    friend class _oListIter;

public:
    ULONG Count() const
	{return m_ulCount; }

    void Clear()
	{ while (m_pStart) delete m_pStart; }

    void Remove(ULONG index)
	{ delete locate(index); }

    virtual ~_oList()
	{Clear(); }

protected:
    _oList() : m_pStart(0), m_pEnd(0), m_ulCount(0)
	{}

    _oListElem * Start() const
	{ return m_pStart; }

    _oListElem * End() const
	{ return m_pEnd; }

    void AddAtStart(_oListElem * pElem)
	{pElem->InsertBefore(this, 0); }

    void AddAtEnd(_oListElem * pElem)
	{pElem->InsertAfter(this, 0); }

    void AddAfter(_oListElem * pElem, _oListElem * pSibling)
	{pElem->InsertAfter(this, pSibling); }

    void AddBefore(_oListElem * pElem, _oListElem * pSibling)
	{pElem->InsertBefore(this, pSibling); }

    _oListElem * locate(ULONG index) const;

private:
    _oListElem *	m_pStart;
    _oListElem *	m_pEnd;
    ULONG		m_ulCount;
};

template <class T>
class oList : public _oList
{
private:

protected:

public:

    oList() : _oList()
	{};

    T Start() const
	{ return ((oListElem<T> *)_oList::Start())->Data(); }

    T End() const
	{ return ((oListElem<T> *)_oList::End())->Data(); }

    void AddAtStart(T Data)
	{ _oList::AddAtStart(new oListElem<T>(Data)); }

    void AddAtEnd(T Data)
	{ _oList::AddAtEnd(new oListElem<T>(Data)); }

    void AddAfter(T Data, _oListElem * pSibling)
	{ _oList::AddAfter(new oListElem<T>(Data), pSibling); }

    void AddBefore(T Data, _oListElem * pSibling)
	{ _oList::AddBefore(new oListElem<T>(Data), pSibling); }

    T operator[] (ULONG index) const
	{ return ((oListElem<T> *)locate(index))->Data(); }

    };


class _oListIter
{
private:
    _oListElem *	m_pCurr;
    _oList *		m_pList;

protected:
    _oListIter(_oList * pList) : m_pList(pList), m_pCurr(pList->Start())
	{}
    virtual ~_oListIter()
	{}

    _oListElem * Curr()
	{ return m_pCurr; }

public:
    BOOL MoveStart()
	{ return (m_pCurr = m_pList->Start()) != 0; }
    BOOL MoveEnd()
	{ return (m_pCurr = m_pList->End()) != 0; }
    BOOL MoveNext()
	{ return (m_pCurr = m_pCurr->Next()) != 0; }
    BOOL MovePrev()
	{ return (m_pCurr = m_pCurr->Prev()) != 0; }
    BOOL IsValid()
	{ return m_pCurr != 0; }
};

template <class T>
class oListIter : public _oListIter
{
private:

protected:

public:
    oListIter(oList<T> & pList) : _oListIter(&pList)
	{}
    T Element()
	{ return ((oListElem<T> *)Curr())->Data(); }

};





#endif /* OLIST_INCLUDED */

