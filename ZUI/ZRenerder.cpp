#include "ZRenerder.h"

namespace ZuiLib
{

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//renderer

	ZRenerder::ZRenerder(SDL_Window* wnd)
		:m_textureMgr(this)
	{
		m_ren=SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
	}

	ZRenerder::~ZRenerder()
	{
		SDL_DestroyRenderer(m_ren);
	}

	RefCountedPtr<ZTexture> ZRenerder::CreateTexture(SDL_Surface * surface)
	{
		SDL_Texture* texture=SDL_CreateTextureFromSurface(m_ren,surface);
		return new RefCountedObject<ZTexture>(texture);
	}

	RefCountedPtr<ZTexture> ZRenerder::CreateTexture(Uint32 format,int access, int w, int h)
	{
		SDL_Texture* texture=SDL_CreateTexture(m_ren,format,access,w,h);
		return new RefCountedObject<ZTexture>(texture);
	}

	RefCountedPtr<ZTexture> ZRenerder::CreateTexture(const char* path)
	{
		SDL_Surface *sfc = IMG_Load(path);
		RefCountedPtr<ZTexture> texture;
		if(sfc)
		{
			texture=CreateTexture(sfc);
			SDL_FreeSurface(sfc);
		}
		return texture;
	}





	bool ZRenerder::DrawImage(const RECT rc,const std::string& sImageName,RECT rcBmpPart, RECT rcCorners, uint32_t dwMask, uint8_t bFade, \
			bool hole, bool xtiled, bool ytiled)
	{
		if (sImageName.empty()) {
			return false;
		}

		RefCountedPtr<ZTexture> texture=GetTextureMgr()->GetTextureEx(sImageName.c_str());

		if (!texture.get()){
			return false;
		}

		if (dwMask)
		{
			texture->SetTextureColorMod(dwMask);
		}
		else
			texture->SetTextureColorMod(0xffffffff);

		if (IsRectEmpty(&rcBmpPart))
		{
			ZSize sz=texture->GetSize();
			rcBmpPart.left=0;
			rcBmpPart.top=0;
			rcBmpPart.right=sz.w;
			rcBmpPart.bottom=sz.h;
		}

		RECT rcTemp = {0};
		RECT rcDest = {0};

        if( !hole ) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if( !xtiled && !ytiled ) {
				Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top),\
					ZRect( rcBmpPart.left + rcCorners.left,rcBmpPart.top + rcCorners.top,\
					rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
					rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom
					));
            }
            else if( xtiled && ytiled ) {
                int lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                int lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                for( int j = 0; j < iTimesY; ++j ) {
                    int lDestTop = rcDest.top + lHeight * j;
                    int lDestBottom = rcDest.top + lHeight * (j + 1);
                    int lDrawHeight = lHeight;
                    if( lDestBottom > rcDest.bottom ) {
                        lDrawHeight -= lDestBottom - rcDest.bottom;
                        lDestBottom = rcDest.bottom;
                    }
                    for( int i = 0; i < iTimesX; ++i ) {
                        int lDestLeft = rcDest.left + lWidth * i;
                        int lDestRight = rcDest.left + lWidth * (i + 1);
                        int lDrawWidth = lWidth;
                        if( lDestRight > rcDest.right ) {
                            lDrawWidth -= lDestRight - rcDest.right;
                            lDestRight = rcDest.right;
                        }

						Draw(texture,ZRect(rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
                            lDestRight - lDestLeft, lDestBottom - lDestTop),ZRect(rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight));
                    }
                }
            }
            else if( xtiled ) {
                int lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                for( int i = 0; i < iTimes; ++i ) {
                    int lDestLeft = rcDest.left + lWidth * i;
                    int lDestRight = rcDest.left + lWidth * (i + 1);
                    int lDrawWidth = lWidth;
                    if( lDestRight > rcDest.right ) {
                        lDrawWidth -= lDestRight - rcDest.right;
                        lDestRight = rcDest.right;
                    }

					Draw(texture,ZRect(lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom),\
						ZRect(rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                        lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom));
                }
            }
            else { // ytiled
                int lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                for( int i = 0; i < iTimes; ++i ) {
                    int lDestTop = rcDest.top + lHeight * i;
                    int lDestBottom = rcDest.top + lHeight * (i + 1);
                    int lDrawHeight = lHeight;
                    if( lDestBottom > rcDest.bottom ) {
                        lDrawHeight -= lDestBottom - rcDest.bottom;
                        lDestBottom = rcDest.bottom;
                    }

					Draw(texture,ZRect(rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop),\
						ZRect( rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight));
                }
            }

        }

        // left-top
        if( rcCorners.left > 0 && rcCorners.top > 0 ) {
            rcDest.left = rc.left;
            rcDest.top = rc.top;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect( rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),ZRect(rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top));
        }
        // top
        if( rcCorners.top > 0 ) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.top;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom), \
				ZRect(rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
                rcCorners.left - rcCorners.right, rcCorners.top));
        }
        // right-top
        if( rcCorners.right > 0 && rcCorners.top > 0 ) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.top;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),ZRect(rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right,rcCorners.top));
        }
        // left
        if( rcCorners.left > 0 ) {
            rcDest.left = rc.left;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),\
				ZRect( rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
                    rcBmpPart.top - rcCorners.top - rcCorners.bottom));
        }
        // right
        if( rcCorners.right > 0 ) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),\
				ZRect( rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
                    rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom));
        }
        // left-bottom
        if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
            rcDest.left = rc.left;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),\
				ZRect(rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom));
        }
        // bottom
        if( rcCorners.bottom > 0 ) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;


			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),\
				ZRect( rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom));
        }
        // right-bottom
        if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;

			Draw(texture,ZRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom),\
				ZRect(rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right,rcCorners.bottom));
        }
  
		return true;
	}


	bool ZRenerder::DrawImageString(const ZRect& rc , const char* pStrImage, const char* pStrModify)
	{

		// 1、aaa.jpg
		// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
		// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

		std::string sImageName = pStrImage;
		RECT rcItem = {rc.x,rc.y,rc.x+rc.w,rc.y+rc.h};
		RECT rcBmpPart={0,0,0,0};
		RECT rcCorner={0,0,0,0};
		uint32_t dwMask = 0;
		uint8_t bFade = 0xFF;
		bool bHole = false;
		bool bTiledX = false;
		bool bTiledY = false;

		int image_count = 0;

		std::string sItem;
		std::string sValue;
		char* pstr = NULL;

		for( int i = 0; i < 2; ++i,image_count = 0 ) {
			if( i == 1)
				pStrImage = pStrModify;

			if( !pStrImage ) continue;

			while( *pStrImage != '\0' ) {
				sItem.clear();
				sValue.clear();
				while( *pStrImage > '\0' && *pStrImage <= ' ' ) pStrImage = CharNext(pStrImage);
				while( *pStrImage != '\0' && *pStrImage != '=' && *pStrImage > ' ' ) {
					const char* pstrTemp = CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sItem += *pStrImage++;
					}
				}
				while( *pStrImage > '\0' && *pStrImage <= ' ' ) pStrImage = CharNext(pStrImage);
				if( *pStrImage++ != '=' ) break;
				while( *pStrImage > '\0' && *pStrImage <= ' ' ) pStrImage = CharNext(pStrImage);
				if( *pStrImage++ != '\'' ) break;
				while( *pStrImage != '\0' && *pStrImage != '\'' ) {
					const char* pstrTemp = CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sValue += *pStrImage++;
					}
				}
				if( *pStrImage++ != '\'' ) break;
				if( !sValue.empty() ) {
					if( sItem == "file" || sItem == "res" ) {
						if( image_count > 0 )
							DrawImage(rcItem, sImageName, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);
						sImageName = sValue;
						if( sItem == "file" )
							++image_count;
					}
					else if( sItem == "dest" ) {
						rcItem.left = rc.x + strtol(sValue.c_str(), &pstr, 10);    
						rcItem.top = rc.y + strtol(pstr + 1, &pstr, 10);
						rcItem.right = rc.x +strtol(pstr + 1, &pstr, 10);
						rcItem.bottom =rc.y +strtol(pstr + 1, &pstr, 10);
					}
					else if( sItem == "source" ) {
						rcBmpPart.left = strtol(sValue.c_str(), &pstr, 10);    
						rcBmpPart.top = strtol(pstr + 1, &pstr, 10);
						rcBmpPart.right = strtol(pstr + 1, &pstr, 10);
						rcBmpPart.bottom = strtol(pstr + 1, &pstr, 10); 
					}
					else if( sItem == "corner" ) {
						rcCorner.left = strtol(sValue.c_str(), &pstr, 10);    
						rcCorner.top = strtol(pstr + 1, &pstr, 10);
						rcCorner.right = strtol(pstr + 1, &pstr, 10);
						rcCorner.bottom = strtol(pstr + 1, &pstr, 10); 
					}
					else if( sItem == "mask" ) {
						if( sValue[0] == '#') 
							dwMask = strtoul(sValue.c_str() + 1, &pstr, 16);
						else 
							dwMask = strtoul(sValue.c_str(), &pstr, 16);
					}
					else if( sItem == "fade" ) {
						bFade = (uint8_t)strtoul(sValue.c_str(), &pstr, 10);
					}
					else if( sItem == "hole" ) {
						bHole = (strcmp(sValue.c_str(), "true") == 0);
					}
					else if( sItem == "xtiled" ) {
						bTiledX = (strcmp(sValue.c_str(), "true") == 0);
					}
					else if( sItem == "ytiled") {
						bTiledY = (strcmp(sValue.c_str(),"true") == 0);
					}
				}
				if( *pStrImage++ != ' ' ) break;
			}
		}

		DrawImage(rcItem, sImageName, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);
		return true;
	}




	bool ZRenerder::DrawText(RefCountedPtr<ZFont> font,const char *text,uint32_t color,const ZRect& rc,uint32_t style)
	{
		SDL_Color scolor=ConvertColor(color);
		if (font.get() && font.get()->m_font)
		{
			SDL_Texture* texture=NULL;
			ZTextCache* cache=NULL;
			TextStyle textStyle;
			textStyle.bold=(style & DT_STYLE_BOLD)!=0;
			textStyle.color=color;
			textStyle.italic=(style & DT_STYLE_ITALIC)!=0;
			textStyle.underline=(style & DT_STYLE_UNDERLINE)!=0;

			if ((style & DT_END_ELLIPSIS)>0)
			{
				//判断长度是否超过指定长度
				ZSize sz;
				font->TextSize(text,sz);
				if (sz.w > rc.w)
				{
					//长度过长，加...
					int len=strlen(text);
					for (int i=len-1;i>=0;--i)
					{
						ZString str(text,i+1);
						str.Append("...");
						font->TextSize(str,sz);
						if (sz.w<rc.w)
						{
							cache=font->GetTextCache(this,str,&textStyle);
							break;
						}
					}

					if(!cache)
						cache=font->GetTextCache(this,"...",&textStyle);
				}
				else
				{
					cache=font->GetTextCache(this,text,&textStyle);
				}

			}
			else
				cache=font->GetTextCache(this,text,&textStyle);

			if (cache)
				texture=cache->GetTexture();
	
			if(texture)
			{
				ZRect rcSrc;
				ZRect rcDst=rc;
				uint32_t format=0;
				int access=0;
				SDL_QueryTexture(texture,&format,&access,&rcSrc.w,&rcSrc.h);
				rcDst.w=rcSrc.w;
				rcDst.h=rcSrc.h;

				if ((style & DT_CENTER)>0)
				{
					rcDst.x=rc.x+(rc.w-rcSrc.w)/2;
					rcDst.y=rc.y+(rc.h-rcSrc.h)/2;
					rcDst.w=rcSrc.w;
					rcDst.h=rcSrc.h;
				}
				else
				{
					if ((style & DT_VCENTER)>0)
					{
						rcDst.y=rc.y+(rc.h-rcSrc.h)/2;
						rcDst.h=rcSrc.h;
					}
					else if ((style & DT_BOTTOM)>0)
					{
						rcDst.y=rc.y+rc.h-rcSrc.h;
						rcDst.h=rcSrc.h;
					}

					if ((style & DT_RIGHT)>0)
					{
						//right
						rcDst.x=rc.x+rc.w-rcSrc.w;
						rcDst.w=rcSrc.w;
					}
					else
					{
						//left
						rcDst.x=rc.x;
						rcDst.w=rcSrc.w;
					}
				}


				//裁剪rc之外的部分
				if (rcDst.x<rc.x)
				{
					int offset = rc.x-rcDst.x;
					rcDst.x += offset;
					rcSrc.x += offset;
				}

				if (rcDst.y<rc.y)
				{
					int offset=rc.y-rcDst.y;
					rcDst.y+=offset;
					rcSrc.y+=offset;
				}

				if (rcDst.GetRight() > rc.GetRight())
				{
					int offset=rcDst.GetRight()-rc.GetRight();
					rcDst.w-=offset;
					rcSrc.w-=offset;
				}


				if (rcDst.GetBottom() > rc.GetBottom())
				{
					int offset=rcDst.GetBottom()-rc.GetBottom();
					rcDst.h-=offset;
					rcSrc.h-=offset;
				}
				SDL_RenderCopy(m_ren,texture,&rcSrc,&rcDst);
				return true;
			}
			else
				return false;

		}
		else
			return false;
	}




	bool ZRenerder::DrawText(const char *text,uint32_t color,const ZRect& rc,const char* font,int size,uint32_t style)
	{
		ZFontPtr pfont=m_fontMgr.GetFont(font,size);
		if (pfont.get())
		{
			return DrawText(pfont,text,color,rc,style);
		}
		else
			return false;
	}


	bool ZRenerder::DrawText(const char *text,uint32_t color,const ZRect& rc,ZTextInfo* info,uint32_t style)
	{
		if(info==NULL)
			info=m_fontMgr.GetDefaultTextInfo();
		ZFontPtr pfont=m_fontMgr.GetFont(info->name,info->size);
		if (pfont.get())
		{
			if (info->bold)
			{
				style |= DT_STYLE_BOLD;
			}

			if (info->italic)
			{
				style |= DT_STYLE_ITALIC;
			}

			if (info->underline)
			{
				style |= DT_STYLE_UNDERLINE;
			}
			return DrawText(pfont,text,color,rc,style);
		}
		else
			return false;
	}



}










