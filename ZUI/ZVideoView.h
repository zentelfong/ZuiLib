#ifndef __ZUIVIDEO_VIEW_H__
#define __ZUIVIDEO_VIEW_H__
#include "ZCommon.h"

namespace ZuiLib
{

class ZVideoViewImpl;

class ZVideoView
{
public:
	ZVideoView(void);
	~ZVideoView(void);




private:
	ZVideoViewImpl* m_impl;
};










}






#endif