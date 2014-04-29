#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <PxPhysicsAPI.h>
#include "PxSimulationEventCallback.h"

using namespace physx;

class Renderable  {
public:
    virtual void draw() const = 0;
    virtual QVector<PxVec3> getVerts() = 0;
    virtual QVector<PxU32> getInds() = 0;
};

#endif // RENDERABLE_H
