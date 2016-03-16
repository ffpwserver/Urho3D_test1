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

#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class Scene;
}

using namespace Urho3D;

/// This first example, maintaining tradition, prints a "Hello World" message.
/// Furthermore it shows:
///     - Using the Sample / Application classes, which initialize the Urho3D engine and run the main loop
///     - Adding a Text element to the graphical user interface
///     - Subscribing to and handling of update events
class HelloWorld : public Application
{
    URHO3D_OBJECT(HelloWorld, Application);

public:
    /// Construct.
    HelloWorld(Context* context);

    virtual void Setup();
    virtual void Start();
    virtual void Stop();

    void CreateScene();

private:
    void CreateText();
    void CreateFloor();
    void CreateFloor2();
    void CreatePyramid();
    void CreateStaticModel();
    void CreateCamera();
    void CreateBillboards();
    void CreateLights();
    void CreateSkybox();
    void SetupViewport();
    void MoveCamera(float timeDelta);
    void AnimateScene(float timeDelta);
    void ToggleLight();
    void SpawnObject();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

private:
    SharedPtr<Scene> scene_;
    SharedPtr<Node> cameraNode_;
    SharedPtr<Node> rearCameraNode_;
    
    SharedPtr<Scene> rttScene_;
    SharedPtr<Node> rttCameraNode_;
    float yaw_;
    float pitch_;
    float roll_;
    bool drawDebug_;
};


