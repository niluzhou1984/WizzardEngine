
#include <stdio.h>
#include <string>
#include <string.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <map>
#include <algorithm>

#include "WeInternCommon.h"
#include "WeBspReader.h"

using namespace WeBSPParse;

static void resetPlane(WeCPlane& plane)
{
	memset(&plane, 0, sizeof(WeCPlane));
}

unsigned char planeTypeForNormal(WeCPlane& plane)
{
	if (plane.normal[0] == 1.0)
	{
		return PLANE_X;
	}
	else if (plane.normal[1] == 1.0)
	{
		return PLANE_Y;
	}
	else if (plane.normal[2] == 1.0)
	{
		return PLANE_Z;
	}
	else
	{
		return PLANE_NON_AXIAL;
	}
}

static unsigned char planeNearbitsForNormal(WeCPlane& plane)
{
	unsigned char bits = 0;
	for (int j = 0; j < 3; j++)
	{
		if (plane.normal[j] < 0)
		{
			bits |= 1 << j;
		}
	}

	return bits;
}

static unsigned char planeFarbitsForNormal(WeCPlane& plane)
{
	unsigned char bits = 0;
	for (int j = 0; j < 3; j++)
	{
		if (-plane.normal[j] < 0)
		{
			bits |= 1 << j;
		}
	}

	return bits;
}

static void boundBrush(WeCBrush& b)
{
	b.bounds[0][0] = -b.sides[0].plane.dist;
	b.bounds[1][0] = b.sides[1].plane.dist;

	b.bounds[0][1] = -b.sides[2].plane.dist;
	b.bounds[1][1] = b.sides[3].plane.dist;

	b.bounds[0][2] = -b.sides[4].plane.dist;
	b.bounds[1][2] = b.sides[5].plane.dist;
}


WeLittleEndianReader::WeLittleEndianReader() :
mContent(NULL),
mLen(0),
mSeekPos(0)
{

}

template<typename T> static bool expandArray(T* & array, int curSize, int newSize)
{
	WE_ASSERT(curSize < newSize);
	T* tmp = new T[newSize];

	if (tmp == NULL)
		return false;

	memcpy(tmp, array, curSize*sizeof(T));
	delete array;
	array = tmp;

	return true;
};

WeLittleEndianReader::~WeLittleEndianReader()
{
	if (mContent)
		delete mContent;

}


bool WeLittleEndianReader::load(const char* fileName)
{
	FILE* fp = fopen(fileName, "rb");

	if (!fp)
		return false;
	fseek(fp, 0L, SEEK_END);
	mLen = ftell(fp);
	mContent = new unsigned char[mLen];
	if (!mContent) {
		fclose(fp);
		return false;
	}
	fseek(fp, 0L, SEEK_SET);

	int readLen = fread(mContent, mLen, 1, fp);
	if (readLen != 1)
	{
		delete mContent;
		mContent = NULL;
		fclose(fp);
		return false;
	}

	fclose(fp);
	mSeekPos = 0;
}

bool WeLittleEndianReader::load(const unsigned char* rawMesh, int len)
{
	mContent = new unsigned char[len];
	if (!mContent)
		return false;

	memcpy(mContent, rawMesh, len*sizeof(char));
	mLen = len;
	mSeekPos = 0;
	return true;
}


void WeLittleEndianReader::reset()
{
	mSeekPos = 0;
}

bool WeLittleEndianReader::checkEOF()
{
	return (mSeekPos >= mLen);
}

unsigned char WeLittleEndianReader::read()
{
	if (checkEOF())
		throw WeErrorCode::ERROR_BSP;

	return mContent[mSeekPos++];
}

bool WeLittleEndianReader::readBoolean()
{

	int b = read();
	return (b != 0);
}
unsigned char WeLittleEndianReader::readByte()
{

	unsigned char b = read();
	return b;
}

int WeLittleEndianReader::readUnsignedByte()
{
	unsigned int b = read();
	return b;
}
short WeLittleEndianReader::readShort()
{
	unsigned int b1 = read();
	unsigned int b2 = read();

	return (short)(((b2 << 24) >> 16) + (b1 << 24) >> 24);
}



int WeLittleEndianReader::readUnsignedShort()
{
	unsigned int b1 = read();
	unsigned int b2 = read();
	return ((b1 << 24) >> 16) + ((b1 << 24) >> 24);
}
char WeLittleEndianReader::readChar()
{
	return (char)read();
}
int WeLittleEndianReader::readInt()
{
	unsigned int b1 = read();
	unsigned int b2 = read();
	unsigned int b3 = read();
	unsigned int b4 = read();
	
	return (b4 << 24) + ((b3 << 24) >> 8) + ((b2 << 24) >> 16)
		+ ((b1 << 24) >> 24);
}


long long WeLittleEndianReader::readLong()
{
	unsigned long long b1 = read();
	unsigned long long b2 = read();
	unsigned long long b3 = read();
	unsigned long long b4 = read();
	unsigned long long b5 = read();
	unsigned long long b6 = read();
	unsigned long long b7 = read();
	unsigned long long b8 = read();

	return (b8 << 56) + ((b7 << 56) >> 8) + ((b6 << 56) >> 16)
		+ ((b5 << 56) >> 24) + ((b4 << 56) >> 32)
		+ ((b3 << 56) >> 40) + ((b2 << 56) >> 48)
		+ ((b1 << 56) >> 56);
}


double WeLittleEndianReader::readDouble()
{

	long long l = readLong();
	double d;
	memcpy(&d, &l, sizeof(double));
	return d;
}


float WeLittleEndianReader::readFloat()
{
	int i = readInt();
	float f;
	memcpy(&f, &i, sizeof(float));
	return f;
}
int WeLittleEndianReader::skipBytes(int n)
{
	mSeekPos += n;
	if (checkEOF())
	{
		throw WeErrorCode::ERROR_BSP;
	}

	return n;
}

WeBspReader::WeBspReader() :
mShaders(NULL),
mShaderNum(0),
mPlanes(NULL),
mPlaneNum(0),
mBrushSides(NULL),
mBrushSideNum(0),
mBrushes(NULL),
mBrushNum(0),
mLeafBrushes(NULL),
mLeafBrushNum(0)
{
	memset(&mMeshData, 0, sizeof(WeRawMeshData));
	memset(&mHeader, 0, sizeof(WeBspHeader));
}
WeBspReader::~WeBspReader()
{
	if (mShaders)
		delete mShaders;
	if (mPlanes)
		delete mPlanes;
	if (mBrushSides)
		delete mBrushSides;
	if (mBrushes)
		delete mBrushes;
	if (mLeafBrushes)
		delete mLeafBrushes;
}


#ifdef __CROSSBRIDGE__
bool WeBspReader::readRawMesh(const unsigned char* rawMesh, int len)
#else
bool WeBspReader::readRawMesh(const char* meshFile)
#endif

{
	try{

#ifdef __CROSSBRIDGE__
		if(!mStream.load(rawMesh, len))
			return false;
#else
		if (!mStream.load(meshFile))
			return false;
#endif

		loadHeader();
		loadShaders();
		loadLeafBrushes();
		loadPlanes();
		loadBrushSides();
		loadBrushes();
		loadSubModels();
		createRawMesh();
		WE_DEBUG_HANDLE(WeErrorCode::INFO_DEBUG, "Parser Bsp  File Success!");
	}
	catch (WeErrorCode::Enum e)
	{
		WE_ERROR_HANDLE(e, "Parser Bsp File!");
		return false;
	}


	return true;
}


void WeBspReader::readString(char* dst, int len)
{
	for (int i = 0; i < len; i++)
	{
		dst[i] = mStream.readChar();
	}
}

void WeBspReader::loadHeader()
{
	mStream.reset();
	int indent = mStream.readInt();
	int version = mStream.readInt();
	mHeader.ident = indent;
	mHeader.version = version;

	for (int i = 0; i < HEADER_LUMPS; i++) {
		mHeader.lumps[i].fileofs = mStream.readInt();
		mHeader.lumps[i].filelen = mStream.readInt();
	}
}

void WeBspReader::loadShaders()
{
	mStream.reset();
	WeLump l = mHeader.lumps[LUMP_SHADERS];
	mStream.skipBytes(l.fileofs);
	if ((l.filelen % DSHADER_SIZE) != 0) {
		throw - 1;
	}
	int count = l.filelen / DSHADER_SIZE;

	if (count < 1) {
		throw - 1;
	}
	
	mShaders = new WeDShader[count];
	mShaderNum = count;
	for (int i = 0; i < count; i++) {
		readString(mShaders[i].shader, MAX_QPATH);
		mShaders[i].surfaceFlags = mStream.readInt();
		mShaders[i].contentFlags = mStream.readInt();
	}
}
void WeBspReader::loadLeafBrushes()
{
	mStream.reset();
	WeLump l = mHeader.lumps[LUMP_LEAFBRUSHES];
	mStream.skipBytes(l.fileofs);
	if ((l.filelen % INT_SIZE) != 0) {
		throw WeErrorCode::ERROR_BSP;
	}

	int count = l.filelen / INT_SIZE;
	mLeafBrushes = new int[count + BOX_BRUSHES];
	mLeafBrushNum = count;

	for (int i = 0; i < count; i++) {
		mLeafBrushes[i] = mStream.readInt();
	}
}

void WeBspReader::loadPlanes()
{
	mStream.reset();
	WeLump l = mHeader.lumps[LUMP_PLANES];
	mStream.skipBytes(l.fileofs);
	if ((l.filelen % DPLANE_SIZE) != 0) {
		throw WeErrorCode::ERROR_BSP;
	}
	int count = l.filelen / DPLANE_SIZE;
	mPlaneNum = count;
	mPlanes = new WeCPlane[count];
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < 3; j++) {
			mPlanes[i].normal[j] = mStream.readFloat();
		}
		mPlanes[i].dist = mStream.readFloat();
		mPlanes[i].type = planeTypeForNormal(mPlanes[i]);
		mPlanes[i].nearbits = planeNearbitsForNormal(mPlanes[i]);
		mPlanes[i].farbits = planeFarbitsForNormal(mPlanes[i]);
	}
}
void WeBspReader::loadBrushSides()
{
	mStream.reset();
	WeLump l = mHeader.lumps[LUMP_BRUSHSIDES];
	mStream.skipBytes(l.fileofs);
	if ((l.filelen % DBRUSHSIDE_SIZE) != 0) {
		throw WeErrorCode::ERROR_BSP;
	}
	int count = l.filelen / DBRUSHSIDE_SIZE;

	mBrushSideNum = count;
	mBrushSides = new WeCBrushSide[count];

	for (int i = 0; i < count; i++) {
		int planeNum = mStream.readInt();
		int shaderNum = mStream.readInt();
		mBrushSides[i].plane = mPlanes[planeNum];
		mBrushSides[i].shaderNum = shaderNum;
		if (shaderNum < 0 || shaderNum >= mShaderNum) {
			throw WeErrorCode::ERROR_BSP;
		}

		mBrushSides[i].surfaceFlags = mShaders[shaderNum].surfaceFlags;
	}
}

void WeBspReader::loadBrushes()
{
	mStream.reset();
	WeLump l = mHeader.lumps[LUMP_BRUSHES];
	mStream.skipBytes(l.fileofs);
	if ((l.filelen % DBRUSH_SIZE) != 0) {
		throw - 1;
	}
	int count = l.filelen / DBRUSH_SIZE;
	mBrushes = new WeCBrush[count];
	mBrushNum = count;
	for (int i = 0; i < count; i++)
	{
		WeCBrush brush;
		int firstSide = mStream.readInt();
		int numSides = mStream.readInt();
		int shaderNum = mStream.readInt();
		brush.numsides = numSides;
		brush.sides = new WeCBrushSide[numSides];
		for (int j = firstSide; j < firstSide + numSides; j++)
		{
			brush.sides[j - firstSide] = mBrushSides[j];
		}
		brush.shaderNum = shaderNum;
		brush.contents = mShaders[shaderNum].contentFlags;
		boundBrush(brush);

		mBrushes[i] = brush;

	}
}

void WeBspReader::loadSubModels()
{
	mStream.reset();
	WeLump l = mHeader.lumps[LUMP_MODELS];
	mStream.skipBytes(l.fileofs);
	if ((l.filelen % DMODEL_SIZE) != 0) {
		throw WeErrorCode::ERROR_BSP;
	}
	int count = l.filelen / DMODEL_SIZE;


	for (int i = 0; i < count; i++) {

		for (int j = 0; j < 3; j++) {
			mStream.readFloat();
			mStream.readFloat();
		}

		if (i == 0) {
			continue;
		}

		mStream.readInt();
		mStream.readInt();
		int firstBrush = mStream.readInt();
		int numBrushes = mStream.readInt();
		if (!expandArray(mLeafBrushes, mLeafBrushNum + BOX_BRUSHES,
			mLeafBrushNum + numBrushes + BOX_BRUSHES))
			throw WeErrorCode::ERROR_BSP;

		for (int j = 0; j < numBrushes; j++) {
			mLeafBrushes[mLeafBrushNum++] = firstBrush + j;
		}
	}
}

void WeBspReader::createRawMesh()
{

	std::map<int, bool> brushmap;
	int bn = 0;
	for (int i = 0; i < mLeafBrushNum; i++)
	{
		int brushnum = mLeafBrushes[i];
		if (brushmap.count(brushnum) > 0)
			continue;

		brushmap[brushnum] = true;

		WeCBrush b = mBrushes[brushnum];
		if ((b.contents & MASK_PLAYERSOLID) == 0)
			continue;
		bn++;
		addBrush(b);
	}
}


void WeBspReader::addBrush(const WeCBrush& b)
{
	int faceCount = b.numsides;
	WeConvex convex;
	memset(&convex, 0, sizeof(WeConvex));
	convex.faces = new int[faceCount];
	convex.maxFaceNum = faceCount;

	for (int i = 0; i < faceCount; i++)
	{
		WeCBrushSide side = b.sides[i];
		WeCPlane plane = side.plane;
		WeWinding* w = getBaseWinding(plane);

		WeVec3 normal, clipNormal;
		weVecCopy(normal, plane.normal);

		WeCPlane p;
		bool past = false;
		for (int j = 0; j < faceCount; j++)
		{
			if (j == i){
				past = true;
				continue;
			}
			WeCPlane clipPlane = b.sides[j].plane;
			weVecCopy(clipNormal, clipPlane.normal);

			if (weVecDot(normal, clipNormal) > 0.999
				&& fabs(plane.dist - clipPlane.dist) < 0.01)
			{
				if (past)
				{
					return;
				}
				continue;
			}


			p.normal[0] = -clipPlane.normal[0];
			p.normal[1] = -clipPlane.normal[1];
			p.normal[2] = -clipPlane.normal[2];
			p.dist = -clipPlane.dist;

			WeWinding* neww = clipWinding(*w, p, false);
			if (neww != w)
			{
				freeWinding(w);
			}

			w = neww;

			if (w == NULL)
			{
				break;
			}
		}

		if (w == NULL || w->numpoints < 3)
		{
			continue;
		}

		int idx = addFace(*w, normal);
		convex.faces[convex.faceNum++] = idx;
		freeWinding(w);
	}
	addConvex(convex);
	//debugConvex(convex);
}

int WeBspReader::addFace(const WeWinding& w, const WeVec3& normal)
{

	WeFace face;
	memset(&face, 0, sizeof(WeFace));
	face.indices = new int[w.numpoints];
	face.vertexNum = w.numpoints;
	weVecCopy(face.normal, normal);

	//add vertices to raw mesh data
	WeVec3* vertices = mMeshData.vertices;
	int vertexNum = mMeshData.vertexNum;
	int maxVertexNum = mMeshData.maxVertexNum;
	int numpoints = w.numpoints;

	if (vertexNum + numpoints >= maxVertexNum)
	{
		maxVertexNum += numpoints + 128;

		if (!expandArray(vertices, vertexNum, maxVertexNum))
			throw WeErrorCode::ERROR_BSP;

		mMeshData.vertices = vertices;
		mMeshData.maxVertexNum = maxVertexNum;
	}

	for (int i = 0; i < numpoints; i++, vertexNum++)
	{
		weVecCopy(vertices[vertexNum], w.points[i]);
		face.indices[i] = vertexNum;
	}

	mMeshData.vertexNum = vertexNum;

	//add face to raw mesh data
	WeFace* faces = mMeshData.faces;
	int faceNum = mMeshData.faceNum;
	int maxFaceNum = mMeshData.maxFaceNum;
	if (faceNum >= maxFaceNum)
	{
		maxFaceNum += 256;
		if (!expandArray(faces, faceNum, maxFaceNum))
		{
			throw WeErrorCode::ERROR_BSP;
		}

		mMeshData.faces = faces;
		mMeshData.maxFaceNum = maxFaceNum;
	}

	counterClockFace(vertices, face);
	faces[mMeshData.faceNum++] = face;

	return faceNum;

}

void WeBspReader::addConvex(const WeConvex& convex)
{
	WeConvex* cs = mMeshData.convexities;
	int num = mMeshData.convexNum;
	int maxNum = mMeshData.maxConvextNum;
	if (num >= maxNum)
	{
		maxNum += 128;
		if (!expandArray(cs, num, maxNum))
		{
			throw WeErrorCode::ERROR_BSP;
		}

		mMeshData.convexities = cs;
		mMeshData.maxConvextNum = maxNum;
	}

	cs[mMeshData.convexNum++] = convex;
}

void WeBspReader::debugConvex(const WeConvex& convex)
{
	int faceNum = convex.faceNum;
	
	for (int i = 0; i < faceNum; i++)
	{
		WeFace& face = mMeshData.faces[convex.faces[i]];
		WeVec3& normal = face.normal;
		WE_INFO_HANDLE(WeErrorCode::INFO_DEBUG, "   Face: %d index %d, norm %f %f %f", i, convex.faces[i], normal.x, normal.y, normal.z);
		for (int j = 0; j < face.vertexNum; j++)
		{
			WeVec3& v = mMeshData.vertices[face.indices[j]];
			WE_INFO_HANDLE(WeErrorCode::INFO_DEBUG, "       Vertex %d index %d: %f %f %f",j, face.indices[j], v.x, v.y, v.z);
		}
	}
}

WeWinding* WeBspReader::allocateWinding(int points)
{
	WeWinding* w;
	int size;
	if (points > MAX_POINTS_ON_WINDING)
	{
		return NULL;
	}
	size = WINDING_SIZE(points);
	w = (WeWinding*)malloc(size);
	memset(w, 0, size);
	return w;
}
void WeBspReader::freeWinding(WeWinding* w)
{
	free(w);
}

WeWinding* WeBspReader::getBaseWinding(const WeCPlane& plane)
{

	int i, x;
	float max, v;

	max = -BOGUS_RANGE;

	x = -1;

	for (i = 0; i < 3; i++)
	{
		v = fabs(plane.normal[i]);
		if (v > max)
		{
			x = i;
			max = v;
		}
	}

	if (x == -1){
		return NULL;
	}

	WeVec3 org, vright, vup, vnormal;

	weVecCopy(vnormal, plane.normal);

	switch (x)
	{
	case 0:
	case 1:
		vup[2] = 1;
		break;
	case 2:
		vup[0] = 1;
		break;
	}

	v = weVecDot(vup, vnormal);
	weVecMA(vup, vnormal, -v, vup);
	weVecNormalize(vup);

	org = vnormal;
	weVecScale(org, plane.dist);

	weVecCross(vup, vnormal, vright);
	weVecScale(vup, BOGUS_RANGE);
	weVecScale(vright, BOGUS_RANGE);

	WeWinding* w = allocateWinding(4);
	weVecSub(org, vright, w->points[0]);
	weVecAdd(w->points[0], vup, w->points[0]);

	weVecAdd(org, vright, w->points[1]);
	weVecAdd(w->points[1], vup, w->points[1]);

	weVecAdd(org, vright, w->points[2]);
	weVecSub(w->points[2], vup, w->points[2]);

	weVecSub(org, vright, w->points[3]);
	weVecSub(w->points[3], vup, w->points[3]);

	w->numpoints = 4;
	return w;
}

WeWinding* WeBspReader::clipWinding(WeWinding& in, const WeCPlane& split, bool keepon)
{
	float dists[MAX_POINTS_ON_WINDING];
	int sides[MAX_POINTS_ON_WINDING];
	int counts[3];

	memset(counts, 0, sizeof(int) * 3);

	float dot;
	int i, j;

	WeVec3 splitNormal;
	weVecCopy(splitNormal, split.normal);

	for (i = 0; i < in.numpoints; i++)
	{
		dot = weVecDot(in.points[i], splitNormal);
		dot -= split.dist;
		dists[i] = dot;

		if (dot > ON_EPSILON)
		{
			sides[i] = SIDE_FRONT;
		}
		else if (dot < -ON_EPSILON)
		{
			sides[i] = SIDE_BACK;
		}
		else
		{
			sides[i] = SIDE_ON;
		}
		counts[sides[i]]++;
	}
	sides[i] = sides[0];
	dists[i] = dists[0];

	if (keepon && counts[0] == 0
		&& counts[1] == 0)
	{
		return &in;
	}

	if (counts[0] == 0)
	{
		return NULL;
	}

	if (counts[1] == 0)
	{
		return &in;
	}

	int maxpts = in.numpoints + 4;
	WeWinding* neww = allocateWinding(maxpts);
	WeVec3 mid;
	WeVec3 p1;
	WeVec3 p2;
	for (i = 0; i < in.numpoints; i++)
	{
		p1 = in.points[i];
		if (sides[i] == SIDE_ON)
		{
			weVecCopy(neww->points[neww->numpoints], p1);
			neww->numpoints++;
			continue;
		}

		if (sides[i] == SIDE_FRONT)
		{
			weVecCopy(neww->points[neww->numpoints], p1);
			neww->numpoints++;
		}

		if (sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
		{
			continue;
		}

		p2 = in.points[(i + 1) % in.numpoints];
		dot = dists[i] / (dists[i] - dists[i + 1]);

		for (j = 0; j < 3; j++)
		{
			if (split.normal[j] == 1){
				mid[j] = split.dist;
			}
			else if (split.normal[j] == -1)
			{
				mid[j] = -split.dist;
			}
			else
			{
				mid[j] = p1[j] + dot*(p2[j] - p1[j]);
			}
		}


		weVecCopy(neww->points[neww->numpoints], mid);
		neww->numpoints++;
	}



	if (neww->numpoints > maxpts)
	{
		return NULL;
	}

	return neww;
}


void WeBspReader::counterClockFace(WeVec3* vertices, WeFace& face)
{
	
	if (face.vertexNum < 3) return;

	WeVec3 center;
	getFaceCenter(vertices, face, center);

	int vertexNum = face.vertexNum;
	int* indices = face.indices;
	int* newIndices = new int[vertexNum];
	int indexNum = 0;

	float* positive = new float[vertexNum];
	std::map<float, int> positiveMap;
	float* negative = new float[vertexNum];
	std::map<float, int> negativeMap;

	int posNum = 0, negNum = 0;

	WeVec3 baseDir;
	int baseIdx = face.indices[0];
	weVecSub(vertices[baseIdx], center, baseDir);
	weVecNormalize(baseDir);
	newIndices[indexNum++] = baseIdx;

	WeVec3 cur, cr;
	float sn, cs;
	int cmnLineCount = 0;
	for (int i = 1; i < vertexNum; i++){
		int idx = indices[i];

		weVecSub(vertices[idx], center, cur);
		weVecNormalize(cur);
		cs = weVecDot(baseDir, cur);
		weVecCross(baseDir, cur, cr);
		weVecNormalize(cr);
		sn = weVecDot(face.normal, cr);
		if (sn >= 0) {
			positive[posNum++] = cs;
			positiveMap[cs] = idx;
		}
		else{
			negative[negNum++] = cs;
			negativeMap[cs] = idx;
		}
		
		if (fabs(sn) < FLOAT_EPSILON){
			cmnLineCount++;
		}
	}

	//all vertexes in common line
	if (cmnLineCount == (vertexNum - 1))
	{
		return;
	}


	//from low to high order
	std::sort(positive, positive + posNum);

	//from hight to low order
	std::sort(negative, negative + negNum);
	std::reverse(negative, negative + negNum);

	//add vertex of up base vector
	for (int i = 0; i < posNum; i++){
		newIndices[indexNum++] = positiveMap[positive[i]];
	}

	//add vertex of down base vector
	for (int i = 0; i < negNum; i++){
		newIndices[indexNum++] = negativeMap[negative[i]];
	}

	delete face.indices;
	face.indices = newIndices;
}

void WeBspReader::getFaceCenter(WeVec3* vertices, WeFace& face, WeVec3& center)
{
	memset(&center, 0, sizeof(WeVec3));

	if (!face.vertexNum) return;

	for (int i = 0; i < face.vertexNum; i++)
	{
		int idx = face.indices[i];
		weVecCopy(center, vertices[idx]);
	}

	weVecScaleInverse(center, face.vertexNum);
}
