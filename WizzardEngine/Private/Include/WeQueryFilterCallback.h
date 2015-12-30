#ifndef WE_QUERY_FILTER_CALLBACK_H
#define WE_QUERY_FILTER_CALLBACK_H

#include "WeCommon.h"
#include "PxQueryFiltering.h"

using namespace physx;
class WeScene;

class WeQueryFilterCallback : public PxQueryFilterCallback
{
public:
	WeQueryFilterCallback(WeScene* scene);
	virtual ~WeQueryFilterCallback(){};

	void setQuery(WeQueryDesc* queryDesc, WeQueryResult* result);

	virtual PxQueryHitType::Enum preFilter(
		const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags);

	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit);

protected:
	WeQueryDesc*	mQueryDesc;
	WeQueryResult*	mQueryResult;
	WeScene*		mScene;
};


#endif