/* $Id: apartment.h,v 1.1 2000-04-27 22:21:35 davidr Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef APARTMENT_INCLUDED
#define APARTMENT_INCLUDED 1

class	Apartment;

class	ObjectContext;

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
enum enAptType
{
    APT_TN = 0,
    APT_MT = 1,
    APT_ST = 2,
    APT_MAIN = 3,
    APT_HOST = 4
};

enum enThreadingModel
{
    THM_UNDEFINED = 0,
    THM_APARTMENT,
    THM_FREE,
    THM_BOTH,
    THM_NEUTRAL
};

// ----------------------------------------------------------------------
// Attributes associated with an object...
// ----------------------------------------------------------------------
class Attributes
{
public:
    enThreadingModel	GetThreadingModel(void ) const
	{ return m_ThreadingModel; }

    BOOL		GetConfiguredComponent(void ) const
	{ return m_fConfiguredComponent; }

protected:

private:
    enThreadingModel	m_ThreadingModel;
    BOOL		m_fConfiguredComponent;

    // Loads more stuff to do with COM+ object context to go here :-)

};

// ----------------------------------------------------------------------
// COntext in which an object runs...
// ----------------------------------------------------------------------
class	ObjectContext
{
private:
    GUID		m_Guid;
    Apartment *		m_pApt;

    // Loads more stuff to do with COM+ object context to go here :-)

protected:

public:

    ObjectContext();

    const GUID &	GetGUID(void ) const		// Object context GUID...
    	{ return m_Guid; }

    const Apartment *	GetApt(void ) const	// Associated apartment...
    	{ return m_pApt; }

    void		SetApt(Apartment * const pApt)
    	{ m_pApt = pApt; }

    // Longhand methods...
    BOOL		IsCompatible(Attributes * const pTarget) const;
};

// ----------------------------------------------------------------------
// Class to represent an apartment.
// Apartments have varing thread affinity dependant on their type.
// ----------------------------------------------------------------------
class Apartment
{
private:
    GUID		m_Guid;
    ObjectContext *	m_pDefaultContext;
    HWND		m_hwndPump;
    enAptType		m_AptType;

protected:

public:

    Apartment(enAptType aptType);
    virtual ~Apartment();

    enAptType		GetAptType(void ) const
    	{ return m_AptType; }

    const GUID &	GetGUID(void ) const
    	{ return m_Guid; }

    ObjectContext * const GetDefaultContext(void ) const
	{ return m_pDefaultContext; }

    BOOL		IsMessageBased(void ) const
	{ return !((m_AptType == APT_MT) || (m_AptType == APT_TN)); }

    // Longhand methods...
    BOOL		IsCompatible(Attributes * const pTarget) const;
};

// ----------------------------------------------------------------------
// Info stored against each thread by COM
// ----------------------------------------------------------------------
class ThreadInfo
{
private:
    LONG		m_RefCount;

    Apartment *		m_pAptHome;
    ObjectContext *	m_pCurContext;

protected:
public:
    ThreadInfo() : m_RefCount(1), m_pAptHome(0), m_pCurContext(0)
	{};

    virtual ~ThreadInfo()
	{ delete m_pAptHome; }

    LONG		AddRef(void )
    	{ return ++m_RefCount; }

    LONG		Release(void )
    	{ return --m_RefCount; }

    Apartment *		GetHomeApt(void )
	{ return m_pAptHome; }

    void		SetHomeApt(Apartment * pApt)
	{ m_pAptHome = pApt; }

    ObjectContext *	GetCurContext(void )
	{ return m_pCurContext; }

    void		SetCurContext(ObjectContext * curContext)
	{ m_pCurContext = curContext; }

};


#endif /* APARTMENT_INCLUDED */
