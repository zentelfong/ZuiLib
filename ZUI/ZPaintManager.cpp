#include "ZPaintManager.h"
#include "ZControl.h"
#include "ZWindow.h"

namespace ZuiLib
{

	typedef struct tagFINDTABINFO
	{
		ZControl* pFocus;
		ZControl* pLast;
		bool bForward;
		bool bNextIsIt;
	} FINDTABINFO;

	typedef struct tagFINDSHORTCUT
	{
		char ch;
		bool bPickNext;
	} FINDSHORTCUT;

	ZPaintManager::ZPaintManager()
		:m_render(NULL),
		m_pRoot(NULL),
		m_pFocus(NULL),
		m_pEventHover(NULL),
		m_pEventKey(NULL),
		m_bFirstLayout(true),
		m_bFocusNeeded(false),
		m_bUpdateNeeded(false)
	{
		m_dwDefaultDisabledColor = 0xFFA7A6AA;
		m_dwDefaultFontColor = 0xFF000000;
		m_dwDefaultLinkFontColor = 0xFF0000FF;
		m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
		m_dwDefaultSelectedBkColor = 0xFFBAE4FF;
		SetRectEmpty(&m_rcInvalidate);
		SetRectEmpty(&m_rcSizeBox);
		SetRectEmpty(&m_rcCaption);
		SetRectEmpty(&m_rcPaint);

		memset(&m_ptLastMousePos,0,sizeof(m_ptLastMousePos));
		memset(&m_szMinWindow,0,sizeof(m_szMinWindow));
		memset(&m_szMaxWindow,0,sizeof(m_szMaxWindow));
		memset(&m_szInitWindowSize,0,sizeof(m_szInitWindowSize));

	}


    ZPaintManager::~ZPaintManager()
	{
		 // Delete the control-tree structures
		DoDelayedCleanup();
		delete m_pRoot;
	}


    void ZPaintManager::Init(ZWindow* pWnd)
	{
		m_window=pWnd;
		m_render=pWnd->GetRenderer();
		if (m_szInitWindowSize.cx>0 && m_szInitWindowSize.cy>0)
		{
			pWnd->SetSize(m_szInitWindowSize);
		}

		if (m_szMinWindow.cx>0 && m_szMinWindow.cy>0)
		{
			pWnd->SetMinimumSize(m_szMinWindow);
		}

		if (m_szMaxWindow.cx>0 && m_szMaxWindow.cy>0)
		{
			pWnd->SetMaximumSize(m_szMaxWindow);
		}
	}

	void ZPaintManager::NeedUpdate()
	{
		m_bUpdateNeeded = true;
	}

    ZRenerder* ZPaintManager::GetRenerder() const
	{
		return m_render;
	}

    ZWindow* ZPaintManager::GetWindow() const
	{
		return m_window;
	}

	void ZPaintManager::SetMousePos(POINT pt)
	{
		m_ptLastMousePos=pt;
	}
	
	POINT ZPaintManager::GetMousePos() const
	{
		return m_ptLastMousePos;
	}

	SIZE ZPaintManager::GetInitSize()
	{
		return m_szInitWindowSize;
	}

    void ZPaintManager::SetInitSize(int cx, int cy)
	{
		m_szInitWindowSize.cx=cx;
		m_szInitWindowSize.cy=cy;
	}

    RECT ZPaintManager::GetSizeBox()
	{
		return m_rcSizeBox;
	}
    void ZPaintManager::SetSizeBox(RECT rcSizeBox)
	{
		m_rcSizeBox=rcSizeBox;
	}

    RECT ZPaintManager::GetCaptionRect()
	{
		return m_rcCaption;
	}

    void ZPaintManager::SetCaptionRect(RECT rcCaption)
	{
		m_rcCaption=rcCaption;
	}

    SIZE ZPaintManager::GetMinInfo() const
	{
		return m_szMinWindow;
	}

    void ZPaintManager::SetMinInfo(int cx, int cy)
	{
		m_szMinWindow.cx=cx;
		m_szMinWindow.cy=cy;
	}
    
	SIZE ZPaintManager::GetMaxInfo() const
	{
		return m_szMaxWindow;
	}
    
	void ZPaintManager::SetMaxInfo(int cx, int cy)
	{
		m_szMaxWindow.cx=cx;
		m_szMaxWindow.cy=cy;	
	}

    uint32_t ZPaintManager::GetDefaultDisabledColor() const
	{
		return m_dwDefaultDisabledColor;
	}
    void ZPaintManager::SetDefaultDisabledColor(uint32_t dwColor)
	{
		m_dwDefaultDisabledColor=dwColor;
	}
    uint32_t ZPaintManager::GetDefaultFontColor() const
	{
		return m_dwDefaultFontColor;
	}
    void ZPaintManager::SetDefaultFontColor(uint32_t dwColor)
	{
		m_dwDefaultFontColor=dwColor;
	}
    uint32_t ZPaintManager::GetDefaultLinkFontColor() const
	{
		return m_dwDefaultLinkFontColor;
	}
    void ZPaintManager::SetDefaultLinkFontColor(uint32_t dwColor)
	{
		m_dwDefaultLinkFontColor=dwColor;
	}
    uint32_t ZPaintManager::GetDefaultLinkHoverFontColor() const
	{
		return m_dwDefaultLinkHoverFontColor;
	}
    void ZPaintManager::SetDefaultLinkHoverFontColor(uint32_t dwColor)
	{
		m_dwDefaultLinkHoverFontColor=dwColor;
	}
    uint32_t ZPaintManager::GetDefaultSelectedBkColor() const
	{
		return m_dwDefaultSelectedBkColor;
	}
    void ZPaintManager::SetDefaultSelectedBkColor(uint32_t dwColor)
	{
		m_dwDefaultSelectedBkColor=dwColor;
	}


    void ZPaintManager::AddDefaultAttributeList(const char* pStrControlName, const char* pStrControlAttrList)
	{
		if (pStrControlName && pStrControlAttrList)
		{
			char* oldAttr=(char*)m_DefaultAttrHash.Find(pStrControlName);
			FreeString(oldAttr);
			m_DefaultAttrHash.Set(pStrControlName,AllocString(pStrControlAttrList));
		}
	}


    const char* ZPaintManager::GetDefaultAttributeList(const char* pStrControlName) const
	{
		return (char*)m_DefaultAttrHash.Find(pStrControlName);
	}

    bool ZPaintManager::RemoveDefaultAttributeList(const char* pStrControlName)
	{
		char* str=(char*)m_DefaultAttrHash.Find(pStrControlName);
		if(str && m_DefaultAttrHash.Remove(pStrControlName))
		{
			FreeString(str);
			return true;
		}
		else
			return false;
	}

    const ZHashMap& ZPaintManager::GetDefaultAttribultes() const
	{
		return m_DefaultAttrHash;
	}

    void ZPaintManager::RemoveAllDefaultAttributeList()
	{
		for (ZHashMap::Iterator i=m_DefaultAttrHash.Begin();i!=m_DefaultAttrHash.End();++i)
		{
			FreeString((char*) i.Data());
		}
		m_DefaultAttrHash.RemoveAll();
	}


    bool ZPaintManager::AttachDialog(ZControl* pControl)
	{
		AddDelayedCleanup(m_pRoot);
		m_pRoot=pControl;
		SetFocus(NULL);
		m_pEventKey = NULL;
		m_pEventHover = NULL;
		m_pEventClick = NULL;
		m_bUpdateNeeded = true;
		m_bFirstLayout = true;
		m_bFocusNeeded = true;
		return InitControls(pControl);
	}

    bool ZPaintManager::InitControls(ZControl* pControl, ZControl* pParent)
	{
		assert(pControl);
		if( pControl == NULL ) return false;
		pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), true);
		pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
		return true;
	}

    void ZPaintManager::ReapObjects(ZControl* pControl)
	{
		if( pControl == m_pEventKey ) m_pEventKey = NULL;
		if( pControl == m_pEventHover ) m_pEventHover = NULL;
		if( pControl == m_pEventClick ) m_pEventClick = NULL;
		if( pControl == m_pFocus ) m_pFocus = NULL;

		const ZString& sName = pControl->GetName();
		if( !sName.IsEmpty() ) {
			if( pControl == FindControl(sName) ) m_mNameHash.Remove(sName);
		}
	}

    bool ZPaintManager::AddOptionGroup(const ZChar* pStrGroupName, ZControl* pControl)
	{
		void* lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			ZPtrArray* aOptionGroup = static_cast<ZPtrArray*>(lp);
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<ZControl*>(aOptionGroup->GetAt(i)) == pControl ) {
					return false;
				}
			}
			aOptionGroup->Add(pControl);
		}
		else {
			ZPtrArray* aOptionGroup = new ZPtrArray(6);
			aOptionGroup->Add(pControl);
			m_mOptionGroup.Set(pStrGroupName, aOptionGroup);
		}
		return true;
	}
    ZPtrArray* ZPaintManager::GetOptionGroup(const ZChar* pStrGroupName)
	{
		void* lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) return static_cast<ZPtrArray*>(lp);
		return NULL;
	}
    void ZPaintManager::RemoveOptionGroup(const ZChar* pStrGroupName, ZControl* pControl)
	{
		void* lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			ZPtrArray* aOptionGroup = static_cast<ZPtrArray*>(lp);
			if( aOptionGroup == NULL ) return;
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<ZControl*>(aOptionGroup->GetAt(i)) == pControl ) {
					aOptionGroup->Remove(i);
					break;
				}
			}
			if( aOptionGroup->IsEmpty() ) {
				delete aOptionGroup;
				m_mOptionGroup.Remove(pStrGroupName);
			}
		}

	}
    void ZPaintManager::RemoveAllOptionGroups()
	{
		ZPtrArray* aOptionGroup;
		for (ZHashMap::Iterator i=m_mOptionGroup.Begin();i!=m_mOptionGroup.End();++i)
		{
			aOptionGroup = static_cast<ZPtrArray*>(i.Data());
			delete aOptionGroup;
		}
		m_mOptionGroup.RemoveAll();	
	}

    ZControl* ZPaintManager::GetFocus() const
	{
		return m_pFocus;
	}

    void ZPaintManager::SetFocus(ZControl* pControl)
	{
		if( pControl == NULL ) return;
		// Set focus to new control
		if( pControl != NULL 
			&& pControl->GetManager() == this 
			&& pControl->IsVisible() 
			&& pControl->IsEnabled() ) 
		{
			m_pFocus = pControl;
			TEventUI event = { 0 };
			event.Type = UIEVENT_SETFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = base::Time();
			m_pFocus->Event(event);
		}
	}

    void ZPaintManager::SetFocusNeeded(ZControl* pControl)
	{
		if( pControl == NULL ) return;
		if( m_pFocus != NULL ) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = base::Time();
			m_pFocus->Event(event);
			m_pFocus = NULL;
		}
		FINDTABINFO info = { 0 };
		info.pFocus = pControl;
		info.bForward = false;
		m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		m_bFocusNeeded = true;
		if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
	}

    bool ZPaintManager::SetNextTabControl(bool bForward)
	{

		// Find next/previous tabbable control
		FINDTABINFO info1 = { 0 };
		info1.pFocus = m_pFocus;
		info1.bForward = bForward;
		ZControl* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		if( pControl == NULL ) {  
			if( bForward ) {
				// Wrap around
				FINDTABINFO info2 = { 0 };
				info2.pFocus = bForward ? NULL : info1.pLast;
				info2.bForward = bForward;
				pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
			}
			else {
				pControl = info1.pLast;
			}
		}
		if( pControl != NULL ) SetFocus(pControl);
		m_bFocusNeeded = false;
		return true;
	}


    void ZPaintManager::AddDelayedCleanup(ZControl* pControl)
	{
		if(pControl)
		{
			pControl->SetManager(this, NULL, false);
			m_aDelayedCleanup.Add(pControl);
			m_window->PostClearUpEvent();
		}
	}

	void ZPaintManager::DoDelayedCleanup()
	{
		for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
			delete static_cast<ZControl*>(m_aDelayedCleanup[i]);
	}

	void ZPaintManager::DestroyControl(ZControl* ctl)
	{
		if(!ctl)
			return;
		if (ctl->IsDelayDestroy())
		{
			AddDelayedCleanup(ctl);
		}
		else
			delete ctl;
	}

	ZControl* ZPaintManager::GetRoot() const
	{
		return m_pRoot;
	}

    ZControl* ZPaintManager::FindControl(POINT pt) const
	{
		if(m_pRoot)
			return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
		else
			return NULL;
	}
    ZControl* ZPaintManager::FindControl(const char* pstrName) const
	{
		return static_cast<ZControl*>(m_mNameHash.Find(pstrName));
	}
    ZControl* ZPaintManager::FindSubControlByPoint(ZControl* pParent, POINT pt) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		assert(pParent);
		return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}
    ZControl* ZPaintManager::FindSubControlByName(ZControl* pParent, const ZChar* pstrName) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		assert(pParent);
		return pParent->FindControl(__FindControlFromName, (void*)pstrName, UIFIND_ALL);
	}


	ZControl* ZPaintManager::__FindControlFromNameHash(ZControl* pThis, void* pData)
	{
		ZPaintManager* pManager = static_cast<ZPaintManager*>(pData);
		const ZString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		// Add this control to the hash list
		pManager->m_mNameHash.Set(sName, pThis);
		return NULL; // Attempt to add all controls
	}

	ZControl* ZPaintManager::__FindControlFromCount(ZControl* pThis, void* pData)
	{
		int* pnCount = static_cast<int*>(pData);
		(*pnCount)++;
		return NULL;  // Count all controls
	}

	ZControl* ZPaintManager::__FindControlFromPoint(ZControl* pThis, void* pData)
	{
		POINT* pPoint = static_cast<POINT*>(pData);
		if(PtInRect(&pThis->GetPos(),*pPoint))
			return pThis;
		else
			return NULL;
	}

	ZControl* ZPaintManager::__FindControlFromTab(ZControl* pThis, void* pData)
	{
		FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
		if( pInfo->pFocus == pThis ) {
			if( pInfo->bForward ) pInfo->bNextIsIt = true;
			return pInfo->bForward ? NULL : pInfo->pLast;
		}
		if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
		pInfo->pLast = pThis;
		if( pInfo->bNextIsIt ) return pThis;
		if( pInfo->pFocus == NULL ) return pThis;
		return NULL;  // Examine all controls
	}

	ZControl* ZPaintManager::__FindControlFromShortcut(ZControl* pThis, void* pData)
	{
		if( !pThis->IsVisible() ) return NULL; 
		FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
		if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
		if( strstr(pThis->GetClass(), "LabelUI") != NULL ) return NULL;   // Labels never get focus!
		return pFS->bPickNext ? pThis : NULL;
	}

	ZControl* ZPaintManager::__FindControlFromUpdate(ZControl* pThis, void* pData)
	{
		return pThis->IsUpdateNeeded() ? pThis : NULL;
	}

	ZControl* ZPaintManager::__FindControlFromName(ZControl* pThis, void* pData)
	{
		const char* pstrName = static_cast<const char*>(pData);
		const ZString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		return (stricmp(sName, pstrName) == 0) ? pThis : NULL;
	}

	void ZPaintManager::Invalidate(RECT rc)
	{
		JoinRect(&m_rcInvalidate,&rc);
	}

	RECT ZPaintManager::GetInvalidateRect()
	{
		return m_rcInvalidate;
	}

	void ZPaintManager::DoPaint(ZRenerder* hDC, const RECT& rcClient)
	{
	   if (!m_pRoot)
		   return;

	   if (!EqualRect(&rcClient,&m_rcPaint))
	   {
		   m_rcPaint=rcClient;
		   m_pRoot->SetPos(rcClient);
	   }

        // Set focus to first control?
        if( m_bFocusNeeded ) {
            SetNextTabControl();
        }

		m_pRoot->DoPaint(hDC,m_rcInvalidate);
	}
	
	int ZPaintManager::GetPostPaintCount() const
	{
		return m_aPostPaintControls.GetSize();
	}

    bool ZPaintManager::AddPostPaint(ZControl* pControl)
	{
		if(m_aPostPaintControls.Find(pControl) < 0)
			return false;
		m_aPostPaintControls.Add(pControl);
		return true;
	}

    bool ZPaintManager::RemovePostPaint(ZControl* pControl)
	{
		for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
			if( static_cast<ZControl*>(m_aPostPaintControls[i]) == pControl ) {
				return m_aPostPaintControls.Remove(i);
			}
		}
		return false;
	}

    bool ZPaintManager::SetPostPaintIndex(ZControl* pControl, int iIndex)
	{
		RemovePostPaint(pControl);
		return m_aPostPaintControls.InsertAt(iIndex, pControl);
	}

	//Window µÄÊôÐÔÖµ
	void ZPaintManager::SetAttribute(const char* pstrName, const char* pstrValue)
	{
		 if( strcmp(pstrName, "size") == 0 ) {
			char* pstr = NULL;
			int cx = strtol(pstrValue, &pstr, 10);   assert(pstr);    
			int cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
			SetInitSize(cx, cy);
		} 
		else if( strcmp(pstrName, "sizebox") == 0 ) {
			RECT rcSizeBox = { 0 };
			char* pstr = NULL;
			rcSizeBox.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
			rcSizeBox.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
			rcSizeBox.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
			rcSizeBox.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
			SetSizeBox(rcSizeBox);
		}
		else if( strcmp(pstrName, "caption") == 0 ) {
			RECT rcCaption = { 0 };
			char* pstr = NULL;
			rcCaption.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
			rcCaption.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
			rcCaption.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
			rcCaption.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
			SetCaptionRect(rcCaption);
		}
		else if( strcmp(pstrName, "mininfo") == 0 ) {
			char* pstr = NULL;
			int cx = strtol(pstrValue, &pstr, 10);   assert(pstr);    
			int cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
			SetMinInfo(cx, cy);
		}
		else if( strcmp(pstrName, "maxinfo") == 0 ) {
			char* pstr = NULL;
			int cx = strtol(pstrValue, &pstr, 10);   assert(pstr);    
			int cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
			SetMaxInfo(cx, cy);
		}
		else if( strcmp(pstrName, "disabledfontcolor") == 0 ) {
			if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
			char* pstr = NULL;
			color_t clrColor = strtol(pstrValue, &pstr, 16);
			SetDefaultDisabledColor(clrColor);
		} 
		else if( strcmp(pstrName, "defaultfontcolor") == 0 ) {
			if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
			char* pstr = NULL;
			color_t clrColor = strtol(pstrValue, &pstr, 16);
			SetDefaultFontColor(clrColor);
		}
		else if( strcmp(pstrName, "linkfontcolor") == 0 ) {
			if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
			char* pstr = NULL;
			color_t clrColor = strtol(pstrValue, &pstr, 16);
			SetDefaultLinkFontColor(clrColor);
		} 
		else if( strcmp(pstrName, "linkhoverfontcolor") == 0 ) {
			if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
			char* pstr = NULL;
			color_t clrColor = strtol(pstrValue, &pstr, 16);
			SetDefaultLinkHoverFontColor(clrColor);
		} 
		else if( strcmp(pstrName, "selectedcolor") == 0 ) {
			if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
			char* pstr = NULL;
			color_t clrColor = strtol(pstrValue, &pstr, 16);
			SetDefaultSelectedBkColor(clrColor);
		} 
	}

	void ZPaintManager::OnMouseEvent(ZUIEVENT_TYPE type,POINT pt)
	{
		if (!m_pRoot)
			return;
		switch(type)
		{
		case UIEVENT_BUTTONDOWN:
			{
				m_ptLastMousePos = pt;
				ZControl* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				m_pEventClick = pControl;
				pControl->SetFocus();
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONDOWN;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.dwTimestamp = base::Time();
				pControl->Event(event);
			}
			break;
		case UIEVENT_BUTTONUP:
			{
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONUP;
				event.pSender = m_pEventClick;
				event.ptMouse = pt;
				event.dwTimestamp = base::Time();
				m_pEventClick->Event(event);
				m_pEventClick = NULL;
			}
			break;
		case UIEVENT_DBLCLICK:
			{
				m_ptLastMousePos = pt;
				ZControl* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_DBLCLICK;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.dwTimestamp = base::Time();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;

		case UIEVENT_RBUTTONDOWN:
			{
				m_ptLastMousePos = pt;
				ZControl* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				pControl->SetFocus();
				TEventUI event = { 0 };
				event.Type = UIEVENT_RBUTTONDOWN;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.dwTimestamp = base::Time();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;

		case UIEVENT_RBUTTONUP:
			{
				m_ptLastMousePos = pt;
				ZControl* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_RBUTTONUP;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.dwTimestamp = base::Time();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case UIEVENT_MOUSEMOVE:
			{
				m_ptLastMousePos = pt;
				ZControl* pNewHover = FindControl(pt);
				if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.dwTimestamp = base::Time();
				if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventHover;
					m_pEventHover->Event(event);
					m_pEventHover = NULL;
				}
				if( pNewHover != m_pEventHover && pNewHover != NULL ) {
					event.Type = UIEVENT_MOUSEENTER;
					event.pSender = pNewHover;
					pNewHover->Event(event);
					m_pEventHover = pNewHover;
				}
				if( m_pEventClick != NULL ) {
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}
				else if( pNewHover != NULL ) {
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = pNewHover;
					pNewHover->Event(event);
				}

			}
			break;
		case UIEVENT_SCROLLWHEEL:
			{
				ZControl* pControl = FindControl(m_ptLastMousePos);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_SCROLLWHEEL;
				event.pSender = pControl;
				event.wParam = pt.y>0?SBT_LINEUP:SBT_LINEDOWN;
				event.lParam = abs(pt.y);
				event.dwTimestamp = base::Time();
				pControl->Event(event);
			}
			break;
		}
	}

}//namespace
