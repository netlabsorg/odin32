
#include "ole32.h"

#include "oString.h"

// ======================================================================
// oStringBase - Base stuff for oString classes
// ======================================================================

// Block size.
long oStringBase::blockSize = 32;

// Initialise to nothing...
oStringBase::oStringBase(long bytePerChar)
{
    m_bytePerChar = bytePerChar;
    m_Heap = GetProcessHeap();
}

// Destructor
oStringBase::~oStringBase()
{
    freeBuf();
}

// calculate the buffersize needed for this string (quantised to blockSize)
long oStringBase::calcBufLen(long strLen)
{
    long	tmp;

    tmp = ((strLen * m_bytePerChar) / blockSize) + 1;

    return tmp * blockSize;
}

// Get a buffer of required length...
void oStringBase::getBuf(long strLen)
{
    // Sanity check...
    if (strLen < 1)
    {
    	dprintf(("oStringBase: Warning! ::getBuf - strlen < 1!"));
    	strLen = 1;
    }

    // Allocate new buffer...
    m_bufLen = calcBufLen(strLen);
    m_strBuf = (LPSTR)HeapAlloc(m_Heap, 0, m_bufLen);
    m_strLen = 1;
    m_strBuf[0] = 0;	// Ensure terminated.
}

// Free buffer...
void oStringBase::freeBuf()
{
    HeapFree(m_Heap, 0, m_strBuf);
}

// Re-allocate buffer if more space is required...
void oStringBase::adjBuf(long strLen)
{
    if ((strLen * m_bytePerChar) > m_bufLen)
    {
	long	bufLen = calcBufLen(strLen);

	// Allocate new buffer...
	m_strBuf = (LPSTR)HeapReAlloc(m_Heap, 0, m_strBuf, bufLen);

	// Save new buffer...
	m_bufLen = bufLen;
    }
}

// ======================================================================
// oStringA - String manipulation based on ASCII
// ======================================================================

// Initialise to nothing...
oStringA::oStringA( void) : oStringBase(sizeof(char))
{
    getBuf( 1);	// Incl. terminator...
}

// Initialise to a specified length...
oStringA::oStringA(int defLen)   : oStringBase(sizeof(char))
{
    getBuf( defLen + 1);
}

// Initialise from Unicode string...
oStringA::oStringA(LPCWSTR pUnicode)  : oStringBase(sizeof(char))
{
    long	strLen = lstrlenW(pUnicode) + 1;

    getBuf( strLen);
    UnicodeToAscii((LPWSTR)pUnicode, m_strBuf);
    m_strLen = strLen;
}

// Initialise from ASCII string...
oStringA::oStringA(LPCSTR pAscii)  : oStringBase(sizeof(char))
{
    long	strLen = strlen(pAscii) + 1;

    getBuf( strLen);
    strcpy(m_strBuf, pAscii);
    m_strLen = strLen;
}

// Initialise from another oString
oStringA::oStringA(const oStringA &ref)  : oStringBase(sizeof(char))
{
    getBuf( ref.m_strLen);
    strcpy(m_strBuf, ref.m_strBuf);
    m_strLen = ref.m_strLen;
}

// Initialise from CLSID
oStringA::oStringA( REFCLSID pClsId)  : oStringBase(sizeof(char))
{
    getBuf( 50);	// Incl. terminator...

    // Assign string...
    operator=(pClsId);
}

// Return pointer to string...
oStringA::operator LPSTR()
{
    return (LPSTR)m_strBuf;
}


// Assign
oStringA oStringA::operator =  (const oStringA & string)
{
    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( string.m_strLen);
    strcpy(m_strBuf, string.m_strBuf);
    m_strLen = string.m_strLen;
    return *this;
}

oStringA oStringA::operator =  (LPCSTR pAscii)
{
    long	strLen = strlen(pAscii) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( strLen);
    strcpy(m_strBuf, pAscii);
    m_strLen = strLen;
    return *this;
}

oStringA oStringA::operator =  (LPCWSTR pUnicode)
{
    long	strLen = lstrlenW(pUnicode) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( strLen);
    UnicodeToAscii((LPWSTR)pUnicode, m_strBuf);
    m_strLen = strLen;
    return *this;
}

oStringA oStringA::operator =  (REFCLSID rClsId)
{
    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( 50);
    m_strLen  = sprintf(m_strBuf, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	    rClsId->Data1,
	    rClsId->Data2,
	    rClsId->Data3,
	    rClsId->Data4[0],
	    rClsId->Data4[1],
	    rClsId->Data4[2],
	    rClsId->Data4[3],
	    rClsId->Data4[4],
	    rClsId->Data4[5],
	    rClsId->Data4[6],
	    rClsId->Data4[7]);
    return *this;
}

// Add String to String
oStringA oStringA::operator +  (const oStringA & string)
{
    // two terminators to account for...
    oStringA	product(m_strLen + string.m_strLen - 1);

    memcpy(product.m_strBuf, m_strBuf, m_strLen);
    strcpy(product.m_strBuf + m_strLen - 1, string.m_strBuf);
    product.m_strLen = m_strLen + string.m_strLen - 1;

    return product;
}

// Add String to ASCII string
oStringA oStringA::operator +  (LPCSTR pAscii)
{
    long	strLen = m_strLen + strlen(pAscii);

    oStringA	product(strLen);

    memcpy(product.m_strBuf, m_strBuf, m_strLen);
    strcpy(product.m_strBuf + m_strLen - 1, pAscii);
    product.m_strLen = strLen;

    return product;
}

// Add String to UNICODE string
oStringA oStringA::operator +  (LPCWSTR pUnicode)
{
    long	strLen = m_strLen + lstrlenW(pUnicode);

    oStringA	product(strLen);

    memcpy(product.m_strBuf, m_strBuf, m_strLen);
    UnicodeToAscii((LPWSTR)pUnicode, product.m_strBuf + m_strLen - 1);
    product.m_strLen = strLen;

    return product;
}

// Concatenate string object
oStringA oStringA::operator += (const oStringA & string)
{
    // two terminators to account for...
    long	strLen = m_strLen + string.m_strLen - 1;

    adjBuf( strLen);
    strcpy(m_strBuf + m_strLen - 1, string.m_strBuf);
    m_strLen = strLen;

    return *this;
}

// Concatenate Ascii string
oStringA oStringA::operator +=  (LPCSTR pAscii)
{
    long	strLen = m_strLen + strlen(pAscii);

    adjBuf( strLen);
    strcpy(m_strBuf + m_strLen - 1, pAscii);
    m_strLen = strLen;

    return *this;
}

// Concatenate Unicode string
oStringA oStringA::operator +=  (LPCWSTR pUnicode)
{
    long	strLen = m_strLen + lstrlenW(pUnicode);

    adjBuf( strLen);
    UnicodeToAscii((LPWSTR)pUnicode, m_strBuf + m_strLen - 1);
    m_strLen = strLen;

    return *this;
}

// ======================================================================
// oStringW - String manipulation based on Unicode
// ======================================================================

// Initialise to nothing...
oStringW::oStringW( void) : oStringBase(sizeof(WCHAR))
{
    getBuf( 1);	// Incl. terminator...
}

// Initialise to a specified length...
oStringW::oStringW(int defLen) : oStringBase(sizeof(WCHAR))
{
    getBuf( defLen + 1);
}

// Initialise from Unicode string...
oStringW::oStringW(LPCWSTR pUnicode) : oStringBase(sizeof(WCHAR))
{
    long	strLen = lstrlenW((const WCHAR *)pUnicode) + 1;

    getBuf(strLen);
    lstrcpyW((LPWSTR)m_strBuf, pUnicode);
    m_strLen = strLen;
}

// Initialise from Unicode string...
oStringW::oStringW(const wchar_t * pUnicode) : oStringBase(sizeof(WCHAR))
{
    long	strLen = lstrlenW((LPWSTR)pUnicode) + 1;

    getBuf(strLen);
    lstrcpyW((LPWSTR)m_strBuf, (LPCWSTR)pUnicode);
    m_strLen = strLen;
}

// Initialise from ASCII string...
oStringW::oStringW(LPCSTR pAscii) : oStringBase(sizeof(WCHAR))
{
    long	strLen = strlen(pAscii) + 1;

    getBuf( strLen);
    AsciiToUnicode((char *)pAscii, (LPWSTR)m_strBuf);
    m_strLen = strLen;
}

// Initialise from another oString
oStringW::oStringW(const oStringW &ref) : oStringBase(sizeof(WCHAR))
{
    getBuf( ref.m_strLen);
    lstrcpyW((LPWSTR)m_strBuf, (LPWSTR)ref.m_strBuf);
    m_strLen = ref.m_strLen;
}

// Initialise from CLSID
oStringW::oStringW( REFCLSID pClsId) : oStringBase(sizeof(WCHAR))
{
    getBuf( 50);	// Incl. terminator...

    // Assign string...
    operator=(pClsId);
}

// Return pointer to string...
oStringW::operator LPWSTR()
{
    return (LPWSTR)m_strBuf;
}

// Assign
oStringW oStringW::operator =  (const oStringW & string)
{
    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( string.m_strLen);
    lstrcpyW((LPWSTR)m_strBuf, (LPWSTR)string.m_strBuf);
    m_strLen = string.m_strLen;
    return *this;
}

oStringW oStringW::operator =  (LPCSTR pAscii)
{
    long	strLen = strlen(pAscii) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( strLen);
    AsciiToUnicode((char *)pAscii, (LPWSTR)m_strBuf);
    m_strLen = strLen;
    return *this;
}

oStringW oStringW::operator =  (LPCWSTR pUnicode)
{
    long	strLen = lstrlenW(pUnicode) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( strLen);
    lstrcpyW((LPWSTR)m_strBuf, pUnicode);
    m_strLen = strLen;
    return *this;
}

oStringW oStringW::operator =  (const wchar_t * pUnicode)
{
    long	strLen = lstrlenW((LPWSTR)pUnicode) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( strLen);
    lstrcpyW((LPWSTR)m_strBuf, (LPWSTR)pUnicode);
    m_strLen = strLen;
    return *this;
}

oStringW oStringW::operator =  (REFCLSID rClsId)
{
    char	tmp[50];

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf( 50);
    m_strLen  = sprintf(tmp, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	    rClsId->Data1,
	    rClsId->Data2,
	    rClsId->Data3,
	    rClsId->Data4[0],
	    rClsId->Data4[1],
	    rClsId->Data4[2],
	    rClsId->Data4[3],
	    rClsId->Data4[4],
	    rClsId->Data4[5],
	    rClsId->Data4[6],
	    rClsId->Data4[7]);
    AsciiToUnicode(tmp, (LPWSTR)m_strBuf);
    return *this;
}

// Add String to String
oStringW oStringW::operator +  (const oStringW & string)
{
    // two terminators to account for...
    oStringW	product(m_strLen + string.m_strLen - 1);

    lstrcpyW((LPWSTR)product.m_strBuf, (LPWSTR)m_strBuf);
    lstrcpyW((LPWSTR)product.m_strBuf + m_strLen - 1, (LPWSTR)string.m_strBuf);
    product.m_strLen = m_strLen + string.m_strLen - 1;

    return product;
}

// Add String to ASCII string
oStringW oStringW::operator +  (LPCSTR pAscii)
{
    long	strLen = m_strLen + strlen(pAscii);

    oStringW	product(strLen);

    lstrcpyW((LPWSTR)product.m_strBuf, (LPWSTR)m_strBuf);
    AsciiToUnicode((char *)pAscii, (LPWSTR)product.m_strBuf + m_strLen - 1);
    product.m_strLen = strLen;

    return product;
}

// Add String to UNICODE string
oStringW oStringW::operator +  (LPCWSTR pUnicode)
{
    long	strLen = m_strLen + lstrlenW(pUnicode);

    oStringW	product(strLen);

    lstrcpyW((LPWSTR)product.m_strBuf, (LPWSTR)m_strBuf);
    lstrcpyW((LPWSTR)product.m_strBuf + m_strLen - 1, (LPWSTR)pUnicode);
    product.m_strLen = strLen;

    return product;
}

// Add String to UNICODE string
oStringW oStringW::operator +  (const wchar_t * pUnicode)
{
    long	strLen = m_strLen + lstrlenW((LPWSTR)pUnicode);

    oStringW	product(strLen);

    lstrcpyW((LPWSTR)product.m_strBuf, (LPWSTR)m_strBuf);
    lstrcpyW((LPWSTR)product.m_strBuf + m_strLen - 1, (LPWSTR)pUnicode);
    product.m_strLen = strLen;

    return product;
}

// Concatenate string object
oStringW oStringW::operator += (const oStringW & string)
{
    // two terminators to account for...
    long	strLen = m_strLen + string.m_strLen - 1;

    adjBuf( strLen);
    lstrcpyW((LPWSTR)m_strBuf + m_strLen - 1, (LPWSTR)string.m_strBuf);
    m_strLen = strLen;

    return *this;
}

// Concatenate Ascii string
oStringW oStringW::operator +=  (LPCSTR pAscii)
{
    long	strLen = m_strLen + strlen(pAscii);

    adjBuf( strLen);
    AsciiToUnicode((char *)pAscii, (LPWSTR)m_strBuf + m_strLen - 1);
    m_strLen = strLen;

    return *this;
}

// Concatenate Unicode string
oStringW oStringW::operator +=  (LPCWSTR pUnicode)
{
    long	strLen = m_strLen + lstrlenW(pUnicode);

    adjBuf( strLen);
    lstrcpyW((LPWSTR)m_strBuf + m_strLen - 1, (LPWSTR)pUnicode);
    m_strLen = strLen;

    return *this;
}

// Concatenate Unicode string
oStringW oStringW::operator +=  (const wchar_t * pUnicode)
{
    long	strLen = m_strLen + lstrlenW((LPWSTR)pUnicode);

    adjBuf( strLen);
    lstrcpyW((LPWSTR)m_strBuf + m_strLen - 1, (LPWSTR)pUnicode);
    m_strLen = strLen;

    return *this;
}


