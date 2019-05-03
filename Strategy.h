/********************************************************
*	Strategy.h : 策略接口文件                           *
*	张永锋                                              *
*	zhangyf07@gmail.com                                 *
*	2010.8                                              *
*********************************************************/

#ifndef STRATEGY_H_
#define	STRATEGY_H_
#ifndef BUILD_DRIVER
#define _EXPORT extern "C" __declspec(dllexport)
#else
#define _EXPORT
#endif

#include "Point.h"

_EXPORT Point* getPoint(const int M, const int N, const int* top, const int* _board,
	const int lastX, const int lastY, const int noX, const int noY);

_EXPORT void clearPoint(Point* p);



#endif