#include "stdafx.h"
#include ".\constant.h"

CConstant::CConstant(void)
{
}

CConstant::~CConstant(void)
{
}

bool CConstant::LoadRegularTable(CString strFileName)
{
	FILE * pFile = fopen(strFileName.GetBuffer(0), "r");
	if (pFile == NULL)
		return false;

	for (int i = 0; i < 256; i++)
		fscanf(pFile, "%d", &m_pRegularTable[i]);

	fclose(pFile);
	return true;
}

bool CConstant::LoadRegularTableEx(CString strFileName)
{
	FILE * pFile = fopen(strFileName.GetBuffer(0), "r");
	if (pFile == NULL)
		return false;

	for (int i = 0; i < 256; i++)
		fscanf(pFile, "%d %d %d", &m_pRegularTableEx[i][0], &m_pRegularTableEx[i][1], &m_pRegularTableEx[i][2]);

	fclose(pFile);
	return true;
}

bool CConstant::LoadHashSize(CString strFileName)
{
	FILE * pFile = fopen(strFileName.GetBuffer(0), "r");
	if (pFile == NULL)
		return false;

	for (int i = 0; i < 40; i++)
		fscanf(pFile, "%d", &m_pHashSize[i]);

	fclose(pFile);
	return true;
}

const int CConstant::I_SHIFT_FP[6][4] = {
	{0, 1, 2, 3},
	{4, 5, 6, 7},
	{0, 4, 1, 5},
	{2, 6, 3, 7},
	{0, 2, 4, 6},
	{1, 3, 5, 7}
};

const int CConstant::I_SHIFT_EP[12][2] = {
	{0, 4}, {1, 5}, {2, 6}, {3, 7},
	{0, 2}, {4, 6}, {1, 3}, {5, 7},
	{0, 1}, {2, 3}, {4, 5}, {6, 7}
};

const int CConstant::II_SHIFT_CF[3][2] = {
	{1, 0}, {3, 2}, {5, 4}
};

const int CConstant::II_SHIFT_CE[3][2][2] = {
	{{3, 2}, {1, 0}},
	{{7, 6}, {5, 4}},
	{{11, 10}, {9, 8}}
};

const int CConstant::II_SHIFT_CP[2][2][2] = {
	{{7, 6}, {5, 4}},
	{{3, 2}, {1, 0}}
};

const int CConstant::III_SHIFT_CEP[3][2][2][2] = {
	{{{3, 7}, {2, 6}}, {{1, 5}, {0, 4}}},
	{{{5, 7}, {1, 3}}, {{4, 6}, {0, 2}}},
	{{{6, 7}, {4, 5}}, {{2, 3}, {0, 1}}}
};

const int CConstant::OBJECT_NUMBER[4] = {
	8, 12, 6, 1
};

const int CConstant::OBJECT_START_POSITION[4] = {
	0, 8, 20, 26
};

const float CConstant::FLOAT_INFINITE = 1e20f;
