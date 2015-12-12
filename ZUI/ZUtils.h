#ifndef __UTILS_H__
#define __UTILS_H__

#include "ZCommon.h"
#include <vector>


namespace ZuiLib
{

	typedef uint32_t color_t;

	inline color_t ConvertColor(SDL_Color color)
	{
		return *(color_t*)(&color);
	}

	inline SDL_Color ConvertColor(color_t color)
	{
		return *(SDL_Color*)(&color);
	}

	typedef struct tagRECT
	{
		int    left;
		int    top;
		int    right;
		int    bottom;
	} RECT, *PRECT,*NPRECT, *LPRECT;

	typedef struct tagSIZE
	{
		int        cx;
		int        cy;
	} SIZE, *PSIZE, *LPSIZE;

	typedef struct tagPOINT
	{
		int  x;
		int  y;
	} POINT, *PPOINT, *NPPOINT, *LPPOINT;

	//RECT Api
	bool PtInRect(const RECT* lprc,POINT lpt);
	bool SetRect(LPRECT lprc,int xLeft,int yTop,int xRight,int yBottom);
	bool SetRectEmpty(LPRECT lprc);
	bool CopyRect(LPRECT lprcDst,const RECT *lprcSrc);
	bool InflateRect( LPRECT lprc, int dx, int dy);
	bool IntersectRect(LPRECT lprcDst,const RECT *lprcSrc1,const RECT *lprcSrc2);
	bool UnionRect(LPRECT lprcDst,const RECT *lprcSrc1,const RECT *lprcSrc2);

	bool OffsetRect(LPRECT lprc,int dx,int dy);
	bool IsRectEmpty(const RECT *lprc);
	bool EqualRect(const RECT *lprc1,const RECT *lprc2);
	bool JoinRect(RECT *lpdst,const RECT *src);

	inline const char* CharNext(const char* ptr)
	{
		if(*ptr==NULL)
			return ptr;
		else
			return (ptr+1);
	}

#define ZMIN(x, y) (((x) < (y)) ? (x) : (y))
#define ZMAX(x, y) (((x) > (y)) ? (x) : (y))



	class ZPoint:public SDL_Point
	{
	public:
		ZPoint()
		{
			x=y=0;
		}

		ZPoint(int _x, int _y)
		{
			x=_x;
			y=_y;
		}
	};

	class ZSize
	{
	public:
		ZSize()
			:w(0),h(0)
		{
		}


		ZSize(int cx, int cy)
			:w(cx),h(cy)
		{
		}

		int w;
		int h;
	};



	class ZRect:public SDL_Rect
	{
	public:
		ZRect(){
			x=y=w=h=0;
		}
		ZRect(int _x, int _y, int _w, int _h){
			x=_x;
			y=_y;
			w=_w;
			h=_h;
		}

		ZRect(SDL_Rect rc)
		{
			x=rc.x;
			y=rc.y;
			w=rc.w;
			h=rc.h;
		}

		ZRect(RECT rc)
		{
			x=rc.left;
			y=rc.top;
			w=rc.right-rc.left;
			h=rc.bottom-rc.top;
		}

		RECT GetCRect()const
		{
			RECT rc={x,y,x+w,y+h};
			return rc;
		}

		inline ZPoint GetPos()const {return ZPoint(x,y);}

		inline int GetWidth() const{return w;}
		inline int GetHeight() const{return h;}
		inline void Empty(){x=y=w=h=0;}
		inline bool IsNull() const{return (x==0 && y==0 && w==0 && h==0);}
		inline bool IsEmpty()const {return (w==0 && h==0);}
		inline int GetRight()const {return x+w;}
		inline int GetBottom()const {return y+h;}
		inline int GetLeft()const {return x;}
		inline int GetTop()const {return y;}

		//移动矩形
		void Offset(int cx, int cy)
		{
			x+=cx;
			y+=cx;
		}

		bool operator==(const ZRect& rfs)
		{
			return (x==rfs.x && y==rfs.y && w == rfs.w && h == rfs.h);
		}

		ZRect& operator=(const ZRect& rfs)
		{
			x=rfs.x;
			y=rfs.y;
			w=rfs.w;
			h=rfs.h;
			return *this;
		}


		bool PtInRect(ZPoint pt)const
		{
			return (pt.x<=x+w && pt.x>=x && pt.y<=y+h && pt.y>=y);
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//array

	
	class ZPtrArray
	{
	public:
		typedef void* data_t;

		ZPtrArray(int iPreallocSize = 0);
		ZPtrArray(const ZPtrArray& src);
		~ZPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(data_t iIndex) const;
		void Add(data_t pData);

		bool SetAt(int iIndex, data_t pData);
		bool InsertAt(int iIndex, data_t pData);
		bool InsertAt(int iIndex,const ZPtrArray& pData);

		bool Remove(int iIndex);
		bool Remove(const data_t ptr);

		int GetSize() const;
		data_t GetData();

		data_t GetAt(int iIndex) const;
		data_t operator[] (int nIndex) const;

		data_t Front()const;
		data_t Back()const;

	private:
		typedef std::vector<data_t> container_t;
		container_t m_impl;
	};




	//////////////////////////////////////////////////////////////////////////////////////////////////
	//hashmap
	struct ZHashMapImpl;
	class ZHashMap
	{
	public:
		typedef void* data_t;

		ZHashMap();
		~ZHashMap();

		data_t Find(const char* key) const;
		void Set(const char* key, data_t pData);
		bool Remove(const char* key);
		void RemoveAll();
		int GetSize() const;

		class Iterator
		{
		public:
			Iterator(ZHashMapImpl* data);
			Iterator(const Iterator& rfs);
			Iterator& operator++();
			bool operator==(const Iterator& rfs);
			bool operator!=(const Iterator& rfs);
			data_t operator*();
			data_t Data();
			const char* Key();
			void SetData(data_t data);
		private:
			friend class ZHashMap;
			ZHashMapImpl* m_data;
		};

		Iterator Begin();
		Iterator End();
		bool Remove(Iterator* it);
	private:
		DISALLOW_COPY_AND_ASSIGN(ZHashMap);
		ZHashMapImpl* m_impl;
	};


	//保存引用计数对象
	struct ZRefHashMapImpl;
	class ZRefHashMap
	{
	public:
		typedef RefCountedBase* data_t;

		ZRefHashMap();
		~ZRefHashMap();

		data_t Find(const char* key) const;
		void Set(const char* key, data_t pData);
		bool Remove(const char* key);
		void RemoveAll();
		int GetSize() const;

		class Iterator
		{
		public:
			Iterator(ZRefHashMapImpl* data);
			Iterator(const Iterator& rfs);
			Iterator& operator++();
			bool operator==(const Iterator& rfs);
			bool operator!=(const Iterator& rfs);
			data_t operator*();
			data_t Data();
			const char* Key();
		private:
			ZRefHashMapImpl* m_data;
		};

		Iterator Begin();
		Iterator End();
	private:
		DISALLOW_COPY_AND_ASSIGN(ZRefHashMap);
		ZRefHashMapImpl* m_impl;
	};

	//////////////////////////////////////////////////////////////////////
	class ZMutex
	{
	public:
		ZMutex()
		{
			m_mutex=SDL_CreateMutex();
		}

		~ZMutex()
		{
			SDL_DestroyMutex(m_mutex);
		}


		bool Lock()
		{
			return SDL_LockMutex(m_mutex)==0;
		}

		bool Trylock()
		{
			return SDL_TryLockMutex(m_mutex)==0;
		}
		bool Unlock()
		{
			return SDL_UnlockMutex(m_mutex)==0;
		}

	private:
		friend class ZCondition;
		SDL_mutex* m_mutex;
	};

	
	class ZCondition
	{
	public:
		ZCondition()
		{
			m_cdt=SDL_CreateCond();
		}

		~ZCondition()
		{
			SDL_DestroyCond(m_cdt);
		}

		//激活单个
		bool Signal()
		{
			return SDL_CondSignal(m_cdt)==0;
		}
		//激活所有等待的线程
		bool Broadcast()
		{
			return SDL_CondBroadcast(m_cdt)==0;
		}

		//等待
		bool Wait(ZMutex* mutex)
		{
			return SDL_CondWait(m_cdt,mutex->m_mutex)==0;
		}

		//warning On some platforms this function is implemented by looping with a delay of 1 ms
		bool Wait(ZMutex* mutex,uint32_t ms)
		{
			return SDL_CondWaitTimeout(m_cdt,mutex->m_mutex,ms)==0;
		}

	private:
		SDL_cond* m_cdt;
	};

	//信号量
	class ZSemaphore
	{
	public:
		ZSemaphore(uint32_t initial_value=0)
		{
			m_sep=SDL_CreateSemaphore(initial_value);
		}

		~ZSemaphore()
		{
			SDL_DestroySemaphore(m_sep);
		}

		//如果值大于0自动将信号量减1，否则等待信号量被激活
		bool Wait()
		{
			return SDL_SemWait(m_sep)==0;
		}

		bool Wait(uint32_t timeout)
		{
			return SDL_SemWaitTimeout(m_sep,timeout)==0;
		}

		bool TryWait()
		{
			return SDL_SemTryWait(m_sep)==0;
		}

		//增加信号量
		bool Signal()
		{
			return SDL_SemPost(m_sep)==0;
		}

		int Value()
		{
			return SDL_SemValue(m_sep);
		}
	private:
		SDL_semaphore* m_sep;
	};

}// namespace ZuiLib

#endif // __UTILS_H__