#ifndef __ZUISCROLLBAR_H__
#define __ZUISCROLLBAR_H__

#include "ZControl.h"

namespace ZuiLib
{
class ZContainer;

class ZScrollBar:public ZControl
{
public:
	ZScrollBar(void);
	~ZScrollBar(void);


    const char* GetClass() const;
    void* GetInterface(const char* pstrName);

    ZContainer* GetOwner() const;
    void SetOwner(ZContainer* pOwner);

    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);
    void SetFocus();

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetScrollRange() const;
    void SetScrollRange(int nRange);
    int GetScrollPos() const;
    void SetScrollPos(int nPos);
    int GetLineSize() const;
    void SetLineSize(int nSize);

    bool GetShowButton1();
    void SetShowButton1(bool bShow);
    const char* GetButton1NormalImage();
    void SetButton1NormalImage(const char* pStrImage);
    const char* GetButton1HotImage();
    void SetButton1HotImage(const char* pStrImage);
    const char* GetButton1PushedImage();
    void SetButton1PushedImage(const char* pStrImage);
    const char* GetButton1DisabledImage();
    void SetButton1DisabledImage(const char* pStrImage);

    bool GetShowButton2();
    void SetShowButton2(bool bShow);
    const char* GetButton2NormalImage();
    void SetButton2NormalImage(const char* pStrImage);
    const char* GetButton2HotImage();
    void SetButton2HotImage(const char* pStrImage);
    const char* GetButton2PushedImage();
    void SetButton2PushedImage(const char* pStrImage);
    const char* GetButton2DisabledImage();
    void SetButton2DisabledImage(const char* pStrImage);

    const char* GetThumbNormalImage();
    void SetThumbNormalImage(const char* pStrImage);
    const char* GetThumbHotImage();
    void SetThumbHotImage(const char* pStrImage);
    const char* GetThumbPushedImage();
    void SetThumbPushedImage(const char* pStrImage);
    const char* GetThumbDisabledImage();
    void SetThumbDisabledImage(const char* pStrImage);

    const char* GetRailNormalImage();
    void SetRailNormalImage(const char* pStrImage);
    const char* GetRailHotImage();
    void SetRailHotImage(const char* pStrImage);
    const char* GetRailPushedImage();
    void SetRailPushedImage(const char* pStrImage);
    const char* GetRailDisabledImage();
    void SetRailDisabledImage(const char* pStrImage);

    const char* GetBkNormalImage();
    void SetBkNormalImage(const char* pStrImage);
    const char* GetBkHotImage();
    void SetBkHotImage(const char* pStrImage);
    const char* GetBkPushedImage();
    void SetBkPushedImage(const char* pStrImage);
    const char* GetBkDisabledImage();
    void SetBkDisabledImage(const char* pStrImage);

    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(const char* pstrName, const char* pstrValue);

    void DoPaint(ZRenerder* hDC, const RECT& rcPaint);

    void PaintBk(ZRenerder* hDC);
    void PaintButton1(ZRenerder* hDC);
    void PaintButton2(ZRenerder* hDC);
    void PaintThumb(ZRenerder* hDC);
    void PaintRail(ZRenerder* hDC);

protected:

    enum { 
        DEFAULT_SCROLLBAR_SIZE = 16,
        DEFAULT_TIMERID = 10,
    };

    bool m_bHorizontal;
    int m_nRange;
    int m_nScrollPos;
    int m_nLineSize;
    ZContainer* m_pOwner;
    POINT ptLastMouse;
    int m_nLastScrollPos;
    int m_nLastScrollOffset;
    int m_nScrollRepeatDelay;

    ZString m_sBkNormalImage;
    ZString m_sBkHotImage;
    ZString m_sBkPushedImage;
    ZString m_sBkDisabledImage;

    bool m_bShowButton1;
    RECT m_rcButton1;
    uint32_t m_uButton1State;
    ZString m_sButton1NormalImage;
    ZString m_sButton1HotImage;
    ZString m_sButton1PushedImage;
    ZString m_sButton1DisabledImage;

    bool m_bShowButton2;
    RECT m_rcButton2;
    uint32_t m_uButton2State;
    ZString m_sButton2NormalImage;
    ZString m_sButton2HotImage;
    ZString m_sButton2PushedImage;
    ZString m_sButton2DisabledImage;

    RECT m_rcThumb;
    uint32_t m_uThumbState;
    ZString m_sThumbNormalImage;
    ZString m_sThumbHotImage;
    ZString m_sThumbPushedImage;
    ZString m_sThumbDisabledImage;

    ZString m_sRailNormalImage;
    ZString m_sRailHotImage;
    ZString m_sRailPushedImage;
    ZString m_sRailDisabledImage;

    ZString m_sImageModify;

};












}




#endif