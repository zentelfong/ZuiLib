#include "ZControlFactory.h"
#include <assert.h>

namespace ZuiLib
{

ZControlFactoryMgr* ZControlFactoryMgr::s_mgr=NULL;

ZControlFactoryMgr* ZControlFactoryMgr::Instance()
{
	if (!s_mgr)
	{
		s_mgr=new ZControlFactoryMgr;
	}
	return s_mgr;
}


ZControlFactoryMgr::ZControlFactoryMgr(void)
{
}


ZControlFactoryMgr::~ZControlFactoryMgr(void)
{
}


void ZControlFactoryMgr::AddFactory(const char* name,ZControlFactoryBase* factory)
{
	assert(m_map.Find(name)==NULL);
	m_map.Set(name,factory);
}

void ZControlFactoryMgr::RemoveFactory(const char* name)
{
	m_map.Remove(name);
}

ZControl* ZControlFactoryMgr::BuildControl(const char* name)
{
	ZControlFactoryBase* factory=(ZControlFactoryBase*)m_map.Find(name);
	if (factory)
		return factory->CreateControl();
	else
		return NULL;
}














}
