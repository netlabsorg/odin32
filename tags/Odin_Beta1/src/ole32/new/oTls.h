/* $Id: oTls.h,v 1.1 2000-04-27 22:21:36 davidr Exp $ */
/*
 * Wrapper for the WIN32 TLS functions...
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef OTLS_INCLUDED
#define OTLS_INCLUDED 1

template <class VARPTR>
class oTlsValue
{
private:
    DWORD	m_Index;

protected:

public:
    oTlsValue(VARPTR const defVal = 0) : m_Index(TlsAlloc())
	{ dprintf(("OTLS: TlsAlloc %d", m_Index)); SetValue(defVal); }

    virtual ~oTlsValue()
	{ dprintf(("OTLS: TlsFree %d", m_Index)); TlsFree(m_Index); }

    VARPTR const GetValue(void ) const
	{ return (VARPTR const)TlsGetValue(m_Index); }

    void SetValue(VARPTR const newVal )
	{ TlsSetValue(m_Index, (LPVOID)newVal); }

    operator VARPTR const () const
	{ return GetValue(); }

    VARPTR const operator -> () const
	{ return GetValue(); }

    oTlsValue<VARPTR> operator = (VARPTR const newVal)
	{ SetValue(newVal); return *this; };

};


#endif /* OTLS_INCLUDED */
