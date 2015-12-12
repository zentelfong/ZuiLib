#include "ZOption.h"
#include "ZPaintManager.h"
#include "ZControlFactory.h"

namespace ZuiLib
{


ZOption::ZOption() : m_bSelected(false), m_dwSelectedTextColor(0)
{
}

ZOption::~ZOption()
{
    if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
}

const char* ZOption::GetClass() const
{
    return "Option";
}

void* ZOption::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "Option") == 0 ) return static_cast<ZOption*>(this);
    return ZButton::GetInterface(pstrName);
}

void ZOption::SetManager(ZPaintManager* pManager, ZControl* pParent, bool bInit)
{
    ZControl::SetManager(pManager, pParent, bInit);
    if( bInit && !m_sGroupName.IsEmpty() ) {
        if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
    }
}

const char* ZOption::GetGroup() const
{
    return m_sGroupName;
}

void ZOption::SetGroup(const char* pStrGroupName)
{
    if( pStrGroupName == NULL ) {
        if( m_sGroupName.IsEmpty() ) return;
        m_sGroupName.Empty();
    }
    else {
        if( m_sGroupName == pStrGroupName ) return;
        if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
        m_sGroupName = pStrGroupName;
    }

    if( !m_sGroupName.IsEmpty() ) {
        if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
    }
    else {
        if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
    }

    Selected(m_bSelected);
}

bool ZOption::IsSelected() const
{
    return m_bSelected;
}

void ZOption::Selected(bool bSelected)
{
    if( m_bSelected == bSelected ) return;
    m_bSelected = bSelected;
    if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
    else m_uButtonState &= ~UISTATE_SELECTED;

    if( m_pManager != NULL ) {
        if( !m_sGroupName.IsEmpty() ) {
            if( m_bSelected ) {
                ZPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
                for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
                    ZOption* pControl = static_cast<ZOption*>(aOptionGroup->GetAt(i));
                    if( pControl != this ) {
                        pControl->Selected(false);
                    }
                }
				DoLuaEvent("selectChanged",m_bSelected);
            }
        }
        else {
			DoLuaEvent("selectChanged",m_bSelected);
        }
    }

    Invalidate();
}

bool ZOption::Activate()
{
    if( !ZButton::Activate() ) return false;
    if( !m_sGroupName.IsEmpty() ) Selected(true);
    else Selected(!m_bSelected);

    return true;
}

void ZOption::SetEnabled(bool bEnable)
{
    ZControl::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
        else m_uButtonState = 0;
    }
}

const char* ZOption::GetSelectedImage()
{
    return m_sSelectedImage;
}

void ZOption::SetSelectedImage(const char* pStrImage)
{
    m_sSelectedImage = pStrImage;
    Invalidate();
}

void ZOption::SetSelectedTextColor(color_t dwTextColor)
{
	m_dwSelectedTextColor = dwTextColor;
}

color_t ZOption::GetSelectedTextColor()
{
	if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
	return m_dwSelectedTextColor;
}

const char* ZOption::GetForeImage()
{
	return m_sForeImage;
}

void ZOption::SetForeImage(const char* pStrImage)
{
	m_sForeImage = pStrImage;
	Invalidate();
}

SIZE ZOption::EstimateSize(SIZE szAvailable)
{
	return ZButton::EstimateSize(szAvailable);
}

void ZOption::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "group") == 0 ) SetGroup(pstrValue);
    else if( strcmp(pstrName, "selected") == 0 ) Selected(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "selectedimage") == 0 ) SetSelectedImage(pstrValue);
	else if( strcmp(pstrName, "foreimage") == 0 ) SetForeImage(pstrValue);
	else if( strcmp(pstrName, "selectedtextcolor") == 0 ) {
		if( *pstrValue == '#') pstrValue = CharNext(pstrValue);
		char* pstr = NULL;
		color_t clrColor = strtoul(pstrValue, &pstr, 16);
		SetSelectedTextColor(clrColor);
	}
    else ZButton::SetAttribute(pstrName, pstrValue);
}

void ZOption::PaintStatusImage(ZRenerder* hDC)
{
    m_uButtonState &= ~UISTATE_PUSHED;

    if( (m_uButtonState & UISTATE_SELECTED) != 0 ) {
        if( !m_sSelectedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (const char*)m_sSelectedImage) ) m_sSelectedImage.Empty();
            else goto Label_ForeImage;
        }
    }

    ZButton::PaintStatusImage(hDC);

Label_ForeImage:
	if( !m_sForeImage.IsEmpty() ) {
		if( !DrawImage(hDC, (const char*)m_sForeImage) ) m_sForeImage.Empty();
	}
}

void ZOption::PaintText(ZRenerder* hDC)
{
	if( (m_uButtonState & UISTATE_SELECTED) != 0 )
	{
		color_t oldTextColor = m_dwTextColor;
		if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		hDC->DrawText(m_sText,IsEnabled()?m_dwTextColor:m_dwDisabledTextColor,rc,GetTextInfo(m_sFont),m_uTextStyle);
		m_dwTextColor = oldTextColor;
	}
	else
		ZButton::PaintText(hDC);
}







IMPL_ZCONTROL_FACTORY("Option",ZOption);
}

