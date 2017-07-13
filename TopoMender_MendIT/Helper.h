#ifndef _HELPER
#define _HELPER

//////////////////////////////////////////////////////////////////////////
// helper structures
//////////////////////////////////////////////////////////////////////////

typedef struct _INTER_NODE {
	UCHAR m_cType;
	UCHAR * m_pNode[8];
} InterNode;

typedef struct _LEAF_NODE {
	UINT m_nType;
	UINT m_nVertex;
} LeafNode;

typedef struct _ELEMENT {
	UCHAR m_bObject		: 1;
	UCHAR m_bSkeleton	: 1;
	UCHAR m_bSketch		: 1;
	UCHAR m_cSimpleTag	: 2;
	UCHAR m_cPointer	: 3;	// c: 0-5 f;  f: 0-1 c, 2-5 e; e: 0-3 f, 4-5 p; p: 0-5 e
} Element;

typedef struct _NODEHEADER {
	UCHAR m_cType		: 2;
	UCHAR m_bLastFix	: 1;
	UCHAR m_bLastTime	: 1;
	UCHAR m_bChanged	: 1;
} NodeHeader;

//////////////////////////////////////////////////////////////////////////
// helper macros
//////////////////////////////////////////////////////////////////////////

// switch macros
#define CONSOLE_OUTPUT
// #undef CONSOLE_OUTPUT

#define CONSOLE_DEBUG
// #undef CONSOLE_DEBUG

#define CLEAN_SKELETON
// #undef  CLEAN_SKELETON

#define SIMPLE_SKETCHING
#undef	SIMPLE_SKETCHING

#define BETTER_APPEARANCE
// #undef	BETTER_APPEARANCE

#define SKETCHING_MERGE
// #undef	SKETCHING_MERGE

#define LOGFILE_OUTPUT
// #undef	LOGFILE_OUTPUT

#define OCTREE_LEVEL_RESTRICT
#undef	OCTREE_LEVEL_RESTRICT

#define BETTER_GENERATING_SET NLENGTH(m_nMaxLevel + 3)
#undef	BETTER_GENERATING_SET

// position macros
#define INT64_INVALID 0xffffffffffffffff
#define UPPER_LIMIT 0x40000000000
#define MAX_LEVEL 19
#define NLENGTH(i) (1 << (MAX_LEVEL - (i)))
#define THIN_ELEMENT_WIDTH ((UINT64)NLENGTH(m_nMaxLevel + 1))
#define THICK_ELEMENT_WIDTH(i) ((UINT64)NLENGTH(i) - THIN_ELEMENT_WIDTH)
#define INFINITE_ELEMENT_VOLUME ((UINT64)1 << 60)
#define INT64TRANS(i, j, k) (((UINT64)(i) << 40) | ((UINT64)(j) << 20) | ((UINT64)(k)))
#define INT64GETI(n) (((n) >> 40) & (0xfffff))
#define INT64GETJ(n) (((n) >> 20) & (0xfffff))
#define INT64GETK(n) ((n) & (0xfffff))
#define INT64GETIJK(n, x, y, z) (x = (int)INT64GETI(n), y = (int)INT64GETJ(n), z = (int)INT64GETK(n))
#define INT64TRANSD(i, j, k, d) (((UINT64)(d) << 60) | ((UINT64)(i) << 40) | ((UINT64)(j) << 20) | ((UINT64)(k)))
#define INT64COMBD(n, d) (((UINT64)(d) << 60) | n)
#define INT64GETD(n) ((int)(((n) >> 60) & (0xf)))
#define INT64GETN(n) ((n) & (0xfffffffffffffff))
#define NODE_IN_NODE(n, l, m) (INT64GETI(m) >= INT64GETI(n) && INT64GETI(m) < INT64GETI(n) + NLENGTH(l) && INT64GETJ(m) >= INT64GETJ(n) && INT64GETJ(m) < INT64GETJ(n) + NLENGTH(l) && INT64GETK(m) >= INT64GETK(n) && INT64GETK(m) < INT64GETK(n) + NLENGTH(l))
#define SKELETON_WIDTH_DOUBLE 0.5

// position transfer macros
#define POSITION_TRANS_X(n) (0.5 * m_dbNodeLength * (INT64GETI(n)) + m_vCorner.pVec[0])
#define POSITION_TRANS_Y(n) (0.5 * m_dbNodeLength * (INT64GETJ(n)) + m_vCorner.pVec[1])
#define POSITION_TRANS_Z(n) (0.5 * m_dbNodeLength * (INT64GETK(n)) + m_vCorner.pVec[2])
#define POSITION_TRANS(n) (CVector3D(POSITION_TRANS_X(n), POSITION_TRANS_Y(n), POSITION_TRANS_Z(n)))

// cell flag macros
#define GET_CHANGED(p) ((*(p)) & 0x10)
#define GET_LAST_TIME(p) ((*(p)) & 0x8)
#define GET_LAST_FIX(p) ((*(p)) & 0x4)
#define NOT_LEAF(p) ((*(p) & 0x3) == 0)
#define NOT_INFINITE(p) ((*(p) & 0x3) != 3)

// storage flag macros
#define GET_STORAGE_TAG(n, i)  (((n) >> ((i) + 5)) & 1)
#define SET_STORAGE_TAG(n, i)  ((n) |= (0x1 << ((i) + 5)))
#define UNSET_STORAGE_TAG(n, i) ((n) &= ~(0x1 << ((i) + 5)))
#define GET_POINT_INSIDE(n, i)  ((((n) >> (i)) & 1) == 0)

// contouring inside flag macros
#define GET_INSIDE_TAG(n, i)  (((n) >> (i)) & 1)
#define SET_INSIDE_TAG(n, i)  ((n) |= (0x1 << (i)))
#define IS_CELL_IRREGULAR(n)  ((n) & 0x80000000)

// containing macros
#define X_E		(_x == x)
#define X_I		(_x > x && _x < x + l)
#define X_IE	(_x >= x && _x < x + l)
#define X_ET	(_x == x || _x + _l == x)
#define X_IET	(x >= _x && x <= _x + _l)
#define X_IT	(_x < x + l && x < _x + _l)
#define X_INF	(x == 0 || x == NLENGTH(0))
#define Y_E		(_y == y)
#define Y_I		(_y > y && _y < y + l)
#define Y_IE	(_y >= y && _y < y + l)
#define Y_ET	(_y == y || _y + _l == y)
#define Y_IET	(y >= _y && y <= _y + _l)
#define Y_IT	(_y < y + l && y < _y + _l)
#define Y_INF	(y == 0 || y == NLENGTH(0))
#define Z_E		(_z == z)
#define Z_I		(_z > z && _z < z + l)
#define Z_IE	(_z >= z && _z < z + l)
#define Z_ET	(_z == z || _z + _l == z)
#define Z_IET	(z >= _z && z <= _z + _l)
#define Z_IT	(_z < z + l && z < _z + _l)
#define Z_INF	(z == 0 || z == NLENGTH(0))

#endif