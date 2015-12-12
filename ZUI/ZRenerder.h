#ifndef __ZUIREBERDER_H__
#define __ZUIREBERDER_H__

#include "ZCommon.h"
#include "ZUtils.h"
#include "ZTexture.h"
#include "ZFont.h"

namespace ZuiLib
{


#define DT_TOP                      0x00000000
#define DT_LEFT                     0x00000000
#define DT_CENTER                   0x00000001
#define DT_RIGHT                    0x00000002
#define DT_VCENTER                  0x00000004
#define DT_BOTTOM                   0x00000008
#define DT_WORDBREAK                0x00000010
#define DT_SINGLELINE               0x00000020
#define DT_EXPANDTABS               0x00000040
#define DT_END_ELLIPSIS             0x00008000

//字体样式
#define DT_STYLE_BOLD				0x00010000
#define DT_STYLE_ITALIC				0x00020000
#define DT_STYLE_UNDERLINE			0x00040000
#define DT_STYLE_STRIKETHROUGH		0x00080000




class ZRenerder
{
public:
	ZRenerder(SDL_Window* wnd);
	~ZRenerder();

	//创建texture
	RefCountedPtr<ZTexture> CreateTexture(SDL_Surface * surface);
	RefCountedPtr<ZTexture> CreateTexture(Uint32 format,int access, int w, int h);
	RefCountedPtr<ZTexture> CreateTexture(const char* path);

	inline bool RenderTargetSupported()
	{
		return SDL_RenderTargetSupported(m_ren);
	}

	ZSize GetRendererOutputSize()
	{
		ZSize size;
		SDL_GetRendererOutputSize(m_ren,&size.w,&size.h);
		return size;
	}

	int SetLogicalSize(ZSize size)
	{
		return SDL_RenderSetLogicalSize(m_ren,size.w,size.h);
	}

	ZSize GetLogicalSize()
	{
		ZSize size;
		SDL_RenderGetLogicalSize(m_ren,&size.w,&size.h);
		return size;
	}


	void SetViewport(const ZRect& rc)
	{
		SDL_RenderSetViewport(m_ren,&rc);
	}

	ZRect GetViewport()
	{
		SDL_Rect rc;
		SDL_RenderGetViewport(m_ren,&rc);
		return ZRect(rc.x,rc.y,rc.x+rc.w,rc.y+rc.h);
	}


	int SetClipRect(const ZRect& rc)
	{
		return SDL_RenderSetClipRect(m_ren,&rc);
	}

	ZRect GetClipRect()
	{
		SDL_Rect rc;
		SDL_RenderGetClipRect(m_ren,&rc);
		return rc;
	}

	int SetScale(float x,float y)
	{
		return SDL_RenderSetScale(m_ren,x,y);
	}

	void GetScale(float *scaleX, float *scaleY)
	{
		SDL_RenderGetScale(m_ren,scaleX,scaleY);
	}

	int SetDrawColor(uint32_t color)
	{
		SDL_Color scolor=ConvertColor(color);
		return SDL_SetRenderDrawColor(m_ren,scolor.r,scolor.g,scolor.b,scolor.a);
	}

	uint32_t GetDrawColor()
	{
		SDL_Color color;
		SDL_GetRenderDrawColor(m_ren,&color.r,&color.g,&color.b,&color.a);
		return ConvertColor(color);
	}

	int SetDrawBlendMode(SDL_BlendMode blendMode)
	{
		return SDL_SetRenderDrawBlendMode(m_ren,blendMode);
	}


	SDL_BlendMode GetTextureBlendMode()
	{
		SDL_BlendMode blendMode;
		SDL_GetRenderDrawBlendMode(m_ren,&blendMode);
		return blendMode;
	}

	void Clear()
	{
		SDL_RenderClear(m_ren);
	}

	void Present()
	{
		SDL_RenderPresent(m_ren);
	}

	void Draw(RefCountedPtr<ZTexture> texture,const ZRect& dst,const ZRect& src,int rotate=0)
	{
		if(!texture.get())
			return;
		if(rotate==0)
			SDL_RenderCopy(m_ren,texture->m_texture,&src,&dst);
		else
		{
			SDL_RenderCopyEx(m_ren,texture->m_texture,&src,&dst,rotate,NULL,SDL_FLIP_NONE);
		}
	}

	int DrawPoints(const ZPoint * points,int count)
	{
		return SDL_RenderDrawPoints(m_ren,points,count);
	}

	int DrawLine(int x1, int y1, int x2, int y2)
	{
		return SDL_RenderDrawLine(m_ren,x1,y1,x2,y2);
	}

	int DrawLines(const ZPoint * points,int count)
	{
		return SDL_RenderDrawLines(m_ren,points,count);
	}

	int DrawRect(const ZRect & rect)
	{
		return SDL_RenderDrawRect(m_ren,&rect);
	}


	int DrawRoundRect(const ZRect & rect,int rad)
	{
		return roundedRectangleColor(m_ren,rect.x,rect.y,rect.x+rect.w,rect.y+rect.h,rad,GetDrawColor());
	}

	int DrawRects(const ZRect * rects,int count)
	{
		return SDL_RenderDrawRects(m_ren,rects,count);
	}

	int FillRoundRect(const ZRect & rect,int rad)
	{
		return roundedBoxColor(m_ren,rect.x,rect.y,rect.x+rect.w,rect.y+rect.h,rad,GetDrawColor());
	}

	int FillRect(const ZRect & rect)
	{
		return SDL_RenderFillRect(m_ren,&rect);
	}

	int FillRects(const ZRect * rects,int count)
	{
		return SDL_RenderFillRects(m_ren,rects,count);
	}

	bool DrawImage(const RECT rc,const std::string& sImageName, \
			RECT rcBmpPart,RECT rcCorners, uint32_t dwMask, uint8_t bFade, \
			bool hole, bool xtiled, bool ytiled);

	bool DrawImageString(const ZRect& rc , const char* pStrImage, const char* pStrModify);


	//绘制文字,绘制为单行
	bool DrawText(RefCountedPtr<ZFont> font,const char *text,uint32_t color,const ZRect& rc,uint32_t style=DT_TOP);

	bool DrawText(const char *text,uint32_t color,const ZRect& rc,const char* font,int size,uint32_t style=DT_TOP);

	bool DrawText(const char *text,uint32_t color,const ZRect& rc,ZTextInfo* info,uint32_t style=DT_TOP);

	inline ZTextureMgr* GetTextureMgr(){return &m_textureMgr;}
	inline ZFontMgr* GetFontMgr(){return &m_fontMgr;}

	SDL_Renderer* GetRenderer(){return m_ren;}
private:
	friend class ZTexture;
	SDL_Renderer* m_ren;
	ZTextureMgr m_textureMgr;
	ZFontMgr m_fontMgr;
	ZString m_defaultFont;
};




}//namespace


#endif