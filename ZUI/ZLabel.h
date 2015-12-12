#ifndef __ZUILABEL_H__
#define __ZUILABEL_H__

#include "ZControl.h"

namespace ZuiLib
{


class ZLabel:public ZControl
{
public:
	ZLabel(void);
	~ZLabel(void);

    const char* GetClass() const;
    void* GetInterface(const char* pstrName);

    void SetTextStyle(uint32_t uStyle);
	uint32_t GetTextStyle() const;

    void SetTextColor(color_t dwTextColor);
	color_t GetTextColor() const;
    void SetDisabledTextColor(color_t dwTextColor);
	color_t GetDisabledTextColor() const;
    
	void SetFont(const char* font);
	const char* GetFont();

    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);
    void SetAttribute(const char* pstrName, const char* pstrValue);

    void PaintText(ZRenerder* hDC);

protected:
	ZString m_sFont;
    color_t m_dwTextColor;
    color_t m_dwDisabledTextColor;
    uint32_t m_uTextStyle;
    RECT m_rcTextPadding;
    bool m_bShowHtml;
};














}


#endif