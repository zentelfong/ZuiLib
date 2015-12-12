#ifndef __ZUIFONT_H__
#define __ZUIFONT_H__

#include "ZCommon.h"
#include "ZUtils.h"
#include "ZString.h"
#include "base/time.h"
#include <map>

namespace ZuiLib
{
	class ZFont;
	class ZRenerder;

	struct TextStyle 
	{
		bool underline;
		bool bold;
		bool italic;
		color_t color;
	};

	//文字绘图缓存
	class ZTextCache
	{
	public:
		ZTextCache();
		~ZTextCache();

		bool Init(ZFont* font,ZRenerder* ren,const char* text,const TextStyle* style);
		ZTextCache* Next(){return m_next;}
		void Mark();//标记


		TextStyle* GetStyle(){return &m_style;}
		ZRenerder* GetRenerder(){return m_ren;}
		SDL_Texture* GetTexture(){return m_cacheTexture;}
	private:
		friend class ZFont;
		uint32_t m_mark;//保存上次绘图时间
		TextStyle m_style;
		ZTextCache* m_next;
		SDL_Texture* m_cacheTexture;
		ZRenerder* m_ren;
		static int s_count;
	};


	class ZFont:public RefCountedBase
	{
	public:
		enum FontStyle
		{
			FS_NORMAL=TTF_STYLE_NORMAL,
			FS_BOLD=TTF_STYLE_BOLD,
			FS_ITALIC=TTF_STYLE_ITALIC,
			FS_UNDERLINE=TTF_STYLE_UNDERLINE,
			FS_STRIKETHROUGH=TTF_STYLE_STRIKETHROUGH
		};

		enum FontHinting{
			FH_NORMAL=TTF_HINTING_NORMAL,
			FH_LIGHT=TTF_HINTING_LIGHT,
			FH_MONO=TTF_HINTING_MONO,
			FH_NONE=TTF_HINTING_NONE
		};



		ZFont(void);
		~ZFont(void);

		static RefCountedPtr<ZFont> Create()
		{
			return new RefCountedObject<ZFont>();
		}

		bool OpenFont(const char* file,int height);
		bool OpenFont(const char* file,int height,long index);


		int GetStyle();
		void SetStyle(int style);

		int GetOutline();
		void SetOutline(int outline);

		FontHinting GetHinting();
		void SetHinting(FontHinting hinting);

		int Height();

		int Ascent();
		int FontDescent();
		int LineSkip();
		int GetKerning();
		void SetKerning(int allowed);
		int FaceNumber();

		bool TextSize(const char* text,ZSize &sz);//utf8
		bool TextSize(const uint16_t* text,ZSize &sz);//unicode


		ZTextCache* GetTextCache(ZRenerder* ren,const char* text,const TextStyle* style);//查找文本缓存
		void CollectTextCache(uint32_t life);//收集文字缓存
		void FreeTextCache();//释放文字缓存

		bool CloseFont();

		TTF_Font* GetTTF(){return m_font;}
	private:
		friend class ZRenerder;
		TTF_Font* m_font;
		ZHashMap m_textCaches;
	};


	typedef RefCountedPtr<ZFont> ZFontPtr;


	//字体信息
	struct ZFontInfo
	{
		ZString name;
		int size;
		ZFontPtr font;
	};

	//文本信息
	struct ZTextInfo
	{
		ZTextInfo()
			:size(13),underline(false),bold(false),italic(false)
		{

		}

		ZTextInfo(const ZTextInfo& other)
			:name(other.name),size(other.size),underline(other.underline),bold(other.bold),italic(other.italic)
		{

		}

		ZTextInfo& operator=(const ZTextInfo& other)
		{
			name=other.name;
			size=other.size;
			underline=other.underline;
			bold=other.bold;
			italic=other.italic;
			return *this;
		}

		ZString name;
		int size;
		bool underline;
		bool bold;
		bool italic;
	};



	//字体管理器
	class ZFontMgr
	{
	public:
		ZFontMgr();
		~ZFontMgr();

		//获取字体，直接返回已加载的字体，如果没有加载尝试加载
		ZFontPtr GetFont(const char* name,int size);

		//加载字体
		bool LoadFont(ZFontInfo* font);

		//设置文本信息
		bool SetTextInfo(const tinyxml2::XMLElement *elem);

		//获取文本信息
		ZTextInfo* GetTextInfo(const char* name);

		//默认字体
		static ZTextInfo* GetDefaultTextInfo(){return &s_defaultFont;}
		static void SetDefaultTextInfo(ZTextInfo* info){s_defaultFont=*info;}

		void CollectTextCache(uint32_t life);
	private:
		typedef std::vector<ZFontInfo> container_t;
		typedef std::map<ZString,ZTextInfo> map_t;

		static ZTextInfo s_defaultFont;//默认字体
		container_t m_fonts;//加载的字体
		map_t m_textInfos; //保存字体信息，以name为key保持
	};












}//namespace




#endif