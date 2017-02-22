// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Helper for implementing a free-flying camera
//

#pragma once

#include <memory.h>
#include "GamePad.h"


namespace DirectX
{
    class Keyboard;
    class Mouse;

    struct BoundingSphere;
    struct BoundingBox;
}

namespace DX
{
    class FlyCamera
    {
    public:
        FlyCamera();

        FlyCamera(FlyCamera&& moveFrom);
        FlyCamera& operator= (FlyCamera&& moveFrom);

        FlyCamera(FlyCamera const&) = delete;
        FlyCamera& operator=(FlyCamera const&) = delete;

        virtual ~FlyCamera();

        // Perform per-frame update using gamepad controls
        //
        //           Right Thumbstick: Rotate
        //            Left Thumbstick: Translate X/Z
        //     Left Thumbstick Button: Return to default position
        //     Right Thumstick Button: Return to default rotation
        // Left/Right Shoulder Button: Increase/decrease/reset translation sensitivity
        //
        void Update(float elapsedTime, const DirectX::GamePad::State& pad);

        // Perform per-frame update using keyboard & mouse controls
        //
        //          WASD & Arrow Keys: Translate X/Z
        //          PageUp / PageDown: Translate Y
        //                       Home: Reset to default positon/rotation
        //
        //         Hold Left Mouse Button: Mouse look
        //        Hold Right Mouse Button: Translate X/Y (Shift Translates Z)
        //
        void Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& kb);

        // Reset camera to default position/rotation
        void Reset();

        // Set projection window (pixels)
        void SetWindow(int width, int height);

        // Set projection parameters
        void SetProjectionParameters(float fov, float nearDistance, float farDistance, bool lhcoords = false);

        // Set translation sensitivity and limits
        void SetSensitivity(float defaultSensitivity, float minSensitivity, float maxSensitivity, float stepSensitivity);

        // Set rotation rate
        void SetRotationRate(float rotRate);

        // Set position bounds (manually set properties are assumed to be in bounds)
        void SetBoundingBox(const DirectX::BoundingBox& box);

        // Set initial/default location/rotation
        void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position);
        void XM_CALLCONV SetRotation(DirectX::FXMVECTOR rotation);

        // Sets position & rotation from two points
        void XM_CALLCONV SetLookAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target);

        // Behavior control flags
        static const unsigned int c_FlagsInvertY = 0x1;                     // Invert Y axis rotation controls
        static const unsigned int c_FlagsDisableRotateX = 0x2;              // Disables rotation in X
        static const unsigned int c_FlagsDisableRotateY = 0x4;              // Disables rotation in Y
        static const unsigned int c_FlagsDisableRotation = 0x6;             // Disables all rotation controls
        static const unsigned int c_Flags_XY = 0x8;                         // Instead of translate X/Z, do translate in X/Y
        static const unsigned int c_FlagsDisableSensitivityControl = 0x10;  // Disable sensitivity controls

        void SetFlags(unsigned int flags);

        // Returns view and projection matrices for camera
        DirectX::XMMATRIX GetView() const;
        DirectX::XMMATRIX GetProjection() const;

        // Returns the current camera position
        DirectX::XMVECTOR GetPosition() const;

        // Returns the current camera rotation
        DirectX::XMVECTOR GetRotation() const;

        // Returns current behavior control flags
        unsigned int GetFlags() const;

    private:
        // Private implementation.
        class Impl;

        struct aligned_deleter { void operator()(void* p) { _aligned_free(p); } };

        std::unique_ptr<Impl, aligned_deleter> pImpl;
    };
}