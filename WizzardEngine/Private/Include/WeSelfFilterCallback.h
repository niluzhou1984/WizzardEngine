#ifndef WE_SELF_FILTER_CALLBACK_H
#define WE_SELF_FILTER_CALLBACK_H
#include "WeQueryFilterCallback.h"

class WeScene;
class WeSelfFilterCallback : public WeQueryFilterCallback
{
public:
	WeSelfFilterCallback(WeScene* scene);
	virtual ~WeSelfFilterCallback(){};

	virtual PxQueryHitType::Enum preFilter(
		const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags);
};


#endif