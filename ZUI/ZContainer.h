#ifndef __ZUICONTAINER_H__
#define __ZUICONTAINER_H__

#include "ZControl.h"

namespace ZuiLib
{

class IContainerUI
{
public:
    virtual ZControl* GetItemAt(int iIndex) const = 0;
    virtual int GetItemIndex(ZControl* pControl) const  = 0;
    virtual bool SetItemIndex(ZControl* pControl, int iIndex)  = 0;
    virtual int GetCount() const = 0;
    virtual bool Add(ZControl* pControl) = 0;
    virtual bool AddAt(ZControl* pControl, int iIndex)  = 0;
    virtual bool Remove(ZControl* pControl) = 0;
    virtual bool RemoveAt(int iIndex)  = 0;
    virtual void RemoveAll() = 0;
};

class ZScrollBar;

class ZContainer:public ZControl, public IContainerUI
{
public:
	ZContainer(void);
	~ZContainer(void);

    const char* GetClass() const;
    void* GetInterface(const char* pstrName);

    ZControl* GetItemAt(int iIndex) const;
    int GetItemIndex(ZControl* pControl) const;
    bool SetItemIndex(ZControl* pControl, int iIndex);
    int GetCount() const;
    bool Add(ZControl* pControl);
    bool AddAt(ZControl* pControl, int iIndex);
    bool Remove(ZControl* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    void DoEvent(TEventUI& event);
    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    void SetMouseEnabled(bool bEnable = true);

    virtual RECT GetInset() const;
    virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
    virtual int GetChildPadding() const;
    virtual void SetChildPadding(int iPadding);
    virtual bool IsAutoDestroy() const;
    virtual void SetAutoDestroy(bool bAuto);
    virtual bool IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(bool bDelayed);
    virtual bool IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(bool bEnable = true);

    virtual int FindSelectable(int iIndex, bool bForward = true) const;

    void SetPos(RECT rc);
    void DoPaint(ZRenerder* hDC, const RECT& rcPaint);

    void SetAttribute(const char* pstrName, const char* pstrValue);

    void SetManager(ZPaintManager* pManager, ZControl* pParent, bool bInit = true);
    ZControl* FindControl(FINDCONTROLPROC Proc, void* pData, uint32_t uFlags);

    virtual SIZE GetScrollPos() const;
    virtual SIZE GetScrollRange() const;
    virtual void SetScrollPos(SIZE szPos);
    virtual void LineUp();
    virtual void LineDown();
    virtual void PageUp();
    virtual void PageDown();
    virtual void HomeUp();
    virtual void EndDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void PageLeft();
    virtual void PageRight();
    virtual void HomeLeft();
    virtual void EndRight();
    virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual ZScrollBar* GetVerticalScrollBar() const;
    virtual ZScrollBar* GetHorizontalScrollBar() const;

protected:
    virtual void SetFloatPos(int iIndex);
    virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

protected:
    ZPtrArray m_items;
    RECT m_rcInset;
    int m_iChildPadding;
    bool m_bAutoDestroy;
    bool m_bDelayedDestroy;
    bool m_bMouseChildEnabled;
    bool m_bScrollProcess; // 防止SetPos循环调用

    ZScrollBar* m_pVerticalScrollBar;
    ZScrollBar* m_pHorizontalScrollBar;
};

}

#endif