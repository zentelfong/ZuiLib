#ifndef __ZUIBUTTON_H__
#define __ZUIBUTTON_H__

#include "ZLabel.h"

namespace ZuiLib
{

class ZButton : public ZLabel
{
public:
    ZButton();
	~ZButton();

    const char* GetClass() const;
    void* GetInterface(const char* pstrName);
    uint32_t GetControlFlags() const;

    bool Activate();
    void SetEnabled(bool bEnable = true);
    void DoEvent(TEventUI& event);

    const char* GetNormalImage();
    void SetNormalImage(const char* pStrImage);
    const char* GetHotImage();
    void SetHotImage(const char* pStrImage);
    const char* GetPushedImage();
    void SetPushedImage(const char* pStrImage);
    const char* GetFocusedImage();
    void SetFocusedImage(const char* pStrImage);
    const char* GetDisabledImage();
    void SetDisabledImage(const char* pStrImage);

    void SetHotTextColor(color_t dwColor);
	color_t GetHotTextColor() const;
    void SetPushedTextColor(color_t dwColor);
	color_t GetPushedTextColor() const;
    void SetFocusedTextColor(color_t dwColor);
	color_t GetFocusedTextColor() const;
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(const char* pstrName, const char* pstrValue);

	void PaintText(ZRenerder* hDC);
    void PaintStatusImage(ZRenerder* hDC);

protected:
    uint32_t m_uButtonState;
	color_t m_dwHotTextColor;
	color_t m_dwPushedTextColor;
	color_t m_dwFocusedTextColor;

    ZString m_sNormalImage;
    ZString m_sHotImage;
    ZString m_sPushedImage;
    ZString m_sFocusedImage;
    ZString m_sDisabledImage;
};


}//namespace

#endif