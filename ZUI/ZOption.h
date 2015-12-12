#ifndef __ZUIOPTION_H__
#define __ZUIOPTION_H__

#include "ZButton.h"

namespace ZuiLib
{

class ZOption : public ZButton
{
public:
    ZOption();
    ~ZOption();

    const char* GetClass() const;
    void* GetInterface(const char* pstrName);

    void SetManager(ZPaintManager* pManager, ZControl* pParent, bool bInit = true);

    bool Activate();
    void SetEnabled(bool bEnable = true);

    const char* GetSelectedImage();
    void SetSelectedImage(const char* pStrImage);
	
	void SetSelectedTextColor(color_t dwTextColor);
	color_t GetSelectedTextColor();
		
	const char* GetForeImage();
	void SetForeImage(const char* pStrImage);

    const char* GetGroup() const;
    void SetGroup(const char* pStrGroupName = NULL);
    bool IsSelected() const;
    void Selected(bool bSelected);

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(const char* pstrName, const char* pstrValue);

    void PaintStatusImage(ZRenerder* hDC);
	void PaintText(ZRenerder* hDC);

protected:
    bool m_bSelected;
	color_t m_dwSelectedTextColor;

	ZString m_sGroupName;
    ZString m_sSelectedImage;
	ZString m_sForeImage;
};




}



#endif