#ifndef __ZUITEXTURE_H__
#define __ZUITEXTURE_H__

#include "ZCommon.h"
#include "ZUtils.h"
#include <map>
#include <string>
#include <vector>

namespace ZuiLib
{
class ZTextureMgr;
class ZRenerder;


class ZTexture:public RefCountedBase
{
public:
	ZTexture(SDL_Texture* texture);
	ZTexture(ZRenerder* ren,int w,int h);

	~ZTexture();

	void SetTextureColorMod(uint32_t color)
	{
		SDL_Color* pcol=(SDL_Color*)(&color);
		SDL_SetTextureColorMod(m_texture,pcol->r,pcol->g,pcol->b);
		SDL_SetTextureAlphaMod(m_texture,pcol->a);
	}

	uint32_t GetTextureColorMod()
	{
		SDL_Color color;
		SDL_GetTextureColorMod(m_texture,&color.r,&color.g,&color.b);
		SDL_GetTextureAlphaMod(m_texture,&color.a);
		return *(uint32_t*)(&color);
	}


	void SetTextureBlendMode(SDL_BlendMode blendMode)
	{
		SDL_SetTextureBlendMode(m_texture,blendMode);
	}

	SDL_BlendMode GetTextureBlendMode()
	{
		SDL_BlendMode blendMode;
		SDL_GetTextureBlendMode(m_texture,&blendMode);
		return blendMode;
	}

	int SetPixels(ZRect rc,const void *pixels, int pitch)
	{
		return SDL_UpdateTexture(m_texture,&rc,pixels,pitch);
	}

	int SetYUVPixels(ZRect rc, const Uint8 *Yplane, int Ypitch,
                                 const Uint8 *Uplane, int Upitch,
                                 const Uint8 *Vplane, int Vpitch)
	{
		return SDL_UpdateYUVTexture(m_texture,&rc,Yplane,Ypitch,Uplane,Upitch,Vplane,Vpitch);
	}


	int Lock(ZRect rc,void **pixels, int *pitch)
	{
		return SDL_LockTexture(m_texture,&rc,pixels,pitch);
	}

	void Unlock()
	{
		SDL_UnlockTexture(m_texture);
	}

	int glBindTexture(float *texw, float *texh)
	{
		return SDL_GL_BindTexture(m_texture,texw,texh);
	}

	int glUnbindTexture()
	{
		return SDL_GL_UnbindTexture(m_texture);
	}

	ZSize GetSize()
	{
		uint32_t format=0;
		int access=0;
		ZSize sz;
		SDL_QueryTexture(m_texture,&format,&access,&sz.w,&sz.h);
		return sz;
	}

private:
	friend class ZRenerder;
	SDL_Texture* m_texture;
};

typedef RefCountedPtr<ZTexture> ZTexturePtr;


class ZTextureMgr
{
public:
	ZTextureMgr(ZRenerder* ren);
	~ZTextureMgr();

	void SetTexture(const char* name,ZTexturePtr texture);
	ZTexturePtr GetTexture(const char* name);
	ZTexturePtr GetTextureEx(const char* name);

	//从文件加载图片路径为utf8
	ZTexturePtr LoadImage(const char* img);

	static void SetPath(const char* path){s_resourcePath=path;}
private:
	std::map<std::string,ZTexturePtr> m_textures;
	ZRenerder* m_ren;
	static std::string s_resourcePath;
};


}//namespace


#endif