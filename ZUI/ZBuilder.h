#ifndef __ZUIBUILDER_H__
#define __ZUIBUILDER_H__

#include "tinyxml2/tinyxml2.h"

namespace ZuiLib
{
class ZPaintManager;
class ZControl;

class ZBuilder
{
public:
	ZBuilder();
	~ZBuilder(void);

	bool LoadXml(const char* xmlFile);
	ZControl* Create(ZPaintManager* pManager, ZControl* pParent = NULL);

private:
	ZControl* _Parse(tinyxml2::XMLElement* parent, ZControl* pParent, ZPaintManager* pManager);
	tinyxml2::XMLDocument m_xml;
};


}//namespace

#endif