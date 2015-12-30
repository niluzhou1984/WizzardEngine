#include<stdlib.h>
#include<time.h>
#include "WeScene.h"
#include "We.h"

static const float WE_DEFAULT_MATERIALINFO[3] = { 0.5f, 0.5f, 0.1f };

void* weCreateScene(WeSceneDesc* sceneDesc)
{


	WeMaterialInfo materialInfo;
	materialInfo.staticFriction = sceneDesc->staticFriction;
	materialInfo.dynamicFriction = sceneDesc->dynamicFriction;
	materialInfo.restitution = sceneDesc->restitution;

	WeScene* scene = new WeScene(sceneDesc->flag, sceneDesc->gravity, materialInfo);
	
	if (scene != NULL)
	{
		scene->initScene();
		WE_INFO_HANDLE(WeErrorCode::INFO_DEBUG, "Init Scene End!");
		if (scene->getError())
		{
			return NULL;
		}

#ifdef __CROSSBRIDGE__
		scene->buildSceneWithMesh(sceneDesc->rawMesh, sceneDesc->rawMeshLen);
#else
		scene->buildSceneWithMesh(sceneDesc->meshFile);
#endif

		WeMaterialInfo bodyMaterialInfo;
		bodyMaterialInfo.staticFriction = sceneDesc->bodyStaticFriction;
		bodyMaterialInfo.dynamicFriction = sceneDesc->bodyDynamicFriction;
		bodyMaterialInfo.restitution = sceneDesc->bodyRestitution;
		scene->setBodyMaterial(bodyMaterialInfo);

		if (scene->getError()){
			WE_INFO_HANDLE(WeErrorCode::INFO_DEBUG, "buildSceneWithMesh failed!");
			weFreeScene(scene);
			return NULL;
		}
	}
		
	return scene;
}

void weFreeScene(void* scene)
{
	delete ((WeScene*)scene);
}



bool weGetError(void* scene)
{
	if (!scene)
		return true;

	return ((WeScene*)scene)->getError();
}

bool weQuery(void* scene, WeQueryType::Enum queryType, WeQueryDesc* queryDesc, WeQueryResult* queryResult)
{
	bool res = false;
	if (queryType == WeQueryType::QUERY_OVERLAP ||
		queryType == WeQueryType::QUERY_SWEEP)
	{
		//adjust to center of the mesh
		WeVec3 start(queryDesc->start[0], queryDesc->start[1], queryDesc->start[2]);

		WeVec3 min(queryDesc->min[0], queryDesc->min[1], queryDesc->min[2]);
		WeVec3 max(queryDesc->max[0], queryDesc->max[1], queryDesc->max[2]);

		WeVec3 offset, bound;
		weVecAdd(min, max, offset);
		weVecScale(offset, 0.5f);
		weVecAdd(start, offset, start);
		weVecSub(max, min, bound);

		memcpy(queryDesc->start, &start, sizeof(WeVec3));
		memcpy(queryDesc->bound, &bound, sizeof(WeVec3));

		if (queryType == WeQueryType::QUERY_SWEEP){
			WeVec3 end(queryDesc->end[0], queryDesc->end[1], queryDesc->end[2]);
			weVecAdd(end, offset, end);
			memcpy(queryDesc->end, &end, sizeof(WeVec3));
		}
		res = ((WeScene*)scene)->query(queryType, *queryDesc, *queryResult);

		if (res)
		{
			queryResult->end[0] -= offset[0];
			queryResult->end[1] -= offset[1];
			queryResult->end[2] -= offset[2];
		}
	}
	else{
		res = ((WeScene*)scene)->query(queryType, *queryDesc, *queryResult);
	}

	
	return res;
}

const float* weGetDefaultMaterialInfo()
{
	return WE_DEFAULT_MATERIALINFO;
}

bool weAddDynamicEntity(void* scene, int index, const float pos[3], const float bound[3], const float materialInfo[3])
{
	WeVec3 p, b, m;
	memcpy(&p, pos, sizeof(WeVec3));
	memcpy(&b, bound, sizeof(WeVec3));
	memcpy(&m, materialInfo, sizeof(WeVec3));
	return ((WeScene*)scene)->addDynamicBody(index, p, b, m);
}
bool weSetDynamicEntityBound(void* scene, int index, const float bound[3])
{
	WeVec3 b;
	memcpy(&b, bound, sizeof(WeVec3));
	return ((WeScene*)scene)->setDynamicBodyBound(index, b);
}

bool weRemoveDynamicEntity(void* scene, int index)
{
	return ((WeScene*)scene)->removeDynamicBody(index);
}

bool weMoveDynamicEntity(void* scene, int index, const float pos[3], bool simulate)
{
	WeVec3 p;
	memcpy(&p, pos, sizeof(WeVec3));

	return ((WeScene*)scene)->moveDynamicBody(index, p, simulate);
}

bool weGetDynamicEntityPos(void* scene, int index, float pos[3])
{
	WeVec3 p;
	if (((WeScene*)scene)->getDynamicBodyPos(index, p))
	{
		memcpy(pos, &p, sizeof(WeVec3));
		return true;
	}
	
	return false;
}
