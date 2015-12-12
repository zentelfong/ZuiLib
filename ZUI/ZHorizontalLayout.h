#ifndef __ZUIHORIZONTALLAYOUT_H__
#define __ZUIHORIZONTALLAYOUT_H__

#include "ZContainer.h"

namespace ZuiLib
{



class ZHorizontalLayout:public ZContainer
{
public:
	ZHorizontalLayout(void);
	~ZHorizontalLayout(void);

	const char* GetClass() const;
	void* GetInterface(const char* pstrName);
	uint32_t GetControlFlags() const;

	void SetSepWidth(int iWidth);
	int GetSepWidth() const;
	void SetSepImmMode(bool bImmediately);
	bool IsSepImmMode() const;
	void SetAttribute(const char* pstrName, const char* pstrValue);
	void DoEvent(TEventUI& event);

	void SetPos(RECT rc);
	void DoPostPaint(ZRenerder* hDC, const RECT& rcPaint);

	RECT GetThumbRect(bool bUseNew = false) const;

protected:
	int m_iSepWidth;
	uint32_t m_uButtonState;
	POINT ptLastMouse;
	RECT m_rcNewPos;
	bool m_bImmMode;
};











}//namespace


#endif