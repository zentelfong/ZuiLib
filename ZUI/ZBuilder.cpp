#include "ZBuilder.h"
#include "ZControl.h"
#include "ZPaintManager.h"
#include "ZControlFactory.h"
#include "ZContainer.h"

namespace ZuiLib
{
using namespace tinyxml2;


ZBuilder::ZBuilder()
{

}

bool ZBuilder::LoadXml(const char* xmlFile)
{
	XMLError error=m_xml.LoadFile(xmlFile);
	return (error==XML_SUCCESS);
}


ZBuilder::~ZBuilder(void)
{
}


ZControl* ZBuilder::Create(ZPaintManager* pManager, ZControl* pParent)
{
	XMLElement* root=m_xml.RootElement();
	if (root)
	{
		XMLNode* node=root->FirstChild();
		while(node)
		{
			XMLElement* elem= node->ToElement();
			if (elem)
			{
				if (strcmp(elem->Name(),"Font")==0)
				{
					if(pManager)
						pManager->GetRenerder()->GetFontMgr()->SetTextInfo(elem);
				}
				else if (strcmp(elem->Name(),"Default")==0)
				{
					const char* name=elem->Attribute("name");
					const char* text=elem->Attribute("value");
					if(!text)
						text=elem->Value();

					if(name && text)
						pManager->AddDefaultAttributeList(name,text);
				}

			}
			node=node->NextSibling();
		}
		if( strcmp(root->Name(),"Window") == 0 ) 
		{
			const char* value=root->Attribute("size");
			if (value)
			{
                char* pstr = NULL;
                int cx = strtol(value, &pstr, 10);  assert(pstr);    
                int cy = strtol(pstr + 1, &pstr, 10);  assert(pstr); 
                pManager->SetInitSize(cx, cy);
			}

			value=root->Attribute("sizebox");
			if (value)
			{
                RECT rcSizeBox = { 0 };
                char* pstr = NULL;
                rcSizeBox.left = strtol(value, &pstr, 10);  assert(pstr);    
                rcSizeBox.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
                rcSizeBox.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
                rcSizeBox.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
                pManager->SetSizeBox(rcSizeBox);
			}

			value=root->Attribute("caption");
			if (value)
			{
                RECT rcCaption = { 0 };
                char* pstr = NULL;
                rcCaption.left = strtol(value, &pstr, 10);  assert(pstr);    
                rcCaption.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
                rcCaption.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
                rcCaption.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
                pManager->SetCaptionRect(rcCaption);
			}

			value=root->Attribute("mininfo");
			if (value)
			{
                char* pstr = NULL;
                int cx = strtol(value, &pstr, 10);  assert(pstr);    
                int cy = strtol(pstr + 1, &pstr, 10);  assert(pstr); 
                pManager->SetMinInfo(cx, cy);
			}

			value=root->Attribute("maxinfo");
			if (value)
			{
                char* pstr = NULL;
                int cx = strtol(value, &pstr, 10);  assert(pstr);    
                int cy = strtol(pstr + 1, &pstr, 10);  assert(pstr); 
                pManager->SetMinInfo(cx, cy);
			}

			value=root->Attribute("disabledfontcolor");
			if (value)
			{
                if( *value == '#') value = CharNext(value);
                char* pstr = NULL;
                color_t clrColor = strtoul(value, &pstr, 16);
                pManager->SetDefaultDisabledColor(clrColor);
			}

			value=root->Attribute("defaultfontcolor");
			if (value)
			{
                if( *value == '#') value = CharNext(value);
                char* pstr = NULL;
                color_t clrColor = strtoul(value, &pstr, 16);
                pManager->SetDefaultFontColor(clrColor);
			}

			value=root->Attribute("linkfontcolor");
			if (value)
			{
                if( *value == '#') value = CharNext(value);
                char* pstr = NULL;
                color_t clrColor = strtoul(value, &pstr, 16);
                pManager->SetDefaultLinkFontColor(clrColor);
			}

			value=root->Attribute("linkhoverfontcolor");
			if (value)
			{
                if( *value == '#') value = CharNext(value);
                char* pstr = NULL;
                color_t clrColor = strtoul(value, &pstr, 16);
                pManager->SetDefaultLinkHoverFontColor(clrColor);
			}

			value=root->Attribute("selectedcolor");
			if (value)
			{
                if( *value == '#') value = CharNext(value);
                char* pstr = NULL;
                color_t clrColor = strtoul(value, &pstr, 16);
                pManager->SetDefaultSelectedBkColor(clrColor);
			}

		}
		
	}

	return _Parse(root, pParent, pManager);;
}





ZControl* ZBuilder::_Parse(tinyxml2::XMLElement* parent, ZControl* pParent, ZPaintManager* pManager)
{
	IContainerUI* pContainer = NULL;
	ZControl* pReturn=NULL;
	XMLNode* node=parent->FirstChild();
	while(node)
	{
		ZControl* pControl=NULL;
		XMLElement* elem= node->ToElement();
		if (elem)
		{
			if (strcmp(elem->Name(),"Include")==0)
			{
				const char* srcFile=elem->Attribute("source");
				ZBuilder builder;
				if (builder.LoadXml(srcFile))
				{
					pControl=builder.Create(pManager,pParent);
				}
			}
			else
			{
				pControl=ZControlFactoryMgr::Instance()->BuildControl(elem->Name());
			}

			if (!pControl)
			{
				printf("Error Create Control %s.\n",elem->Name());
				node=node->NextSibling();
				continue;
			}

			if (!elem->NoChildren())
			{
				_Parse(elem,pControl,pManager);
			}
			if( pParent != NULL ) 
			{
				if( pContainer == NULL ) 
					pContainer = static_cast<IContainerUI*>(pParent->GetInterface("IContainer"));
				assert(pContainer);
				if( pContainer == NULL ) return NULL;
				if( !pContainer->Add(pControl) ) {
					delete pControl;
					node=node->NextSibling();
					continue;
				}
			}
			
			// Init default attributes
			if( pManager ) {
				pControl->SetManager(pManager, NULL, false);
				const char* pDefaultAttributes = pManager->GetDefaultAttributeList(elem->Name());
				if( pDefaultAttributes ) {
					pControl->ApplyAttributeList(pDefaultAttributes);
				}
			}

			const XMLAttribute* attr=elem->FirstAttribute();
			// Process attributes
			while (attr)
			{
				pControl->SetAttribute(attr->Name(),attr->Value());

				attr=attr->Next();
			}

			// Return first item
			if( pReturn == NULL ) pReturn = pControl;
		}
		node=node->NextSibling();
	}

	return pReturn;
}










}

