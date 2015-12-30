#ifndef WE_BSP_READER_H
#define WE_BSP_READER_H

#include "WeVector.h"

namespace WeBSPParse
{

	const int MAX_QPATH = 64;
	const int BOX_BRUSHES = 1;
	const short PLANE_X = 0;
	const short PLANE_Y = 1;
	const short PLANE_Z = 2;
	const short PLANE_NON_AXIAL = 3;
	const unsigned int SURF_NOIMPACT = 0x10;

	const float MAX_WORLDCOORD = 64 * 1024;
	const float MIN_WORLDCOORD = -64 * 1024;
	const float BOGUS_RANGE = MAX_WORLDCOORD + 1;
	const int MAX_POINTS_ON_WINDING = 64;
	const float ON_EPSILON = 0.01f;

	const int SIDE_FRONT = 0;
	const int SIDE_ON = 2;
	const int SIDE_BACK = 1;
	const int SIDE_CROSS = -2;



	const int	LUMP_ENTITIES = 0;
	const int	LUMP_SHADERS = 1;
	const int	LUMP_PLANES = 2;
	const int	LUMP_NODES = 3;
	const int	LUMP_LEAFS = 4;
	const int	LUMP_LEAFSURFACES = 5;
	const int	LUMP_LEAFBRUSHES = 6;
	const int	LUMP_MODELS = 7;
	const int	LUMP_BRUSHES = 8;
	const int	LUMP_BRUSHSIDES = 9;
	const int	LUMP_DRAWVERTS = 10;
	const int	LUMP_DRAWINDEXES = 11;
	const int	LUMP_FOGS = 12;
	const int	LUMP_SURFACES = 13;
	const int	LUMP_LIGHTMAPS = 14;
	const int	LUMP_LIGHTGRID = 15;
	const int	LUMP_VISIBILITY = 16;
	const int	HEADER_LUMPS = 17;

	const int LUMP_VERT_SIZE = 44;
	const int LUMP_INT_SIZE = 4;
	const int LUMP_FACE_SIZE = 104;

	const int MST_PLANAR = 1;
	const int MST_TRIANGLE_SOUP = 3;

	//for double calculation result
	const double FLOAT_EPSILON = 0.000001;

	//zoom in the scale
	const double ZOOM_SCALE_X = 1;
	const double ZOOM_SCALE_Y = 1;
	const double ZOOM_SCALE_Z = 1;

	struct WeLump
	{
		int fileofs;
		int filelen;

	};


	struct WeCPlane
	{
		WeVec3 normal;
		float dist;
		unsigned char type;
		unsigned char nearbits;
		unsigned char farbits;
	};


	struct WeCBrushSide
	{
		WeCPlane plane;
		int surfaceFlags;
		int shaderNum;
	};

	struct WeDShader
	{
		int surfaceFlags;
		int contentFlags;
		char shader[MAX_QPATH];
	};

	struct WeCBrush
	{
		int shaderNum;
		int contents;
		WeVec3 bounds[2];
		int numsides;
		WeCBrushSide* sides;
		int checkCount;
	};

	const int INT_SIZE = 4;
	const int FLOAT_SIZE = 4;
	const int DMODEL_SIZE = 40;
	const int DBRUSH_SIZE = 12;
	const int DBRUSHSIDE_SIZE = 8;
	const int DPLANE_SIZE = 16;
	const int DSHADER_SIZE = 2 * 4 + MAX_QPATH;


	enum WeDecals
	{
		TEX_DEFAULT = 0,
		TEX_DIRT = 3072,		//ÎÛ¹¸
		TEX_CONCRETE = 3200,	//»ìÄýÍÁ
		TEX_GRATE = 4096,	//×°¸ñÕ¤
		TEX_VENT = 8192,		//Í¨·ç¿×£» ÅÅÆø¿×
		TEX_TILE = 12416,		//ÍßÆ¬
		TEX_COMPUTER = 8320,

		TEX_METAL = 4096,		//½ðÊô
		TEX_WOOD = 8192,		//Ä¾Í·

		TEX_LADDERS = 27824,	//ÅÀÌÝ×Ó
		TEX_SKYBOX = 134164,	//Ìì¿ÕºÐ

		StatirContentMask = 536936448
	};


	const int MAX_SUBMODELS = 256;
	const int MAX_POSITION_LEAFS = 1024;
	const float SURFACE_CLIP_EPSILON = 0.08f;

	const int BOX_MODEL_HANDLE = 255;

	const int CONTENTS_SOLID = 1;
	const int CONTENTS_LAVA = 8;
	const int CONTENTS_SLIME = 16;
	const int CONTENTS_WATER = 32;

	const int CONTENTS_PLAYERCLIP = 0x10000;
	const int CONTENTS_BODY = 0x2000000;
	const int CONTENTS_BOTCLIP = 0x400000;
	const int CONTENTS_CORPSE = 0x4000000;

	const int MASK_PLAYERSOLID = (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_BODY);
	const int MASK_SHOT = (CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE);
	const int MASK_WATER = (CONTENTS_WATER | CONTENTS_LAVA | CONTENTS_SLIME);

	struct WeBspHeader
	{
		int ident;
		int version;
		WeLump lumps[HEADER_LUMPS];
	};


	struct WeFace
	{
		int* indices;
		int vertexNum;
		WeVec3 normal;
	};

	struct WeConvex
	{
		int* faces;
		int faceNum;
		int maxFaceNum;
	};

	struct WeRawMeshData
	{
		WeVec3* vertices;
		int vertexNum;
		int maxVertexNum;

		WeFace* faces;
		int faceNum;
		int maxFaceNum;

		WeConvex* convexities;
		int convexNum;
		int maxConvextNum;
	};

	class WeLittleEndianReader
	{
	public:
		WeLittleEndianReader();
		~WeLittleEndianReader();

		bool load(const char* fileName);
		bool load(const unsigned char* rawMesh, int len);

		void reset();

		bool readBoolean();
		unsigned char readByte();
		int readUnsignedByte();
		short readShort();
		int readUnsignedShort();
		char readChar();
		int readInt();
		long long readLong();
		double readDouble();
		float readFloat();
		int skipBytes(int n);

	private:
		unsigned char* mContent;
		int		mLen;
		int		mSeekPos;

		bool checkEOF();
		unsigned char read();
	};

#define WINDING_SIZE( pt ) ( sizeof( int )*2 + sizeof( float )*3*( pt ) )

	struct WeWinding
	{
		int numpoints;
		int maxpoints;
		WeVec3 points[8];
	};

	class WeBspReader
	{

	public:
		WeBspReader();
		~WeBspReader();
#ifdef __CROSSBRIDGE__
		bool readRawMesh(const unsigned char* rawMesh, int len);
#else
		bool readRawMesh(const char* meshFile);
#endif
		WeRawMeshData& getRawMeshData(){ return mMeshData; }
	private:

		void addBrush(const WeCBrush& brush);
		WeWinding* allocateWinding(int points);
		void freeWinding(WeWinding* w);
		WeWinding* getBaseWinding(const WeCPlane& plane);
		WeWinding* clipWinding(WeWinding& in, const WeCPlane& split, bool keepon);
		int addFace(const WeWinding& w, const WeVec3& normal);
		void addConvex(const WeConvex& convex);
		void debugConvex(const WeConvex& convex);

		void counterClockFace(WeVec3* vertices, WeFace& face);
		void getFaceCenter(WeVec3* vertices, WeFace& face, WeVec3& center);

		void readString(char* dst, int len);

		void loadHeader();
		void loadShaders();
		void loadLeafBrushes();
		void loadPlanes();
		void loadBrushSides();
		void loadBrushes();
		void loadSubModels();
		void createRawMesh();

		WeBspHeader mHeader;
		WeDShader*  mShaders;
		int		  mShaderNum;
		WeCPlane*   mPlanes;
		int       mPlaneNum;
		WeCBrushSide* mBrushSides;
		int			mBrushSideNum;
		WeCBrush*     mBrushes;
		int         mBrushNum;
		int*	    mLeafBrushes;
		int         mLeafBrushNum;
		WeLittleEndianReader mStream;

		WeRawMeshData  mMeshData;

	};

}


#endif
