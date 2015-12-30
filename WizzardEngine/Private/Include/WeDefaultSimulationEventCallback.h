#ifndef WE_DEFAULT_SIMULATION_EVENT_CALLBACK_H
#define WE_DEFAULT_SIMULATION_EVENT_CALLBACK_H
#include "PxSimulationEventCallback.h"
using namespace physx;

class WeDefaultSimulationEventCallback : public PxSimulationEventCallback
{

public:
	WeDefaultSimulationEventCallback(){};
	~WeDefaultSimulationEventCallback(){};
	virtual	void	onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs){};
	virtual	void	onTrigger(PxTriggerPair*, PxU32) {}
	virtual void	onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void	onWake(PxActor**, PxU32) {}
	virtual void	onSleep(PxActor**, PxU32){}
};

#endif