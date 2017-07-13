#include "StdAfx.h"
#include ".\timemeter.h"

CTimeMeter::CTimeMeter(void)
{
}

CTimeMeter::~CTimeMeter(void)
{
}

void CTimeMeter::Start()
{
	m_dwStart = ::GetTickCount();
}

void CTimeMeter::End()
{
	m_dwEnd = ::GetTickCount();
}

void CTimeMeter::Print()
{
	printf("%d.%d seconds", (m_dwEnd - m_dwStart) / 1000, (m_dwEnd - m_dwStart) % 1000);
}
