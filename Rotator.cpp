
#include <Urho3D/Scene/Node.h>

#include "Rotator.h"

Rotator::Rotator(Context* context)
: LogicComponent(context),
rotationSpeed_(Vector3::ZERO)
{
	SetUpdateEventMask(USE_UPDATE);
}

void Rotator::Update(float timeStep)
{
	node_->Rotate(Quaternion(rotationSpeed_.x_ * timeStep, rotationSpeed_.y_ * timeStep, rotationSpeed_.z_ * timeStep));
}
