/* $Id: oString.h,v 1.1 2001-04-26 19:26:12 sandervl Exp $ */

#if !defined(OSTRING_INCLUDED)
#define OSTRING_INCLUDED

class oStringBase
{
protected:
    char *	m_strBuf;	// Buffer.

    long	m_bufLen;	// Current allocated length
    				// of buffer (bytes)
    				
    long	m_bytePerChar;	// Number of bytes per character.
    long	m_strLen;	// Length of string 
    				// incl. zero terminator

    HANDLE	m_Heap;		// heap handle.

    static long blockSize;

    oStringBase(long bytePerChar);
    virtual ~oStringBase();

    void freeBuf();
    void getBuf(long newLen);
    void adjBuf(long newLen);
    long calcBufLen(long newLen);
};

class oStringA : public oStringBase
{
public:
    oStringA(void);
    oStringA(int defLen, int fill);
    oStringA(LPCWSTR pUnicode);
    oStringA(LPCSTR pAscii);
    oStringA(REFCLSID pclsId);
    oStringA(const oStringA &ref);
    oStringA(const ULONG val);
    oStringA(const LONG val);
    oStringA(const USHORT val);
    oStringA(const SHORT val);

    operator LPSTR();

    // Assignment
    oStringA operator =  (const oStringA & string);
    oStringA operator =  (LPCSTR pAscii);
    oStringA operator =  (LPCWSTR pUnicode);
    oStringA operator =  (REFCLSID pClsId);

    // Concatenation
    oStringA operator +  (const oStringA & string);
    oStringA operator +  (LPCSTR pAscii);
    oStringA operator +  (LPCWSTR pUnicode);

    // Concatenation & Assignment.
    oStringA operator += (const oStringA & string);
    oStringA operator += (LPCSTR pAscii);
    oStringA operator += (LPCWSTR pUnicode);

};

class oStringW : public oStringBase
{
public:
    oStringW(void);
    oStringW(int defLen, int fill);
    oStringW(LPCWSTR pUnicode);
    oStringW(const wchar_t * pUnicode);
    oStringW(LPCSTR pAscii);
    oStringW(REFCLSID pclsId);
    oStringW(const oStringW &ref);

    operator LPWSTR();

    // Assignment
    oStringW operator =  (const oStringW & string);
    oStringW operator =  (LPCSTR pAscii);
    oStringW operator =  (LPCWSTR pUnicode);
    oStringW operator =  (const wchar_t * pUnicode);
    oStringW operator =  (REFCLSID pClsId);

    // Concatenation
    oStringW operator +  (const oStringW & string);
    oStringW operator +  (LPCSTR pAscii);
    oStringW operator +  (LPCWSTR pUnicode);
    oStringW operator +  (const wchar_t * pUnicode);

    // Concatenation & Assignment.
    oStringW operator += (const oStringW & string);
    oStringW operator += (LPCSTR pAscii);
    oStringW operator += (LPCWSTR pUnicode);
    oStringW operator += (const wchar_t * pUnicode);

};

#endif // OSTRING_INCLUDED
