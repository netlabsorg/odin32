
#if !defined(OSTRING_INCLUDED)
#define OSTRING_INCLUDED

class oStringA
{
private:
    LPSTR	m_strBuf;	// Buffer for string data.
    
    long	m_bufLen;	// Current allocated length
    				// of buffer
    				
    long	m_strLen;	// Length of string 
    				// incl. zero terminator

    HANDLE	m_Heap;		// heap handle.

    static long blockSize;

protected:
    void freeBuf();
    void getBuf(long newLen);
    void adjBuf(long newLen);
    long calcBufLen(long newLen);

public:
    oStringA(void);
    oStringA(int defLen);
    oStringA(LPCWSTR pUnicode);
    oStringA(LPCSTR pAscii);
    oStringA(const oStringA &ref);
    virtual ~oStringA();

    operator LPSTR();

    // Equality
    oStringA operator =  (const oStringA & string);
    oStringA operator =  (LPCSTR pAscii);
    oStringA operator =  (LPCWSTR pUnicode);

    // Concatenation
    oStringA operator +  (const oStringA & string);
    oStringA operator +  (LPCSTR pAscii);
    oStringA operator +  (LPCWSTR pUnicode);

    // Concatenation
    oStringA operator += (const oStringA & string);
    oStringA operator += (LPCSTR pAscii);
    oStringA operator += (LPCWSTR pUnicode);

};

#endif // OSTRING_INCLUDED
