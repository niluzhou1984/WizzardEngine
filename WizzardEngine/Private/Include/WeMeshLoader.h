#ifndef WE_MESH_LOADER_H
#define WE_MESH_LOADER_H

#include "PxPhysics.h"
#include "PxCollection.h"
#include "cooking/PxCooking.h"

using namespace physx;
class WeMeshLoader
{


public:
	WeMeshLoader();
	~WeMeshLoader();
#ifdef __CROSSBRIDGE__
	bool createConvexMesh(PxCooking& cooking, PxPhysics& physics, const unsigned char* rawMesh, int len);
#else
	bool createConvexMesh(PxCooking& cooking, PxPhysics& physics, const char* fileName);
#endif
	PxCollection* getConvexMeshes(){ return mMeshes; }

private:
	PxCollection*  mMeshes;
};


#endif