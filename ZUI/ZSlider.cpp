#include "ZSlider.h"
#include "ZRenerder.h"
#include "ZPaintManager.h"
#include "ZControlFactory.h"

namespace ZuiLib
{



ZSlider::ZSlider() : m_uButtonState(0), m_nStep(1)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    m_szThumb.cx = m_szThumb.cy = 10;
}

ZSlider::~ZSlider(void)
{
}


const char* ZSlider::GetClass() const
{
    return "Slider";
}

uint32_t ZSlider::GetControlFlags() const
{
    if( IsEnabled() ) return UIFLAG_SETCURSOR;
    else return 0;
}

void* ZSlider::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "Slider") == 0 ) return static_cast<ZSlider*>(this);
    return ZProgress::GetInterface(pstrName);
}

void ZSlider::SetEnabled(bool bEnable)
{
    ZControl::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        m_uButtonState = 0;
    }
}

int ZSlider::GetChangeStep()
{
    return m_nStep;
}

void ZSlider::SetChangeStep(int step)
{
    m_nStep = step;
}

void ZSlider::SetThumbSize(SIZE szXY)
{
    m_szThumb = szXY;
}

RECT ZSlider::GetThumbRect() const
{
    if( m_bHorizontal ) {
        int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
        int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
		RECT rc={left, top, left + m_szThumb.cx, top + m_szThumb.cy};
        return rc; 
    }
    else {
        int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
        int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
		RECT rc={left, top, left + m_szThumb.cx, top + m_szThumb.cy};
        return rc; 
    }
}

const char* ZSlider::GetThumbImage() const
{
    return m_sThumbImage;
}

void ZSlider::SetThumbImage(const char* pStrImage)
{
    m_sThumbImage = pStrImage;
    Invalidate();
}

const char* ZSlider::GetThumbHotImage() const
{
    return m_sThumbHotImage;
}

void ZSlider::SetThumbHotImage(const char* pStrImage)
{
    m_sThumbHotImage = pStrImage;
    Invalidate();
}

const char* ZSlider::GetThumbPushedImage() const
{
    return m_sThumbPushedImage;
}

void ZSlider::SetThumbPushedImage(const char* pStrImage)
{
    m_sThumbPushedImage = pStrImage;
    Invalidate();
}

void ZSlider::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else ZProgress::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
    {
        if( IsEnabled() ) {
            RECT rcThumb = GetThumbRect();
            if( PtInRect(&rcThumb, event.ptMouse) ) {
                m_uButtonState |= UISTATE_CAPTURED;
            }
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~UISTATE_CAPTURED;
        }
        if( m_bHorizontal ) {
            if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
            else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
            else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
        }
        else {
            if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
            else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
            else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
        }
		DoLuaEvent("valueChanged",GetValue());
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_SCROLLWHEEL ) 
    {
        switch(event.wParam) {
        case SBT_LINEUP:
            SetValue(GetValue() + GetChangeStep());
            break;
        case SBT_LINEDOWN:
            SetValue(GetValue() - GetChangeStep());
			break;
        }
		DoLuaEvent("valueChanged",GetValue());
		return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            if( m_bHorizontal ) {
                if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
                else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
                else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
            }
            else {
                if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
                else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
                else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
            }
            Invalidate();
        }
        return;
    }
    //if( event.Type == UIEVENT_SETCURSOR )
    //{
    //    RECT rcThumb = GetThumbRect();
    //    if( IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {
    //        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
    //        return;
    //    }
    //}
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
    ZControl::DoEvent(event);
}


void ZSlider::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "thumbimage") == 0 ) SetThumbImage(pstrValue);
    else if( strcmp(pstrName, "thumbhotimage") == 0 ) SetThumbHotImage(pstrValue);
    else if( strcmp(pstrName, "thumbpushedimage") == 0 ) SetThumbPushedImage(pstrValue);
    else if( strcmp(pstrName, "thumbsize") == 0 ) {
        SIZE szXY = {0};
        char* pstr = NULL;
        szXY.cx = strtol(pstrValue, &pstr, 10);   assert(pstr);    
        szXY.cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
        SetThumbSize(szXY);
    }
    else if( strcmp(pstrName, "step") == 0 ) {
        SetChangeStep(atoi(pstrValue));
    }
    else ZProgress::SetAttribute(pstrName, pstrValue);
}

void ZSlider::PaintStatusImage(ZRenerder* hDC)
{
    ZProgress::PaintStatusImage(hDC);

    RECT rcThumb = GetThumbRect();
    rcThumb.left -= m_rcItem.left;
    rcThumb.top -= m_rcItem.top;
    rcThumb.right -= m_rcItem.left;
    rcThumb.bottom -= m_rcItem.top;
    if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
        if( !m_sThumbPushedImage.IsEmpty() ) {
            m_sImageModify.Empty();
            m_sImageModify.Format("dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
            if( !DrawImage(hDC, (const char*)m_sThumbPushedImage, (const char*)m_sImageModify) ) m_sThumbPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if( !m_sThumbHotImage.IsEmpty() ) {
            m_sImageModify.Empty();
            m_sImageModify.Format("dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
            if( !DrawImage(hDC, (const char*)m_sThumbHotImage, (const char*)m_sImageModify) ) m_sThumbHotImage.Empty();
            else return;
        }
    }

    if( !m_sThumbImage.IsEmpty() ) {
        m_sImageModify.Empty();
        m_sImageModify.Format("dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
        if( !DrawImage(hDC, (const char*)m_sThumbImage, (const char*)m_sImageModify) ) m_sThumbImage.Empty();
        else return;
    }
}












IMPL_ZCONTROL_FACTORY("Slider",ZSlider);

}
