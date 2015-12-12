#include "ZApplication.h"
#include "ZWindow.h"
#include <assert.h>

namespace ZuiLib
{
	ZApplication* ZApplication::s_app=NULL;

	ZWindow* ZWindowFromWindow(SDL_Window* wnd)
	{
		return (ZWindow*)SDL_GetWindowData(wnd,WND_USER_DATA);
	}

	ZWindow* ZWindowFromId(uint32_t id)
	{
		SDL_Window* wnd=SDL_GetWindowFromID(id);
		if(wnd)
			return (ZWindow*)SDL_GetWindowData(wnd,WND_USER_DATA);
		else
			return NULL;
	}



	ZApplication::ZApplication()
		:m_quit(false)
	{
		assert(s_app==NULL);
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
		IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
		TTF_Init();
		s_app=this;
	}

	ZApplication::~ZApplication()
	{
		IMG_Quit();
		TTF_Quit();
		SDL_Quit();
		assert(s_app);
		s_app=NULL;
	}



	ZApplication* ZApplication::Instance()
	{
		assert(s_app);
		return s_app;
	}


	void ZApplication::PostMessage(RefCountedPtr<ZMessage> msg)
	{
		base::CritScope lock(&m_lock);
		m_msgQueue.push_back(msg);
	}

	void ZApplication::Quit()
	{
		m_quit=true;
	}

	void ZApplication::DoMessageLoop()
	{
		SDL_Event event;
		//while (!m_quit)
		{
			//Read any events that occured, for now we'll just quit if any event occurs
			while (SDL_WaitEvent(&event) && !m_quit)
			{
				switch(event.type)
				{
				case SDL_QUIT:
					return;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					{
						ZWindow* wnd=ZWindowFromId(event.key.windowID);
						if (wnd)
						{
							wnd->KeyboardEvent(event.key);
						}
					}	
					break;
				case SDL_MOUSEMOTION:
					{
						ZWindow* wnd=ZWindowFromId(event.motion.windowID);
						if (wnd)
						{
							wnd->MouseMotionEvent(event.motion);
						}
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					{
						ZWindow* wnd=ZWindowFromId(event.button.windowID);
						if (wnd)
						{
							wnd->MouseButtonEvent(event.button);
						}
					}
					break;
				case SDL_MOUSEWHEEL:
					{
						ZWindow* wnd=ZWindowFromId(event.wheel.windowID);
						if (wnd)
						{
							wnd->MouseWheelEvent(event.wheel);
						}
					}
					break;
				case SDL_WINDOWEVENT:
					{
						ZWindow* wnd=ZWindowFromId(event.window.windowID);
						if (wnd)
						{
							wnd->WindowEvent(event.window);
						}
					}
					break;
				case SDL_USEREVENT:
					{
						ZWindow* wnd=ZWindowFromId(event.user.windowID);
						if (wnd)
						{
							wnd->UserEvent(event.user);
						}
					}
					break;
				case SDL_TEXTINPUT:
					{
						ZWindow* wnd=ZWindowFromId(event.text.windowID);
						if (wnd)
						{
							wnd->TextInputEvent(event.text);
						}

					}
					break;
				case SDL_TEXTEDITING:
					{
						ZWindow* wnd=ZWindowFromId(event.edit.windowID);
						if (wnd)
						{
							wnd->TextEditingEvent(event.edit);
						}

					}
					break;
				case SDL_SYSWMEVENT:
					{
						event.syswm.msg;
					}
					break;
				}

			}

			//Ω¯––‰÷»æ
			//for (std::vector<ZWindow*>::iterator i=m_windows.begin();i!=m_windows.end();++i)
			//{
			//	(*i)->Render();
			//}
		}
	}



	bool ZApplication::AddWindow(ZWindow* wnd)
	{
		for (std::vector<ZWindow*>::iterator i=m_windows.begin();i!=m_windows.end();++i)
		{
			if (*i==wnd)
				return false;
		}
		m_windows.push_back(wnd);
		return true;
	}

	bool ZApplication::RemoveWindow(ZWindow* wnd)
	{
		for (std::vector<ZWindow*>::iterator i=m_windows.begin();i!=m_windows.end();++i)
		{
			if (*i==wnd)
			{
				m_windows.erase(i);
				return true;
			}
		}
		return false;
	}






}

