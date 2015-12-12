#ifndef __ZUILAYOUT_H__
#define __ZUILAYOUT_H__

#include "ZContainer.h"

namespace ZuiLib
{


class ZLayout:public ZContainer
{
public:
	ZLayout(void);
	~ZLayout(void);

	const char* GetClass() const;
	void* GetInterface(const char* pstrName);

	void SetAttribute(const char* pstrName, const char* pstrValue);
	void SetPos(RECT rc);

protected:
	void SetHorizontalPos(RECT rc);
	void SetVerticalPos(RECT rc);

	bool m_bVertical;//是否是垂直布局
};

}//namespace


#endif