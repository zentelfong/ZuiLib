
#include "support/uthash.h"
#include "ZUtils.h"

#define free nedfree
#define malloc nedmalloc
#define realloc nedrealloc

namespace ZuiLib
{

	bool PtInRect(const RECT* lprc,POINT lpt)
	{
		return (lpt.x <= lprc->right && lpt.x >= lprc->left && lpt.y < lprc->bottom && lpt.y > lprc->top);
	}

	bool SetRect(LPRECT lprc,int xLeft,int yTop,int xRight,int yBottom)
	{
		if(!lprc)
			return false;
		lprc->left=xLeft;
		lprc->top=yTop;
		lprc->right=xRight;
		lprc->bottom=yBottom;
		return true;
	}
	bool SetRectEmpty(LPRECT lprc)
	{
		if(!lprc)
			return false;
		lprc->left=0;
		lprc->top=0;
		lprc->right=0;
		lprc->bottom=0;
		return true;
	}
	bool CopyRect(LPRECT lprcDst,const RECT *lprcSrc)
	{
		if(!lprcDst || !lprcSrc)
			return false;
		lprcDst->left=lprcSrc->left;
		lprcDst->top=lprcSrc->top;
		lprcDst->right=lprcSrc->right;
		lprcDst->bottom=lprcSrc->bottom;
		return true;
	}
	bool InflateRect( LPRECT lprc, int dx, int dy)
	{
		if(!lprc)
			return false;
		lprc->left-=dx;
		lprc->right+=dx;
		lprc->top-=dy;
		lprc->bottom+=dy;
		return true;
	}

	bool IntersectRect(LPRECT lprcDst,const RECT *lprcSrc1,const RECT *lprcSrc2)
	{
		if (!lprcSrc1 || !lprcSrc2)
			return false;

		RECT rc;
		rc.left = SDL_max(lprcSrc1->left, lprcSrc2->left);
		rc.right = SDL_min(lprcSrc1->right, lprcSrc2->right);
		rc.top = SDL_max(lprcSrc1->top, lprcSrc2->top);
		rc.bottom = SDL_min(lprcSrc1->bottom, lprcSrc2->bottom);
		if (lprcDst)
			*lprcDst=rc;
		if (rc.left >= rc.right || rc.top >= rc.bottom)
		{
			return false;
		}
		else
			return true;
	}

	bool UnionRect(LPRECT lprcDst,const RECT *lprcSrc1,const RECT *lprcSrc2)
	{
		if (!lprcSrc1 || !lprcSrc2)
			return false;

		RECT rc;
		rc.left = SDL_min(lprcSrc1->left, lprcSrc2->left);
		rc.right = SDL_max(lprcSrc1->right, lprcSrc2->right);
		rc.top = SDL_min(lprcSrc1->top, lprcSrc2->top);
		rc.bottom = SDL_max(lprcSrc1->bottom, lprcSrc2->bottom);
		if (lprcDst)
			*lprcDst=rc;
		return true;
	}

	bool OffsetRect(LPRECT lprc,int dx,int dy)
	{
		if (!lprc)
			return false;
		lprc->left+=dx;
		lprc->right+=dx;
		lprc->top+=dy;
		lprc->bottom+=dy;
		return true;
	}

	bool IsRectEmpty(const RECT *lprc)
	{
		if (lprc)
			return lprc->right==lprc->left && lprc->top==lprc->bottom;
		else
			return false;
	}

	bool EqualRect(const RECT *lprc1,const RECT *lprc2)
	{
		if(!lprc1 || !lprc2)
			return false;
		return (lprc1->left==lprc2->left && lprc1->top==lprc2->top && lprc1->right==lprc2->right && lprc1->bottom==lprc2->bottom);
	}

	bool JoinRect(RECT *lpdst,const RECT *src)
	{
		if (!lpdst || !src)
			return false;

		if( src->left < lpdst->left ) lpdst->left = src->left;
		if( src->top < lpdst->top ) lpdst->top = src->top;
		if( src->right > lpdst->right ) lpdst->right = src->right;
		if( src->bottom > lpdst->bottom ) lpdst->bottom = src->bottom;
		return true;
	}



	ZPtrArray::ZPtrArray(int iPreallocSize)
	{
		m_impl.reserve(iPreallocSize);
	}

	ZPtrArray::ZPtrArray(const ZPtrArray& src)
		:m_impl(src.m_impl)
	{
		
	}
	ZPtrArray::~ZPtrArray()
	{

	}

	void ZPtrArray::Empty()
	{
		m_impl.clear();
	}
	void ZPtrArray::Resize(int iSize)
	{
		m_impl.resize(iSize);
	}
	bool ZPtrArray::IsEmpty() const
	{
		return  m_impl.empty();
	}

	int ZPtrArray::Find(data_t val) const
	{
		for (int i=0;i<m_impl.size();++i)
		{
			if (m_impl[i]==val)
			{
				return i;
			}
		}
		return -1;
	}

	void ZPtrArray::Add(data_t pData)
	{
		m_impl.push_back(pData);
	}


	bool ZPtrArray::SetAt(int iIndex, data_t pData)
	{
		if (iIndex<=GetSize())
		{
			m_impl[iIndex]=pData;
			return true;
		}
		else
			return false;
	}
	bool ZPtrArray::InsertAt(int iIndex, data_t pData)
	{
		if (iIndex<=GetSize())
		{
			m_impl.insert(m_impl.begin()+iIndex,pData);
			return true;
		}
		else
			return false;
	}
	bool ZPtrArray::Remove(int iIndex)
	{
		if(iIndex<GetSize() && iIndex>=0)
		{
			m_impl.erase(m_impl.begin()+iIndex);
			return true;
		}
		else
			return false;
	}

	bool ZPtrArray::Remove(const data_t ptr)
	{
		for (container_t::iterator i=m_impl.begin();i!=m_impl.end();++i)
		{
			if (*i==ptr)
			{
				m_impl.erase(i);
				return true;
			}
		}
		return false;
	}

	int ZPtrArray::GetSize() const
	{
		return m_impl.size();
	}
	ZPtrArray::data_t ZPtrArray::GetData()
	{
		return m_impl.front();
	}
	ZPtrArray::data_t ZPtrArray::GetAt(int iIndex) const
	{
		if(iIndex>=0 && iIndex<m_impl.size())
			return m_impl.at(iIndex);
		else
			return NULL;
	}
	ZPtrArray::data_t ZPtrArray::operator[] (int iIndex) const
	{
		return GetAt(iIndex);
	}

	ZPtrArray::data_t ZPtrArray::Front()const
	{
		return m_impl.front();
	}

	ZPtrArray::data_t ZPtrArray::Back()const
	{
		return m_impl.back();
	}











	///////////////////////////////////////////////////////////////////////////////////////////////
	//hashmap
	struct ZHashMapImpl
	{
		UT_hash_handle hh;
		void* data;
		char key[1];
	};


	ZHashMap::ZHashMap()
		:m_impl(NULL)
	{
	}
	ZHashMap::~ZHashMap()
	{
		RemoveAll();
	}

	ZHashMap::data_t ZHashMap::Find(const char* key) const
	{
		ZHashMapImpl* val=NULL;
		HASH_FIND_STR( m_impl, key,val);
		if (val)
			return val->data;
		else
			return NULL;
	}

	void ZHashMap::Set(const char* key, data_t pData)
	{
		struct ZHashMapImpl * val;
		int keyLen=strlen(key);
		val = (struct ZHashMapImpl*)malloc(sizeof(struct ZHashMapImpl)+ keyLen*sizeof(char));
		val->data=pData;
		strcpy(val->key,key);
		HASH_ADD_KEYPTR(hh,m_impl,key,keyLen,val );
	}

	bool ZHashMap::Remove(const char* key)
	{
		ZHashMapImpl* val=NULL;
		HASH_FIND_STR( m_impl, key,val);
		if(val)
		{
			HASH_DEL(m_impl,val);
			free(val);
			return true;
		}
		else
			return false;
	}


	void ZHashMap::RemoveAll()
	{
		if (!m_impl)
			return;
		ZHashMapImpl *val=NULL,*tmp=NULL;
		HASH_ITER(hh, m_impl, val, tmp) 
		{
			HASH_DEL(m_impl,val);
			free(val);
		}
		m_impl=NULL;
	}

	bool ZHashMap::Remove(Iterator* it)
	{
		if(it)
		{
			HASH_DEL(m_impl,it->m_data);
			free(it->m_data);
			return true;
		}
		else
			return false;
	}

	int ZHashMap::GetSize() const
	{
		return HASH_COUNT(m_impl);
	}

	ZHashMap::Iterator ZHashMap::Begin()
	{
		return Iterator(m_impl);
	}

	ZHashMap::Iterator ZHashMap::End()
	{
		return Iterator(NULL);
	}


	ZHashMap::Iterator::Iterator(ZHashMapImpl* data)
		:m_data(data)
	{
	}

	ZHashMap::Iterator::Iterator(const Iterator& rfs)
	{
		m_data=rfs.m_data;
	}

	ZHashMap::Iterator& ZHashMap::Iterator::operator++()
	{
		if(m_data)
			m_data=(struct ZHashMapImpl*)(m_data->hh.next);
		return *this;
	}

	bool ZHashMap::Iterator::operator==(const Iterator& rfs)
	{
		return m_data==rfs.m_data;
	}

	bool ZHashMap::Iterator::operator!=(const Iterator& rfs)
	{
		return m_data!=rfs.m_data;
	}

	ZHashMap::data_t ZHashMap::Iterator::operator*()
	{
		if(m_data)
			return m_data->data;
		else
			return NULL;
	}

	ZHashMap::data_t ZHashMap::Iterator::Data()
	{
		if(m_data)
			return m_data->data;
		else
			return NULL;
	}

	void ZHashMap::Iterator::SetData(data_t data)
	{
		m_data->data=data;
	}

	const char* ZHashMap::Iterator::Key()
	{
		if(m_data)
			return m_data->key;
		else
			return NULL;
	}


	/////////////////////////////////////////////////////////////////////////////////////////
	//hashmap
	struct ZRefHashMapImpl
	{
		UT_hash_handle hh;
		RefCountedBase* data;
		char key[1];
	};


	ZRefHashMap::ZRefHashMap()
		:m_impl(NULL)
	{
	}
	ZRefHashMap::~ZRefHashMap()
	{
		RemoveAll();
	}

	ZRefHashMap::data_t ZRefHashMap::Find(const char* key) const
	{
		ZRefHashMapImpl* val=NULL;
		HASH_FIND_STR( m_impl, key,val);
		if (val)
			return val->data;
		else
			return NULL;
	}

	void ZRefHashMap::Set(const char* key, data_t pData)
	{
		struct ZRefHashMapImpl * val;
		int keyLen=strlen(key);
		val = (struct ZRefHashMapImpl*)malloc(sizeof(struct ZRefHashMapImpl)+ keyLen*sizeof(char));
		val->data=pData;
		if(val->data)
			val->data->AddRef();
		strcpy(val->key,key);
		HASH_ADD_KEYPTR(hh,m_impl,key,keyLen,val );
	}

	bool ZRefHashMap::Remove(const char* key)
	{
		ZRefHashMapImpl* val=NULL;
		HASH_FIND_STR( m_impl, key,val);
		if(val)
		{
			HASH_DEL(m_impl,val);
			if(val->data)
				val->data->Release();
			free(val);
			return true;
		}
		else
			return false;
	}


	void ZRefHashMap::RemoveAll()
	{
		if (!m_impl)
			return;
		ZRefHashMapImpl *val=NULL,*tmp=NULL;
		HASH_ITER(hh, m_impl, val, tmp) 
		{
			HASH_DEL(m_impl,val);
			if(val->data)
				val->data->Release();
			free(val);
		}
		m_impl=NULL;
	}


	int ZRefHashMap::GetSize() const
	{
		return HASH_COUNT(m_impl);
	}

	ZRefHashMap::Iterator ZRefHashMap::Begin()
	{
		return Iterator(m_impl);
	}

	ZRefHashMap::Iterator ZRefHashMap::End()
	{
		return Iterator(NULL);
	}


	ZRefHashMap::Iterator::Iterator(ZRefHashMapImpl* data)
		:m_data(data)
	{
	}

	ZRefHashMap::Iterator::Iterator(const Iterator& rfs)
	{
		m_data=rfs.m_data;
	}

	ZRefHashMap::Iterator& ZRefHashMap::Iterator::operator++()
	{
		if(m_data)
			m_data=(struct ZRefHashMapImpl*)(m_data->hh.next);
		return *this;
	}

	bool ZRefHashMap::Iterator::operator==(const Iterator& rfs)
	{
		return m_data==rfs.m_data;
	}

	bool ZRefHashMap::Iterator::operator!=(const Iterator& rfs)
	{
		return m_data!=rfs.m_data;
	}

	ZRefHashMap::data_t ZRefHashMap::Iterator::operator*()
	{
		if(m_data)
			return m_data->data;
		else
			return NULL;
	}

	ZRefHashMap::data_t ZRefHashMap::Iterator::Data()
	{
		if(m_data)
			return m_data->data;
		else
			return NULL;
	}

	const char* ZRefHashMap::Iterator::Key()
	{
		if(m_data)
			return m_data->key;
		else
			return NULL;
	}








} // namespace DuiLib