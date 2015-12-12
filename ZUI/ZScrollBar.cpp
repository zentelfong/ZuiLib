#include "ZScrollBar.h"
#include "ZPaintManager.h"
#include "ZContainer.h"
#include "ZControlFactory.h"

namespace ZuiLib
{

ZScrollBar::ZScrollBar() : m_bHorizontal(false), m_nRange(100), m_nScrollPos(0), m_nLineSize(8), 
m_pOwner(NULL), m_nLastScrollPos(0), m_nLastScrollOffset(0), m_nScrollRepeatDelay(0), m_uButton1State(0), \
m_uButton2State(0), m_uThumbState(0), m_bShowButton1(true), m_bShowButton2(true)
{
    m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
    ptLastMouse.x = ptLastMouse.y = 0;
    memset(&m_rcThumb,0, sizeof(m_rcThumb));
    memset(&m_rcButton1,0, sizeof(m_rcButton1));
    memset(&m_rcButton2,0, sizeof(m_rcButton2));
}

ZScrollBar::~ZScrollBar(void)
{
}

const char* ZScrollBar::GetClass() const
{
    return "ScrollBar";
}

void* ZScrollBar::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "ScrollBar") == 0 ) return static_cast<ZScrollBar*>(this);
    return ZControl::GetInterface(pstrName);
}

ZContainer* ZScrollBar::GetOwner() const
{
    return m_pOwner;
}

void ZScrollBar::SetOwner(ZContainer* pOwner)
{
    m_pOwner = pOwner;
}

void ZScrollBar::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
}

void ZScrollBar::SetEnabled(bool bEnable)
{
    ZControl::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        m_uButton1State = 0;
        m_uButton2State = 0;
        m_uThumbState = 0;
    }
}

void ZScrollBar::SetFocus()
{
    if( m_pOwner != NULL ) m_pOwner->SetFocus();
    else ZControl::SetFocus();
}

bool ZScrollBar::IsHorizontal()
{
    return m_bHorizontal;
}

void ZScrollBar::SetHorizontal(bool bHorizontal)
{
    if( m_bHorizontal == bHorizontal ) return;

    m_bHorizontal = bHorizontal;
    if( m_bHorizontal ) {
        if( m_cxyFixed.cy == 0 ) {
            m_cxyFixed.cx = 0;
            m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
        }
    }
    else {
        if( m_cxyFixed.cx == 0 ) {
            m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
            m_cxyFixed.cy = 0;
        }
    }

    //if( m_pOwner != NULL ) m_pOwner->NeedUpdate(); else NeedParentUpdate();
}

int ZScrollBar::GetScrollRange() const
{
    return m_nRange;
}

void ZScrollBar::SetScrollRange(int nRange)
{
    if( m_nRange == nRange ) return;
    
    m_nRange = nRange;
    if( m_nRange < 0 ) m_nRange = 0;
    if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
    SetPos(m_rcItem);
}

int ZScrollBar::GetScrollPos() const
{
    return m_nScrollPos;
}

void ZScrollBar::SetScrollPos(int nPos)
{
    if( m_nScrollPos == nPos ) return;

    m_nScrollPos = nPos;
    if( m_nScrollPos < 0 ) m_nScrollPos = 0;
    if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
    SetPos(m_rcItem);
}

int ZScrollBar::GetLineSize() const
{
    return m_nLineSize;
}

void ZScrollBar::SetLineSize(int nSize)
{
    m_nLineSize = nSize;
}

bool ZScrollBar::GetShowButton1()
{
    return m_bShowButton1;
}

void ZScrollBar::SetShowButton1(bool bShow)
{
    m_bShowButton1 = bShow;
    SetPos(m_rcItem);
}

const char* ZScrollBar::GetButton1NormalImage()
{
    return m_sButton1NormalImage;
}

void ZScrollBar::SetButton1NormalImage(const char* pStrImage)
{
    m_sButton1NormalImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetButton1HotImage()
{
    return m_sButton1HotImage;
}

void ZScrollBar::SetButton1HotImage(const char* pStrImage)
{
    m_sButton1HotImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetButton1PushedImage()
{
    return m_sButton1PushedImage;
}

void ZScrollBar::SetButton1PushedImage(const char* pStrImage)
{
    m_sButton1PushedImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetButton1DisabledImage()
{
    return m_sButton1DisabledImage;
}

void ZScrollBar::SetButton1DisabledImage(const char* pStrImage)
{
    m_sButton1DisabledImage = pStrImage;
    Invalidate();
}

bool ZScrollBar::GetShowButton2()
{
    return m_bShowButton2;
}

void ZScrollBar::SetShowButton2(bool bShow)
{
    m_bShowButton2 = bShow;
    SetPos(m_rcItem);
}

const char* ZScrollBar::GetButton2NormalImage()
{
    return m_sButton2NormalImage;
}

void ZScrollBar::SetButton2NormalImage(const char* pStrImage)
{
    m_sButton2NormalImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetButton2HotImage()
{
    return m_sButton2HotImage;
}

void ZScrollBar::SetButton2HotImage(const char* pStrImage)
{
    m_sButton2HotImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetButton2PushedImage()
{
    return m_sButton2PushedImage;
}

void ZScrollBar::SetButton2PushedImage(const char* pStrImage)
{
    m_sButton2PushedImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetButton2DisabledImage()
{
    return m_sButton2DisabledImage;
}

void ZScrollBar::SetButton2DisabledImage(const char* pStrImage)
{
    m_sButton2DisabledImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetThumbNormalImage()
{
    return m_sThumbNormalImage;
}

void ZScrollBar::SetThumbNormalImage(const char* pStrImage)
{
    m_sThumbNormalImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetThumbHotImage()
{
    return m_sThumbHotImage;
}

void ZScrollBar::SetThumbHotImage(const char* pStrImage)
{
    m_sThumbHotImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetThumbPushedImage()
{
    return m_sThumbPushedImage;
}

void ZScrollBar::SetThumbPushedImage(const char* pStrImage)
{
    m_sThumbPushedImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetThumbDisabledImage()
{
    return m_sThumbDisabledImage;
}

void ZScrollBar::SetThumbDisabledImage(const char* pStrImage)
{
    m_sThumbDisabledImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetRailNormalImage()
{
    return m_sRailNormalImage;
}

void ZScrollBar::SetRailNormalImage(const char* pStrImage)
{
    m_sRailNormalImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetRailHotImage()
{
    return m_sRailHotImage;
}

void ZScrollBar::SetRailHotImage(const char* pStrImage)
{
    m_sRailHotImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetRailPushedImage()
{
    return m_sRailPushedImage;
}

void ZScrollBar::SetRailPushedImage(const char* pStrImage)
{
    m_sRailPushedImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetRailDisabledImage()
{
    return m_sRailDisabledImage;
}

void ZScrollBar::SetRailDisabledImage(const char* pStrImage)
{
    m_sRailDisabledImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetBkNormalImage()
{
    return m_sBkNormalImage;
}

void ZScrollBar::SetBkNormalImage(const char* pStrImage)
{
    m_sBkNormalImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetBkHotImage()
{
    return m_sBkHotImage;
}

void ZScrollBar::SetBkHotImage(const char* pStrImage)
{
    m_sBkHotImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetBkPushedImage()
{
    return m_sBkPushedImage;
}

void ZScrollBar::SetBkPushedImage(const char* pStrImage)
{
    m_sBkPushedImage = pStrImage;
    Invalidate();
}

const char* ZScrollBar::GetBkDisabledImage()
{
    return m_sBkDisabledImage;
}

void ZScrollBar::SetBkDisabledImage(const char* pStrImage)
{
    m_sBkDisabledImage = pStrImage;
    Invalidate();
}

void ZScrollBar::SetPos(RECT rc)
{
    ZControl::SetPos(rc);
    rc = m_rcItem;

    if( m_bHorizontal ) {
        int cx = rc.right - rc.left;
        if( m_bShowButton1 ) cx -= m_cxyFixed.cy;
        if( m_bShowButton2 ) cx -= m_cxyFixed.cy;
        if( cx > m_cxyFixed.cy ) {
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if( m_bShowButton1 ) {
                m_rcButton1.right = rc.left + m_cxyFixed.cy;
                m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
            }
            else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.top = rc.top;
            m_rcButton2.right = rc.right;
            if( m_bShowButton2 ) {
                m_rcButton2.left = rc.right - m_cxyFixed.cy;
                m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
            }
            else {
                m_rcButton2.left = m_rcButton2.right;
                m_rcButton2.bottom = m_rcButton2.top;
            }

            m_rcThumb.top = rc.top;
            m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
            if( m_nRange > 0 ) {
                int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
                if( cxThumb < m_cxyFixed.cy ) cxThumb = m_cxyFixed.cy;

                m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
                m_rcThumb.right = m_rcThumb.left + cxThumb;
                if( m_rcThumb.right > m_rcButton2.left ) {
                    m_rcThumb.left = m_rcButton2.left - cxThumb;
                    m_rcThumb.right = m_rcButton2.left;
                }
            }
            else {
                m_rcThumb.left = m_rcButton1.right;
                m_rcThumb.right = m_rcButton2.left;
            }
        }
        else {
            int cxButton = (rc.right - rc.left) / 2;
            if( cxButton > m_cxyFixed.cy ) cxButton = m_cxyFixed.cy;
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if( m_bShowButton1 ) {
                m_rcButton1.right = rc.left + cxButton;
                m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
            }
            else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.top = rc.top;
            m_rcButton2.right = rc.right;
            if( m_bShowButton2 ) {
                m_rcButton2.left = rc.right - cxButton;
                m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
            }
            else {
                m_rcButton2.left = m_rcButton2.right;
                m_rcButton2.bottom = m_rcButton2.top;
            }

            memset(&m_rcThumb,0, sizeof(m_rcThumb));
        }
    }
    else {
        int cy = rc.bottom - rc.top;
        if( m_bShowButton1 ) cy -= m_cxyFixed.cx;
        if( m_bShowButton2 ) cy -= m_cxyFixed.cx;
        if( cy > m_cxyFixed.cx ) {
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if( m_bShowButton1 ) {
                m_rcButton1.right = rc.left + m_cxyFixed.cx;
                m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
            }
            else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.left = rc.left;
            m_rcButton2.bottom = rc.bottom;
            if( m_bShowButton2 ) {
                m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
                m_rcButton2.right = rc.left + m_cxyFixed.cx;
            }
            else {
                m_rcButton2.top = m_rcButton2.bottom;
                m_rcButton2.right = m_rcButton2.left;
            }

            m_rcThumb.left = rc.left;
            m_rcThumb.right = rc.left + m_cxyFixed.cx;
            if( m_nRange > 0 ) {
                int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
                if( cyThumb < m_cxyFixed.cx ) cyThumb = m_cxyFixed.cx;

                m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
                m_rcThumb.bottom = m_rcThumb.top + cyThumb;
                if( m_rcThumb.bottom > m_rcButton2.top ) {
                    m_rcThumb.top = m_rcButton2.top - cyThumb;
                    m_rcThumb.bottom = m_rcButton2.top;
                }
            }
            else {
                m_rcThumb.top = m_rcButton1.bottom;
                m_rcThumb.bottom = m_rcButton2.top;
            }
        }
        else {
            int cyButton = (rc.bottom - rc.top) / 2;
            if( cyButton > m_cxyFixed.cx ) cyButton = m_cxyFixed.cx;
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if( m_bShowButton1 ) {
                m_rcButton1.right = rc.left + m_cxyFixed.cx;
                m_rcButton1.bottom = rc.top + cyButton;
            }
            else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.left = rc.left;
            m_rcButton2.bottom = rc.bottom;
            if( m_bShowButton2 ) {
                m_rcButton2.top = rc.bottom - cyButton;
                m_rcButton2.right = rc.left + m_cxyFixed.cx;
            }
            else {
                m_rcButton2.top = m_rcButton2.bottom;
                m_rcButton2.right = m_rcButton2.left;
            }

            memset(&m_rcThumb,0, sizeof(m_rcThumb));
        }
    }
}

void ZScrollBar::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else ZControl::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        return;
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
    {
        if( !IsEnabled() ) return;

        m_nLastScrollOffset = 0;
        m_nScrollRepeatDelay = 0;
        
		//m_pManager->SetTimer(this, DEFAULT_TIMERID, 50U);

        if( PtInRect(&m_rcButton1, event.ptMouse) ) {
            m_uButton1State |= UISTATE_PUSHED;
            if( !m_bHorizontal ) {
                if( m_pOwner != NULL ) m_pOwner->LineUp(); 
                else SetScrollPos(m_nScrollPos - m_nLineSize);
            }
            else {
                if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
                else SetScrollPos(m_nScrollPos - m_nLineSize);
            }
        }
        else if( PtInRect(&m_rcButton2, event.ptMouse) ) {
            m_uButton2State |= UISTATE_PUSHED;
            if( !m_bHorizontal ) {
                if( m_pOwner != NULL ) m_pOwner->LineDown(); 
                else SetScrollPos(m_nScrollPos + m_nLineSize);
            }
            else {
                if( m_pOwner != NULL ) m_pOwner->LineRight(); 
                else SetScrollPos(m_nScrollPos + m_nLineSize);
            }
        }
        else if( PtInRect(&m_rcThumb, event.ptMouse) ) {
            m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
            ptLastMouse = event.ptMouse;
            m_nLastScrollPos = m_nScrollPos;
        }
        else {
            if( !m_bHorizontal ) {
                if( event.ptMouse.y < m_rcThumb.top ) {
                    if( m_pOwner != NULL ) m_pOwner->PageUp(); 
                    else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
                }
                else if ( event.ptMouse.y > m_rcThumb.bottom ){
                    if( m_pOwner != NULL ) m_pOwner->PageDown(); 
                    else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
                }
            }
            else {
                if( event.ptMouse.x < m_rcThumb.left ) {
                    if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
                    else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
                }
                else if ( event.ptMouse.x > m_rcThumb.right ){
                    if( m_pOwner != NULL ) m_pOwner->PageRight(); 
                    else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
                }
            }
        }
        if( m_pManager != NULL && m_pOwner == NULL )
		{
			DoLuaEvent("scroll");
		}
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        m_nScrollRepeatDelay = 0;
        m_nLastScrollOffset = 0;
        
		//m_pManager->KillTimer(this, DEFAULT_TIMERID);

        if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
            m_uThumbState &= ~( UISTATE_CAPTURED | UISTATE_PUSHED );
            Invalidate();
        }
        else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
            m_uButton1State &= ~UISTATE_PUSHED;
            Invalidate();
        }
        else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
            m_uButton2State &= ~UISTATE_PUSHED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
            if( !m_bHorizontal ) {
                 m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / \
                    (m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top - 2 * m_cxyFixed.cx);
            }
            else {
                m_nLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_nRange / \
                    (m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left - 2 * m_cxyFixed.cy);
            }


			if( !m_bHorizontal ) {
				SIZE sz={m_pOwner->GetScrollPos().cx,m_nLastScrollPos + m_nLastScrollOffset};
				if( m_pOwner != NULL ) 
					m_pOwner->SetScrollPos(sz); 
				else
					SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
			else {
				SIZE sz={m_nLastScrollPos + m_nLastScrollOffset,m_pOwner->GetScrollPos().cy};
				if( m_pOwner != NULL ) 
					m_pOwner->SetScrollPos(sz); 
				else 
					SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
        }
        else {
            if( (m_uThumbState & UISTATE_HOT) != 0 ) {
                if( !PtInRect(&m_rcThumb, event.ptMouse) ) {
                    m_uThumbState &= ~UISTATE_HOT;
                }
            }
            else {
                if( !IsEnabled() ) return;
                if( PtInRect(&m_rcThumb, event.ptMouse) ) {
                    m_uThumbState |= UISTATE_HOT;
                }
            }
        }



        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID )
    {
        ++m_nScrollRepeatDelay;
        if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) 
		{
            if( !m_bHorizontal ) {
				SIZE sz={m_pOwner->GetScrollPos().cx,m_nLastScrollPos + m_nLastScrollOffset};
                if( m_pOwner != NULL ) 
					m_pOwner->SetScrollPos(sz); 
                else
					SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
            }
            else {
				SIZE sz={m_nLastScrollPos + m_nLastScrollOffset,m_pOwner->GetScrollPos().cy};
                if( m_pOwner != NULL ) 
					m_pOwner->SetScrollPos(sz); 
                else 
					SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
            }
            Invalidate();
        }
        else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
            if( m_nScrollRepeatDelay <= 5 ) return;
            if( !m_bHorizontal ) {
                if( m_pOwner != NULL ) 
					m_pOwner->LineUp(); 
                else 
					SetScrollPos(m_nScrollPos - m_nLineSize);
            }
            else {
                if( m_pOwner != NULL ) 
					m_pOwner->LineLeft(); 
                else 
					SetScrollPos(m_nScrollPos - m_nLineSize);
            }
        }
        else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
            if( m_nScrollRepeatDelay <= 5 ) return;
            if( !m_bHorizontal ) {
                if( m_pOwner != NULL ) 
					m_pOwner->LineDown(); 
                else 
					SetScrollPos(m_nScrollPos + m_nLineSize);
            }
            else {
                if( m_pOwner != NULL ) 
					m_pOwner->LineRight(); 
                else 
					SetScrollPos(m_nScrollPos + m_nLineSize);
            }
        }
        else 
		{
            if( m_nScrollRepeatDelay <= 5 ) return;
            POINT pt = { 0 };
            //::GetCursorPos(&pt);
            //::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
            if( !m_bHorizontal ) {
                if( pt.y < m_rcThumb.top ) {
                    if( m_pOwner != NULL ) 
						m_pOwner->PageUp(); 
                    else 
						SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
                }
                else if ( pt.y > m_rcThumb.bottom ){
                    if( m_pOwner != NULL ) 
						m_pOwner->PageDown(); 
                    else 
						SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
                }
            }
            else {
                if( pt.x < m_rcThumb.left ) {
                    if( m_pOwner != NULL )
						m_pOwner->PageLeft(); 
                    else
						SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
                }
                else if ( pt.x > m_rcThumb.right ){
                    if( m_pOwner != NULL )
						m_pOwner->PageRight(); 
                    else
						SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
                }
            }
        }
        if( m_pManager != NULL && m_pOwner == NULL )
		{
			DoLuaEvent("scroll");
		}
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( IsEnabled() ) {
            m_uButton1State |= UISTATE_HOT;
            m_uButton2State |= UISTATE_HOT;
            if( PtInRect(&m_rcThumb, event.ptMouse) ) 
				m_uThumbState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( IsEnabled() ) {
            m_uButton1State &= ~UISTATE_HOT;
            m_uButton2State &= ~UISTATE_HOT;
            m_uThumbState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }

    if( m_pOwner != NULL ) 
		m_pOwner->DoEvent(event); 
	else 
		ZControl::DoEvent(event);
}

void ZScrollBar::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "button1normalimage") == 0 ) SetButton1NormalImage(pstrValue);
    else if( strcmp(pstrName, "button1hotimage") == 0 ) SetButton1HotImage(pstrValue);
    else if( strcmp(pstrName, "button1pushedimage") == 0 ) SetButton1PushedImage(pstrValue);
    else if( strcmp(pstrName, "button1disabledimage") == 0 ) SetButton1DisabledImage(pstrValue);
    else if( strcmp(pstrName, "button2normalimage") == 0 ) SetButton2NormalImage(pstrValue);
    else if( strcmp(pstrName, "button2hotimage") == 0 ) SetButton2HotImage(pstrValue);
    else if( strcmp(pstrName, "button2pushedimage") == 0 ) SetButton2PushedImage(pstrValue);
    else if( strcmp(pstrName, "button2disabledimage") == 0 ) SetButton2DisabledImage(pstrValue);
    else if( strcmp(pstrName, "thumbnormalimage") == 0 ) SetThumbNormalImage(pstrValue);
    else if( strcmp(pstrName, "thumbhotimage") == 0 ) SetThumbHotImage(pstrValue);
    else if( strcmp(pstrName, "thumbpushedimage") == 0 ) SetThumbPushedImage(pstrValue);
    else if( strcmp(pstrName, "thumbdisabledimage") == 0 ) SetThumbDisabledImage(pstrValue);
    else if( strcmp(pstrName, "railnormalimage") == 0 ) SetRailNormalImage(pstrValue);
    else if( strcmp(pstrName, "railhotimage") == 0 ) SetRailHotImage(pstrValue);
    else if( strcmp(pstrName, "railpushedimage") == 0 ) SetRailPushedImage(pstrValue);
    else if( strcmp(pstrName, "raildisabledimage") == 0 ) SetRailDisabledImage(pstrValue);
    else if( strcmp(pstrName, "bknormalimage") == 0 ) SetBkNormalImage(pstrValue);
    else if( strcmp(pstrName, "bkhotimage") == 0 ) SetBkHotImage(pstrValue);
    else if( strcmp(pstrName, "bkpushedimage") == 0 ) SetBkPushedImage(pstrValue);
    else if( strcmp(pstrName, "bkdisabledimage") == 0 ) SetBkDisabledImage(pstrValue);
    else if( strcmp(pstrName, "hor") == 0 ) SetHorizontal(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "linesize") == 0 ) SetLineSize(atoi(pstrValue));
    else if( strcmp(pstrName, "range") == 0 ) SetScrollRange(atoi(pstrValue));
    else if( strcmp(pstrName, "value") == 0 ) SetScrollPos(atoi(pstrValue));
    else if( strcmp(pstrName, "showbutton1") == 0 ) SetShowButton1(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "showbutton2") == 0 ) SetShowButton2(strcmp(pstrValue, "true") == 0);
    else ZControl::SetAttribute(pstrName, pstrValue);
}

void ZScrollBar::DoPaint(ZRenerder* hDC, const RECT& rcPaint)
{
    if( !IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	hDC->SetClipRect(m_rcPaint);
    PaintBk(hDC);
    PaintButton1(hDC);
    PaintButton2(hDC);
    PaintThumb(hDC);
    PaintRail(hDC);
}

void ZScrollBar::PaintBk(ZRenerder* hDC)
{
    if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
    else m_uThumbState &= ~ UISTATE_DISABLED;

    if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
        if( !m_sBkDisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sBkDisabledImage) ) m_sBkDisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
        if( !m_sBkPushedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sBkPushedImage) ) m_sBkPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
        if( !m_sBkHotImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sBkHotImage) ) m_sBkHotImage.Empty();
            else return;
        }
    }

    if( !m_sBkNormalImage.IsEmpty() ) {
        if( !DrawImage(hDC, (const char*)m_sBkNormalImage) ) m_sBkNormalImage.Empty();
        else return;
    }
}

void ZScrollBar::PaintButton1(ZRenerder* hDC)
{
    if( !m_bShowButton1 ) return;
    
    if( !IsEnabled() ) m_uButton1State |= UISTATE_DISABLED;
    else m_uButton1State &= ~ UISTATE_DISABLED;

    m_sImageModify.Empty();
    m_sImageModify.Format("dest='%d,%d,%d,%d'", m_rcButton1.left - m_rcItem.left, \
        m_rcButton1.top - m_rcItem.top, m_rcButton1.right - m_rcItem.left, m_rcButton1.bottom - m_rcItem.top);

    if( (m_uButton1State & UISTATE_DISABLED) != 0 ) {
        if( !m_sButton1DisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sButton1DisabledImage, (const char*)m_sImageModify) ) m_sButton1DisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
        if( !m_sButton1PushedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sButton1PushedImage, (const char*)m_sImageModify) ) m_sButton1PushedImage.Empty();
            else return;
        }
    }
    else if( (m_uButton1State & UISTATE_HOT) != 0 ) {
        if( !m_sButton1HotImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sButton1HotImage, (const char*)m_sImageModify) ) m_sButton1HotImage.Empty();
            else return;
        }
    }

    if( !m_sButton1NormalImage.IsEmpty() ) {
        if( !DrawImage(hDC, (const char*)m_sButton1NormalImage, (const char*)m_sImageModify) ) m_sButton1NormalImage.Empty();
        else return;
    }

    color_t dwBorderColor = 0xFF85E4FF;
	hDC->SetDrawColor(dwBorderColor);
    hDC->DrawRect(m_rcButton1);
}

void ZScrollBar::PaintButton2(ZRenerder* hDC)
{
    if( !m_bShowButton2 ) return;
    
    if( !IsEnabled() ) m_uButton2State |= UISTATE_DISABLED;
    else m_uButton2State &= ~ UISTATE_DISABLED;

    m_sImageModify.Empty();
    m_sImageModify.Format("dest='%d,%d,%d,%d'", m_rcButton2.left - m_rcItem.left, \
        m_rcButton2.top - m_rcItem.top, m_rcButton2.right - m_rcItem.left, m_rcButton2.bottom - m_rcItem.top);

    if( (m_uButton2State & UISTATE_DISABLED) != 0 ) {
        if( !m_sButton2DisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sButton2DisabledImage, (const char*)m_sImageModify) ) m_sButton2DisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
        if( !m_sButton2PushedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sButton2PushedImage, (const char*)m_sImageModify) ) m_sButton2PushedImage.Empty();
            else return;
        }
    }
    else if( (m_uButton2State & UISTATE_HOT) != 0 ) {
        if( !m_sButton2HotImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sButton2HotImage, (const char*)m_sImageModify) ) m_sButton2HotImage.Empty();
            else return;
        }
    }

    if( !m_sButton2NormalImage.IsEmpty() ) {
        if( !DrawImage(hDC, (const char*)m_sButton2NormalImage, (const char*)m_sImageModify) ) m_sButton2NormalImage.Empty();
        else return;
    }

    color_t dwBorderColor = 0xFF85E4FF;
	hDC->SetDrawColor(dwBorderColor);
    hDC->DrawRect(m_rcButton2);
}

void ZScrollBar::PaintThumb(ZRenerder* hDC)
{
    if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
    if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
    else m_uThumbState &= ~ UISTATE_DISABLED;

    m_sImageModify.Empty();
    m_sImageModify.Format("dest='%d,%d,%d,%d'", m_rcThumb.left - m_rcItem.left, \
        m_rcThumb.top - m_rcItem.top, m_rcThumb.right - m_rcItem.left, m_rcThumb.bottom - m_rcItem.top);

    if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
        if( !m_sThumbDisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sThumbDisabledImage, (const char*)m_sImageModify) ) m_sThumbDisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
        if( !m_sThumbPushedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sThumbPushedImage, (const char*)m_sImageModify) ) m_sThumbPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
        if( !m_sThumbHotImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sThumbHotImage, (const char*)m_sImageModify) ) m_sThumbHotImage.Empty();
            else return;
        }
    }

    if( !m_sThumbNormalImage.IsEmpty() ) {
        if( !DrawImage(hDC, (const char*)m_sThumbNormalImage, (const char*)m_sImageModify) ) m_sThumbNormalImage.Empty();
        else return;
    }

    color_t dwBorderColor = 0xFF85E4FF;
	hDC->SetDrawColor(dwBorderColor);
    hDC->DrawRect(m_rcThumb);
}

void ZScrollBar::PaintRail(ZRenerder* hDC)
{
    if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
    if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
    else m_uThumbState &= ~ UISTATE_DISABLED;

    m_sImageModify.Empty();
    if( !m_bHorizontal ) {
        m_sImageModify.Format("dest='%d,%d,%d,%d'", m_rcThumb.left - m_rcItem.left, \
            (m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top - m_cxyFixed.cx / 2, \
            m_rcThumb.right - m_rcItem.left, \
            (m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top + m_cxyFixed.cx - m_cxyFixed.cx / 2);
    }
    else {
        m_sImageModify.Format("dest='%d,%d,%d,%d'", \
            (m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left - m_cxyFixed.cy / 2, \
            m_rcThumb.top - m_rcItem.top, \
            (m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left + m_cxyFixed.cy - m_cxyFixed.cy / 2, \
            m_rcThumb.bottom - m_rcItem.top);
    }

    if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
        if( !m_sRailDisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sRailDisabledImage, (const char*)m_sImageModify) ) m_sRailDisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
        if( !m_sRailPushedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sRailPushedImage, (const char*)m_sImageModify) ) m_sRailPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
        if( !m_sRailHotImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sRailHotImage, (const char*)m_sImageModify) ) m_sRailHotImage.Empty();
            else return;
        }
    }

    if( !m_sRailNormalImage.IsEmpty() ) {
        if( !DrawImage(hDC, (const char*)m_sRailNormalImage, (const char*)m_sImageModify) ) m_sRailNormalImage.Empty();
        else return;
    }
}




















IMPL_ZCONTROL_FACTORY("ScrollBar",ZScrollBar);

}