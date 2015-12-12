#include "ZButton.h"
#include "ZRenerder.h"
#include "ZPaintManager.h"
#include "ZControlFactory.h"

namespace ZuiLib
{



ZButton::ZButton() : m_uButtonState(0), m_dwHotTextColor(0), m_dwPushedTextColor(0), m_dwFocusedTextColor(0)
{
    m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
}


ZButton::~ZButton()
{

}

const char* ZButton::GetClass() const
{
    return "Button";
}

void* ZButton::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "Button") == 0 ) return static_cast<ZButton*>(this);
    return ZLabel::GetInterface(pstrName);
}

uint32_t ZButton::GetControlFlags() const
{
    return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
}

void ZButton::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else ZLabel::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
		DoLuaEvent("setFocus");
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
		DoLuaEvent("killFocus");
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
		if (IsKeyboardEnabled()) {
			if( event.chKey == SDLK_SPACE || event.chKey == SDLK_RETURN ) {
				Activate();
				return;
			}
		}
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
    {
        if( PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            if( PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
            else m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            if( PtInRect(&m_rcItem, event.ptMouse) ) Activate();
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
			DoLuaEvent("menu");
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( IsEnabled() ) {
            m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( IsEnabled() ) {
            m_uButtonState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    //if( event.Type == UIEVENT_SETCURSOR ) {
    //    ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
    //    return;
    //}
    ZLabel::DoEvent(event);
}

bool ZButton::Activate()
{
    if( !ZControl::Activate() ) return false;
	DoLuaEvent("click");
    return true;
}

void ZButton::SetEnabled(bool bEnable)
{
    ZControl::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        m_uButtonState = 0;
    }
}

void ZButton::SetHotTextColor(color_t dwColor)
{
	m_dwHotTextColor = dwColor;
}

color_t ZButton::GetHotTextColor() const
{
	return m_dwHotTextColor;
}

void ZButton::SetPushedTextColor(color_t dwColor)
{
	m_dwPushedTextColor = dwColor;
}

color_t ZButton::GetPushedTextColor() const
{
	return m_dwPushedTextColor;
}

void ZButton::SetFocusedTextColor(color_t dwColor)
{
	m_dwFocusedTextColor = dwColor;
}

color_t ZButton::GetFocusedTextColor() const
{
	return m_dwFocusedTextColor;
}

const char* ZButton::GetNormalImage()
{
    return m_sNormalImage;
}

void ZButton::SetNormalImage(const char* pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

const char* ZButton::GetHotImage()
{
    return m_sHotImage;
}

void ZButton::SetHotImage(const char* pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

const char* ZButton::GetPushedImage()
{
    return m_sPushedImage;
}

void ZButton::SetPushedImage(const char* pStrImage)
{
    m_sPushedImage = pStrImage;
    Invalidate();
}

const char* ZButton::GetFocusedImage()
{
    return m_sFocusedImage;
}

void ZButton::SetFocusedImage(const char* pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

const char* ZButton::GetDisabledImage()
{
    return m_sDisabledImage;
}

void ZButton::SetDisabledImage(const char* pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

SIZE ZButton::EstimateSize(SIZE szAvailable)
{
    return ZLabel::EstimateSize(szAvailable);
}

void ZButton::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "normalimage") == 0 ) SetNormalImage(pstrValue);
    else if( strcmp(pstrName, "hotimage") == 0 ) SetHotImage(pstrValue);
    else if( strcmp(pstrName, "pushedimage") == 0 ) SetPushedImage(pstrValue);
    else if( strcmp(pstrName, "focusedimage") == 0 ) SetFocusedImage(pstrValue);
    else if( strcmp(pstrName, "disabledimage") == 0 ) SetDisabledImage(pstrValue);
    else if( strcmp(pstrName, "hottextcolor") == 0 ) {
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        color_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetHotTextColor(clrColor);
    }
    else if( strcmp(pstrName, "pushedtextcolor") == 0 ) {
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        color_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetPushedTextColor(clrColor);
    }
    else if( strcmp(pstrName, "focusedtextcolor") == 0 ) {
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        color_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetFocusedTextColor(clrColor);
    }
    else ZLabel::SetAttribute(pstrName, pstrValue);
}

void ZButton::PaintText(ZRenerder* hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

    if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
    if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

    if( m_sText.IsEmpty() ) return;

    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

	color_t clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

	if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
		clrColor = GetPushedTextColor();
	else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
		clrColor = GetHotTextColor();
	else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
		clrColor = GetFocusedTextColor();

	GetRenerder()->DrawText(m_sText,clrColor,rc,GetTextInfo(m_sFont),m_uTextStyle);
}

void ZButton::PaintStatusImage(ZRenerder* hDC)
{
    if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
    else m_uButtonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
    else m_uButtonState &= ~ UISTATE_DISABLED;

    if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
        if( !m_sDisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sDisabledImage) ) m_sDisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
        if( !m_sPushedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sPushedImage) ) m_sPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if( !m_sHotImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sHotImage) ) m_sHotImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
        if( !m_sFocusedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sFocusedImage) ) m_sFocusedImage.Empty();
            else return;
        }
    }

    if( !m_sNormalImage.IsEmpty() ) {
        if( !DrawImage(hDC, (const char*)m_sNormalImage) ) m_sNormalImage.Empty();
        else return;
    }
}







IMPL_ZCONTROL_FACTORY("Button",ZButton);




















}
