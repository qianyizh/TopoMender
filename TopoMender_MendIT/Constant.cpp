#include "Constant.h"

#include "Regular.h"
#include "RegCons.h"
#include "HashSize.h"

void Constant::Initialize()
{
	LoadRegularTable();
	LoadRegularTableEx();
	LoadHashSize();
}

void Constant::LoadRegularTable()
{
	for (int i = 0; i < 256; i++)
		m_pRegularTable[i] = regular[i];
}

void Constant::LoadRegularTableEx()
{
	for (int i = 0; i < 256; i++)
		for(int j = 0; j < 3; j++)
			m_pRegularTableEx[i][j] = regcons[i][j];
}

void Constant::LoadHashSize()
{
	for (int i = 0; i < 40; i++)
		m_pHashSize[i] = hashsize[i];
}

const int Constant::I_SHIFT_FP[6][4] = {
	{0, 1, 2, 3},
	{4, 5, 6, 7},
	{0, 4, 1, 5},
	{2, 6, 3, 7},
	{0, 2, 4, 6},
	{1, 3, 5, 7}
};

const int Constant::I_SHIFT_EP[12][2] = {
	{0, 4}, {1, 5}, {2, 6}, {3, 7},
	{0, 2}, {4, 6}, {1, 3}, {5, 7},
	{0, 1}, {2, 3}, {4, 5}, {6, 7}
};

const int Constant::II_SHIFT_CF[3][2] = {
	{1, 0}, {3, 2}, {5, 4}
};

const int Constant::II_SHIFT_CE[3][2][2] = {
	{{3, 2}, {1, 0}},
	{{7, 6}, {5, 4}},
	{{11, 10}, {9, 8}}
};

const int Constant::II_SHIFT_CP[2][2][2] = {
	{{7, 6}, {5, 4}},
	{{3, 2}, {1, 0}}
};

const int Constant::III_SHIFT_CEP[3][2][2][2] = {
	{{{3, 7}, {2, 6}}, {{1, 5}, {0, 4}}},
	{{{5, 7}, {1, 3}}, {{4, 6}, {0, 2}}},
	{{{6, 7}, {4, 5}}, {{2, 3}, {0, 1}}}
};

const float Constant::FLOAT_INFINITE = 1e20f;