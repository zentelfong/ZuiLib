#ifndef __ZUIWINDOW_H__
#define __ZUIWINDOW_H__

#include "SDL.h"
#include "ZUtils.h"
#include "ZRenerder.h"
#include "ZPaintManager.h"
#include "ZFont.h"

namespace ZuiLib
{

enum SDL_EVENT_TYPE{
	SET_CLEAR_UP=0
};


/*
	sdl_windowswindow.c

    SDL_WindowData *data = (SDL_WindowData *)window->driverdata;
    HWND hwnd = data->hwnd;

*/

class ZWindow
{
public:
	ZWindow(void);
	~ZWindow(void);

	enum {
		WINDOW_POS_CENTER=SDL_WINDOWPOS_CENTERED
	};

	bool CreateWindow(const char *title,int x, int y, int w, int h, uint32_t flags=SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	bool SetWindowTitle(const char *title);
	const char * GetWindowTitle();


	ZRenerder* GetRenderer(){return m_render;}
	ZPaintManager* GetManager(){return &m_mgr;}

	void SetWindowIcon(SDL_Surface *ico);


	void SetPosition(POINT pt);
	POINT GetPosition();

	void SetSize(SIZE sz);
	SIZE GetSize();

	void SetMinimumSize(SIZE sz);
	SIZE GetMinimumSize();

	void SetMaximumSize(SIZE sz);
	SIZE GetMaximumSize();

	void ShowWindow(bool bShow);
	void RaiseWindow();

	void SetMaximize();
	void SetMinimize();
	void SetRestore();
	
	void SetGrab(bool grab);
	bool GetGrab();

	bool SetBrightness(float bright);
	float GetBrightness();

	void Update(const ZRect * rects,int numrects);

	void Destroy();

	void PostClearUpEvent();
	virtual void Render(const RECT* rc);

protected:
	void Init();
	
	virtual void MouseButtonEvent(const SDL_MouseButtonEvent& event);
	virtual void MouseWheelEvent(const SDL_MouseWheelEvent& event);
	virtual void MouseMotionEvent(const SDL_MouseMotionEvent& event);
	virtual void WindowEvent(const SDL_WindowEvent& event);
	virtual void KeyboardEvent(const SDL_KeyboardEvent& event){}
	virtual void TextInputEvent(const SDL_TextInputEvent& event){}
	virtual void TextEditingEvent(const SDL_TextEditingEvent& event){}
	virtual void UserEvent(const SDL_UserEvent& event);

private:
	DISALLOW_COPY_AND_ASSIGN(ZWindow);

	friend class ZApplication;

	SDL_Window * m_win;
	ZRenerder * m_render;
	ZPaintManager m_mgr;

	uint32_t m_lastColect;
	uint32_t m_startTime;//开始时间
	uint32_t m_tick;//更新时间
	uint32_t m_tickCount;//帧数
};

}//namespace
#endif