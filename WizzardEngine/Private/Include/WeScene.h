#ifndef WE_SCENE_H
#define WE_SCENE_H

#include <map>
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

#include "WeInternCommon.h"
#include "WeCommon.h"
#include "WeVector.h"
#include "WeDefaultSimulationEventCallback.h"
#include "WeQueryFilterCallback.h"
#include "WeSelfFilterCallback.h"

using namespace physx;
using namespace std;



typedef struct
{
	float	staticFriction;
	float	dynamicFriction;
	float   restitution;
} WeMaterialInfo;

class WeScene
{
		
public: 
	WeScene(int flag, float gravity, WeMaterialInfo& materialInfo);
	~WeScene();

	void initScene();

#ifdef __CROSSBRIDGE__
	void buildSceneWithMesh(const unsigned char* rawMesh, int len);
#else
	void buildSceneWithMesh(const char* meshFile);
#endif

	bool getError();

	//player dynamic body
	void setBodyMaterial(WeMaterialInfo& materialInfo);
	bool addDynamicBody(int bodyIndex, WeVec3 pos, WeVec3 bound = WeVec3(90.0f, 90.0f, 170.0f), WeVec3 materialInfo = WeVec3(0.5f, 0.5f,1.0f));
	bool setDynamicBodyBound(int bodyIndex, WeVec3 bound);
	bool removeDynamicBody(int bodyIndex);
	bool moveDynamicBody(int bodyIndex, WeVec3 pos, bool simulate = false);
	bool getDynamicBodyPos(int bodyIndex, WeVec3& pos);

	//filter
	static PxFilterFlags defaultFilter(PxFilterObjectAttributes attributes0,
										PxFilterData filterData0,
										PxFilterObjectAttributes attributes1,
										PxFilterData filterData1,
										PxPairFlags& pairFlags,
										const void* constantBlock,
										PxU32 constantBlockSize);


	//collision detector
	bool query(WeQueryType::Enum queryType, WeQueryDesc& queryDesc, WeQueryResult& result);
	bool setQueryResult(WeQueryType::Enum queryType, PxLocationHit& hit, PxVec3& dir, float distance, WeQueryResult& result);
	
private:
	
	void setStatus(WeStatusCode::Enum status){ mStatus = status; }
	void freeActor(PxRigidActor* actor);

	const static int      MAX_BODY_NUM = 32;

	PxDefaultErrorCallback				mDefaultErrorCallback;
	PxDefaultAllocator					mDefaultAllocatorCallback;
	WeDefaultSimulationEventCallback	mDefaultSimulationEventCallback;
	WeMaterialInfo						mMaterialInfo;
	float								mGravity;
	int									mFlag;
	WeStatusCode::Enum					mStatus;


	PxFoundation*			mFoundation;
	PxPhysics*				mPhysics;
	PxCooking*				mCooking;
	PxProfileZoneManager*	mProfileZoneManager;
	PxDefaultCpuDispatcher*	mCpuDispatcher;
	PxCudaContextManager*	mCudaContextManager;
	PxScene*				mScene;
	PxRigidStatic*			mWorld;
	PxMaterial*				mMaterial;

	WeMaterialInfo				mBodyMaterialInfo;
	PxMaterial*					mBodyMaterial;
	map<int, PxRigidDynamic*>	mBodyMap;
	int							mBodyNum;

	WeQueryFilterCallback*		mQueryFilterCallback;
	WeSelfFilterCallback*		mSelfFilterCallback;
	
};


#endif