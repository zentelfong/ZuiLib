#include "ZWindow.h"
#include "ZApplication.h"
#include "ZControl.h"

#undef DrawText

namespace ZuiLib
{
extern ZWindow* ZWindowFromWindow(SDL_Window* wnd);


ZWindow::ZWindow(void)
	:m_win(NULL),m_render(NULL),m_lastColect(0),m_startTime(base::Time()),m_tick(0),m_tickCount(0)
{
	ZApplication::Instance()->AddWindow(this);
}


ZWindow::~ZWindow(void)
{
	ZApplication::Instance()->RemoveWindow(this);
	delete m_render;
	SDL_DestroyWindow(m_win);
}


#ifdef WIN32

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#endif


HRESULT WindowHitTestHandle(SDL_Window* wnd,HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	ZWindow* zwnd=ZWindowFromWindow(wnd);
	if (zwnd)
	{
		::POINT pt; 
		pt.x = GET_X_LPARAM(lParam); 
		pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(hWnd, &pt);

		::RECT rcClient;
		::GetClientRect(hWnd, &rcClient);

		if( !::IsZoomed(hWnd) ) {
			RECT rcSizeBox = zwnd->GetManager()->GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}


		RECT rcCaption = zwnd->GetManager()->GetCaptionRect();
		if( pt.x < rcClient.left + rcCaption.left \
			|| pt.x > rcClient.right - rcCaption.right \
			|| pt.y < rcCaption.top \
			|| pt.y > rcClient.bottom-rcCaption.bottom ) 
		{
			ZuiLib::POINT pt2={pt.x,pt.y};
			ZControl* pControl = static_cast<ZControl*>(zwnd->GetManager()->FindControl(pt2));
			if( pControl && strcmp(pControl->GetClass(), "ButtonUI") != 0 && 
				strcmp(pControl->GetClass(),"OptionUI") != 0 /*&&
				_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 */)
				return HTCAPTION;
		}
	}
	return HTCLIENT;
}


//»æÍ¼²Ù×÷
HRESULT WindowRenderHandle(SDL_Window* wnd,const RECT* rc)
{
	ZWindow* zwnd=ZWindowFromWindow(wnd);
	if (zwnd)
		zwnd->Render((ZuiLib::RECT*)rc);
	return 0;
}

#endif




bool ZWindow::CreateWindow(const char *title,int x, int y, int w, int h, uint32_t flags)
{
	if (m_win)
		return false;
	m_win= SDL_CreateWindow(title, x,y,w,h,flags);
	
	if (m_win ){
		SDL_SetWindowBordered(m_win,SDL_FALSE);
		//SDL_SetWindowTransparent(m_win,SDL_TRUE);
		SDL_SetWindowData(m_win,WND_USER_DATA,this);

#ifdef WIN32
		SDL_SetWindowData(m_win,"EV_HitTest",WindowHitTestHandle);
		SDL_SetWindowData(m_win,"EV_Render",WindowRenderHandle);
#endif

		m_render=new ZRenerder(m_win);
		m_mgr.Init(this);
		Init();
		return true;
	}
	else
		return false;
}


void ZWindow::SetWindowIcon(SDL_Surface *ico)
{
	SDL_SetWindowIcon(m_win,ico);
}

void ZWindow::SetPosition(POINT pt)
{
	SDL_SetWindowPosition(m_win,pt.x,pt.y);
}

POINT ZWindow::GetPosition()
{
	POINT pt={0};
	SDL_GetWindowPosition(m_win,&pt.x,&pt.y);
	return pt;
}

void ZWindow::SetSize(SIZE sz)
{
	SDL_SetWindowSize(m_win,sz.cx,sz.cy);
}

SIZE ZWindow::GetSize()
{
	SIZE sz={0};
	SDL_GetWindowSize(m_win,&sz.cx,&sz.cy);
	return sz;
}


void ZWindow::SetMinimumSize(SIZE sz)
{
	SDL_SetWindowMinimumSize(m_win,sz.cx,sz.cy);
}

SIZE ZWindow::GetMinimumSize()
{
	SIZE sz={0};
	SDL_GetWindowMinimumSize(m_win,&sz.cx,&sz.cy);
	return sz;
}

void ZWindow::SetMaximumSize(SIZE sz)
{
	SDL_SetWindowMaximumSize(m_win,sz.cx,sz.cy);
}

SIZE ZWindow::GetMaximumSize()
{
	SIZE sz={0};
	SDL_GetWindowMaximumSize(m_win,&sz.cx,&sz.cy);
	return sz;
}

void ZWindow::ShowWindow(bool bShow)
{
	if (bShow)
		SDL_ShowWindow(m_win);
	else
		SDL_HideWindow(m_win);
}

void ZWindow::RaiseWindow()
{
	SDL_RaiseWindow(m_win);
}

void ZWindow::SetMaximize()
{
	SDL_MaximizeWindow(m_win);
}
void ZWindow::SetMinimize()
{
	SDL_MinimizeWindow(m_win);
}
void ZWindow::SetRestore()
{
	SDL_MinimizeWindow(m_win);
}

void ZWindow::SetGrab(bool grab)
{
	SDL_SetWindowGrab(m_win,SDL_bool(grab));
}
bool ZWindow::GetGrab()
{
	return SDL_GetWindowGrab(m_win)==SDL_TRUE;
}

bool ZWindow::SetBrightness(float bright)
{
	return SDL_SetWindowBrightness(m_win,bright)==0;
}

void ZWindow::Update(const ZRect * rects,int numrects)
{
	if (!rects)
	{
		SDL_UpdateWindowSurface(m_win);
	}
	else
	{
		SDL_UpdateWindowSurfaceRects(m_win,rects,numrects);
	}
}

void ZWindow::Destroy()
{
	SDL_DestroyWindow(m_win);
}


float ZWindow::GetBrightness()
{
	return SDL_GetWindowBrightness(m_win);
}

void ZWindow::PostClearUpEvent()
{
	SDL_Event event;
	event.type=SDL_USEREVENT;
	event.user.code=SET_CLEAR_UP;
	event.user.windowID=SDL_GetWindowID(m_win);
	SDL_PushEvent(&event);
}


void ZWindow::UserEvent(const SDL_UserEvent& event)
{
	switch(event.code)
	{
	case SET_CLEAR_UP:
		m_mgr.DoDelayedCleanup();
		break;
	default:
		break;
	};
}

void ZWindow::MouseButtonEvent(const SDL_MouseButtonEvent& ev)
{
	POINT pt={ev.x,ev.y};
	ZUIEVENT_TYPE evType=UIEVENT_NONE;
	if(ev.button==1)
	{
		if (ev.type ==SDL_MOUSEBUTTONDOWN)
		{
			if(ev.clicks==1)
				evType=UIEVENT_BUTTONDOWN;
			else
				evType=UIEVENT_DBLCLICK;
		}
		else if (ev.type ==SDL_MOUSEBUTTONUP)
		{
			evType=UIEVENT_BUTTONUP;
		}
	}
	else
	{
		if (ev.type ==SDL_MOUSEBUTTONDOWN)
		{
			if(ev.clicks==1)
				evType=UIEVENT_RBUTTONDOWN;
		}
		else if (ev.type ==SDL_MOUSEBUTTONUP)
		{
			evType=UIEVENT_RBUTTONUP;
		}
	}

	if(evType)
		m_mgr.OnMouseEvent(evType,pt);
}

void ZWindow::MouseWheelEvent(const SDL_MouseWheelEvent& ev)
{
	POINT pt={ev.x,ev.y};
	m_mgr.OnMouseEvent(UIEVENT_SCROLLWHEEL,pt);	
}

void ZWindow::MouseMotionEvent(const SDL_MouseMotionEvent& ev)
{
	POINT pt={ev.x,ev.y};
	m_mgr.OnMouseEvent(UIEVENT_MOUSEMOVE,pt);
}

void ZWindow::WindowEvent(const SDL_WindowEvent& event)
{
	switch(event.event)
	{
	case SDL_WINDOWEVENT_ENTER:

		break;
	case SDL_WINDOWEVENT_LEAVE:
		{
			POINT pt={-1,-1};
			m_mgr.OnMouseEvent(UIEVENT_MOUSELEAVE,pt);
		}
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:

		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:

		break;

	}
}

void ZWindow::Init()
{

}

void ZWindow::Render(const RECT* rc)
{
	m_tick=base::Time()-m_startTime;
	if (m_render)
	{
		//²Á³ý±³¾°
		m_render->SetDrawColor(0xdddddddd);
		m_render->Clear();
		m_mgr.DoPaint(m_render,*rc);
		m_render->Present();

		if (m_tick-m_lastColect>1000)
		{
			//Ã¿Ãë¼ì²é»ØÊÕ
			m_render->GetFontMgr()->CollectTextCache(m_tick-m_lastColect);
			m_lastColect=m_tick;
		}
	}
}


}//namespace

