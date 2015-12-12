#ifndef __ZUIPROGRESS_H__
#define __ZUIPROGRESS_H__

#include "ZLabel.h"

namespace ZuiLib
{

class ZProgress:public ZLabel
{
public:
	ZProgress(void);
	~ZProgress(void);

    const char* GetClass() const;
    void* GetInterface(const char* pstrName);

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    const char* GetForeImage() const;
    void SetForeImage(const char* pStrImage);

    void SetAttribute(const char* pstrName, const char* pstrValue);
    void PaintStatusImage(ZRenerder* hDC);

protected:
    bool m_bHorizontal;
    int m_nMax;
    int m_nMin;
    int m_nValue;

    ZString m_sForeImage;
    ZString m_sForeImageModify;
};



}//namespace



#endif