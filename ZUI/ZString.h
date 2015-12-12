#ifndef __ZUISTRING_H__
#define __ZUISTRING_H__

#include <assert.h>
namespace ZuiLib
{
	typedef char ZChar;

	class ZString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		ZString();
		ZString(const ZChar ch);
		ZString(const ZString& src);
		ZString(const ZChar* str, int nLen = -1);
		~ZString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		ZChar GetAt(int nIndex) const;
		void Append(const ZChar* pstr);
		void Assign(const ZChar* pstr, int nLength = -1);
		const ZChar* GetString() const;

		void SetAt(int nIndex, ZChar ch);
		operator const ZChar*() const;

		ZChar operator[] (int nIndex) const;
		const ZString& operator=(const ZString& src);
		const ZString& operator=(const ZChar ch);
		const ZString& operator=(const ZChar* pstr);


		ZString operator+(const ZString& src) const;
		ZString operator+(const ZChar* pstr) const;
		const ZString& operator+=(const ZString& src);
		const ZString& operator+=(const ZChar* pstr);
		const ZString& operator+=(const ZChar ch);

		bool operator == (const ZChar* str) const;
		bool operator != (const ZChar* str) const;
		bool operator <= (const ZChar* str) const;
		bool operator <  (const ZChar* str) const;
		bool operator >= (const ZChar* str) const;
		bool operator >  (const ZChar* str) const;

		int Compare(const ZChar* pstr) const;
		int CompareNoCase(const ZChar* pstr) const;

		void MakeUpper();
		void MakeLower();

		ZString Left(int nLength) const;
		ZString Mid(int iPos, int nLength = -1) const;
		ZString Right(int nLength) const;

		int Find(ZChar ch, int iPos = 0) const;
		int Find(const ZChar* pstr, int iPos = 0) const;
		int ReverseFind(ZChar ch) const;
		int Replace(const ZChar* pstrFrom, const ZChar* pstrTo);

		int __cdecl Format(const ZChar* pstrFormat, ...);
        int __cdecl Format(const ZChar* pstrFormat, va_list Args);
		int __cdecl SmallFormat(const ZChar* pstrFormat, ...);

	protected:
		ZChar* m_pstr;
		ZChar m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};


}//namespace



#endif //__ZUISTRING_H__