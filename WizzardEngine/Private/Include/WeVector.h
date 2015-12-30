#ifndef WE_VECTOR_H
#define WE_VECTOR_H

#include "WeInternCommon.h"
#include <math.h>

typedef struct WeVec3_t{
	union{
		float v[3];
		struct{
			float x;
			float y;
			float z;
		};
	};
	
	WeVec3_t(float x = 0.0f, float y = 0.0f, float z = 0.0f){ v[0] = x; v[1] = y;  v[2] = z; }
	float& operator[] (int i) { return v[i]; }
	const float& operator[]  (int i) const { return v[i]; } 
} WeVec3;


float weVecLength(const WeVec3& v);

float weVecSqr(const WeVec3& v);
void weVecNormalize(WeVec3& v);
void weVecSetTo(const float x, const float y, const float z, WeVec3& vout);
float weVecDot(const WeVec3& v1, const WeVec3& v2);
void  weVecCross(const WeVec3& v1, const WeVec3& v2, WeVec3& vout);

void weVecMA(const WeVec3& v1, const WeVec3& v2, const float f, WeVec3& vout);

void weVecSub(const WeVec3& v1, const WeVec3& v2, WeVec3& vout);
void weVecAdd(const WeVec3& v1, const WeVec3& v2, WeVec3& vout);

void weVecScale(WeVec3& v, float scale);
void weVecScaleInverse(WeVec3& v, float scale);
void weVecNegate(WeVec3& v);
void weVecCopy(WeVec3& dest, const WeVec3& src);

#endif