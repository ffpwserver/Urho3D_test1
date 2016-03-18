#pragma once

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>

using namespace Urho3D;

class RagdollComponent : public Component
{
	URHO3D_OBJECT(RagdollComponent, Component);

public:
	RagdollComponent(Context* context);

protected:
	virtual void OnNodeSet(Node* node) override;

	virtual void HandleUpdate(StringHash eventType, VariantMap& eventData);

private:
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

	void CreateRagdollBone(const String& boneName, ShapeType type, const Vector3& size, const Vector3& pos, const Quaternion& rotation);

	void CreateRagdollConstraint(const String& boneName, const String& parentName, ConstraintType type, const Vector3& axis, const Vector3& parentAxis, const Vector2& highLimit, const Vector2& lowLimit, bool disableCollision = true);
};



