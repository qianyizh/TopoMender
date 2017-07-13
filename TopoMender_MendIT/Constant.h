#ifndef _CONSTANT
#define _CONSTANT

#include "resources.h"
using namespace std;

//extern ofstream fout;

class Constant {
public:
	int m_pRegularTable[256];
	int m_pHashSize[40];
	int m_pRegularTableEx[256][3];
	const static int I_SHIFT_FP[6][4];
	const static int I_SHIFT_EP[12][2];
	const static int II_SHIFT_CF[3][2];
	const static int II_SHIFT_CE[3][2][2];
	const static int II_SHIFT_CP[2][2][2];
	const static int III_SHIFT_CEP[3][2][2][2];
	const static float FLOAT_INFINITE;
public:
	Constant(){}
	~Constant(){}
public:
	void Initialize();
	void LoadRegularTable();
	void LoadRegularTableEx();
	void LoadHashSize();
};

#endif