========================================================================
    OVERVIEW : TopoMender Project Overview
========================================================================

This project is aiming for Volume Data Topology Repair, based on "Skeletion-based 
Topology Repair of Solid Models" submitted to SIGGRAPH 2006.

Implemented By :
	Qian-Yi Zhou (Tsinghua Univ.)
	
Current Version :
	Version 1.0, based on SimpleHuma, last modify -- 9, Feb, 2006
	
Sections In This File :
	HOWTOUSE : Instructs how to use this software
	FILELIST : Descriptions For Important Files
	DATASTRUCTURE : Important Data Structures Of The Project
	PROCEDURES : Important Procedures And Functions
	PIPELINE : Shows the pipeline that would help understanding

Special Thanks :
	Tao Ju : introduced the most important ideas, and give lots of help at implementation.
		Also, his program PolyMender is a necessary pre-processing of this project.
	Shi-Min Hu : the manager of our group, gave lots of instructions.


========================================================================
	HOWTOUSE : Instructs how to use this software
========================================================================

The software is a windows console application run on Windows 2000/XP with SP2 (or higher).
Three TXT files (e.g. HashSize.txt, RegCons.txt, regular.txt) must be placed in the same
folder with the EXE file.
The command line is as following :

Usage :			TopoMender [-skt] SOGFile ThinThresh GrowThresh ModelFile
Example :		TopoMender eight.sog 1.0 1.0 eight.ply

SOGFile :		Input file, must be of SOG file format
ModelFile :		Output file, could be of either OBJ or PLY file format
ThinThresh :	A threshold used in thinning process, handles thinner than it would be broken
GrowThresh :	A threshold used in growing process, handles narrower than it would be filled

-skt :			Indicates a corresponding SKT file would be generated


========================================================================
    FILELIST : Descriptions For Important Files
========================================================================

TopoMender.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

TopoMender.cpp
    This is the main application source file.

AssemblyInfo.cpp
	Contains custom attributes for modifying assembly metadata.

OcTree.cpp
	Core algorithm file.
	It contains definitions of the most important Data Structures, which are explained in
	Section DATASTRUCTURE. Especially those used for OcTree creation/destruction/relative
	algorithm.
	Also, it contains main algorithm functions such as : thinning, MST creation, patch
	growing, and their dual functions used for dual application. Further more, the file
	contains OcTree creation functions either from a SOG file or another rough OcTree, and
	also a contouring function used for generating triangular meshes from the final result.
	The pipeline using the functions of OcTree.cpp to achieve Topology Repair could be
	found in TopoMender.cpp. And the SOG file could be acquired by using the latest version
	of PolyMender[A software made by Tao Ju and could be downloaded from Tao Ju's Homepage]
	on a certain mesh file to repair.
	
MemoryManager.cpp
	This is an assistant file for memory management. It contains the definition of 
	CMemoryManager which tries to allocate 1MB sized memory blocks in order to save both
	time and space for managerment of the main Data Structure. The CMemoryManager supports
	random size Data Structure.
	
Constant.cpp
	Contains a CConstant class which includes constants used in the implementation either
	loaded from outer files or encoded in implementation.

Vector3D.cpp
	Contains a CVector3D class rewrited from Vector3D class in Huma project[implemented by
	Xupin Zhu(?)].

HashMap.cpp
	Contains a double-indexed hash map data structure used for vertex index sorting in 
	Contouring algorithm.


========================================================================
	DATASTRUCTURE : Important Data Structures Of The Project
========================================================================

Since the data structure of most class are clear and easy to read from the .h files, so
here we will only see some data structures which are encoded in very tight form.

Position Of Point/Edge/Face/Cell
	The position is encoded in an unsigned 64 bits integer(UINT64), as :
		0                   20                  40                  60        64(End)
		-----------------------------------------------------------------------
		|         Z         |         Y         |         X         | Dir | R |
		-----------------------------------------------------------------------
		         20                  20                  20            3    1  
	Here R is short for reserved flag.
	Dir is short for direction, which indicates an edge's direction is ALONG x-axis(0)/
	y-axis(1)/z-axis(2) or a face's direction is ORTHOGONAL to x-axis(0)/y-axis(1)/
	z-axis(2).
	X, Y, Z are the three minimum coordinates of all the points on the object (Say, the
	point of the point, or the left-down-near-point of the cell or similar points of the
	edge or face). The original	point is the left-down-near-point of the root-cell, 
	while the unit of length is	2^(-20) * length of the root-cell.
	
Encoding Of An OcTree Node
	An OcTree Node stores all the information of a cell -- including either the
	children-node pointers for an intermediate cell or the flags and retracing
	information used in algorithm of all the points/edges/faces/cells belonging to this
	cell if the cell is a leaf cell.
	The first two bits of the node denote the type of the cell, which might be :
		0 -- an intermediate node,
		1 -- an empty node,
		2 -- a leaf node.
		3 -- an outside node.
	The later two types are all leaf nodes and they share a same data structure as after
	initilization they are equally treated in the algorithm.
	The structure of an INTERMEDIATE node is :
		1 byte which is 0 in the lowest 2 bits indicate this is an intermediate node, while
		in the 3rd bit, a m_bLastTime flag (details see description of L flag in leaf node).
		8 pointers pointing to 8 children nodes of this node.
	The structure of a LEAF node is :
		2 UINT64 record all information we use in the main algorithm,
		1 UINT records the vertex index during contouring process, should be 0xffffffff if
			the cell is not REGULAR.
		0 ~ 6 RETRACE_EDGE structure (each includs 1 float type weight and a UINT64 type
			position). The number of such structures is determined in the first 2 UINT64
			tags, by calling InitFaceTags() function and then recreate the OcTree from
			original OcTree using LoadFromOcTree() function.
	The first 2 UINT64 tags are encoded as following :
		0       4                        3132                    56       62  64(End)
		-----------------------------------------------------------------------
		| T |L|F|        I/O Tags        |R|       Edge-RI       |Face-RIT| C |
		-----------------------------------------------------------------------
		  2  1 1           27             1          24              6      2  

		0                                                      54             64(End)
		-----------------------------------------------------------------------
		|                       Simple Tags                    |      R       |
		-----------------------------------------------------------------------
		                             54                               10       
	Here, R is short for reserved bits.
	T means the type of the cell.
	L is a flag which means this cell has or has not been changed during last thinning/dual
	thinning step. Which is similar to m_bLastTime flag in SimpleHuma project.
	F is a flag of same type as L, however, represent if a node has been set a Simple tag
	inside during the first traversal. It determines if a node should be expanded during the
	second traversal.
	I/O Tags are tags indicates the points/edges/faces/cells if they are inside or outside.
	The memory block stores tags of 8 points (4 ~ 11), 12 edges (12 ~ 23), 6 faces (24 ~
	29) and 1 cell (30). Note that these tags should NEVER be changed during algorithm.
	Edge-RI stores the Edge-Retracing-Information by allocation 2 bits for each edge.
	Face-RIT stores the Face-Retracing-Information-Tag, indicates which faces should have
	RETRACE_EDGE structures following the node structure.
	C represents if the node has been changed during breaking(62th bit) or filling(63th bit).
	Simple Tags record the 2-bits simple states of all elements in the cell. The meaning of
	the tags are :
		00 -- a current inside element without simple tag
		01 -- a current outside element without simple tag
		10 -- a element with simple tag, it should be inside during thinning and outside
			during the dual process. And in MST creation, this tag has been borrowed as NOT
			have been set out/in tag during breaking/filling.
		11 -- a element with overflowed-simple tag, it should only appear during dual
			thnning. And in growing process, this tag has been borrowed as the cutting plane.


========================================================================
	PROCEDURES : Important Procedures And Functions
========================================================================

Traversal() and RecTraversalCell/Face/Edge/Point()
	These procedures provide a method to make a traversal over all the elements in the
	OcTree. A detailed description of these procedures could be found in the reference papers.
	The behaviors during each traversal are determined by the COcTree::m_nType, the detailed
	explaination could be found in the head of OcTree.h.
	Note, there are lots of derivatives of these procedures, which share a common
	nomenclature : RecXxxxxxxxxCell/Face/Edge/Point().
	
RecXxxxxxx()
	On the other hand, we need a way of making only a traversal on nodes, so that we use the
	procedures with the same nomenclature : RecXxxxxxx() to get such result. A procedure
	calls itself recursively if the node passed as a parameter to it is NOT a leaf node.
	Some procedures of this type :
		RecLoadFromSOG() : load from a SOG file, and create the OcTree structure.
		RecLoadFromOcTree() : create a new OcTree allocating RETRACE_EDGE structure
			correctly from another OcTree.
		RecRegularTest() : make a regular test on each leaf node. LEAF_NODE::m_nVertex are
			determined during this procedure.
		RecFixTags() : assign I/O tags to simple tags, or do the opposite.


========================================================================
	PIPELINE : Shows the pipeline that would help understanding
========================================================================

In this part, I would like to describe the transition of the two most important flags (I/O
tag and Simple tag) for eash object, and the pipeline including all the procedures.

Part I : Initialization
	LoadFromSOG()[InitOcTree] : load I/O tags for cell/point, and set I/O tags for face/edge,
		only if ALL the points belong to the face/edge are inside, the face/edge is set
		inside.
	InitFaceTags()[InitOcTree] : this procedure sets the Face-RIT tags.
	LoadFromOcTree() : reconstruct a OcTree which includes correct RETRACE_EDGE structures.

Part II : Main Break Process
	RecFixTags() : from now on, we use the Simple tags. In this procedure, all the Simple tags
		are set as the same as I/O tags for every object.
	Thin() : the procedure runs for many steps until no change has occured in the last step.
		In each step, we make two traversal over the INSIDE part of OcTree.
		During the first traversal, all the objects are checked if it is simple. If so, it's
			Simple tag is set to 10.
		During the second traversal, all the objects with its Simple tag 10 are checked if it
			is still simple now. If so, its Simple tag is set to 01 (temporary outside).
		After running this procedure, the I/O tags of each object should NOT be changed, while
		the Simple tags should represent a skeleton with 00 value, others 01 value.
	CreateMST() : now we pick up a seed point and name the neighbor edges of the point waiting
		edges (Simple tags for all the unprocessed point/edge are 10, for those in waiting
		list or those who have been removed are 11, and for those in the MST are 00). A
		minimum heap is kept to maintain the waiting edges queue and ensure the poped edge 
		always has a minimum weight in the queue. In that case a Minimum Spanning Tree is 
		generated, the Simple tags of the objects on the tree are still 00, while the Simple
		tags of removal edges are set to 11.
