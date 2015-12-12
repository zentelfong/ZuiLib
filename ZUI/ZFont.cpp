#include "ZFont.h"
#include "ZRenerder.h"


namespace ZuiLib
{

//////////////////////////////////////////////////////////////////////////
int ZTextCache::s_count=0;

ZTextCache::ZTextCache()
{
	m_mark=base::Time();
	SDL_memset(&m_style,0,sizeof(m_style));
	m_next=NULL;
	m_cacheTexture=NULL;
	m_ren=NULL;

	++s_count;
}


ZTextCache::~ZTextCache()
{
	if(m_cacheTexture)
		SDL_DestroyTexture(m_cacheTexture);
	--s_count;
	printf("cache text count %d\n",s_count);
}

void ZTextCache::Mark()
{
	m_mark=base::Time();
}

bool ZTextCache::Init(ZFont* font,ZRenerder* ren,const char* text,const TextStyle* style)
{
	m_ren=ren;
	m_style=*style;
	SDL_Color* scolor=(SDL_Color*)(&m_style.color);

	int fstyle=0;
	if (style->bold)
	{
		fstyle|=ZFont::FS_BOLD;
	}

	if (style->italic)
	{
		fstyle|=ZFont::FS_ITALIC;
	}

	if (style->underline)
	{
		fstyle|=ZFont::FS_UNDERLINE;
	}

	font->SetStyle(fstyle);

	SDL_Surface* surface=TTF_RenderUTF8_Blended(font->GetTTF(),text,*scolor);
	if (surface)
	{
		m_cacheTexture=SDL_CreateTextureFromSurface(ren->GetRenderer(),surface);
		SDL_FreeSurface(surface);
		return true;
	}
	return false;
}



ZTextCache* ZFont::GetTextCache(ZRenerder* ren,const char* text,const TextStyle* style)
{
	ZTextCache* fcache=static_cast<ZTextCache*>(m_textCaches.Find(text));
	ZTextCache* cache=fcache;
	while (cache)
	{
		if (cache->GetRenerder()==ren && SDL_memcmp(cache->GetStyle(),style,sizeof(TextStyle))==0)
		{
			cache->Mark();
			return cache;
		}
		cache=cache->Next();
	}

	//添加到缓存
	cache=new ZTextCache();
	if (cache->Init(this,ren,text,style))
	{
		if (fcache)
		{
			cache->m_next=fcache->m_next;
			fcache->m_next=cache;
		}
		else
		{
			m_textCaches.Set(text,cache);
		}
		return cache;
	}
	else
	{
		delete cache;
		return NULL;
	}
}



/////////////////////////////////////////////////////////////////////////////////////

ZFont::ZFont(void)
	:m_font(NULL)
{
}


ZFont::~ZFont(void)
{
	CloseFont();
}

bool ZFont::OpenFont(const char* file,int height)
{
	CloseFont();
	m_font=TTF_OpenFont(file,height);
	return m_font!=NULL;
}


bool ZFont::OpenFont(const char* file,int height,long index)
{
	CloseFont();
	m_font=TTF_OpenFontIndex(file,height,index);
	return m_font!=NULL;
}



bool ZFont::CloseFont()
{
	if (m_font)
	{
		FreeTextCache();
		TTF_CloseFont(m_font);
		m_font=NULL;
		return true;
	}
	else
		return false;
}

int ZFont::GetStyle()
{
	if(m_font) 
		return TTF_GetFontStyle(m_font);
	else
		return 0;
}


void ZFont::SetStyle(int style)
{
	if(m_font)
		TTF_SetFontStyle(m_font,style);
}

int ZFont::GetOutline()
{
	if(m_font)
		return TTF_GetFontOutline(m_font);
	else
		return -1;
}

void ZFont::SetOutline(int outline)
{
	if(m_font)
		TTF_SetFontOutline(m_font,outline);
}


ZFont::FontHinting ZFont::GetHinting()
{
	if(m_font)
		return (FontHinting)TTF_GetFontHinting(m_font);
	else
		return FH_NONE;
}
void ZFont::SetHinting(FontHinting hinting)
{
	if(m_font)
		TTF_SetFontHinting(m_font,hinting);
}

int ZFont::Height()
{
	if(m_font)
		return TTF_FontHeight(m_font);
	else
		return 0;
}


int ZFont::Ascent()
{
	if (m_font)
		return TTF_FontAscent(m_font);
	else
		return -1;
}

int ZFont::FontDescent()
{
	if (m_font)
		return TTF_FontDescent(m_font);
	else
		return -1;
}

int ZFont::LineSkip()
{
	if (m_font)
		return TTF_FontLineSkip(m_font);
	else
		return -1;
}

int ZFont::GetKerning()
{
	if (m_font)
		return TTF_GetFontKerning(m_font);
	else
		return -1;
}

void ZFont::SetKerning(int allowed)
{
	if(m_font)
		return TTF_SetFontKerning(m_font,allowed);
}

int ZFont::FaceNumber()
{
	if (m_font)
		return TTF_FontFaces(m_font);
	else
		return 0;
}


bool ZFont::TextSize(const char* text,ZSize &sz)
{
	if(m_font)
	{
		TTF_SizeUTF8(m_font,text,&sz.w,&sz.h);
		return true;
	}
	else
		return false;
}

bool ZFont::TextSize(const uint16_t* text,ZSize &sz)
{
	if(m_font)
	{
		TTF_SizeUNICODE(m_font,text,&sz.w,&sz.h);
		return true;
	}
	else
		return false;
}


/////////////////////////////////////////////////////////////////////////

ZTextInfo ZFontMgr::s_defaultFont;

ZFontMgr::ZFontMgr()
{

}

ZFontMgr::~ZFontMgr()
{

}

ZFontPtr ZFontMgr::GetFont(const char* name,int size)
{
	for (container_t::iterator i=m_fonts.begin();i!=m_fonts.end();++i)
	{
		if (i->name==name && i->size==size)
		{
			return i->font;
		}
	}

	ZFontInfo info;
	info.name=name;
	info.size=size;
	if (LoadFont(&info))
	{
		m_fonts.push_back(info);
		return info.font;
	}
	else
		return NULL;
}


bool ZFontMgr::LoadFont(ZFontInfo* fontInfo)
{
	if(!fontInfo)
		return false;
	ZFontPtr font= ZFont::Create();
	if (font->OpenFont(fontInfo->name,fontInfo->size))
	{
		font->SetHinting(ZFont::FH_LIGHT);
		fontInfo->font=font;
		return true;
	}
	else
		return false;
}


/*
 name,size,bold,underline,italic
 */
bool ZFontMgr::SetTextInfo(const tinyxml2::XMLElement *elem)
{
	assert(elem);
	const char* name=elem->Attribute("name");
	if (!name)
		return false;

	ZTextInfo info;
	info.name=elem->Attribute("file");
	info.bold=elem->BoolAttribute("bold");
	info.italic=elem->BoolAttribute("italic");
	info.size=elem->IntAttribute("size");
	info.underline=elem->BoolAttribute("underline");
	//设置到map表
	m_textInfos[name]=info;
	return true;
}

ZTextInfo* ZFontMgr::GetTextInfo(const char* name)
{
	assert(name);
 	map_t::iterator find=m_textInfos.find(name);
	if (find==m_textInfos.end())
		return NULL;
	else
		return &(find->second);
}

void ZFontMgr::CollectTextCache(uint32_t life)
{
	for (container_t::iterator i=m_fonts.begin();i!=m_fonts.end();++i)
	{
		i->font->CollectTextCache(life);
	}
}

//////////////////////////////////////////////////////////////////


void ZFont::CollectTextCache(uint32_t life)
{
	uint32_t deadline=base::Time()-life;
	for (ZHashMap::Iterator i=m_textCaches.Begin();i!=m_textCaches.End();)
	{
		ZTextCache* cache=(ZTextCache*)i.Data();
		bool delItr=false;

		ZTextCache* preCache=cache;
		while(cache)
		{
			if (cache->m_mark<deadline)
			{
				//释放
				ZTextCache* del=cache;
				if (cache==i.Data())
				{
					cache=cache->Next();
					if(cache)
						i.SetData(cache);
					else
					{
						ZHashMap::Iterator it=i;
						++i;
						m_textCaches.Remove(&it);
						delItr=true;
						delete del;
						break;
					}
					preCache=cache;
				}
				else
				{
					preCache->m_next=cache->Next();
					cache=preCache->m_next;
				}
				delete del;
			}
			else
			{
				preCache=cache;
				cache=cache->Next();
			}
		}

		if(!delItr)
			++i;
	}
	
}

void ZFont::FreeTextCache()
{
	for (ZHashMap::Iterator i=m_textCaches.Begin();i!=m_textCaches.End();++i)
	{
		ZTextCache* cache=(ZTextCache*)i.Data();
		while(cache)
		{
			ZTextCache* temp=cache; 
			cache=cache->Next();
			delete temp;
		}
	}
	m_textCaches.RemoveAll();
}


}//namespace