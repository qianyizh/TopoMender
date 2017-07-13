#ifndef _TIMEMETER
#define _TIMEMETER

#include "resources.h"
using namespace std;

class TimeMeter
{
public:
	SYSTEMTIME starttime;
	SYSTEMTIME endtime;
public:
	TimeMeter();
	~TimeMeter(){}
	void Start();
	void End();
	void Print();
};

#endif