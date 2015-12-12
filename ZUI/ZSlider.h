#ifndef __ZUISLIDER_H__
#define __ZUISLIDER_H__

#include "ZProgress.h"

namespace ZuiLib
{


class ZSlider:public ZProgress
{
public:
	ZSlider(void);
	~ZSlider(void);


    const char* GetClass() const;
    uint32_t GetControlFlags() const;
    void* GetInterface(const char* pstrName);

    void SetEnabled(bool bEnable = true);

    int GetChangeStep();
    void SetChangeStep(int step);
    void SetThumbSize(SIZE szXY);
    RECT GetThumbRect() const;
    const char* GetThumbImage() const;
    void SetThumbImage(const char* pStrImage);
    const char* GetThumbHotImage() const;
    void SetThumbHotImage(const char* pStrImage);
    const char* GetThumbPushedImage() const;
    void SetThumbPushedImage(const char* pStrImage);

    void DoEvent(TEventUI& event);
    void SetAttribute(const char* pstrName, const char* pstrValue);
    void PaintStatusImage(ZRenerder* hDC);

protected:
    SIZE m_szThumb;
    uint32_t m_uButtonState;
    int m_nStep;

    ZString m_sThumbImage;
    ZString m_sThumbHotImage;
    ZString m_sThumbPushedImage;
    ZString m_sImageModify;
};



}

#endif