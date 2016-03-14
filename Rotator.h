#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/DebugNew.h>

using namespace Urho3D;

class Rotator : public LogicComponent
{
	URHO3D_OBJECT(Rotator, LogicComponent);

public:
	Rotator(Context* context);

	void SetRotationSpeed(const Vector3& speed) 
	{
		rotationSpeed_ = speed;
	}

	virtual void Update(float timeStep);

	const Vector3& GetRotationSpeed() const 
	{
		return rotationSpeed_;
	}

private:
	Vector3 rotationSpeed_;
};