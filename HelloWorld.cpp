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

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/BillboardSet.h>
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


#include "HelloWorld.h"


// Expands to this example's entry-point
URHO3D_DEFINE_APPLICATION_MAIN(HelloWorld)

HelloWorld::HelloWorld(Context* context)
: Application(context)
, yaw_(0.0f)
, pitch_(0.0f)
, roll_(0.0f)
, drawDebug_(false)
{
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

    SetupViewport();

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloWorld, HandleUpdate));

    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(HelloWorld, HandlePostRenderUpdate));
    // CreateText();
}

void HelloWorld::Stop()
{
    engine_->DumpResources(true);
}

void HelloWorld::CreateScene()
{
    // create a scene
    scene_ = new Scene(context_);

    // create octree and debug renderer for scene
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-80.0f, 80.0f));
    zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));

    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.5f, -1.0f, 0.5f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(1.4f);

    CreateFloor();

    CreateStaticModel();

    CreateBillboards();

    CreateLights();

    CreateCamera();
}

void HelloWorld::CreateFloor()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
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

    cameraNode_ = scene_->CreateChild("Camera");
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(300.0f);

    rearCameraNode_ = cameraNode_->CreateChild("RearCamera");
    rearCameraNode_->SetRotation(Quaternion(180.0f, Vector3::UP));
    Camera* rearCamera = rearCameraNode_->CreateComponent<Camera>();
    rearCamera->SetFarClip(200.0f);
    rearCamera->SetViewOverrideFlags(VO_LOW_MATERIAL_QUALITY | VO_DISABLE_OCCLUSION | VO_DISABLE_OCCLUSION);

    cameraNode_->SetPosition(Vector3(0.0f, 40.0f, 0.0f));
    pitch_ = 90.0f;

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
        GetSubsystem<Renderer>()->DrawDebugGeometry(true);
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

    if (input->GetKeyDown('W'))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeDelta);
    if (input->GetKeyDown('S'))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeDelta);
    if (input->GetKeyDown('A'))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeDelta);
    if (input->GetKeyDown('D'))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeDelta);
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

void HelloWorld::AnimateScene(float timeDelta)
{
    const float LIGHT_ROTATION_SPEED = 40.0f;

    Node* lightNode = scene_->GetChild("Light");
    lightNode->Rotate(Quaternion(0.0f, LIGHT_ROTATION_SPEED * timeDelta, 0.0f), TS_WORLD);
}


