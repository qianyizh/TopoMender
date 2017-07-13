#include "TimeMeter.h"

TimeMeter::TimeMeter() {
	GetLocalTime(&starttime);
	GetLocalTime(&endtime);
}
void TimeMeter::Start()
{
	GetLocalTime(&starttime);
}

void TimeMeter::End()
{
	GetLocalTime(&endtime);
}

void TimeMeter::Print()
{
	int m1 = starttime.wMinute;
	int s1 = starttime.wSecond;
	int ms1 = starttime.wMilliseconds;
	int m2 = endtime.wMinute;
	int s2 = endtime.wSecond;
	int ms2 = endtime.wMilliseconds;	
	int timeCost = 60* 1000 * (m2 - m1) + 1000*(s2 - s1) + ms2 - ms1;
	printf("Time Cost is : %d msec\n",timeCost);
}
