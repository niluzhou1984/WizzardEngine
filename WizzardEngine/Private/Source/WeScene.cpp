#include "PxVec3.h"
#include "PxMath.h"
#include "PsThread.h"
#include "PxSceneLock.h"
#include "geometry/PxBoxGeometry.h"
#include "extensions/PxExtensionsAPI.h"


#include "WeInternCommon.h"
#include "WeCommon.h"
#include "WeMeshLoader.h"
#include "WeScene.h"



WeScene::WeScene(int flag, float gravity, WeMaterialInfo& materialInfo)
{
	mFoundation = NULL;
	mProfileZoneManager = NULL;
	mPhysics = NULL;
	mCooking = NULL;
	mCpuDispatcher = NULL;
	mCudaContextManager = NULL;
	mScene = NULL;
	mWorld = NULL;
	mMaterial = NULL;

	mFlag = flag;
	mGravity = gravity;
	mMaterialInfo = materialInfo;
	mStatus = WeStatusCode::STATUS_OK;

	mBodyNum = 0;
	mBodyMaterial = NULL;
	
	mQueryFilterCallback = NULL;
	mSelfFilterCallback = NULL;
}


WeScene::~WeScene()
{
	
	if (mQueryFilterCallback)
		delete mQueryFilterCallback;

	if (mSelfFilterCallback)
		delete mSelfFilterCallback;
	{
		PxSceneWriteLock scopedLock(*mScene);
		map<int, PxRigidDynamic*>::iterator it;
		for (it = mBodyMap.begin(); it != mBodyMap.end(); ++it)
		{
			PxRigidDynamic* body = it->second;
			freeActor(body);
			body->release();
		}

		if (mWorld){
			freeActor(mWorld);
			mWorld->release();
		}
	}
	

	if (mScene)
		mScene->release();

	if (mCudaContextManager)
		mCudaContextManager->release();

	if (mCpuDispatcher)
		mCpuDispatcher->release();

	if (mCooking)
		mCooking->release();

	if (mMaterial)
		mMaterial->release();

	if (mBodyMaterial)
		mBodyMaterial->release();

	if (mPhysics)
		mPhysics->release();

	if (mProfileZoneManager)
		mProfileZoneManager->release();

	if (mFoundation)
		mFoundation->release();
}

void WeScene::freeActor(PxRigidActor* actor)
{

	int nShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)malloc(nShapes*sizeof(PxShape*));
	if (shapes != NULL)
	{
		
		int n = actor->getShapes(shapes, nShapes);
		for (int i = 0; i < n; i++)
		{
			actor->detachShape(*(shapes[i]));
		}
	}

	free(shapes);
}

void WeScene::initScene()
{
	try
	{

		mStatus = WeStatusCode::STATUS_OK;
		
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation)
			throw WeErrorCode::ERROR_SCENE_INIT;

		mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
		if (!mProfileZoneManager)
			throw WeErrorCode::ERROR_SCENE_INIT;

		PxTolerancesScale scale;
		bool recordMemoryAllocations = (mFlag & WeSceneFlag::SCENE_RECORD_MEMORY_ALLOC) ? true : false;
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, recordMemoryAllocations, mProfileZoneManager);
		if (!mPhysics)
			throw WeErrorCode::ERROR_SCENE_INIT;
		if (!PxInitExtensions(*mPhysics))
			throw WeErrorCode::ERROR_SCENE_INIT;

		PxCookingParams params(scale);
		params.skinWidth = 0.0001f;
		params.meshWeldTolerance = 0.001f;
		params.meshSizePerformanceTradeOff = 0.70f;
		params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
		mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);
		if (!mCooking)
			throw WeErrorCode::ERROR_SCENE_INIT;

		PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, 0.0f, mGravity);

		sceneDesc.filterShader = defaultFilter;
		sceneDesc.simulationEventCallback = &mDefaultSimulationEventCallback;
		sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
		sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;

		int nbThreads = 1;
		if (mFlag & WeSceneFlag::SCENE_MULTI_THREAD)
		{
			nbThreads = PxMax(PxI32(shdfnd::Thread::getNbPhysicalCores()) - 1, 1);
		}else if (mFlag & WeSceneFlag::SCENE_NO_THREAD)
		{
			nbThreads = 0;
		}

		if (!sceneDesc.cpuDispatcher)
		{
			mCpuDispatcher = PxDefaultCpuDispatcherCreate(nbThreads);
			if (!mCpuDispatcher)
				throw WeErrorCode::ERROR_SCENE_INIT;

			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}
		if (mFlag & WeSceneFlag::SCENE_SUPPORT_GPU_PHYSX)
		{
			PxCudaContextManagerDesc cudaContextManagerDesc;

			if (mFlag & WeSceneFlag::SCENE_RENDER_ENABLE_CUDA_INTEROP)
			{
				/*
				if (!mApplication.getCommandLine().hasSwitch("nointerop"))
				{
				switch (getRenderer()->getDriverType())
				{
				case Renderer::DRIVER_DIRECT3D11:
				cudaContextManagerDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
				break;
				case Renderer::DRIVER_OPENGL:
				cudaContextManagerDesc.interopMode = PxCudaInteropMode::OGL_INTEROP;
				break;
				};

				cudaContextManagerDesc.graphicsDevice = getRenderer()->getDevice();
				}
				*/
			}

			mCudaContextManager = PxCreateCudaContextManager(*mFoundation, cudaContextManagerDesc, mProfileZoneManager);
			if (!sceneDesc.gpuDispatcher && mCudaContextManager)
			{
				sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
			}

		}
		

		sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

		/*
		if (mFlag & WeSceneFlag::SCENE_INVERTED_FIX_STEPPER)
		{
			
			sceneDesc.simulationOrder = PxSimulationOrder::eSOLVE_COLLIDE;
		}*/

		mScene = mPhysics->createScene(sceneDesc);
		if (!mScene)
			throw WeErrorCode::ERROR_SCENE_INIT;
		//mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, mInitialDebugRender ? mDebugRenderScale : 0.0f);
		//mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		PxSceneWriteLock scopedLock(*mScene);
		PxTransform pose = PxTransform(PxIdentity);
		mWorld = mPhysics->createRigidStatic(pose);
		if (!mWorld)
			throw WeErrorCode::ERROR_SCENE_INIT;
		mScene->addActor(*mWorld);

		mMaterial = mPhysics->createMaterial(mMaterialInfo.staticFriction, mMaterialInfo.dynamicFriction, mMaterialInfo.restitution);
		if (!mMaterial)
			throw WeErrorCode::ERROR_SCENE_INIT;

		mQueryFilterCallback = new WeQueryFilterCallback(this);
		if (!mQueryFilterCallback)
			throw WeErrorCode::ERROR_SCENE_INIT;
		mSelfFilterCallback = new WeSelfFilterCallback(this);
		if (!mSelfFilterCallback)
			throw WeErrorCode::ERROR_SCENE_INIT;

		WE_INFO_HANDLE(WeErrorCode::INFO_DEBUG, "Init Scene Success!");
	}
	catch (WeErrorCode::Enum e)
	{

		WE_ERROR_HANDLE(e, "Init Scene Failed!");
		setStatus(WeStatusCode::STATUS_ERROR);
	}
	
}

bool WeScene::getError()
{
	return mStatus != WeStatusCode::STATUS_OK;
}

#ifdef __CROSSBRIDGE__
void WeScene::buildSceneWithMesh(const unsigned char* rawMesh, int len)
#else
void WeScene::buildSceneWithMesh(const char* meshFile)
#endif
{
	if (getError())
		return;

	WeMeshLoader loader;

#ifdef __CROSSBRIDGE__
	if (!loader.createConvexMesh(*mCooking, *mPhysics, rawMesh, len))
#else
	if (!loader.createConvexMesh(*mCooking, *mPhysics, meshFile))
#endif
	{
		setStatus(WeStatusCode::STATUS_ERROR);
		return;
	}

	PxSceneWriteLock scopedLock(*mScene);
	PxCollection* meshes = loader.getConvexMeshes();
	int nmeshes = meshes->getNbObjects();

	for (int i = 0; i < nmeshes; i++)
	{
		PxConvexMesh& mesh = (PxConvexMesh&)meshes->getObject(i);
		PxShape* aConvexShape = mWorld->createShape(PxConvexMeshGeometry(&mesh), *mMaterial);
		if (!aConvexShape)
		{
			WE_DEBUG_HANDLE(WeErrorCode::WARN_CREATE_SHAPE, "Can not create convex shape.");
			continue;
		}

			
	}
}

void WeScene::setBodyMaterial(WeMaterialInfo& materialInfo)
{
	mBodyMaterialInfo = mMaterialInfo;
	if (mBodyMaterial)
		mBodyMaterial->release();
	mBodyMaterial = mPhysics->createMaterial(mBodyMaterialInfo.staticFriction, mBodyMaterialInfo.dynamicFriction, mBodyMaterialInfo.restitution);
	if (!mBodyMaterial)
		setStatus(WeStatusCode::STATUS_ERROR);
}

bool WeScene::addDynamicBody(int bodyIndex, WeVec3 pos, WeVec3 bound, WeVec3 materialInfo)
{
	if (mBodyNum > MAX_BODY_NUM)
		return false;

	PxSceneWriteLock scopedLock(*mScene);
	PxTransform pose(PxIdentity);
	pose.p = PxVec3(pos.x, pos.y, pos.z);
	PxRigidDynamic* body = mPhysics->createRigidDynamic(pose);

	PxShape* shape = body->createShape(PxBoxGeometry(bound.x / 2, bound.y / 2, bound.z / 2), *mBodyMaterial);
	//set body index to shape
	PxFilterData filterData;
	filterData.word0 = bodyIndex;
	shape->setQueryFilterData(filterData);

	mBodyMap[bodyIndex] = body;

	return true;
}

bool WeScene::setDynamicBodyBound(int bodyIndex, WeVec3 bound)
{
	if (!mBodyMap.count(bodyIndex))
		return false;

	PxSceneWriteLock scopedLock(*mScene);
	PxRigidDynamic* body = mBodyMap[bodyIndex];
	PxShape* shape;
	int nshape = body->getShapes(&shape, 1);

	WE_ASSERT(nshape == 1);

	PxBoxGeometry bg;
	bg.halfExtents = PxVec3(bound.x / 2, bound.y / 2, bound.z / 2);
	shape->setGeometry(bg);
	
	return true;
}

bool WeScene::moveDynamicBody(int bodyIndex, WeVec3 pos, bool simulate)
{
	if (!mBodyMap.count(bodyIndex))
		return false;

	PxSceneWriteLock scopedLock(*mScene);
	PxRigidDynamic* body = mBodyMap[bodyIndex];

	PxTransform pose(PxIdentity);
	pose.p = PxVec3(pos.x, pos.y, pos.z);
	if (simulate)
	{
		body->setKinematicTarget(pose);
	}
	else{
		body->setGlobalPose(pose);
	}

	return true;
}

bool WeScene::getDynamicBodyPos(int bodyIndex, WeVec3& pos)
{
	if (!mBodyMap.count(bodyIndex))
		return false;

	PxSceneReadLock scopedLock(*mScene);
	PxRigidDynamic* body = mBodyMap[bodyIndex];
	PxTransform pose = body->getGlobalPose();
	pos.x = pose.p.x;
	pos.y = pose.p.y;
	pos.z = pose.p.z;

	return true;
}

bool WeScene::removeDynamicBody(int bodyIndex)
{
	if (mBodyMap.count(bodyIndex))
	{
		PxSceneWriteLock scopedLock(*mScene);
		PxRigidDynamic* body = mBodyMap[bodyIndex];
		body->release();
		mBodyMap.erase(bodyIndex);
	}

	return true;
}


PxFilterFlags WeScene::defaultFilter(PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	/*
	if (isCCDActive(filterData0) || isCCDActive(filterData1))
	{
	pairFlags |= PxPairFlag::eCCD_LINEAR;
	}

	if (needsContactReport(filterData0, filterData1))
	{
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}
	
	pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
	*/
	return PxFilterFlags();

}

bool WeScene::query(WeQueryType::Enum queryType, WeQueryDesc& queryDesc, WeQueryResult& result)
{

	PxSceneReadLock scopedLock(*mScene);
	PxVec3 start(queryDesc.start[0], queryDesc.start[1], queryDesc.start[2]);
	int flag = queryDesc.flag;

	PxVec3 dir; 
	float distance = 0;
	if (queryType != WeQueryType::QUERY_OVERLAP)
	{
		PxVec3 end(queryDesc.end[0], queryDesc.end[1], queryDesc.end[2]);
		dir = end - start;
		distance = dir.normalize();
		memcpy(result.end, queryDesc.end, sizeof(float)*3);
	}
		
	result.flag = WeHitFlag::TRACE_NONE;
	result.fraction = 1.0f;

	PxQueryFilterData filterData;
	filterData.flags = PxQueryFlag::ePOSTFILTER | PxQueryFlag::eANY_HIT;
	PxQueryFilterCallback* filterCallback = NULL;
	switch (flag)
	{
		case WeSceneQueryFlag::QUERY_WORLD:
			filterData.flags |= PxQueryFlag::eSTATIC;
			mQueryFilterCallback->setQuery(&queryDesc, &result);
			filterCallback = mQueryFilterCallback;
			break;
		case WeSceneQueryFlag::QUERY_OTHER_PLAYER:
			filterData.flags |= PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;
			mSelfFilterCallback->setQuery(&queryDesc, &result);
			filterCallback = mSelfFilterCallback;
			break;
		case WeSceneQueryFlag::QUERY_ALL_PLAYER:
			filterData.flags |= PxQueryFlag::eDYNAMIC;
			mQueryFilterCallback->setQuery(&queryDesc, &result);
			filterCallback = mQueryFilterCallback;
			break;
		case WeSceneQueryFlag::QUERY_EXCEPT_SELF:
			filterData.flags |= PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;
			mSelfFilterCallback->setQuery(&queryDesc, &result);
			filterCallback = mSelfFilterCallback;
			break;
		case WeSceneQueryFlag::QUERY_ALL:
			filterData.flags |= PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
			mQueryFilterCallback->setQuery(&queryDesc, &result);
			filterCallback = mQueryFilterCallback;
			break;
		default:
			return false;
	}

	switch (queryType)
	{
		case WeQueryType::QUERY_RAYRECAST:	
		{
			PxRaycastBuffer hit;
			if (!mScene->raycast(start, dir, distance, hit, PxHitFlag::eDEFAULT, filterData, filterCallback)){
				return true;
			}
				
			return setQueryResult(queryType, hit.block, dir, distance, result);
			break;
		}
		case WeQueryType::QUERY_SWEEP:
		{
			PxBoxGeometry boxGeom(PxVec3(queryDesc.bound[0] / 2, queryDesc.bound[1] / 2, queryDesc.bound[2] / 2));
			PxTransform pose(PxIdentity);
			pose.p = PxVec3(start[0], start[1], start[2]);
			PxSweepBuffer hit;
			if (!mScene->sweep(boxGeom, pose, dir, distance, hit, PxHitFlag::eDEFAULT, filterData, filterCallback)){
				return true;
			}
				
			return setQueryResult(queryType, hit.block, dir, distance, result);
			break;
		}
		case WeQueryType::QUERY_OVERLAP:
		{
			PxBoxGeometry boxGeom(PxVec3(queryDesc.bound[0] / 2, queryDesc.bound[1] / 2, queryDesc.bound[2] / 2));
			PxTransform pose(PxIdentity);
			pose.p = PxVec3(start[0], start[1], start[2]);
			PxOverlapBuffer hit;
			if (mScene->overlap(boxGeom, pose, hit, filterData, filterCallback))
				result.flag = WeHitFlag::TRACE_OVERLAP;
			return true;
		}
		default:
			return false;
	}

}

bool WeScene::setQueryResult(WeQueryType::Enum queryType, PxLocationHit& hit, PxVec3& dir, float distance, WeQueryResult& result)
{
	if (hit.flags & PxHitFlag::eDEFAULT)
	{
		result.fraction = hit.distance / distance;
		if (queryType == WeQueryType::QUERY_SWEEP)
		{
			PxVec3 offset = dir*(-hit.distance);
			result.end[0] = result.end[0] + offset[0];
			result.end[1] = result.end[1] + offset[1];
			result.end[2] = result.end[2] + offset[2];
		}
		else
		{
			result.end[0] = hit.position[0];
			result.end[1] = hit.position[1];
			result.end[2] = hit.position[2];
		}
		

		/*
		in the case of the raycast start point originally inside the
		solid shape, the hit normal is set to the opposition of
		raycast direction!
		*/
		result.hitNormal[0] = hit.normal[0];
		result.hitNormal[1] = hit.normal[1];
		result.hitNormal[2] = hit.normal[2];

		if (hit.hadInitialOverlap())
		{
			if (result.flag == WeHitFlag::TRACE_END_SOLID)
			{
				result.flag = WeHitFlag::TRACE_ALL_SOLID;
			}
			else
				result.flag = WeHitFlag::TRACE_START_SOLID;
		}

		return true;
	}
	return false;
}




