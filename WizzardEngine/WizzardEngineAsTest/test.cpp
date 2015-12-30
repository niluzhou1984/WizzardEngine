#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../Public/Include/WeCommon.h"
#include "../../Public/Include/We.h"

unsigned char* loadMeshRawData(int &len)
{
	FILE* fp = fopen("./scene47.bsp", "rb");
	if (!fp){
		printf("Can not open mesh file!\n");
		return NULL;
	}
	
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	unsigned char* data = new unsigned char[len];
	if (!data) {
		printf("Null mesh data!\n");
		fclose(fp);
		return false;
	}
	fseek(fp, 0L, SEEK_SET);
		
	int readLen = fread(data, len, 1, fp);
	if (readLen != 1)
	{
		delete data;
		data = NULL;
		fclose(fp);
		return false;
	}

	fclose(fp);

	return data;
}

int main()
{
	
	WeSceneDesc desc;
	desc.staticFriction = 0.5f;
	desc.dynamicFriction = 0.5f;
	desc.restitution = 0.1f;
	desc.bodyStaticFriction = 0.5f;
	desc.bodyDynamicFriction = 0.5f;
	desc.bodyRestitution = 0.1f;
	desc.gravity = 2034;
	desc.flag = WeSceneFlag::SCENE_NO_THREAD;
	desc.rawMesh = loadMeshRawData(desc.rawMeshLen);

	void* scene = weCreateScene(&desc);

	if(!scene)
	{
		printf("NULL  Scenee \n");
		return 0;
	}
	
	if (weGetError(scene))
	{
		printf("Error Create Scene!\n");
	}else{
		printf("CreateScene OK!\n");
		
	}
	fflush(stdout);
	
	WeQueryDesc queryDesc;
	
	queryDesc.start[0] = 6575.555837213967;
	queryDesc.start[1] = 6000.80062605554;
	queryDesc.start[2] = 434.41999511718745;
	queryDesc.end[0] = 6565.027187608346;
	queryDesc.end[1] = 6200.085216733808;
	queryDesc.end[2] = 434.41999511718745;

	queryDesc.flag = WeSceneQueryFlag::QUERY_WORLD;
	
	/*
	WeQueryResult result;
	if (weQuery(scene, WeQueryType::QUERY_RAYRECAST, &queryDesc, &result))
	{
		printf("Recast Query Recast Success: %d %f %f %f %f\n", result.flag, result.fraction, result.end[0], result.end[1], result.end[2]);
	}
	else{
		printf("Recast Query Recast Failed!\n");
	}
	fflush(stdout);
	*/
	queryDesc.min[0] = -45;
	queryDesc.min[1] = -45;
	queryDesc.min[2] = 0;

	queryDesc.max[0] = 45;
	queryDesc.max[1] = 45;
	queryDesc.max[2] = 170;

	WeQueryResult result2;
	printf("Start Query Test!\n");
	if (weQuery(scene, WeQueryType::QUERY_SWEEP, &queryDesc, &result2))
	{
		printf("Sweep Query Sweep Success:%d, %f %f %f %f\n", result2.flag, result2.fraction, result2.end[0], result2.end[1], result2.end[2]);
	}
	else
	{
		printf("Sweep Query Sweep Failed!\n");
	}
	fflush(stdout);
	
	queryDesc.start[0] = 6575.555837213967;
	queryDesc.start[1] = 6000.80062605554;
	queryDesc.start[2] = 434.41999511718745;
	queryDesc.end[0] = 6565.027187608346;
	queryDesc.end[1] = 6020.085216733808;
	queryDesc.end[2] = 434.41999511718745;

	WeQueryResult result3;
	printf("Start Query Test!\n");
	if (weQuery(scene, WeQueryType::QUERY_SWEEP, &queryDesc, &result2))
	{
		printf("Sweep Query Sweep Success:%d, %f %f %f %f\n", result2.flag, result2.fraction, result2.end[0], result2.end[1], result2.end[2]);
	}
	else
	{
		printf("Sweep Query Sweep Failed!\n");
	}
	fflush(stdout);
	
/*
	queryDesc.start[0] = 6000;
	queryDesc.start[1] = 4500;
	queryDesc.start[2] = 800;
	WeQueryResult result4;
	if (weQuery(scene, WeQueryType::QUERY_OVERLAP, &queryDesc, &result4))
	{
		printf("Overlap Query Success:%d\n", result4.flag);
	}
	else
	{
		printf("Overlap Query Failed!");
	}
	fflush(stdout);
	
	weFreeScene(scene);
*/
	return 0;
}