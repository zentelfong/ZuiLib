#ifndef __ZUILAYOUT_H__
#define __ZUILAYOUT_H__

#include "ZContainer.h"

namespace ZuiLib
{


class ZTabLayout:public ZContainer
{
public:
	ZTabLayout(void);
	~ZTabLayout(void);


	const char* GetClass() const;
	void* GetInterface(const char* pstrName);

	bool Add(ZControl* pControl);
	bool AddAt(ZControl* pControl, int iIndex);
	bool Remove(ZControl* pControl);
	void RemoveAll();
	int GetCurSel() const;
	bool SelectItem(int iIndex);
	bool SelectItem(ZControl* pControl);

	void SetPos(RECT rc);

	void SetAttribute(const char* pstrName, const char* pstrValue);

protected:
	int m_iCurSel;
};

}//namespace

#endif