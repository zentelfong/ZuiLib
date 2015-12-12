#include "ZTabLayout.h"
#include "ZPaintManager.h"
#include "ZControlFactory.h"


namespace ZuiLib
{


	ZTabLayout::ZTabLayout() : m_iCurSel(-1)
	{
	}

	ZTabLayout::~ZTabLayout(void)
	{
	}

	const char* ZTabLayout::GetClass() const
	{
		return "TabLayout";
	}

	void* ZTabLayout::GetInterface(const char* pstrName)
	{
		if( strcmp(pstrName, "TabLayout") == 0 ) 
			return static_cast<ZTabLayout*>(this);
		else
			return ZContainer::GetInterface(pstrName);
	}

	bool ZTabLayout::Add(ZControl* pControl)
	{
		bool ret = ZContainer::Add(pControl);
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = GetItemIndex(pControl);
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool ZTabLayout::AddAt(ZControl* pControl, int iIndex)
	{
		bool ret = ZContainer::AddAt(pControl, iIndex);
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = GetItemIndex(pControl);
		}
		else if( m_iCurSel != -1 && iIndex <= m_iCurSel )
		{
			m_iCurSel += 1;
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool ZTabLayout::Remove(ZControl* pControl)
	{
		if( pControl == NULL) return false;

		int index = GetItemIndex(pControl);
		bool ret = ZContainer::Remove(pControl);
		if( !ret ) return false;

		if( m_iCurSel == index)
		{
			if( GetCount() > 0 )
			{
				m_iCurSel=0;
				GetItemAt(m_iCurSel)->SetVisible(true);
			}
			else
				m_iCurSel=-1;
			NeedParentUpdate();
		}
		else if( m_iCurSel > index )
		{
			m_iCurSel -= 1;
		}

		return ret;
	}

	void ZTabLayout::RemoveAll()
	{
		m_iCurSel = -1;
		ZContainer::RemoveAll();
		NeedParentUpdate();
	}

	int ZTabLayout::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool ZTabLayout::SelectItem(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			if( it == iIndex ) {
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();
				SetPos(m_rcItem);
			}
			else GetItemAt(it)->SetVisible(false);
		}
		NeedParentUpdate();

		if( m_pManager != NULL ) {
			m_pManager->SetNextTabControl();
		}
		return true;
	}

	bool ZTabLayout::SelectItem( ZControl* pControl )
	{
		int iIndex=GetItemIndex(pControl);
		if (iIndex==-1)
			return false;
		else
			return SelectItem(iIndex);
	}

	void ZTabLayout::SetAttribute(const char* pstrName, const char* pstrValue)
	{
		if( strcmp(pstrName, "selectedid") == 0 ) 
			SelectItem(atoi(pstrValue));
		return ZContainer::SetAttribute(pstrName, pstrValue);
	}

	void ZTabLayout::SetPos(RECT rc)
	{
		ZControl::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			ZControl* pControl = static_cast<ZControl*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
				continue;
			}

			if( it != m_iCurSel ) continue;

			RECT rcPadding = pControl->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == 0 ) {
				sz.cx = ZMAX(0, szAvailable.cx);
			}
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			if(sz.cy == 0) {
				sz.cy = ZMAX(0, szAvailable.cy);
			}
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
			pControl->SetPos(rcCtrl);
		}
	}






	IMPL_ZCONTROL_FACTORY("TabLayout",ZTabLayout);


}
