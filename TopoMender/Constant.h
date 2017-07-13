#pragma once

class CConstant
{
public:
	CConstant(void);
	~CConstant(void);

	const static int I_SHIFT_FP[6][4];
	const static int I_SHIFT_EP[12][2];
	const static int II_SHIFT_CF[3][2];
	const static int II_SHIFT_CE[3][2][2];
	const static int II_SHIFT_CP[2][2][2];
	const static int III_SHIFT_CEP[3][2][2][2];

	const static int OBJECT_NUMBER[4];
	const static int OBJECT_START_POSITION[4];

	const static float FLOAT_INFINITE;

public:
	int m_pRegularTable[256];
	int m_pHashSize[40];
	int m_pRegularTableEx[256][3];

public:
	bool LoadRegularTable(CString strFileName);
	bool LoadRegularTableEx(CString strFileName);
	bool LoadHashSize(CString strFileName);
};
