#ifndef __ZUICONTROL_H__
#define __ZUICONTROL_H__

#include "ZString.h"
#include "ZUtils.h"
#include "SDL.h"


namespace ZuiLib
{

class ZPaintManager;
class ZRenerder;
class ZControl;
struct ZTextInfo;

struct TEventUI
{
    int Type;
    ZControl* pSender;
    uint32_t dwTimestamp;
    POINT ptMouse;
    char chKey;
    uint16_t wKeyState;
	uint32_t wParam;
	uint32_t lParam;
};

typedef ZControl* (* FINDCONTROLPROC)(ZControl*, void*);

class ZControl
{
public:
    ZControl();
    virtual ~ZControl();

	//调用lua处理事件
	bool DoLuaEvent(const char* evName);
	bool DoLuaEvent(const char* evName,int param);
	bool DoLuaEvent(const char* evName,const char* param);
	bool DoLuaEvent(const char* evName,bool param);

public:
    virtual ZString GetName() const;
    virtual void SetName(const char* pstrName);
    virtual const char* GetClass() const;
    virtual void* GetInterface(const char* pstrName);
    virtual uint32_t GetControlFlags() const;

    virtual bool Activate();
    ZPaintManager* GetManager() const;
    virtual void SetManager(ZPaintManager* pManager, ZControl* pParent, bool bInit = true);
	ZRenerder* GetRenerder() const;

	ZTextInfo* GetTextInfo(const char* name);

    virtual ZControl* GetParent() const;

    // 文本相关
    virtual ZString GetText() const;
    virtual void SetText(const char* pstrText);

    // 图形相关
    uint32_t GetBkColor() const;
    void SetBkColor(uint32_t dwBackColor);

    const char* GetBkImage();
    void SetBkImage(const char* pStrImage);
	uint32_t GetFocusBorderColor() const;
	void SetFocusBorderColor(uint32_t dwBorderColor);

    bool DrawImage(SDL_Renderer* hDC, const char* pStrImage, const char* pStrModify = NULL);

	//边框相关
	int GetBorderSize() const;
	void SetBorderSize(int nZSize);
	uint32_t GetBorderColor() const;
	void SetBorderColor(uint32_t dwBorderColor);


	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);
	bool IsDelayDestroy(){return m_bDelayDestroy;}

    // 位置相关
    virtual const RECT& GetPos() const;
    virtual void SetPos(RECT rc);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);


    // 鼠标提示
    virtual ZString GetToolTip() const;
    virtual void SetToolTip(const char* pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	 GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

    // 快捷键
    virtual char GetShortcut() const;
    virtual void SetShortcut(char ch);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户属性
    virtual const ZString& GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(const char* pstrText); // 辅助函数，供用户使用
    virtual intptr_t GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(intptr_t pTag); // 辅助函数，供用户使用

    // 一些重要的属性
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);
	
    virtual ZControl* FindControl(FINDCONTROLPROC Proc, void* pData, uint32_t uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);

    virtual void SetAttribute(const char* pstrName, const char* pstrValue);
    ZControl* ApplyAttributeList(const char* pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

    virtual void DoPaint(ZRenerder* hDC, const RECT& rcPaint);

    virtual void PaintBkColor(ZRenerder* hDC);
    virtual void PaintBkImage(ZRenerder* hDC);
    virtual void PaintStatusImage(ZRenerder* hDC);
    virtual void PaintText(ZRenerder* hDC);
    virtual void PaintBorder(ZRenerder* hDC);

	bool DrawImage(ZRenerder* hDC, const char* pStrImage,  const char* pStrModify=NULL);
protected:
    ZPaintManager* m_pManager;


    ZControl* m_pParent;
    ZString m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
    RECT m_rcItem;
    RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    bool m_bFocused;
    bool m_bFloat;
	bool m_bDelayDestroy;

    ZString m_sText;
    ZString m_sToolTip;
    char m_chShortcut;
    ZString m_sUserData;
    intptr_t m_pTag;

    uint32_t m_dwBackColor;

    ZString m_sBkImage;
	ZString m_sForeImage;
    uint32_t m_dwBorderColor;
	uint32_t m_dwFocusBorderColor;
    bool m_bColorHSL;
    int m_nBorderSize;
	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
};




















}//namespace




#endif //__ZUICONTROL_H__