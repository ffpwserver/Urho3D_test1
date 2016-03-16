//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// #include <Urho3D/Engine/Console.h>
// #include <Urho3D/UI/Cursor.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/DebugHud.h>
// #include <Urho3D/IO/FileSystem.h>

#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/RenderSurface.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/Texture2D.h>
// #include <Urho3D/Scene/SceneEvents.h>
// #include <Urho3D/UI/Sprite.h>
// #include <Urho3D/Core/Timer.h>
// #include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>

#include <Urho3D/DebugNew.h>

#include "HelloWorld.h"
#include "Rotator.h"

// Expands to this example's entry-point
URHO3D_DEFINE_APPLICATION_MAIN(HelloWorld)

HelloWorld::HelloWorld(Context* context)
: Application(context)
, yaw_(0.0f)
, pitch_(0.0f)
, roll_(0.0f)
, drawDebug_(false)
{
    context->RegisterFactory<Rotator>();
}

void HelloWorld::Setup()
{
    Application::Setup();

    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["FullScreen"]  = false;
    engineParameters_["Headless"]    = false;
    engineParameters_["Sound"]       = false;

    // Construct a search path to find the resource prefix with two entries:
    // The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
    // The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
    if (!engineParameters_.Contains("ResourcePrefixPaths"))
        engineParameters_["ResourcePrefixPaths"] = ";../share/Resources;../share/Urho3D/Resources";
}

void HelloWorld::Start()
{
    Application::Start();

    CreateScene();

    CreateSkybox();

    CreateFloor();

    CreateFloor2();

    CreatePyramid();

    CreateStaticModel();

    CreateBillboards();

    CreateLights();

    CreateCamera();

    SetupViewport();

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloWorld, HandleUpdate));

    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(HelloWorld, HandlePostRenderUpdate));
}

void HelloWorld::Stop()
{
    engine_->DumpResources(true);
}

void HelloWorld::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    {
        // create scene for render to texture
        rttScene_ = new Scene(context_);

        rttScene_->CreateComponent<Octree>();

        Node* zoneNode = rttScene_->CreateChild("Zone");
        Zone* zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(0.05f, 0.1f, 0.15f));
        zone->SetFogColor(Color(0.1f, 0.2f, 0.3f));
        zone->SetFogStart(10.0f);
        zone->SetFogEnd(100.0f);

        const unsigned NUM_OBJECTS = 2000;
        for (unsigned i = 0; i < NUM_OBJECTS; ++i)
        {
            Node* boxNode = rttScene_->CreateChild("Box");
            boxNode->SetPosition(Vector3(Random(200.0f) - 100.0f, Random(200.0f) - 100.0f, Random(200.0f) - 100.0f));
            boxNode->SetRotation(Quaternion(Random(360.0f), Random(360.0f), Random(360.0f)));
            StaticModel* box = boxNode->CreateComponent<StaticModel>();
            box->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            box->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

            Rotator* rotator = boxNode->CreateComponent<Rotator>();
            rotator->SetRotationSpeed(Vector3(Random(20.0f), Random(20.0f), Random(20.0f)));
        }

        rttCameraNode_ = rttScene_->CreateChild("Camera");
        Camera* camera = rttCameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(100.0f);

        Light* light = rttCameraNode_->CreateComponent<Light>();
        light->SetLightType(LIGHT_POINT);
        light->SetRange(30.0f);
    }

    {
        // create main scene
        scene_ = new Scene(context_);

        // create octree and debug renderer for scene
        scene_->CreateComponent<Octree>();
        scene_->CreateComponent<DebugRenderer>();
        scene_->CreateComponent<PhysicsWorld>();

        Node* zoneNode = scene_->CreateChild("Zone");
        Zone* zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));

        Node* lightNode = scene_->CreateChild("DirectionalLight");
        lightNode->SetDirection(Vector3(0.5f, -1.0f, 0.5f));
        Light* light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetColor(Color(1.0f, 1.0f, 1.0f));
        light->SetSpecularIntensity(1.0f);

        {
            // screen's "stone" background
            Node* boxNode = scene_->CreateChild("ScreenBox");
            boxNode->SetPosition(Vector3(0.0f, 10.0f, 0.0f));
            boxNode->SetScale(Vector3(21.0f, 16.0f, 0.5f));
            StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
            boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            boxObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

            // plane for render
            Node* screenNode = scene_->CreateChild("Screen");
            screenNode->SetPosition(Vector3(0.0f, 10.0f, -0.27f));
            screenNode->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
            screenNode->SetScale(Vector3(20.0f, 0.0f, 15.0f));
            StaticModel* screenObject = screenNode->CreateComponent<StaticModel>();
            screenObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

            // real render texture for object
            SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
            renderTexture->SetSize(1024, 768, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
            renderTexture->SetFilterMode(FILTER_BILINEAR);

            // make material from texture && bind this material to screenObj
            SharedPtr<Material> renderMaterial(new Material(context_));
            renderMaterial->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
            renderMaterial->SetTexture(TU_DIFFUSE, renderTexture);
            screenObject->SetMaterial(renderMaterial);

            // 
            RenderSurface* renderSurface = renderTexture->GetRenderSurface();
            SharedPtr<Viewport> rttViewport(new Viewport(context_, rttScene_, rttCameraNode_->GetComponent<Camera>()));
            renderSurface->SetViewport(0, rttViewport);
        }
    }
}

void HelloWorld::CreateFloor()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    for (int i = -10; i <= 10; ++i)
    {
        for (int j = -10; j <= 10; ++j)
        {
            Node* floorNode = scene_->CreateChild("FloorTile");
            floorNode->SetPosition(Vector3(j * 10.2f, -0.5f, i * 10.2f));
            floorNode->SetScale(Vector3(10.f, 1.f, 10.f));
            StaticModel* floorObj = floorNode->CreateComponent<StaticModel>();
            floorObj->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            floorObj->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
        }
    }
}

void HelloWorld::CreateFloor2()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* floorNode = scene_->CreateChild("Floor2");
    floorNode->SetPosition(Vector3(0.0f, 40.0f, 0.0f));
    floorNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
    floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    floorObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    RigidBody* body = floorNode->CreateComponent<RigidBody>();
    CollisionShape* shape = floorNode->CreateComponent<CollisionShape>();
    shape->SetBox(Vector3::ONE);
}

void HelloWorld::CreatePyramid()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    for (int y = 0; y < 20; ++y)
    {
        for (int x = -y; x <= y; ++x)
        {
            Node* boxNode = scene_->CreateChild("Box");
            boxNode->SetPosition(Vector3((float)x, -(float)y + 20.0f + 40.0f, 0.0f));
            StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
            boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneEnvMapSmall.xml"));
            boxObject->SetCastShadows(true);

            RigidBody* body = boxNode->CreateComponent<RigidBody>();
            body->SetMass(1.0f);
            body->SetFriction(0.2f);
            // body->SetCollisionEventMode(COLLISION_NEVER);
            CollisionShape* shape = boxNode->CreateComponent<CollisionShape>();
            shape->SetBox(Vector3::ONE);
        }
    }
}

void HelloWorld::CreateStaticModel()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* mushroomNode = scene_->CreateChild("Mushroom");
    mushroomNode->SetPosition(Vector3(20.f, 0.f, 20.f));
    mushroomNode->SetScale(8.f);

    StaticModel* mushroomObj = mushroomNode->CreateComponent<StaticModel>();
    mushroomObj->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
    mushroomObj->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
    mushroomObj->SetCastShadows(true);
}

void HelloWorld::CreateCamera()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // cameraNode_ = scene_->CreateChild("Camera");
    // create the camera node outside the scene because......
    cameraNode_ = new Node(context_);
    cameraNode_->SetPosition(Vector3(0.0f, 5.0f, -20.0f));
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(1000.0f);

    rearCameraNode_ = cameraNode_->CreateChild("RearCamera");
    rearCameraNode_->SetRotation(Quaternion(180.0f, Vector3::UP));
    Camera* rearCamera = rearCameraNode_->CreateComponent<Camera>();
    rearCamera->SetFarClip(200.0f);
    rearCamera->SetViewOverrideFlags(VO_LOW_MATERIAL_QUALITY | VO_DISABLE_OCCLUSION | VO_DISABLE_OCCLUSION);
    

    // try to create a spot light to camera node
    Light* light = cameraNode_->CreateComponent<Light>();

    light->SetLightType(LIGHT_SPOT);
    light->SetRange(200.0f);
    light->SetRampTexture(cache->GetResource<Texture2D>("Textures/RampExtreme.png"));
    light->SetFov(20.0f);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(2.0f);
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.000025f, 0.5f));

    light->SetShadowFadeDistance(100.0f);
    light->SetShadowDistance(125.0f);
    light->SetShadowResolution(0.5f);
    light->SetShadowNearFarRatio(0.01f);
}

void HelloWorld::CreateBillboards()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    const unsigned int NUM_BILLBOARDNODES = 5;
    const unsigned int NUM_BILLBOARDS = 8;

    for (unsigned i = 0; i < NUM_BILLBOARDNODES; ++i)
    {
        Node* smokeNode = scene_->CreateChild("Smoke");
        smokeNode->SetPosition(Vector3(Random(50.0f) - 25.0f, Random(10.0f) + 5.0f, Random(50.0f) - 25.0f));

        BillboardSet* billboardObj = smokeNode->CreateComponent<BillboardSet>();
        billboardObj->SetNumBillboards(NUM_BILLBOARDS);
        billboardObj->SetMaterial(cache->GetResource<Material>("Materials/LitSmoke.xml"));
        billboardObj->SetSorted(true);

        for (unsigned j = 0; j < NUM_BILLBOARDS; ++j)
        {
            Billboard* bb = billboardObj->GetBillboard(j);
            bb->position_ = Vector3(Random(8.0f) - 4.0f, Random(4.0f) - 2.0f, Random(8.0f) - 4.0f);
            bb->size_ = Vector2(Random(2.0f) + 1.0f, Random(2.0f) + 1.0f);
            bb->rotation_ = Random() * 360.0f;
            bb->enabled_ = true;
        }

        billboardObj->Commit();
    }
}

void HelloWorld::CreateLights()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    const unsigned NUM_LIGHTS = 3;

    Node* lightNode = scene_->CreateChild("Light");
    Light* light = lightNode->CreateComponent<Light>();

    lightNode->SetPosition(Vector3(0.0f, 25.0f, 0.0f));
    lightNode->SetDirection(Vector3(0.0f, -1.5f, 1.0f));

    light->SetLightType(LIGHT_SPOT);
    light->SetRange(60.0f);
    light->SetRampTexture(cache->GetResource<Texture2D>("Textures/RampExtreme.png"));
    light->SetFov(30.0f);
    light->SetColor(Color(0.2f, 0.4f, 0.55f));
    light->SetSpecularIntensity(1.0f);
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.00002f, 0.0f));

    light->SetShadowFadeDistance(100.0f);
    light->SetShadowDistance(125.0f);
    light->SetShadowResolution(0.5f);
    light->SetShadowNearFarRatio(0.01f);
}

void HelloWorld::CreateSkybox()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* skyNode = scene_->CreateChild("Sky");
    Skybox* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));
}

void HelloWorld::CreateText()
{
    // ResourceCache* cache = GetSubsystem<ResourceCache>();

    // // Construct new Text object
    // SharedPtr<Text> helloText(new Text(context_));

    // // Set String to display
    // helloText->SetText("Hello World from Urho3D!");

    // // Set font and text color
    // helloText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30);
    // helloText->SetColor(Color(0.0f, 1.0f, 0.0f));

    // // Align Text center-screen
    // helloText->SetHorizontalAlignment(HA_CENTER);
    // helloText->SetVerticalAlignment(VA_CENTER);

    // // Add Text instance to the UI root element
    // GetSubsystem<UI>()->GetRoot()->AddChild(helloText);
}

void HelloWorld::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // setup two viewports
    renderer->SetNumViewports(2);

    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    ///

    SharedPtr<RenderPath> effectRenderPath = viewport->GetRenderPath()->Clone();
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA2.xml"));
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Blur.xml"));
    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.9f, 0.6f));
    effectRenderPath->SetEnabled("Bloom", false);
    effectRenderPath->SetEnabled("FXAA2", false);
    effectRenderPath->SetEnabled("Blur", false);
    viewport->SetRenderPath(effectRenderPath);

    //
    int left = graphics->GetWidth() * 2 / 3;
    int bottom = graphics->GetHeight() / 3;
    int right = graphics->GetWidth() - 32;
    int top = 32;
    SharedPtr<Viewport> rearViewport(
        new Viewport(
            context_, 
            scene_, 
            rearCameraNode_->GetComponent<Camera>(), 
            IntRect(left, top, right, bottom)
            )
        );
    rearViewport->SetDrawDebug(false);
    renderer->SetViewport(1, rearViewport);
}

void HelloWorld::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float timeDelta = eventData[P_TIMESTEP].GetFloat();

    MoveCamera(timeDelta);

    AnimateScene(timeDelta);

    ToggleLight();
}

void HelloWorld::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    if (drawDebug_)
        scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}

void HelloWorld::MoveCamera(float timeDelta)
{
    // don't move camera if now focus on ui
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();
    RenderPath* effectRenderPath = GetSubsystem<Renderer>()->GetViewport(0)->GetRenderPath();

    IntVector2 mouseMovement = input->GetMouseMove();

    const float MOVE_SPEED = 20.0f;
    const float MOUSE_SENSITIVITY = 0.1f;
    const float ROLL_ANGLE = 0.5f;

    yaw_ += MOUSE_SENSITIVITY * mouseMovement.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMovement.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);
    if (input->GetKeyDown('Q') && !input->GetKeyDown('E'))
        roll_ += ROLL_ANGLE;
    if (input->GetKeyDown('E') && !input->GetKeyDown('Q'))
        roll_ -= ROLL_ANGLE;
    if ((!input->GetKeyDown('Q') && !input->GetKeyDown('E'))
        || (input->GetKeyDown('Q') && input->GetKeyDown('E')))
        roll_ = 0.0f;
    roll_ = Clamp(roll_, -20.0f, 20.0f);

    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, roll_));

    float moveSpeed;
    if (input->GetKeyDown(KEY_SHIFT))
        moveSpeed = MOVE_SPEED * 10.0f;
    else
        moveSpeed = MOVE_SPEED;
    if (input->GetKeyDown('W'))
        cameraNode_->Translate(Vector3::FORWARD * moveSpeed * timeDelta);
    if (input->GetKeyDown('S'))
        cameraNode_->Translate(Vector3::BACK * moveSpeed * timeDelta);
    if (input->GetKeyDown('A'))
        cameraNode_->Translate(Vector3::LEFT * moveSpeed * timeDelta);
    if (input->GetKeyDown('D'))
        cameraNode_->Translate(Vector3::RIGHT * moveSpeed * timeDelta);
    if (input->GetKeyPress(KEY_SPACE))
        drawDebug_ = !drawDebug_;
    if (input->GetKeyPress('I'))
        effectRenderPath->ToggleEnabled("Bloom");
    if (input->GetKeyPress('O'))
        effectRenderPath->ToggleEnabled("FXAA2");
    if (input->GetKeyPress('P'))
        effectRenderPath->ToggleEnabled("Blur");
    if (input->GetKeyPress(KEY_ESC))
        engine_->Exit();
    if (input->GetMouseButtonPress(MOUSEB_LEFT))
        SpawnObject();
}

void HelloWorld::ToggleLight()
{
    Input* input = GetSubsystem<Input>();

    if (input->GetMouseButtonPress(MOUSEB_RIGHT))
    {
        Light* light = cameraNode_->GetComponent<Light>();
        light->SetEnabled(!light->IsEnabled());
    }
}

void HelloWorld::SpawnObject()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* boxNode = scene_->CreateChild("SmallBox");
    boxNode->SetPosition(cameraNode_->GetPosition());
    boxNode->SetRotation(cameraNode_->GetRotation());
    boxNode->SetScale(0.5f);
    StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
    boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneEnvMapSmall.xml"));

    RigidBody* body = boxNode->CreateComponent<RigidBody>();
    CollisionShape* shape = boxNode->CreateComponent<CollisionShape>();
    body->SetMass(40.0f);
    body->SetFriction(0.05f);
    shape->SetBox(Vector3::ONE);

    const float BOX_VELOCITY = 160.0f;
    body->SetLinearVelocity(cameraNode_->GetRotation() * Vector3(0.0f, 0.25f, 1.0f) * BOX_VELOCITY);
}

void HelloWorld::AnimateScene(float timeDelta)
{
    const float LIGHT_ROTATION_SPEED = 40.0f;

    Node* lightNode = scene_->GetChild("Light");
    lightNode->Rotate(Quaternion(0.0f, LIGHT_ROTATION_SPEED * timeDelta, 0.0f), TS_WORLD);
}


