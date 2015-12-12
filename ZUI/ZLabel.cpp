#include "ZLabel.h"
#include "ZPaintManager.h"
#include "ZControlFactory.h"

namespace ZuiLib
{


ZLabel::ZLabel(void)
	:m_uTextStyle(0), m_dwTextColor(0), 
    m_dwDisabledTextColor(0), m_bShowHtml(false)
{
	SetRectEmpty(&m_rcTextPadding);
}

ZLabel::~ZLabel(void)
{

}


const char* ZLabel::GetClass() const
{
	return "Label";
}


void* ZLabel::GetInterface(const char* pstrName)
{
	if( strcmp(pstrName,"Label") == 0 ) return static_cast<ZLabel*>(this);
    return ZControl::GetInterface(pstrName);
}

void ZLabel::SetTextStyle(uint32_t uStyle)
{
	m_uTextStyle = uStyle;
    Invalidate();
}


uint32_t ZLabel::GetTextStyle() const
{
	return m_uTextStyle;
}

void ZLabel::SetTextColor(color_t dwTextColor)
{
	m_dwTextColor = dwTextColor;
}


color_t ZLabel::GetTextColor() const
{
	return m_dwTextColor;
}
void ZLabel::SetDisabledTextColor(color_t dwTextColor)
{
	m_dwDisabledTextColor = dwTextColor;
}
color_t ZLabel::GetDisabledTextColor() const
{
	return m_dwDisabledTextColor;
}
void ZLabel::SetFont(const char* font)
{
	m_sFont=font;
}

const char* ZLabel::GetFont()
{
	return m_sFont;
}
RECT ZLabel::GetTextPadding() const
{
	return m_rcTextPadding;
}
void ZLabel::SetTextPadding(RECT rc)
{
	m_rcTextPadding=rc;
}
bool ZLabel::IsShowHtml()
{
	return m_bShowHtml;
}
void ZLabel::SetShowHtml(bool bShowHtml)
{
	m_bShowHtml=bShowHtml;
}

SIZE ZLabel::EstimateSize(SIZE szAvailable)
{
	return ZControl::EstimateSize(szAvailable);
}


void ZLabel::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        return;
    }
    ZControl::DoEvent(event);
}


void ZLabel::SetAttribute(const char* pstrName, const char* pstrValue)
{
	 if( strcmp(pstrName, "align") == 0 ) {
		if( strstr(pstrValue, "left") != NULL ) {
            m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_TOP | DT_BOTTOM);
            m_uTextStyle |= DT_LEFT;
        }
        if( strstr(pstrValue, "center") != NULL ) {
            m_uTextStyle &= ~(DT_LEFT | DT_RIGHT | DT_TOP | DT_BOTTOM);
            m_uTextStyle |= DT_CENTER;
        }
        if( strstr(pstrValue, "right") != NULL ) {
            m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_TOP | DT_BOTTOM);
            m_uTextStyle |= DT_RIGHT;
        }
		if( strstr(pstrValue, "top") != NULL ) {
			m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_LEFT | DT_RIGHT);
			m_uTextStyle |= DT_TOP;
		}
		if( strstr(pstrValue, "bottom") != NULL ) {
			m_uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_LEFT | DT_RIGHT);
			m_uTextStyle |= DT_BOTTOM;
		}
    }
    else if( strcmp(pstrName, "endellipsis") == 0 ) 
	{
        if( strcmp(pstrValue, "true") == 0 ) 
			m_uTextStyle |= DT_END_ELLIPSIS;
        else 
			m_uTextStyle &= ~DT_END_ELLIPSIS;
    }    
    else if( strcmp(pstrName, "font") == 0 ) 
		SetFont(pstrValue);
    else if( strcmp(pstrName, "textcolor") == 0 ) 
	{
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        color_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetTextColor(clrColor);
    }
    else if( strcmp(pstrName, "disabledtextcolor") == 0 ) 
	{
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        color_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetDisabledTextColor(clrColor);
    }
    else if( strcmp(pstrName, "textpadding") == 0 ) 
	{
        RECT rcTextPadding = { 0 };
        char* pstr = NULL;
        rcTextPadding.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
        rcTextPadding.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
        rcTextPadding.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
        rcTextPadding.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
        SetTextPadding(rcTextPadding);
    }
    else if( strcmp(pstrName, "showhtml") == 0 ) 
		SetShowHtml(strcmp(pstrValue, "true") == 0);
    else
		ZControl::SetAttribute(pstrName, pstrValue);
}

void ZLabel::PaintText(ZRenerder* hDC)
{
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
    if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

    if( m_sText.IsEmpty() ) return;
    int nLinks = 0;
    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

	color_t color=m_dwTextColor;

    if( !IsEnabled() ) {
		color=m_dwDisabledTextColor;
    }

    GetRenerder()->DrawText(m_sText,color,rc,GetTextInfo(m_sFont),m_uTextStyle);
}








	IMPL_ZCONTROL_FACTORY("Label",ZLabel);
}