#include "ZContainer.h"
#include "ZPaintManager.h"
#include "ZScrollBar.h"
#include "ZControlFactory.h"

#define CLAMP(x,a,b) (SDL_min(b,SDL_max(a,x)))

namespace ZuiLib
{

ZContainer::ZContainer() : 
m_iChildPadding(0),
m_bAutoDestroy(true),
m_bDelayedDestroy(true),
m_bMouseChildEnabled(true),
m_pVerticalScrollBar(NULL),
m_pHorizontalScrollBar(NULL),
m_bScrollProcess(false)
{
	memset(&m_rcInset,0, sizeof(m_rcInset));
}

ZContainer::~ZContainer()
{
    m_bDelayedDestroy = false;
    RemoveAll();
    if( m_pVerticalScrollBar ) delete m_pVerticalScrollBar;
    if( m_pHorizontalScrollBar ) delete m_pHorizontalScrollBar;
}

const char* ZContainer::GetClass() const
{
    return "Container";
}

void* ZContainer::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "IContainer") == 0 ) 
		return static_cast<IContainerUI*>(this);
    else if( strcmp(pstrName, "Container") == 0 ) 
		return static_cast<ZContainer*>(this);
	else
		return ZControl::GetInterface(pstrName);
}

ZControl* ZContainer::GetItemAt(int iIndex) const
{
    if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
    return static_cast<ZControl*>(m_items[iIndex]);
}

int ZContainer::GetItemIndex(ZControl* pControl) const
{
    for( int it = 0; it < m_items.GetSize(); it++ ) {
        if( static_cast<ZControl*>(m_items[it]) == pControl ) {
            return it;
        }
    }

    return -1;
}

bool ZContainer::SetItemIndex(ZControl* pControl, int iIndex)
{
    for( int it = 0; it < m_items.GetSize(); it++ ) {
        if( static_cast<ZControl*>(m_items[it]) == pControl ) {
            NeedUpdate();            
            m_items.Remove(it);
            return m_items.InsertAt(iIndex, pControl);
        }
    }

    return false;
}

int ZContainer::GetCount() const
{
    return m_items.GetSize();
}

bool ZContainer::Add(ZControl* pControl)
{
    if( pControl == NULL)
		return false;
    if( m_pManager != NULL )
		m_pManager->InitControls(pControl, this);
    if( IsVisible() ) 
		NeedUpdate();
    else 
		pControl->SetInternVisible(false);
    m_items.Add(pControl);
	return true;
}

bool ZContainer::AddAt(ZControl* pControl, int iIndex)
{
    if( pControl == NULL) return false;

    if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
    if( IsVisible() ) 
		NeedUpdate();
    else 
		pControl->SetInternVisible(false);
    return m_items.InsertAt(iIndex, pControl);
}

bool ZContainer::Remove(ZControl* pControl)
{
    if( pControl == NULL) return false;

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        if( static_cast<ZControl*>(m_items[it]) == pControl ) {
            NeedUpdate();
            if( m_bAutoDestroy ) {
                if( m_bDelayedDestroy && m_pManager ) 
					m_pManager->AddDelayedCleanup(pControl);             
                else 
					delete pControl;
            }
            return m_items.Remove(it);
        }
    }
    return false;
}

bool ZContainer::RemoveAt(int iIndex)
{
    ZControl* pControl = GetItemAt(iIndex);
    if (pControl != NULL) {
        return ZContainer::Remove(pControl);
    }

    return false;
}

void ZContainer::RemoveAll()
{
    for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
        if( m_bDelayedDestroy && m_pManager ) 
			m_pManager->AddDelayedCleanup(static_cast<ZControl*>(m_items[it]));             
        else 
			delete static_cast<ZControl*>(m_items[it]);
    }
    m_items.Empty();
    NeedUpdate();
}

bool ZContainer::IsAutoDestroy() const
{
    return m_bAutoDestroy;
}

void ZContainer::SetAutoDestroy(bool bAuto)
{
    m_bAutoDestroy = bAuto;
}

bool ZContainer::IsDelayedDestroy() const
{
    return m_bDelayedDestroy;
}

void ZContainer::SetDelayedDestroy(bool bDelayed)
{
    m_bDelayedDestroy = bDelayed;
}

RECT ZContainer::GetInset() const
{
    return m_rcInset;
}

void ZContainer::SetInset(RECT rcInset)
{
    m_rcInset = rcInset;
    NeedUpdate();
}

int ZContainer::GetChildPadding() const
{
    return m_iChildPadding;
}

void ZContainer::SetChildPadding(int iPadding)
{
    m_iChildPadding = iPadding;
    NeedUpdate();
}

bool ZContainer::IsMouseChildEnabled() const
{
    return m_bMouseChildEnabled;
}

void ZContainer::SetMouseChildEnabled(bool bEnable)
{
    m_bMouseChildEnabled = bEnable;
}

void ZContainer::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;
    ZControl::SetVisible(bVisible);
    for( int it = 0; it < m_items.GetSize(); it++ ) {
        static_cast<ZControl*>(m_items[it])->SetInternVisible(IsVisible());
    }
}

void ZContainer::SetInternVisible(bool bVisible)
{
    ZControl::SetInternVisible(bVisible);
    if( m_items.IsEmpty() ) return;
    for( int it = 0; it < m_items.GetSize(); it++ ) {
        static_cast<ZControl*>(m_items[it])->SetInternVisible(IsVisible());
    }
}

void ZContainer::SetMouseEnabled(bool bEnabled)
{
    if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetMouseEnabled(bEnabled);
    if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetMouseEnabled(bEnabled);
    ZControl::SetMouseEnabled(bEnabled);
}

void ZContainer::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else ZControl::DoEvent(event);
        return;
    }

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
    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled() )
    {
        if( event.Type == UIEVENT_KEYDOWN ) 
        {
            switch( event.chKey ) {
            case SDLK_DOWN:
             LineDown();
             return;
            case SDLK_UP:
             LineUp();
             return;
            case SDLK_HOME:
             HomeUp();
             return;
            case SDLK_END:
             EndDown();
             return;
            }
        }
        else if( event.Type == UIEVENT_SCROLLWHEEL )
        {
            switch(event.wParam ) {
			case SBT_LINEUP:
              LineUp();
              return;
            case SBT_LINEDOWN:
              LineDown();
              return;
            }
        }
    }
    else if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled() ) {
        if( event.Type == UIEVENT_KEYDOWN ) 
        {
            switch( event.chKey ) {
            case SDLK_DOWN:
                LineRight();
                return;
            case SDLK_UP:
                LineLeft();
                return;
            case SDLK_HOME:
                HomeLeft();
                return;
            case SDLK_END:
                EndRight();
                return;
            }
        }
        else if( event.Type == UIEVENT_SCROLLWHEEL )
        {
            switch(event.wParam ) {
            case SBT_LINEUP:
                LineLeft();
                return;
            case SBT_LINEDOWN:
                LineRight();
                return;
            }
        }
    }
    ZControl::DoEvent(event);
}

SIZE ZContainer::GetScrollPos() const
{
    SIZE sz = {0, 0};
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollPos();
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
    return sz;
}

SIZE ZContainer::GetScrollRange() const
{
    SIZE sz = {0, 0};
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollRange();
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
    return sz;
}

void ZContainer::SetScrollPos(SIZE szPos)
{
    int cx = 0;
    int cy = 0;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
        m_pVerticalScrollBar->SetScrollPos(szPos.cy);
        cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
        m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
        cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if( cx == 0 && cy == 0 ) return;

    RECT rcPos;
    for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
        ZControl* pControl = static_cast<ZControl*>(m_items[it2]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) continue;

        rcPos = pControl->GetPos();
        rcPos.left -= cx;
        rcPos.right -= cx;
        rcPos.top -= cy;
        rcPos.bottom -= cy;
        pControl->SetPos(rcPos);
    }

    Invalidate();
}

void ZContainer::LineUp()
{
    int cyLine = 21;

    SIZE sz = GetScrollPos();
    sz.cy -= cyLine;
    SetScrollPos(sz);
}

void ZContainer::LineDown()
{
    int cyLine = 21;
    SIZE sz = GetScrollPos();
    sz.cy += cyLine;
    SetScrollPos(sz);
}

void ZContainer::PageUp()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
    sz.cy -= iOffset;
    SetScrollPos(sz);
}

void ZContainer::PageDown()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
    sz.cy += iOffset;
    SetScrollPos(sz);
}

void ZContainer::HomeUp()
{
    SIZE sz = GetScrollPos();
    sz.cy = 0;
    SetScrollPos(sz);
}

void ZContainer::EndDown()
{
    SIZE sz = GetScrollPos();
    sz.cy = GetScrollRange().cy;
    SetScrollPos(sz);
}

void ZContainer::LineLeft()
{
    SIZE sz = GetScrollPos();
    sz.cx -= 8;
    SetScrollPos(sz);
}

void ZContainer::LineRight()
{
    SIZE sz = GetScrollPos();
    sz.cx += 8;
    SetScrollPos(sz);
}

void ZContainer::PageLeft()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
		iOffset -= m_pVerticalScrollBar->GetFixedWidth();
    sz.cx -= iOffset;
    SetScrollPos(sz);
}

void ZContainer::PageRight()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
		iOffset -= m_pVerticalScrollBar->GetFixedWidth();
    sz.cx += iOffset;
    SetScrollPos(sz);
}

void ZContainer::HomeLeft()
{
    SIZE sz = GetScrollPos();
    sz.cx = 0;
    SetScrollPos(sz);
}

void ZContainer::EndRight()
{
    SIZE sz = GetScrollPos();
    sz.cx = GetScrollRange().cx;
    SetScrollPos(sz);
}

void ZContainer::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
    if( bEnableVertical && !m_pVerticalScrollBar ) {
        m_pVerticalScrollBar = new ZScrollBar;
        m_pVerticalScrollBar->SetOwner(this);
        m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);
        if ( m_pManager ) {
            const char* pDefaultAttributes = m_pManager->GetDefaultAttributeList("VScrollBar");
            if( pDefaultAttributes ) {
                m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
            }
        }
    }
    else if( !bEnableVertical && m_pVerticalScrollBar ) {
        delete m_pVerticalScrollBar;
        m_pVerticalScrollBar = NULL;
    }

    if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
        m_pHorizontalScrollBar = new ZScrollBar;
        m_pHorizontalScrollBar->SetHorizontal(true);
        m_pHorizontalScrollBar->SetOwner(this);
        m_pHorizontalScrollBar->SetManager(m_pManager, NULL, false);
        if ( m_pManager ) {
            const char* pDefaultAttributes = m_pManager->GetDefaultAttributeList("HScrollBar");
            if( pDefaultAttributes ) {
                m_pHorizontalScrollBar->ApplyAttributeList(pDefaultAttributes);
            }
        }
    }
    else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
        delete m_pHorizontalScrollBar;
        m_pHorizontalScrollBar = NULL;
    }

    NeedUpdate();
}

ZScrollBar* ZContainer::GetVerticalScrollBar() const
{
    return m_pVerticalScrollBar;
}

ZScrollBar* ZContainer::GetHorizontalScrollBar() const
{
    return m_pHorizontalScrollBar;
}

int ZContainer::FindSelectable(int iIndex, bool bForward /*= true*/) const
{
    // NOTE: This is actually a helper-function for the list/combo/ect controls
    //       that allow them to find the next enabled/available selectable item
    if( GetCount() == 0 ) return -1;
    iIndex = CLAMP(iIndex, 0, GetCount() - 1);
    if( bForward ) {
        for( int i = iIndex; i < GetCount(); i++ ) {
            if( GetItemAt(i)->GetInterface("ListItem") != NULL 
                && GetItemAt(i)->IsVisible()
                && GetItemAt(i)->IsEnabled() ) return i;
        }
        return -1;
    }
    else {
        for( int i = iIndex; i >= 0; --i ) {
            if( GetItemAt(i)->GetInterface("ListItem") != NULL 
                && GetItemAt(i)->IsVisible()
                && GetItemAt(i)->IsEnabled() ) return i;
        }
        return FindSelectable(0, true);
    }
}

void ZContainer::SetPos(RECT rc)
{
    ZControl::SetPos(rc);
    if( m_items.IsEmpty() ) return;
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        ZControl* pControl = static_cast<ZControl*>(m_items[it]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) {
            SetFloatPos(it);
        }
        else {
            pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
        }
    }
}

void ZContainer::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "inset") == 0 ) {
        RECT rcInset = { 0 };
        char* pstr = NULL;
        rcInset.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
        rcInset.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
        rcInset.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
        rcInset.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
        SetInset(rcInset);
    }
    else if( strcmp(pstrName, "mousechild") == 0 ) SetMouseChildEnabled(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "vscrollbar") == 0 ) {
        EnableScrollBar(strcmp(pstrValue, "true") == 0, GetHorizontalScrollBar() != NULL);
    }
    else if( strcmp(pstrName, "vscrollbarstyle") == 0 ) {
        EnableScrollBar(true, GetHorizontalScrollBar() != NULL);
        if( GetVerticalScrollBar() ) GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
    }
    else if( strcmp(pstrName, "hscrollbar") == 0 ) {
        EnableScrollBar(GetVerticalScrollBar() != NULL, strcmp(pstrValue, "true") == 0);
    }
    else if( strcmp(pstrName, "hscrollbarstyle") == 0 ) {
        EnableScrollBar(GetVerticalScrollBar() != NULL, true);
        if( GetHorizontalScrollBar() ) GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
    }
    else if( strcmp(pstrName, "childpadding") == 0 ) SetChildPadding(atoi(pstrValue));
    else ZControl::SetAttribute(pstrName, pstrValue);
}

void ZContainer::SetManager(ZPaintManager* pManager, ZControl* pParent, bool bInit)
{
    for( int it = 0; it < m_items.GetSize(); it++ ) {
        static_cast<ZControl*>(m_items[it])->SetManager(pManager, this, bInit);
    }
    
    if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
    if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);
    ZControl::SetManager(pManager, pParent, bInit);
}

ZControl* ZContainer::FindControl(FINDCONTROLPROC Proc, void* pData, uint32_t uFlags)
{
    // Check if this guy is valid
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
    if( (uFlags & UIFIND_HITTEST) != 0 ) {
        if( !PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
        if( !m_bMouseChildEnabled ) {
            ZControl* pResult = NULL;
            if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
            if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
            if( pResult == NULL ) pResult = ZControl::FindControl(Proc, pData, uFlags);
            return pResult;
        }
    }

    ZControl* pResult = NULL;
    if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
    if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
    if( pResult != NULL ) return pResult;

    if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
        ZControl* pControl = ZControl::FindControl(Proc, pData, uFlags);
        if( pControl != NULL ) return pControl;
    }
    RECT rc = m_rcItem;
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
        for( int it = m_items.GetSize() - 1; it >= 0; it-- ) {
            ZControl* pControl = static_cast<ZControl*>(m_items[it])->FindControl(Proc, pData, uFlags);
            if( pControl != NULL ) {
                if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
                    continue;
                else 
                    return pControl;
            }            
        }
    }
    else {
        for( int it = 0; it < m_items.GetSize(); it++ ) {
            ZControl* pControl = static_cast<ZControl*>(m_items[it])->FindControl(Proc, pData, uFlags);
            if( pControl != NULL ) {
                if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
                    continue;
                else 
                    return pControl;
            } 
        }
    }

    if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) pResult = ZControl::FindControl(Proc, pData, uFlags);
    return pResult;
}

void ZContainer::DoPaint(ZRenerder* hDC, const RECT& rcPaint)
{
    RECT rcTemp = { 0 };
    if( !IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

    ZControl::DoPaint(hDC, rcTemp);

    if( m_items.GetSize() > 0 ) {
        RECT rc = m_rcItem;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

		//hDC->SetClipRect(rc);

        if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		
        if( !IntersectRect(&rcTemp, &rcPaint, &rc) ) {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                ZControl* pControl = static_cast<ZControl*>(m_items[it]);
                if( !pControl->IsVisible() ) continue;
                if( !IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcTemp);
                }
            }
        }
        else {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                ZControl* pControl = static_cast<ZControl*>(m_items[it]);
                if( !pControl->IsVisible() ) continue;
                if( !IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcTemp);
                }
                else {
                    if( !IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcTemp);
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
        if( IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
            m_pVerticalScrollBar->DoPaint(hDC, rcTemp);
        }
    }

    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        if( IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
            m_pHorizontalScrollBar->DoPaint(hDC, rcTemp);
        }
    }
}

void ZContainer::SetFloatPos(int iIndex)
{
    // 因为ZControl::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
    if( iIndex < 0 || iIndex >= m_items.GetSize() ) return;

    ZControl* pControl = static_cast<ZControl*>(m_items[iIndex]);

    if( !pControl->IsVisible() ) return;
    if( !pControl->IsFloat() ) return;

    SIZE szXY = pControl->GetFixedXY();
    SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
    RECT rcCtrl = { 0 };
    if( szXY.cx >= 0 ) {
        rcCtrl.left = m_rcItem.left + szXY.cx;
        rcCtrl.right = m_rcItem.left + szXY.cx + sz.cx;
    }
    else {
        rcCtrl.left = m_rcItem.right + szXY.cx - sz.cx;
        rcCtrl.right = m_rcItem.right + szXY.cx;
    }
    if( szXY.cy >= 0 ) {
        rcCtrl.top = m_rcItem.top + szXY.cy;
        rcCtrl.bottom = m_rcItem.top + szXY.cy + sz.cy;
    }
    else {
        rcCtrl.top = m_rcItem.bottom + szXY.cy - sz.cy;
        rcCtrl.bottom = m_rcItem.bottom + szXY.cy;
    }
	//if( pControl->IsRelativePos() )
	//{
	//	TRelativePosUI tRelativePos = pControl->GetRelativePos();
	//	SIZE szParent = {m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top};
	//	if(tRelativePos.szParent.cx != 0)
	//	{
	//		int nIncrementX = szParent.cx-tRelativePos.szParent.cx;
	//		int nIncrementY = szParent.cy-tRelativePos.szParent.cy;
	//		rcCtrl.left += (nIncrementX*tRelativePos.nMoveXPercent/100);
	//		rcCtrl.top += (nIncrementY*tRelativePos.nMoveYPercent/100);
	//		rcCtrl.right = rcCtrl.left+sz.cx+(nIncrementX*tRelativePos.nZoomXPercent/100);
	//		rcCtrl.bottom = rcCtrl.top+sz.cy+(nIncrementY*tRelativePos.nZoomYPercent/100);
	//	}
	//	pControl->SetRelativeParentSize(szParent);
	//}
    pControl->SetPos(rcCtrl);
}

void ZContainer::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
{
    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
        m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
    }

    if( m_pVerticalScrollBar == NULL ) return;

    if( cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible() ) {
        m_pVerticalScrollBar->SetVisible(true);
        m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
        m_pVerticalScrollBar->SetScrollPos(0);
        m_bScrollProcess = true;
        SetPos(m_rcItem);
        m_bScrollProcess = false;
        return;
    }
    // No scrollbar required
    if( !m_pVerticalScrollBar->IsVisible() ) return;

    // Scroll not needed anymore?
    int cyScroll = cyRequired - (rc.bottom - rc.top);
    if( cyScroll <= 0 && !m_bScrollProcess) {
        m_pVerticalScrollBar->SetVisible(false);
        m_pVerticalScrollBar->SetScrollPos(0);
        m_pVerticalScrollBar->SetScrollRange(0);
        SetPos(m_rcItem);
    }
    else
    {
        RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
        m_pVerticalScrollBar->SetPos(rcScrollBarPos);

        if( m_pVerticalScrollBar->GetScrollRange() != cyScroll ) {
            int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));
            if( m_pVerticalScrollBar->GetScrollRange() == 0 ) {
                m_pVerticalScrollBar->SetVisible(false);
                m_pVerticalScrollBar->SetScrollPos(0);
            }
            if( iScrollPos > m_pVerticalScrollBar->GetScrollPos() ) {
                SetPos(m_rcItem);
            }
        }
    }
}







IMPL_ZCONTROL_FACTORY("Container",ZContainer);







}


