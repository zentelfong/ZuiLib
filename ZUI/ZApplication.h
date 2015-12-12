#ifndef __ZUIAPPLICATION_H__
#define __ZUIAPPLICATION_H__
#include "base/constructormagic.h"
#include "base/criticalsection.h"
#include "base/RefBase.h"
#include <deque>
#include <vector>

namespace ZuiLib
{
#ifdef CreateWindow
#undef CreateWindow
#endif

	class ZWindow;
	#define WND_USER_DATA "ZWindowPtr"

	class ZMessage:public base::RefCountedBase
	{
	public:
		virtual void Handle();


		IMPLEMENT_REFCOUNTING(ZMessage);
	};
	
	class ZApplication
	{
	public:
		static ZApplication* Instance();

		ZApplication();
		~ZApplication();

		void PostMessage(RefCountedPtr<ZMessage> msg);
		void DoMessageLoop();
		void Quit();
	protected:
		friend class ZWindow;
		bool AddWindow(ZWindow* wnd);
		bool RemoveWindow(ZWindow* wnd);
	private:
		DISALLOW_COPY_AND_ASSIGN(ZApplication);
		static ZApplication* s_app;

		base::CriticalSection m_lock;
		std::deque<RefCountedPtr<ZMessage>> m_msgQueue;

		bool m_quit;
		std::vector<ZWindow*> m_windows;
	};


}//namespace




#endif