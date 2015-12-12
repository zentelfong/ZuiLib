#include "ZCommon.h"
#include "ZString.h"
#include <stdarg.h>

#define free nedfree
#define malloc nedmalloc
#define realloc nedrealloc

namespace ZuiLib
{
	ZString::ZString() : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = '\0';
	}

	ZString::ZString(const ZChar ch) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
	}

	ZString::ZString(const ZChar* lpsz, int nLen) : m_pstr(m_szBuffer)
	{      
		if(lpsz)
		{
			m_szBuffer[0] = '\0';
			Assign(lpsz, nLen);
		}
		else
		{
			if (m_pstr!=m_szBuffer)
			{
				free(m_pstr);
			}
			m_szBuffer[0] = '\0';
		}
	}

	ZString::ZString(const ZString& src) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = '\0';
		Assign(src.m_pstr);
	}

	ZString::~ZString()
	{
		if( m_pstr != m_szBuffer ) free(m_pstr);
	}

	int ZString::GetLength() const
	{
		const ZChar* ptr=m_pstr;
		while(*ptr)
		{
			++ptr;
		}
		return int(ptr-m_pstr); 
	}

	ZString::operator const ZChar*() const 
	{ 
		return m_pstr; 
	}

	void ZString::Append(const ZChar* pstr)
	{
		int nNewLength = GetLength() + (int) strlen(pstr);
		if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
			if( m_pstr == m_szBuffer ) {
				m_pstr = static_cast<ZChar*>(malloc((nNewLength + 1) * sizeof(ZChar)));
				strcpy(m_pstr, m_szBuffer);
				strcat(m_pstr, pstr);
			}
			else {
				m_pstr = static_cast<ZChar*>(realloc(m_pstr, (nNewLength + 1) * sizeof(ZChar)));
				strcat(m_pstr, pstr);
			}
		}
		else {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
			strcat(m_szBuffer, pstr);
		}
	}

	void ZString::Assign(const ZChar* pstr, int cchMax)
	{
		if( pstr == NULL ) pstr = "";
		cchMax = (cchMax < 0 ? (int) strlen(pstr) : cchMax);
		if( cchMax < MAX_LOCAL_STRING_LEN ) {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
		}
		else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
			if( m_pstr == m_szBuffer ) m_pstr = NULL;
			m_pstr = static_cast<ZChar*>(realloc(m_pstr, (cchMax + 1) * sizeof(ZChar)));
		}
		strncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = '\0';
	}

	bool ZString::IsEmpty() const 
	{ 
		return m_pstr[0] == '\0'; 
	}

	void ZString::Empty() 
	{ 
		if( m_pstr != m_szBuffer ) free(m_pstr);
		m_pstr = m_szBuffer;
		m_szBuffer[0] = '\0'; 
	}

	const ZChar* ZString::GetString() const
	{
		return m_pstr;
	}

	ZChar ZString::GetAt(int nIndex) const
	{
		return m_pstr[nIndex];
	}

	ZChar ZString::operator[] (int nIndex) const
	{ 
		return m_pstr[nIndex];
	}   

	const ZString& ZString::operator=(const ZString& src)
	{      
		Assign(src);
		return *this;
	}

	const ZString& ZString::operator=(const ZChar* lpStr)
	{      
		if (lpStr)
		{
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}


	const ZString& ZString::operator=(const ZChar ch)
	{
		Empty();
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
		return *this;
	}

	ZString ZString::operator+(const ZString& src) const
	{
		ZString sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	ZString ZString::operator+(const ZChar* lpStr) const
	{
		if ( lpStr )
		{
			ZString sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const ZString& ZString::operator+=(const ZString& src)
	{      
		Append(src);
		return *this;
	}

	const ZString& ZString::operator+=(const ZChar* lpStr)
	{      
		if ( lpStr )
		{
			Append(lpStr);
		}
		
		return *this;
	}

	const ZString& ZString::operator+=(const ZChar ch)
	{      
		ZChar str[] = { ch, '\0' };
		Append(str);
		return *this;
	}

	bool ZString::operator == (const ZChar* str) const { return (Compare(str) == 0); };
	bool ZString::operator != (const ZChar* str) const { return (Compare(str) != 0); };
	bool ZString::operator <= (const ZChar* str) const { return (Compare(str) <= 0); };
	bool ZString::operator <  (const ZChar* str) const { return (Compare(str) <  0); };
	bool ZString::operator >= (const ZChar* str) const { return (Compare(str) >= 0); };
	bool ZString::operator >  (const ZChar* str) const { return (Compare(str) >  0); };

	void ZString::SetAt(int nIndex, ZChar ch)
	{
		assert(nIndex>=0 && nIndex<GetLength());
		m_pstr[nIndex] = ch;
	}

	int ZString::Compare(const ZChar* lpsz) const 
	{ 
		return strcmp(m_pstr, lpsz); 
	}

	int ZString::CompareNoCase(const ZChar* lpsz) const 
	{ 
		return stricmp(m_pstr, lpsz); 
	}

	void ZString::MakeUpper() 
	{ 
		strupr(m_pstr); 
	}

	void ZString::MakeLower() 
	{ 
		strlwr(m_pstr); 
	}

	ZString ZString::Left(int iLength) const
	{
		if( iLength < 0 ) iLength = 0;
		if( iLength > GetLength() ) iLength = GetLength();
		return ZString(m_pstr, iLength);
	}

	ZString ZString::Mid(int iPos, int iLength) const
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if( iLength <= 0 ) return ZString();
		return ZString(m_pstr + iPos, iLength);
	}

	ZString ZString::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if( iPos < 0 ) {
			iPos = 0;
			iLength = GetLength();
		}
		return ZString(m_pstr + iPos, iLength);
	}

	int ZString::Find(ZChar ch, int iPos /*= 0*/) const
	{
		assert(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
		const ZChar* p = strchr(m_pstr + iPos, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int ZString::Find(const ZChar* pstrSub, int iPos /*= 0*/) const
	{
		assert(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
		const ZChar* p = strstr(m_pstr + iPos, pstrSub);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int ZString::ReverseFind(ZChar ch) const
	{
		const ZChar* p = strrchr(m_pstr, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int ZString::Replace(const ZChar* pstrFrom, const ZChar* pstrTo)
	{
		ZString sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if( iPos < 0 ) return 0;
		int cchFrom = (int) strlen(pstrFrom);
		int cchTo = (int) strlen(pstrTo);
		while( iPos >= 0 ) {
			sTemp = Left(iPos);
			sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}
    
    int ZString::Format(const ZChar* pstrFormat, va_list Args)
    {
        int nLen, totalLen;
        ZChar *szBuffer;
		
        nLen = _vsnprintf_c(NULL, 0, pstrFormat, Args);
        totalLen = (nLen + 1)*sizeof(ZChar);
        szBuffer = (ZChar*)malloc(totalLen);
		memset(szBuffer,0,totalLen);
        nLen = _vsnprintf_c(szBuffer, nLen + 1, pstrFormat, Args);

        Assign(szBuffer);
        free(szBuffer);

        return nLen;
    }

    int ZString::Format(const ZChar* pstrFormat, ...)
    {
        int nRet;
        va_list Args;

        va_start(Args, pstrFormat);
        nRet = Format(pstrFormat, Args);
        va_end(Args);

        return nRet;

    }

	int ZString::SmallFormat(const ZChar* pstrFormat, ...)
	{
		ZString sFormat = pstrFormat;
		ZChar szBuffer[64] = { 0 };
		va_list argList;
		va_start(argList, pstrFormat);
		int iRet = _vsnprintf_c(szBuffer, sizeof(szBuffer), sFormat, argList);
		va_end(argList);
		Assign(szBuffer);
		return iRet;
	}



}//namespace