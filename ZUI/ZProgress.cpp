#include "ZProgress.h"
#include "ZRenerder.h"
#include "ZControlFactory.h"

namespace ZuiLib
{


ZProgress::ZProgress() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    SetFixedHeight(12);
}

ZProgress::~ZProgress(void)
{
}

const char* ZProgress::GetClass() const
{
    return "Progress";
}

void* ZProgress::GetInterface(const char* pstrName)
{
    if( strcmp(pstrName, "Progress") == 0 ) return static_cast<ZProgress*>(this);
    return ZLabel::GetInterface(pstrName);
}

bool ZProgress::IsHorizontal()
{
    return m_bHorizontal;
}

void ZProgress::SetHorizontal(bool bHorizontal)
{
    if( m_bHorizontal == bHorizontal ) return;

    m_bHorizontal = bHorizontal;
    Invalidate();
}

int ZProgress::GetMinValue() const
{
    return m_nMin;
}

void ZProgress::SetMinValue(int nMin)
{
    m_nMin = nMin;
    Invalidate();
}

int ZProgress::GetMaxValue() const
{
    return m_nMax;
}

void ZProgress::SetMaxValue(int nMax)
{
    m_nMax = nMax;
    Invalidate();
}

int ZProgress::GetValue() const
{
    return m_nValue;
}

void ZProgress::SetValue(int nValue)
{
    m_nValue = nValue;
    Invalidate();
}

const char* ZProgress::GetForeImage() const
{
    return m_sForeImage;
}

void ZProgress::SetForeImage(const char* pStrImage)
{
    if( m_sForeImage == pStrImage ) return;
    m_sForeImage = pStrImage;
}

void ZProgress::SetAttribute(const char* pstrName, const char* pstrValue)
{
    if( strcmp(pstrName, "foreimage") == 0 ) SetForeImage(pstrValue);
    else if( strcmp(pstrName, "hor") == 0 ) SetHorizontal(strcmp(pstrValue, "true") == 0);
    else if( strcmp(pstrName, "min") == 0 ) SetMinValue(atoi(pstrValue));
    else if( strcmp(pstrName, "max") == 0 ) SetMaxValue(atoi(pstrValue));
    else if( strcmp(pstrName, "value") == 0 ) SetValue(atoi(pstrValue));
    else ZLabel::SetAttribute(pstrName, pstrValue);
}

void ZProgress::PaintStatusImage(ZRenerder* hDC)
{
    if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
    if( m_nValue > m_nMax ) m_nValue = m_nMax;
    if( m_nValue < m_nMin ) m_nValue = m_nMin;

    RECT rc = {0};
    if( m_bHorizontal ) {
        rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }
    else {
        rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
        rc.right = m_rcItem.right - m_rcItem.left;
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }

    if( !m_sForeImage.IsEmpty() ) {
        m_sForeImageModify.Empty();
        m_sForeImageModify.Format("dest='%d,%d,%d,%d'", rc.left, rc.top, rc.right, rc.bottom);

        if( !DrawImage(hDC, (const char*)m_sForeImage, (const char*)m_sForeImageModify) ) m_sForeImage.Empty();
        else return;
    }
}






IMPL_ZCONTROL_FACTORY("Progress",ZProgress);



}


