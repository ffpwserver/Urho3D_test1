
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Core/CoreEvents.h>

#include "RagdollComponent.h"

#include <Urho3D/DebugNew.h>

RagdollComponent::RagdollComponent(Context* context)
: Component(context)
{
}

void RagdollComponent::OnNodeSet(Node* node)
{
    if (node)
    {
        SubscribeToEvent(node, E_NODECOLLISION, URHO3D_HANDLER(RagdollComponent, HandleNodeCollision));

        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(RagdollComponent, HandleUpdate));
    }
}

void RagdollComponent::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    Input* input = GetSubsystem<Input>();
    if (input->GetKeyDown('L'))
    {
        node_->RemoveComponent<RigidBody>();
        node_->RemoveComponent<CollisionShape>();

        // Create Custom Rigid body for each bones
        CreateRagdollBone("Bip01_Pelvis", SHAPE_BOX, Vector3(0.3f, 0.2f, 0.25f), Vector3(0.0f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 0.0f));
        CreateRagdollBone("Bip01_Spine1", SHAPE_BOX, Vector3(0.35f, 0.2f, 0.3f), Vector3(0.15f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 0.0f));
        CreateRagdollBone("Bip01_L_Thigh", SHAPE_CAPSULE, Vector3(0.175f, 0.45f, 0.175f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_Thigh", SHAPE_CAPSULE, Vector3(0.175f, 0.45f, 0.175f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_L_Calf", SHAPE_CAPSULE, Vector3(0.15f, 0.55f, 0.15f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_Calf", SHAPE_CAPSULE, Vector3(0.15f, 0.55f, 0.15f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_Head", SHAPE_BOX, Vector3(0.2f, 0.2f, 0.2f), Vector3(0.1f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 0.0f));
        CreateRagdollBone("Bip01_L_UpperArm", SHAPE_CAPSULE, Vector3(0.15f, 0.35f, 0.15f), Vector3(0.1f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_UpperArm", SHAPE_CAPSULE, Vector3(0.15f, 0.35f, 0.15f), Vector3(0.1f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_L_Forearm", SHAPE_CAPSULE, Vector3(0.125f, 0.4f, 0.125f), Vector3(0.2f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_Forearm", SHAPE_CAPSULE, Vector3(0.125f, 0.4f, 0.125f), Vector3(0.2f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));

        // Create Constraints between bones
        CreateRagdollConstraint("Bip01_L_Thigh", "Bip01_Pelvis", CONSTRAINT_CONETWIST, Vector3::BACK, Vector3::FORWARD,
            Vector2(45.0f, 45.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_R_Thigh", "Bip01_Pelvis", CONSTRAINT_CONETWIST, Vector3::BACK, Vector3::FORWARD,
            Vector2(45.0f, 45.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_L_Calf", "Bip01_L_Thigh", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_R_Calf", "Bip01_R_Thigh", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_Spine1", "Bip01_Pelvis", CONSTRAINT_HINGE, Vector3::FORWARD, Vector3::FORWARD,
            Vector2(45.0f, 0.0f), Vector2(-10.0f, 0.0f));
        CreateRagdollConstraint("Bip01_Head", "Bip01_Spine1", CONSTRAINT_CONETWIST, Vector3::LEFT, Vector3::LEFT,
            Vector2(0.0f, 30.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_L_UpperArm", "Bip01_Spine1", CONSTRAINT_CONETWIST, Vector3::DOWN, Vector3::UP,
            Vector2(45.0f, 45.0f), Vector2::ZERO, false);
        CreateRagdollConstraint("Bip01_R_UpperArm", "Bip01_Spine1", CONSTRAINT_CONETWIST, Vector3::DOWN, Vector3::UP,
            Vector2(45.0f, 45.0f), Vector2::ZERO, false);
        CreateRagdollConstraint("Bip01_L_Forearm", "Bip01_L_UpperArm", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_R_Forearm", "Bip01_R_UpperArm", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);

        // disable all keyframe animation
        AnimatedModel* model = GetComponent<AnimatedModel>();
        Skeleton& skeleton = model->GetSkeleton();
        for (unsigned i = 0; i < skeleton.GetNumBones(); ++i)
            skeleton.GetBone(i)->animated_ = false;

        // remove self from scene node
        Remove();
    }
}

void RagdollComponent::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    using namespace NodeCollision;

    RigidBody* otherBody = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());

    if (otherBody->GetMass() > 0.0f)
    {
        node_->RemoveComponent<RigidBody>();
        node_->RemoveComponent<CollisionShape>();

        // Create Custom Rigid body for each bones
        CreateRagdollBone("Bip01_Pelvis", SHAPE_BOX, Vector3(0.3f, 0.2f, 0.25f), Vector3(0.0f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 0.0f));
        CreateRagdollBone("Bip01_Spine1", SHAPE_BOX, Vector3(0.35f, 0.2f, 0.3f), Vector3(0.15f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 0.0f));
        CreateRagdollBone("Bip01_L_Thigh", SHAPE_CAPSULE, Vector3(0.175f, 0.45f, 0.175f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_Thigh", SHAPE_CAPSULE, Vector3(0.175f, 0.45f, 0.175f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_L_Calf", SHAPE_CAPSULE, Vector3(0.15f, 0.55f, 0.15f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_Calf", SHAPE_CAPSULE, Vector3(0.15f, 0.55f, 0.15f), Vector3(0.25f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_Head", SHAPE_BOX, Vector3(0.2f, 0.2f, 0.2f), Vector3(0.1f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 0.0f));
        CreateRagdollBone("Bip01_L_UpperArm", SHAPE_CAPSULE, Vector3(0.15f, 0.35f, 0.15f), Vector3(0.1f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_UpperArm", SHAPE_CAPSULE, Vector3(0.15f, 0.35f, 0.15f), Vector3(0.1f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_L_Forearm", SHAPE_CAPSULE, Vector3(0.125f, 0.4f, 0.125f), Vector3(0.2f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));
        CreateRagdollBone("Bip01_R_Forearm", SHAPE_CAPSULE, Vector3(0.125f, 0.4f, 0.125f), Vector3(0.2f, 0.0f, 0.0f),
            Quaternion(0.0f, 0.0f, 90.0f));

        // Create Constraints between bones
        CreateRagdollConstraint("Bip01_L_Thigh", "Bip01_Pelvis", CONSTRAINT_CONETWIST, Vector3::BACK, Vector3::FORWARD,
            Vector2(45.0f, 45.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_R_Thigh", "Bip01_Pelvis", CONSTRAINT_CONETWIST, Vector3::BACK, Vector3::FORWARD,
            Vector2(45.0f, 45.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_L_Calf", "Bip01_L_Thigh", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_R_Calf", "Bip01_R_Thigh", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_Spine1", "Bip01_Pelvis", CONSTRAINT_HINGE, Vector3::FORWARD, Vector3::FORWARD,
            Vector2(45.0f, 0.0f), Vector2(-10.0f, 0.0f));
        CreateRagdollConstraint("Bip01_Head", "Bip01_Spine1", CONSTRAINT_CONETWIST, Vector3::LEFT, Vector3::LEFT,
            Vector2(0.0f, 30.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_L_UpperArm", "Bip01_Spine1", CONSTRAINT_CONETWIST, Vector3::DOWN, Vector3::UP,
            Vector2(45.0f, 45.0f), Vector2::ZERO, false);
        CreateRagdollConstraint("Bip01_R_UpperArm", "Bip01_Spine1", CONSTRAINT_CONETWIST, Vector3::DOWN, Vector3::UP,
            Vector2(45.0f, 45.0f), Vector2::ZERO, false);
        CreateRagdollConstraint("Bip01_L_Forearm", "Bip01_L_UpperArm", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);
        CreateRagdollConstraint("Bip01_R_Forearm", "Bip01_R_UpperArm", CONSTRAINT_HINGE, Vector3::BACK, Vector3::BACK,
            Vector2(90.0f, 0.0f), Vector2::ZERO);

        // disable all keyframe animation
        AnimatedModel* model = GetComponent<AnimatedModel>();
        Skeleton& skeleton = model->GetSkeleton();
        for (unsigned i = 0; i < skeleton.GetNumBones(); ++i)
            skeleton.GetBone(i)->animated_ = false;

        // remove self from scene node
        Remove();
    }
}

void RagdollComponent::CreateRagdollBone(const String& boneName, ShapeType type, const Vector3& size, const Vector3& pos, const Quaternion& rotation)
{
    Node* boneNode = node_->GetChild(boneName, true);
    if (!boneNode)
    {
        URHO3D_LOGWARNING("Could not find bone " + boneName + " for creating ragdoll pyhsics components");
        return;
    }

    RigidBody* body = boneNode->CreateComponent<RigidBody>();
    body->SetMass(1.0f);
    body->SetLinearDamping(0.05f);
    body->SetAngularDamping(0.85f);
    body->SetLinearRestThreshold(1.5f);
    body->SetAngularRestThreshold(2.5f);

    CollisionShape* shape = boneNode->CreateComponent<CollisionShape>();
    if (SHAPE_BOX == type)
        shape->SetBox(size, pos, rotation);
    else
        shape->SetCapsule(size.x_, size.y_, pos, rotation);
}

void RagdollComponent::CreateRagdollConstraint(const String& boneName, const String& parentName, ConstraintType type,
 const Vector3& axis, const Vector3& parentAxis, const Vector2& highLimit, const Vector2& lowLimit, 
 bool disableCollision)
{
    Node* boneNode = node_->GetChild(boneName, true);
    Node* parentNode = node_->GetChild(parentName, true);

    if (!boneNode)
    {
        URHO3D_LOGWARNING("AAA");
        return;
    }
    if (!parentNode)
    {
        URHO3D_LOGWARNING("BBB");
        return;
    }

    Constraint* constraint = boneNode->CreateComponent<Constraint>();
    constraint->SetConstraintType(type);
    constraint->SetDisableCollision(disableCollision);
    constraint->SetOtherBody(parentNode->GetComponent<RigidBody>());
    constraint->SetWorldPosition(boneNode->GetWorldPosition());
    constraint->SetAxis(axis);
    constraint->SetOtherAxis(parentAxis);
    constraint->SetHighLimit(highLimit);
    constraint->SetLowLimit(lowLimit);
}








