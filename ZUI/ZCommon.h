#ifndef __COMMON_H__
#define __COMMON_H__

//依赖库头文件
#include <stdint.h>
#include "base/malloc/nedmalloc.h"
#include "base/RefBase.h"
#include "SDL.h"
#include "../Image/SDL_image.h"
#include "../TTF/SDL_ttf.h"
#include "sdl_gfx/SDL2_gfxPrimitives.h"

#include "tinyxml2/tinyxml2.h"

namespace ZuiLib
{

	template<class T>
	inline void zswap(T& v1,T& v2)
	{
		T temp=v1;
		v1=v2;
		v2=temp;
	}

	template<class T>
	inline T zmin(T& v1,T& v2)
	{
		if (v2<v1)
			return v2;
		else
			return v1;
	}

	template<class T>
	inline T zmax(T& v1,T& v2)
	{
		if (v1<v2)
			return v2;
		else
			return v1;
	}

	inline char* AllocString(const char* str)
	{
		int len=strlen(str)+1;
		char* ptr=(char* )nedmalloc(len);
		memcpy(ptr,str,len);
		return ptr;
	}

	inline void FreeString(char* str)
	{
		nedfree(str);
	}


#ifndef DISALLOW_ASSIGN
	#define DISALLOW_ASSIGN(TypeName) \
	  void operator=(const TypeName&)
#endif


// A macro to disallow the evil copy constructor and operator= functions
// This should be used in the private: declarations for a class
#ifndef DISALLOW_COPY_AND_ASSIGN
	#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
	  TypeName(const TypeName&);                    \
	  DISALLOW_ASSIGN(TypeName)
#endif


// Alternative, less-accurate legacy name.
#ifndef DISALLOW_EVIL_CONSTRUCTORS
	#define DISALLOW_EVIL_CONSTRUCTORS(TypeName) \
	  DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.

#ifndef DISALLOW_IMPLICIT_CONSTRUCTORS
	#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
	  TypeName();                                    \
	  DISALLOW_EVIL_CONSTRUCTORS(TypeName)
#endif


}//namespace









#endif