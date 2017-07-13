#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// containing check functions
//////////////////////////////////////////////////////////////////////////
bool OcTree::CheckPointForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Point)
{
	if (nCoCell == -1)
		return false;
	int x, y, z, l = NLENGTH(nCoCell);
	INT64GETIJK(n64CoCell, x, y, z);
	int _x, _y, _z;
	INT64GETIJK(n64Point, _x, _y, _z);
	return ( X_I && Y_I && Z_I );
}

bool OcTree::CheckEdgeForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Edge, int nEdge, int dirEdge)
{
	if (nCoCell == -1)
		return false;
	int x, y, z, l = NLENGTH(nCoCell);
	INT64GETIJK(n64CoCell, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nEdge), _d = dirEdge;
	INT64GETIJK(n64Edge, _x, _y, _z);
	switch (_d) {
	case 0:
		return ( X_IE && Y_I && Z_I );
	case 1:
		return ( X_I && Y_IE && Z_I );
	case 2:
		return ( X_I && Y_I && Z_IE );
	}
	return false;
}

bool OcTree::CheckFaceForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Face, int nFace, int dirFace)
{
	if (nCoCell == -1)
		return false;
	int x, y, z, l = NLENGTH(nCoCell);
	INT64GETIJK(n64CoCell, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nFace), _d = dirFace;
	INT64GETIJK(n64Face, _x, _y, _z);
	switch (_d) {
	case 0:
		return ( X_I && Y_IE && Z_IE );
	case 1:
		return ( X_IE && Y_I && Z_IE );
	case 2:
		return ( X_IE && Y_IE && Z_I );
	}
	return false;
}

bool OcTree::CheckCellForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Cell, int nCell)
{
	if (nCoCell == -1) {
		return (nCell == -1);
	}
	int x, y, z, l = NLENGTH(nCoCell);
	INT64GETIJK(n64CoCell, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nCell);
	INT64GETIJK(n64Cell, _x, _y, _z);

	return ( X_IT && Y_IT && Z_IT );
}

bool OcTree::CheckPointForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Point)
{
	int x, y, z, l = NLENGTH(nCoFace), d = dirCoFace;
	INT64GETIJK(n64CoFace, x, y, z);
	int _x, _y, _z;
	INT64GETIJK(n64Point, _x, _y, _z);
	switch (d) {
	case 0:
		return ( X_E && Y_I && Z_I );
	case 1:
		return ( X_I && Y_E && Z_I );
	case 2:
		return ( X_I && Y_I && Z_E );
	}
	return false;
}

bool OcTree::CheckEdgeForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Edge, int nEdge, int dirEdge)
{
	int x, y, z, l = NLENGTH(nCoFace), d = dirCoFace;
	INT64GETIJK(n64CoFace, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nEdge), _d = dirEdge;
	INT64GETIJK(n64Edge, _x, _y, _z);
	switch (d) {
	case 0:
		switch (_d) {
	case 0:
		return ( X_IET && Y_I && Z_I );
	case 1:
		return ( X_E && Y_IE && Z_I );
	case 2:
		return ( X_E && Y_I && Z_IE );
		}
	case 1:
		switch (_d) {
	case 0:
		return ( X_IE && Y_E && Z_I );
	case 1:
		return ( X_I && Y_IET && Z_I );
	case 2:
		return ( X_I && Y_E && Z_IE );
		}
	case 2:
		switch (_d) {
	case 0:
		return ( X_IE && Y_I && Z_E );
	case 1:
		return ( X_I && Y_IE && Z_E );
	case 2:
		return ( X_I && Y_I && Z_IET );
		}
	}
	return false;
}

bool OcTree::CheckFaceForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Face, int nFace, int dirFace)
{
	int x, y, z, l = NLENGTH(nCoFace), d = dirCoFace;
	INT64GETIJK(n64CoFace, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nFace), _d = dirFace;
	INT64GETIJK(n64Face, _x, _y, _z);
	switch (d) {
	case 0:
		switch (_d) {
	case 0:
		return ( X_E && Y_IE && Z_IE );
	case 1:
		return ( X_IET && Y_I && Z_IE );
	case 2:
		return ( X_IET && Y_IE && Z_I );
		}
	case 1:
		switch (_d) {
	case 0:
		return ( X_I && Y_IET && Z_IE );
	case 1:
		return ( X_IE && Y_E && Z_IE );
	case 2:
		return ( X_IE && Y_IET && Z_I );
		}
	case 2:
		switch (_d) {
	case 0:
		return ( X_I && Y_IE && Z_IET );
	case 1:
		return ( X_IE && Y_I && Z_IET );
	case 2:
		return ( X_IE && Y_IE && Z_E );
		}
	}
	return false;
}

bool OcTree::CheckCellForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Cell, int nCell)
{
	int x, y, z, l = NLENGTH(nCoFace), d = dirCoFace;
	INT64GETIJK(n64CoFace, x, y, z);

	if (nCell == -1) {
		switch (d) {
		case 0:
			return X_INF;
		case 1:
			return Y_INF;
		case 2:
			return Z_INF;
		}
	}

	int _x, _y, _z, _l = NLENGTH(nCell);
	INT64GETIJK(n64Cell, _x, _y, _z);

	switch (d) {
	case 0:
		return ( X_IET && Y_IT && Z_IT );
	case 1:
		return ( X_IT && Y_IET && Z_IT );
	case 2:
		return ( X_IT && Y_IT && Z_IET );
	}

	return false;
}

bool OcTree::CheckPointForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Point)
{
	int x, y, z, l = NLENGTH(nCoEdge), d = dirCoEdge;
	INT64GETIJK(n64CoEdge, x, y, z);
	int _x, _y, _z;
	INT64GETIJK(n64Point, _x, _y, _z);
	switch (d) {
	case 0:
		return ( X_I && Y_E && Z_E );
	case 1:
		return ( X_E && Y_I && Z_E );
	case 2:
		return ( X_E && Y_E && Z_I );
	}
	return false;
}

bool OcTree::CheckEdgeForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Edge, int nEdge, int dirEdge)
{
	int x, y, z, l = NLENGTH(nCoEdge), d = dirCoEdge;
	INT64GETIJK(n64CoEdge, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nEdge), _d = dirEdge;
	INT64GETIJK(n64Edge, _x, _y, _z);
	switch (d) {
	case 0:
		switch (_d) {
	case 0:
		return ( X_IE && Y_E && Z_E );
	case 1:
		return ( X_I && Y_IET && Z_E );
	case 2:
		return ( X_I && Y_E && Z_IET );
		}
	case 1:
		switch (_d) {
	case 0:
		return ( X_IET && Y_I && Z_E );
	case 1:
		return ( X_E && Y_IE && Z_E );
	case 2:
		return ( X_E && Y_I && Z_IET );
		}
	case 2:
		switch (_d) {
	case 0:
		return ( X_IET && Y_E && Z_I );
	case 1:
		return ( X_E && Y_IET && Z_I );
	case 2:
		return ( X_E && Y_E && Z_IE );
		}
	}
	return false;
}

bool OcTree::CheckFaceForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Face, int nFace, int dirFace)
{
	int x, y, z, l = NLENGTH(nCoEdge), d = dirCoEdge;
	INT64GETIJK(n64CoEdge, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nFace), _d = dirFace;
	INT64GETIJK(n64Face, _x, _y, _z);
	switch (d) {
	case 0:
		switch (_d) {
	case 0:
		return ( X_I && Y_IET && Z_IET );
	case 1:
		return ( X_IT && Y_E && Z_IET );
	case 2:
		return ( X_IT && Y_IET && Z_E );
		}
	case 1:
		switch (_d) {
	case 0:
		return ( X_E && Y_IT && Z_IET );
	case 1:
		return ( X_IET && Y_I && Z_IET );
	case 2:
		return ( X_IET && Y_IT && Z_E );
		}
	case 2:
		switch (_d) {
	case 0:
		return ( X_E && Y_IET && Z_IT );
	case 1:
		return ( X_IET && Y_E && Z_IT );
	case 2:
		return ( X_IET && Y_IET && Z_I );
		}
	}
	return false;
}

bool OcTree::CheckCellForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Cell, int nCell)
{
	int x, y, z, l = NLENGTH(nCoEdge), d = dirCoEdge;
	INT64GETIJK(n64CoEdge, x, y, z);
	if (nCell == -1) {
		switch (d) {
		case 0:
			return (Y_INF || Z_INF);
		case 1:
			return (X_INF || Z_INF);
		case 2:
			return (X_INF || Y_INF);
		}
	}
	int _x, _y, _z, _l = NLENGTH(nCell);
	INT64GETIJK(n64Cell, _x, _y, _z);
	switch (d) {
	case 0:
		return ( X_IT && Y_IET && Z_IET );
	case 1:
		return ( X_IET && Y_IT && Z_IET );
	case 2:
		return ( X_IET && Y_IET && Z_IT );
	}
	return false;
}

bool OcTree::CheckPointForPoint(UINT64 n64CoPoint, UINT64 n64Point)
{
	int x, y, z;
	INT64GETIJK(n64CoPoint, x, y, z);
	int _x, _y, _z;
	INT64GETIJK(n64Point, _x, _y, _z);

	return (X_E && Y_E && Z_E);
}

bool OcTree::CheckEdgeForPoint(UINT64 n64CoPoint, UINT64 n64Edge, int nEdge, int dirEdge)
{
	int x, y, z;
	INT64GETIJK(n64CoPoint, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nEdge), _d = dirEdge;
	INT64GETIJK(n64Edge, _x, _y, _z);
	switch (_d) {
	case 0:
		return ( X_IET && Y_E && Z_E );
	case 1:
		return ( X_E && Y_IET && Z_E );
	case 2:
		return ( X_E && Y_E && Z_IET );
	}
	return false;
}

bool OcTree::CheckFaceForPoint(UINT64 n64CoPoint, UINT64 n64Face, int nFace, int dirFace)
{
	int x, y, z;
	INT64GETIJK(n64CoPoint, x, y, z);
	int _x, _y, _z, _l = NLENGTH(nFace), _d = dirFace;
	INT64GETIJK(n64Face, _x, _y, _z);
	switch (_d) {
	case 0:
		return ( X_E && Y_IET && Z_IET );
	case 1:
		return ( X_IET && Y_E && Z_IET );
	case 2:
		return ( X_IET && Y_IET && Z_E );
	}

	return false;
}

bool OcTree::CheckCellForPoint(UINT64 n64CoPoint, UINT64 n64Cell, int nCell)
{
	int x, y, z;
	INT64GETIJK(n64CoPoint, x, y, z);

	if (nCell == -1) {
		return ( X_INF || Y_INF || Z_INF );
	}

	int _x, _y, _z, _l = NLENGTH(nCell);
	INT64GETIJK(n64Cell, _x, _y, _z);

	return ( X_IET && Y_IET && Z_IET );
}
