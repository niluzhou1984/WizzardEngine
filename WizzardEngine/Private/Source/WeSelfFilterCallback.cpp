#include "WeScene.h"
#include "WeSelfFilterCallback.h"

WeSelfFilterCallback::WeSelfFilterCallback(WeScene* scene):
	WeQueryFilterCallback(scene)
{

}

PxQueryHitType::Enum WeSelfFilterCallback::preFilter(
	const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{

	if (actor->isRigidDynamic())
	{
		int bodyIndex = filterData.word0;

		if (bodyIndex == mQueryDesc->entityIndex)
		{
			return PxQueryHitType::eNONE;
		}
	}

	return PxQueryHitType::eBLOCK;
}