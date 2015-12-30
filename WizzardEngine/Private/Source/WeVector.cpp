#include "WeVector.h"

float weVecLength(const WeVec3& v)
{
	float a = v[0];
	return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float weVecSqr(const WeVec3& v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

void weVecNormalize(WeVec3& v)
{
	float len = weVecLength(v);
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

void weVecSetTo(const float x, const float y, const float z, WeVec3& vout)
{
	vout[0] = x;
	vout[1] = y;
	vout[2] = z;
}

float weVecDot(const WeVec3& v1, const WeVec3& v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


void  weVecCross(const WeVec3& v1, const WeVec3& v2, WeVec3& vout)
{
	vout[0] = v1[1] * v2[2] - v1[2] * v2[1];
	vout[1] = v1[2] * v2[0] - v1[0] * v2[2];
	vout[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void weVecMA(const WeVec3& v1, const WeVec3& v2, const float f, WeVec3& vout)
{
	vout[0] = v1[0] + v2[0] * f;
	vout[1] = v1[1] + v2[1] * f;
	vout[2] = v1[2] + v2[2] * f;
}

void weVecSub(const WeVec3& v1, const WeVec3& v2, WeVec3& vout)
{
	vout[0] = v1[0] - v2[0];
	vout[1] = v1[1] - v2[1];
	vout[2] = v1[2] - v2[2];
}

void weVecAdd(const WeVec3& v1, const WeVec3& v2, WeVec3& vout)
{
	vout[0] = v1[0] + v2[0];
	vout[1] = v1[1] + v2[1];
	vout[2] = v1[2] + v2[2];
}

void weVecScale(WeVec3& v, float scale)
{
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
}

void weVecScaleInverse(WeVec3& v, float scale)
{
	WE_ASSERT(scale != 0);

	v[0] /= scale;
	v[1] /= scale;
	v[2] /= scale;
}

void weVecNegate(WeVec3& v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

void weVecCopy(WeVec3& dest, const WeVec3& src)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}