#include "extensions/PxDefaultStreams.h"
#include "WeInternCommon.h"
#include "WeBspReader.h"
#include "WeMeshLoader.h"

using namespace WeBSPParse;

WeMeshLoader::WeMeshLoader():
	mMeshes(NULL)
{
	
}
WeMeshLoader::~WeMeshLoader()
{
	if (mMeshes)
		mMeshes->release();
}
#ifdef __CROSSBRIDGE__
bool WeMeshLoader::createConvexMesh(PxCooking& cooking, PxPhysics& physics, const unsigned char* rawMesh, int len)
#else
bool WeMeshLoader::createConvexMesh(PxCooking& cooking, PxPhysics& physics, const char* fileName)
#endif
{
	WeBspReader reader;

#ifdef __CROSSBRIDGE__
	if (!reader.readRawMesh(rawMesh, len))
		return false;
#else
	if (!reader.readRawMesh(fileName))
		return false;
#endif

	WeRawMeshData& meshData = reader.getRawMeshData();
	if (!mMeshes)
		mMeshes = PxCreateCollection();
	
	const int VERTEX_LIMITS = 256;
	for (int i = 0; i < meshData.convexNum; i++)
	{
		WeConvex& convex = meshData.convexities[i];
		PxVec3 vertices[VERTEX_LIMITS];
		int vertexNum = 0;
		bool sucess = true;
		for (int j = 0; j < convex.faceNum; j++)
		{
			WeFace& face = meshData.faces[convex.faces[j]];
			for (int k = 0; k < face.vertexNum; k++)
			{
				if (vertexNum >= VERTEX_LIMITS)
				{
					sucess = false;
					break;
				}
				PxVec3& v = vertices[vertexNum++];
				WeVec3& rawV = meshData.vertices[face.indices[k]];
				v.x = rawV[0];
				v.y = rawV[1];
				v.z = rawV[2];
			}
			if (!sucess){
				break;
			}
		}

		if (!sucess)
			continue;

		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = vertexNum;
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = vertices;
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
		convexDesc.vertexLimit = VERTEX_LIMITS;
		PxDefaultMemoryOutputStream buf;
		if (cooking.cookConvexMesh(convexDesc, buf))
		{
			PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			PxConvexMesh*  convexMesh = physics.createConvexMesh(id);
		
			mMeshes->add(*convexMesh);
		}
		else{
			WE_DEBUG_HANDLE(WeErrorCode::WARN_CONVEX_COOKING, "Can Not Cooking Convex");
		}
	}

	return true;
}