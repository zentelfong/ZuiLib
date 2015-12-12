#ifndef __ZUIPAINTMANAGER_H__
#define __ZUIPAINTMANAGER_H__
#include "ZUtils.h"
#include "ZString.h"
#include "SDL.h"
#include "ZRenerder.h"
#include "base/time.h"

namespace ZuiLib
{

typedef enum ZUIEVENT_TYPE
{
	UIEVENT_NONE=0,
    UIEVENT__FIRST = 1,
    UIEVENT__KEYBEGIN,
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,
    UIEVENT__MOUSEBEGIN,
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
	UIEVENT_RBUTTONUP,
    UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT__MOUSEEND,
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
    UIEVENT_WINDOWZSize,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_NOTIFY,
    UIEVENT_COMMAND,
    UIEVENT__LAST,
};

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004


// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040

enum SCROLLWHEEL_TYPE{
	SBT_LINEDOWN,
	SBT_LINEUP
};


class ZControl;
class ZRenerder;
class ZWindow;

class ZPaintManager
{
public:
    ZPaintManager();
    ~ZPaintManager();

public:
    void Init(ZWindow* pWnd);

	void NeedUpdate();

    ZRenerder* GetRenerder() const;
    ZWindow* GetWindow() const;

	void SetMousePos(POINT pt);
    POINT GetMousePos() const;

    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);

    RECT GetSizeBox();
    void SetSizeBox(RECT rcSizeBox);

    RECT GetCaptionRect();
    void SetCaptionRect(RECT rcCaption);

    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);


    uint32_t GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(uint32_t dwColor);
    uint32_t GetDefaultFontColor() const;
    void SetDefaultFontColor(uint32_t dwColor);
    uint32_t GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(uint32_t dwColor);
    uint32_t GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(uint32_t dwColor);
    uint32_t GetDefaultSelectedBkColor() const;
    void SetDefaultSelectedBkColor(uint32_t dwColor);

	//属性
    void AddDefaultAttributeList(const char* pStrControlName, const char* pStrControlAttrList);
    const char* GetDefaultAttributeList(const char* pStrControlName) const;
    bool RemoveDefaultAttributeList(const char* pStrControlName);
    const ZHashMap& GetDefaultAttribultes() const;
    void RemoveAllDefaultAttributeList();

    bool AttachDialog(ZControl* pControl);
    bool InitControls(ZControl* pControl, ZControl* pParent = NULL);
    void ReapObjects(ZControl* pControl);

    bool AddOptionGroup(const ZChar* pStrGroupName, ZControl* pControl);
    ZPtrArray* GetOptionGroup(const ZChar* pStrGroupName);
    void RemoveOptionGroup(const ZChar* pStrGroupName, ZControl* pControl);
    void RemoveAllOptionGroups();

    ZControl* GetFocus() const;
    void SetFocus(ZControl* pControl);
    void SetFocusNeeded(ZControl* pControl);

    bool SetNextTabControl(bool bForward = true);


    void AddDelayedCleanup(ZControl* pControl);
	void DoDelayedCleanup();

	void DoPaint(ZRenerder* hDC, const RECT& rcPaint);


    ZControl* GetRoot() const;
    ZControl* FindControl(POINT pt) const;
    ZControl* FindControl(const char* pstrName) const;
    ZControl* FindSubControlByPoint(ZControl* pParent, POINT pt) const;
    ZControl* FindSubControlByName(ZControl* pParent, const ZChar* pstrName) const;


	void OnMouseEvent(ZUIEVENT_TYPE type,POINT pt);
	void Invalidate(RECT rc);
	RECT GetInvalidateRect();

	int GetPostPaintCount() const;
    bool AddPostPaint(ZControl* pControl);
    bool RemovePostPaint(ZControl* pControl);
    bool SetPostPaintIndex(ZControl* pControl, int iIndex);

	ZRenerder* GetRenerder(){return m_render;}

	void SetAttribute(const char* pstrName, const char* pstrValue);
private:
    static ZControl* __FindControlFromNameHash(ZControl* pThis, void* pData);
    static ZControl* __FindControlFromCount(ZControl* pThis, void* pData);
    static ZControl* __FindControlFromPoint(ZControl* pThis, void* pData);
    static ZControl* __FindControlFromTab(ZControl* pThis, void* pData);
    static ZControl* __FindControlFromShortcut(ZControl* pThis, void* pData);
    static ZControl* __FindControlFromUpdate(ZControl* pThis, void* pData);
    static ZControl* __FindControlFromName(ZControl* pThis, void* pData);


	void DestroyControl(ZControl* ctl);
private:
	ZWindow*   m_window;
    ZRenerder* m_render;

	RECT m_rcPaint;
	RECT m_rcInvalidate;//无效区域，需要重绘
    //
    ZControl* m_pRoot;
    ZControl* m_pFocus;
    ZControl* m_pEventHover;
    ZControl* m_pEventClick;
    ZControl* m_pEventKey;
    //
    POINT m_ptLastMousePos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;

    RECT m_rcSizeBox;
    RECT m_rcCaption;

    uint32_t m_uTimerID;
    bool m_bFirstLayout;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;

    //
    ZPtrArray m_aDelayedCleanup;
    ZHashMap m_mNameHash;
    ZHashMap m_mOptionGroup;
	ZHashMap m_DefaultAttrHash;

	ZPtrArray m_aPostPaintControls;

    //
    uint32_t m_dwDefaultDisabledColor;
    uint32_t m_dwDefaultFontColor;
    uint32_t m_dwDefaultLinkFontColor;
    uint32_t m_dwDefaultLinkHoverFontColor;
    uint32_t m_dwDefaultSelectedBkColor;

};



}//namespace




#endif