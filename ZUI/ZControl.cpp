#include "ZControl.h"
#include "ZPaintManager.h"
#include "ZRenerder.h"
#include "ZControlFactory.h"


namespace ZuiLib
{


ZControl::ZControl()
	:m_pManager(NULL), 
	m_pParent(NULL), 
	m_bUpdateNeeded(true),
	m_bMenuUsed(false),
	m_bVisible(true), 
	m_bInternVisible(true),
	m_bFocused(false),
	m_bEnabled(true),
	m_bMouseEnabled(true),
	m_bKeyboardEnabled(true),
	m_bFloat(false),
	m_chShortcut('\0'),
	m_pTag(NULL),
	m_dwBackColor(0),
	m_dwBorderColor(0),
	m_dwFocusBorderColor(0),
	m_bColorHSL(false),
	m_nBorderSize(0),
	m_nBorderStyle(0),
	m_nTooltipWidth(300)
{
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 9999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

	memset(&m_rcPadding,0,sizeof(RECT));
	memset(&m_rcItem,0,sizeof(RECT));
	memset(&m_rcPaint,0,sizeof(RECT));
}

ZControl::~ZControl()
{
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

bool ZControl::DoLuaEvent(const char* evName)
{
	return false;
}
bool ZControl::DoLuaEvent(const char* evName,int param)
{
	return false;
}
bool ZControl::DoLuaEvent(const char* evName,const char* param)
{
	return false;
}
bool ZControl::DoLuaEvent(const char* evName,bool param)
{
	return false;
}

ZString ZControl::GetName() const
{
    return m_sName;
}

void ZControl::SetName(const char* pstrName)
{
    m_sName = pstrName;
}

void* ZControl::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "ControlUI") == 0 ) return this;
    return NULL;
}

const char* ZControl::GetClass() const
{
    return "ControlUI";
}

uint32_t ZControl::GetControlFlags() const
{
    return 0;
}

bool ZControl::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

ZPaintManager* ZControl::GetManager() const
{
    return m_pManager;
}

ZRenerder* ZControl::GetRenerder() const
{
	if (m_pManager)
		return m_pManager->GetRenerder();
	else
		return NULL;
}

ZTextInfo* ZControl::GetTextInfo(const char* name)
{
	ZRenerder* ren=GetRenerder();
	if (ren && name && name[0])
		return ren->GetFontMgr()->GetTextInfo(name);
	else
		return ZFontMgr::GetDefaultTextInfo();
}

void ZControl::SetManager(ZPaintManager* pManager, ZControl* pParent, bool bInit)
{
    m_pManager = pManager;
    m_pParent = pParent;
    if( bInit && m_pParent ) Init();
}

ZControl* ZControl::GetParent() const
{
    return m_pParent;
}

ZString ZControl::GetText() const
{
    return m_sText;
}

void ZControl::SetText(const char* pstrText)
{
    if( m_sText == pstrText ) return;

    m_sText = pstrText;
    Invalidate();
}

uint32_t ZControl::GetBkColor() const
{
    return m_dwBackColor;
}

void ZControl::SetBkColor(uint32_t dwBackColor)
{
    if( m_dwBackColor == dwBackColor ) return;

    m_dwBackColor = dwBackColor;
    Invalidate();
}



const char* ZControl::GetBkImage()
{
    return m_sBkImage;
}

void ZControl::SetBkImage(const char* pStrImage)
{
    if( m_sBkImage == pStrImage ) return;

    m_sBkImage = pStrImage;
    Invalidate();
}

uint32_t ZControl::GetBorderColor() const
{
    return m_dwBorderColor;
}

void ZControl::SetBorderColor(uint32_t dwBorderColor)
{
    if( m_dwBorderColor == dwBorderColor ) return;

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

uint32_t ZControl::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void ZControl::SetFocusBorderColor(uint32_t dwBorderColor)
{
    if( m_dwFocusBorderColor == dwBorderColor ) return;

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}


int ZControl::GetBorderSize() const
{
    return m_nBorderSize;
}

void ZControl::SetBorderSize(int nSize)
{
    if( m_nBorderSize == nSize ) return;

    m_nBorderSize = nSize;
    Invalidate();
}



bool ZControl::DrawImage(ZRenerder* hDC, const char* pStrImage, const char* pStrModify)
{
    return hDC->DrawImageString(m_rcItem, pStrImage,pStrModify);
}

const RECT& ZControl::GetPos() const
{
    return m_rcItem;
}

void ZControl::SetPos(RECT rc)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;
	RECT invalidateRc = m_rcItem;
    m_rcItem = rc;

    if( m_pManager == NULL ) return;

    
    if( m_bFloat ) {
        ZControl* pParent = GetParent();
        if( pParent != NULL ) {
            RECT rcParentPos = pParent->GetPos();
            if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
            else m_cXY.cx = m_rcItem.right - rcParentPos.right;
            if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
            else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
            m_cxyFixed.cx = m_rcItem.right - m_rcItem.left;
            m_cxyFixed.cy = m_rcItem.bottom - m_rcItem.top;
        }
    }

	
    m_bUpdateNeeded = false;
    JoinRect(&invalidateRc,&m_rcItem);

    ZControl* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }
    m_pManager->Invalidate(invalidateRc);
}

int ZControl::GetWidth() const
{
    return m_rcItem.right-m_rcItem.left;
}

int ZControl::GetHeight() const
{
    return m_rcItem.bottom-m_rcItem.top;
}

int ZControl::GetX() const
{
    return m_rcItem.left;
}

int ZControl::GetY() const
{
    return m_rcItem.top;
}

RECT ZControl::GetPadding() const
{
    return m_rcPadding;
}

void ZControl::SetPadding(RECT rcPadding)
{
    m_rcPadding = rcPadding;
    NeedParentUpdate();
}

SIZE ZControl::GetFixedXY() const
{
    return m_cXY;
}

void ZControl::SetFixedXY(SIZE szXY)
{
    m_cXY = szXY;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int ZControl::GetFixedWidth() const
{
    return m_cxyFixed.cx;
}

void ZControl::SetFixedWidth(int cx)
{
    if( cx < 0 ) return; 
    m_cxyFixed.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int ZControl::GetFixedHeight() const
{
    return m_cxyFixed.cy;
}

void ZControl::SetFixedHeight(int cy)
{
    if( cy < 0 ) return; 
    m_cxyFixed.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int ZControl::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void ZControl::SetMinWidth(int cx)
{
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int ZControl::GetMaxWidth() const
{
    return m_cxyMax.cx;
}

void ZControl::SetMaxWidth(int cx)
{
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int ZControl::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void ZControl::SetMinHeight(int cy)
{
    if( m_cxyMin.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMin.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int ZControl::GetMaxHeight() const
{
    return m_cxyMax.cy;
}

void ZControl::SetMaxHeight(int cy)
{
    if( m_cxyMax.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMax.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}


ZString ZControl::GetToolTip() const
{
    return m_sToolTip;
}

void ZControl::SetToolTip(const char* pstrText)
{
	ZString strTemp(pstrText);
	strTemp.Replace("<n>","\r\n");
	m_sToolTip=strTemp;
}

void ZControl::SetToolTipWidth( int nWidth )
{
	m_nTooltipWidth=nWidth;
}

int ZControl::GetToolTipWidth( void )
{
	return m_nTooltipWidth;
}

char ZControl::GetShortcut() const
{
    return m_chShortcut;
}

void ZControl::SetShortcut(char ch)
{
    m_chShortcut = ch;
}

bool ZControl::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void ZControl::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const ZString& ZControl::GetUserData()
{
    return m_sUserData;
}

void ZControl::SetUserData(const char* pstrText)
{
    m_sUserData = pstrText;
}

intptr_t ZControl::GetTag() const
{
    return m_pTag;
}

void ZControl::SetTag(intptr_t pTag)
{
    m_pTag = pTag;
}

bool ZControl::IsVisible() const
{
    return m_bVisible && m_bInternVisible;
}

void ZControl::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }
}

void ZControl::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
}

bool ZControl::IsEnabled() const
{
    return m_bEnabled;
}

void ZControl::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool ZControl::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void ZControl::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool ZControl::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void ZControl::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool ZControl::IsFocused() const
{
    return m_bFocused;
}

void ZControl::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this);
}

bool ZControl::IsFloat() const
{
    return m_bFloat;
}

void ZControl::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

ZControl* ZControl::FindControl(FINDCONTROLPROC Proc, void* pData, uint32_t uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !PtInRect(&m_rcItem,*static_cast<LPPOINT>(pData))) ) return NULL;
    return Proc(this, pData);
}

void ZControl::Invalidate()
{
    if( !IsVisible() ) return;

    RECT invalidateRc = m_rcItem;

    ZControl* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }

    if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
}

bool ZControl::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void ZControl::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void ZControl::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }
    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}


void ZControl::Init()
{
    DoInit();
}

void ZControl::DoInit()
{

}

void ZControl::Event(TEventUI& event)
{
    DoEvent(event);
}

void ZControl::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETCURSOR )
    {
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {

    }
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}


void ZControl::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "pos") == 0 ) {
        RECT rcPos = { 0 };
        char* pstr = NULL;
        rcPos.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
        rcPos.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
        rcPos.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
        rcPos.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
        SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
        SetFixedXY(szXY);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( strcmp(pstrName, "padding") == 0 ) {
        RECT rcPadding = { 0 };
        char* pstr = NULL;
        rcPadding.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
        rcPadding.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
        rcPadding.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
        rcPadding.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
        SetPadding(rcPadding);
    }
    else if( strcmp(pstrName, "bkcolor") == 0 || strcmp(pstrName, "bkcolor1") == 0 ) {
        while( *pstrValue > '\0' && *pstrValue <= ' ') pstrValue = CharNext(pstrValue);
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        uint32_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if( strcmp(pstrName, "bordercolor") == 0 ) {
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        uint32_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
    }
    else if( strcmp(pstrName, "focusbordercolor") == 0 ) {
        if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
        char* pstr = NULL;
        uint32_t clrColor = strtoul(pstrValue, &pstr, 16);
        SetFocusBorderColor(clrColor);
    }
	else if( strcmp(pstrName, "bordersize") == 0 ) {
		SetBorderSize(atoi(pstrValue));
	}
    else if( strcmp(pstrName, "bkimage") == 0 ) SetBkImage(pstrValue);
    else if( strcmp(pstrName, "width") == 0 ) SetFixedWidth(atoi(pstrValue));
    else if( strcmp(pstrName, "height") == 0 ) SetFixedHeight(atoi(pstrValue));
    else if( strcmp(pstrName, "minwidth") == 0 ) SetMinWidth(atoi(pstrValue));
    else if( strcmp(pstrName, "minheight") == 0 ) SetMinHeight(atoi(pstrValue));
    else if( strcmp(pstrName, "maxwidth") == 0 ) SetMaxWidth(atoi(pstrValue));
    else if( strcmp(pstrName, "maxheight") == 0 ) SetMaxHeight(atoi(pstrValue));
    else if( strcmp(pstrName, "name") == 0 ) SetName(pstrValue);
    else if( strcmp(pstrName, "text") == 0 ) SetText(pstrValue);
    else if( strcmp(pstrName, "tooltip") == 0 ) SetToolTip(pstrValue);
    else if( strcmp(pstrName, "userdata") == 0 ) SetUserData(pstrValue);
    else if( strcmp(pstrName, "enabled") == 0 ) SetEnabled(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "mouse") == 0 ) SetMouseEnabled(strcmp(pstrValue, "true") == 0);
	else if( strcmp(pstrName, "keyboard") == 0 ) SetKeyboardEnabled(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "visible") == 0 ) SetVisible(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "float") == 0 ) SetFloat(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "shortcut") == 0 ) SetShortcut(pstrValue[0]);
    else if( strcmp(pstrName, "menu") == 0 ) SetContextMenuUsed(strcmp(pstrValue,"true") == 0);
}

ZControl* ZControl::ApplyAttributeList(const char* pstrList)
{
    ZString sItem;
    ZString sValue;
    while( *pstrList != '\0' ) {
        sItem.Empty();
        sValue.Empty();
        while( *pstrList != '\0' && *pstrList != '=' ) {
            const char* pstrTemp = CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        assert( *pstrList == '=' );
        if( *pstrList++ != '=' ) return this;
        assert( *pstrList == '\"' );
        if( *pstrList++ != '\"' ) return this;
        while( *pstrList != '\0' && *pstrList != '\"' ) {
            const char* pstrTemp = CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        assert( *pstrList == '\"' );
        if( *pstrList++ != '\"' ) return this;
        SetAttribute(sItem, sValue);
        if( *pstrList++ != ' ') return this;
    }
    return this;
}

SIZE ZControl::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

void ZControl::DoPaint(ZRenerder* hDC, const RECT& rcPaint)
{
	if( !IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) 
		return;

	hDC->SetClipRect(m_rcPaint);
    // 绘制循序：背景颜色->背景图->状态图->文本->边框
    if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
}

void ZControl::PaintBkColor(ZRenerder* hDC)
{
    if( m_dwBackColor != 0 ) {
		hDC->SetDrawColor(m_dwBackColor);
		hDC->FillRect(m_rcItem);
    }
}

void ZControl::PaintBkImage(ZRenerder* hDC)
{
    if( m_sBkImage.IsEmpty() ) return;
    if( !DrawImage(hDC, m_sBkImage) ) m_sBkImage.Empty();
}

void ZControl::PaintStatusImage(ZRenerder* hDC)
{
    return;
}

void ZControl::PaintText(ZRenerder* hDC)
{
    return;
}

void ZControl::PaintBorder(ZRenerder* hDC)
{
	if(m_dwBorderColor != 0 || m_dwFocusBorderColor != 0)
	{
		if(m_nBorderSize > 0 && ( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ))//画圆角边框
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				hDC->SetDrawColor(m_dwFocusBorderColor);
			else
				hDC->SetDrawColor(m_dwBorderColor);
			hDC->DrawRoundRect(m_rcItem,m_nBorderSize);
		}
		else
		{
			if (IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize > 0)
			{
				hDC->SetDrawColor(m_dwFocusBorderColor);
				hDC->DrawRect(m_rcItem);
			}
			else if(m_nBorderSize > 0)
			{
				hDC->SetDrawColor(m_dwBackColor);
				hDC->DrawRect(m_rcItem);
			}
		}
	}
}




IMPL_ZCONTROL_FACTORY("Control",ZControl);


}