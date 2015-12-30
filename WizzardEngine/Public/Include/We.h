#ifndef WE_H
#define WE_H
#include "WeCommon.h"

void* weCreateScene(WeSceneDesc* sceneDesc);
void weFreeScene(void* scene);

bool weGetError(void* scene);

bool weQuery(void* scene, WeQueryType::Enum queryType, WeQueryDesc* queryDesc, WeQueryResult* queryResult);

bool weAddDynamicEntity(void* scene, int index, const float pos[3], const float bound[3], const float materialInfo[3]);
bool weSetDynamicEntityBound(void* scene, int index, const float bound[3]);
bool weRemoveDynamicEntity(void* scene, int index);
bool weMoveDynamicEntity(void* scene, int index, const float pos[3], bool simulate = false);
bool weGetDynamicEntityPos(void* scene, int index, float pos[3]);


const float* weGetDefaultMaterialInfo();

#endif