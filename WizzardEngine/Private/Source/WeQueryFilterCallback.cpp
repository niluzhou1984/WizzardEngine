#include "geometry/PxGeometryQuery.h"
#include "WeScene.h"
#include "WeQueryFilterCallback.h"

WeQueryFilterCallback::WeQueryFilterCallback(WeScene* scene):
	mScene(scene)
{

}

PxQueryHitType::Enum WeQueryFilterCallback::preFilter(
	const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	return PxSceneQueryHitType::eBLOCK;
}


void WeQueryFilterCallback::setQuery(WeQueryDesc* queryDesc, WeQueryResult* result)
{
	mQueryDesc = queryDesc;
	mQueryResult = result;
}

//check if end solid
PxQueryHitType::Enum WeQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	PxVec3 end(mQueryDesc->end[0], mQueryDesc->end[1], mQueryDesc->end[2]);
	PxShape* shape = hit.shape;
	PxGeometryHolder goemetry = shape->getGeometry();
	
	//test whether the end point inside the geometry
	PxTransform globalPos = hit.actor->getGlobalPose();
	PxReal distance = PxGeometryQuery::pointDistance(end, goemetry.any(), globalPos.transform(shape->getLocalPose()));
	if (distance == 0)
	{
		mQueryResult->flag = WeHitFlag::TRACE_END_SOLID;
	}

	return PxQueryHitType::eBLOCK;
}