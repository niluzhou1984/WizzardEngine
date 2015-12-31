#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Math.h>

#include "PxPreprocessor.h"
#include "PxVec3.h"
#include "PsVecMath.h"
#include "PxPhysics.h"
#include "cooking/PxCooking.h"
#include "PxFoundation.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxMaterial.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "physxprofilesdk/PxProfileZoneManager.h"
#include "pxtask/PxCudaContextManager.h"
#include "PxScene.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultStreams.h"

#include "GuSweepSharedTests.h"
#include "GuGJKRaycast.h"
#include "GuVecBox.h"
#include "GuVecConvexHull.h"
#include "GuConvexMesh.h"

using namespace physx;
using namespace physx::shdfnd;
using namespace physx::shdfnd::aos;
using namespace physx::Gu;


PxDefaultErrorCallback				defaultErrorCallback;
PxDefaultAllocator					defaultAllocatorCallback;

void gjkTest()
{

	PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
	if(!foundation)
	{
		printf("Create Foundation Failed!\n");
		return;
	}

	PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);
	if(!profileZoneManager)
	{
		printf("Create ProfileZoneManager Failed!\n");
		return;
	}

	PxTolerancesScale scale;
	PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, false, profileZoneManager);
	if(!physics)
	{
		printf("Create Physics Failed!\n");
		return;
	}

	PxCookingParams params(scale);
	params.skinWidth = 0.0001f;
	params.meshWeldTolerance = 0.001f;
	params.meshSizePerformanceTradeOff = 0.70f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	PxCooking* cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, params);
	if(!cooking)
	{
		printf("Create Cooking Failed!\n");
		return;
	}

	const Vec3V start = Vec3V(6575.555837213967, 6000.80062605554, 434.41999511718745);
	const Vec3V end =  Vec3V(6565.027187608346, 6020.085216733808, 434.41999511718745);

	const Vec3V zeroV = V3Zero();
	const Vec3V boxExtents = Vec3V(45, 45, 85);
	BoxV a(zeroV, boxExtents);

	const Vec3V vScale = Vec3V(1.0, 1.0, 1.0);
	const PxF32 rx[4] = {0,0,0,1};
	const QuatV vQuat = QuatVLoadU(rx);

	const PxTransform boxTransform(PxVec3(start.x, start.y, start.z + 85));
	const PxTransform pose(PxVec3(0, 0, 0));
	const PsTransformV boxPose = loadTransformA(boxTransform);
	const PsTransformV convexPose = loadTransformU(pose);

	const int VERTEX_LIMITS = 256;
	PxVec3 vertices[VERTEX_LIMITS];
	vertices[0] = PxVec3(6248.39990, 6060.43994, 388.619995);
	vertices[1] = PxVec3(6248.39990, 6060.43994, 462.279999);
	vertices[2] = PxVec3(6248.39990, 6291.58008, 388.619995);
	vertices[3] = PxVec3(6248.39990, 6291.58008, 462.279999);
	vertices[4] = PxVec3(6569.70947, 6060.43994, 388.619995);
	vertices[5] = PxVec3(6569.70947, 6060.43994, 462.279999);
	vertices[6] = PxVec3(6569.70947, 6291.58008, 388.619995);
	vertices[7] = PxVec3(6569.70947, 6291.58008, 462.279999);

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 8;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = vertices;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	convexDesc.vertexLimit = VERTEX_LIMITS;
	PxDefaultMemoryOutputStream buf;
	PxConvexMesh*  rawConvexMesh = NULL;


	if (cooking->cookConvexMesh(convexDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		rawConvexMesh = physics->createConvexMesh(id);
	}else{
		printf("Cooking convex error!\n");
		return;
	}

	if(!rawConvexMesh)
	{
		printf("Null rawConvexMesh!\n");
		return;
	}

	const PxConvexMeshGeometry convexGeom(rawConvexMesh);
	FETCH_CONVEX_HULL_DATA(convexGeom)
	ConvexHullV b(hullData, zeroV, vScale, vQuat);
 
	const PsMatTransformV aToB(convexPose.transformInv(boxPose));

	const FloatV zero = FZero();
	PxVec3 unitDir  = PxVec3(end.x - start.x, end.y - start.y, end.z - start.z);
	PxF32 distance = unitDir.magnitude();
	unitDir = unitDir.getNormalized();
	const Vec3V worldDir = V3LoadU(unitDir);
	const FloatV dist = FLoad(distance);
	const Vec3V dir =convexPose.rotateInv(V3Neg(V3Scale(worldDir, dist)));

	FloatV toi;
	Vec3V closestA, normal;
	printf("Start GJK Test\n");
	bool res = _gjkRelativeRayCast<BoxV, ConvexHullV>(a,   b,  aToB,  zero,  zeroV,  dir,  toi, normal, closestA, 0);
	printf("End Test, result is %d, closestA is %f, %f, %f!\n", res, closestA.x, closestA.y, closestA.z);
}



int main()
{
	
	gjkTest();
	return 0;
}