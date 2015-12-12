#ifndef __ZUICONTROL_FACTORY_H__
#define __ZUICONTROL_FACTORY_H__

#include "ZUtils.h"

namespace ZuiLib
{

class ZControl;

class ZControlFactoryBase
{
public:
	virtual ~ZControlFactoryBase(){}
	virtual ZControl* CreateControl()=0;
};


class ZControlFactoryMgr
{
public:
	static ZControlFactoryMgr* Instance();

	ZControlFactoryMgr(void);
	~ZControlFactoryMgr(void);

	void AddFactory(const char* name,ZControlFactoryBase* factory);
	void RemoveFactory(const char* name);
	ZControl* BuildControl(const char* name);
private:
	static ZControlFactoryMgr* s_mgr;
	ZHashMap m_map;
};


#define IMPL_ZCONTROL_FACTORY(name,control) \
	class control##Factory:public ZControlFactoryBase\
	{\
	public:\
		control##Factory()\
		{\
			ZControlFactoryMgr::Instance()->AddFactory(name,this);\
		}\
		~control##Factory()\
		{\
			ZControlFactoryMgr::Instance()->RemoveFactory(name);\
		}\
		ZControl* CreateControl()\
		{\
			return new control;\
		}\
	};\
	control##Factory control##Factory__;\
	



}//namescpace



#endif