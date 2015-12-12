#include "ZTexture.h"
#include "ZRenerder.h"

namespace ZuiLib
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//texture
	// 
	ZTexture::ZTexture(SDL_Texture* texture)
		:m_texture(texture)
	{
		
	}

	ZTexture::ZTexture(ZRenerder* ren,int w,int h)
	{
		m_texture=SDL_CreateTexture(ren->m_ren,0,SDL_TEXTUREACCESS_STREAMING,w,h);
	}

	ZTexture::~ZTexture()
	{
		SDL_DestroyTexture(m_texture);
	}







	std::string ZTextureMgr::s_resourcePath;




	ZTextureMgr::ZTextureMgr(ZRenerder* ren):m_ren(ren)
	{


	}
	ZTextureMgr::~ZTextureMgr()
	{

	}

	void ZTextureMgr::SetTexture(const char* name,ZTexturePtr texture)
	{
		if(name && texture.get())
			m_textures[name]=texture;
	}

	ZTexturePtr ZTextureMgr::GetTexture(const char* name)
	{
		std::map<std::string,ZTexturePtr>::iterator find=m_textures.find(name);
		if (find!=m_textures.end())
		{
			return find->second;
		}
		else
			return NULL;
	}

	ZTexturePtr ZTextureMgr::GetTextureEx(const char* name)
	{
		ZTexturePtr texture=GetTexture(name);
		if (texture.get())
			return texture;
		else
			return LoadImage(name);
	}

	//从文件加载图片路径为utf8
	ZTexturePtr ZTextureMgr::LoadImage(const char* img)
	{
		std::string path=s_resourcePath;
		path.append(img);
		ZTexturePtr texture=m_ren->CreateTexture(path.c_str());
		if (texture.get())
		{
			SetTexture(path.c_str(),texture);
		}
		return texture;
	}

}










