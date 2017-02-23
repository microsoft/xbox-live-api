// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "FlyCamera.h"

#include "GamePad.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DX;

#pragma warning(disable : 4324)

class __declspec(align(16)) FlyCamera::Impl
{
public:
    Impl() :
        m_nearDistance(0.1f),
        m_farDistance(10000.f),
        m_fov(XM_PI / 4.f),
        m_defaultSensitivity(1.f),
        m_minSensitivity(0.01f),
        m_maxSensitivity(10.f),
        m_stepSensitivity(0.01f),
        m_rotRate(1.f),
        m_flags(0),
        m_lhcoords(false),
#ifdef _XBOX_ONE
        m_width(1920),
        m_height(1080)
#else
        m_width(1280),
        m_height(720)
#endif
    {
        m_view = m_projection = XMMatrixIdentity();
        m_cameraPosition = m_homePosition = g_XMZero;
        m_cameraRotation = m_homeRotation = g_XMIdentityR3;

        m_bounds.Center.x = m_bounds.Center.y = m_bounds.Center.z = 0.f;
        m_bounds.Extents.x = m_bounds.Extents.y = m_bounds.Extents.z = FLT_MAX;

        Reset();
    }

    void Update(float elapsedTime, const GamePad::State& pad)
    {
        using namespace DirectX::SimpleMath;

        float handed = (m_lhcoords) ? 1.f : -1.f;

        Matrix im = XMMatrixInverse(nullptr, GetView());

        // Translate camera
        {
            Vector3 move = Vector3::Zero;

            if (m_flags & c_Flags_XY)
            {
                move.x = pad.thumbSticks.leftX;
                move.y = pad.thumbSticks.leftY;
            }
            else
            {
                move.x = pad.thumbSticks.leftX;
                move.z = pad.thumbSticks.leftY * handed;
            }

            if (move.x != 0 || move.y != 0 || move.z != 0)
            {
                move = Vector3::TransformNormal(move, im);

                m_cameraPosition += move * elapsedTime * m_sensitivity;

                Vector3 minBound = m_bounds.Center - m_bounds.Extents;
                Vector3 maxBound = m_bounds.Center + m_bounds.Extents;
                m_cameraPosition = XMVectorMax(minBound, XMVectorMin(maxBound, m_cameraPosition));

                m_viewDirty = true;
            }
        }
        
        // Rotate camera
        if ((m_flags & c_FlagsDisableRotation) != c_FlagsDisableRotation)
        {
            float invert = (m_flags & c_FlagsInvertY) ? -1.f : 1.f;

            Vector3 spin(pad.thumbSticks.rightX, pad.thumbSticks.rightY * invert, 0.f);
            spin *= elapsedTime * m_rotRate;

            if (m_flags & c_FlagsDisableRotateX)
                spin.x = 0.f;

            if (m_flags & c_FlagsDisableRotateY)
                spin.y = 0.f;

            if (spin.x != 0 || spin.y != 0)
            {
                m_cameraRotation = XMQuaternionMultiply(m_cameraRotation, XMQuaternionRotationAxis(im.Right(), -spin.y * handed));
                m_cameraRotation = XMQuaternionMultiply(m_cameraRotation, XMQuaternionRotationAxis(im.Up(), spin.x * handed));
                m_cameraRotation = XMQuaternionNormalize(m_cameraRotation);
                m_viewDirty = true;
            }
        }

        // Other controls
        if (pad.IsLeftShoulderPressed() && pad.IsRightShoulderPressed())
        {
            m_sensitivity = m_defaultSensitivity;

#ifdef _DEBUG
            (void)GetView();

            char buff[128] = {};
            Vector4 tmp = m_cameraPosition;
            sprintf_s(buff, "cameraPosition = { %2.2ff, %2.2ff, %2.2ff, 0.f};\n", tmp.x, tmp.y, tmp.z);
            OutputDebugStringA(buff);

            tmp = m_cameraRotation;
            sprintf_s(buff, "cameraRotation = { %2.2ff, %2.2ff, %2.2ff, %2.2ff};\n", tmp.x, tmp.y, tmp.z, tmp.w );
            OutputDebugStringA(buff);
#endif
        }
        else if (!(m_flags & c_FlagsDisableSensitivityControl))
        {
            if (pad.IsRightShoulderPressed())
            {
                m_sensitivity += m_stepSensitivity;
                if (m_sensitivity > m_maxSensitivity)
                    m_sensitivity = m_maxSensitivity;
            }
            else if (pad.IsLeftShoulderPressed())
            {
                m_sensitivity -= m_stepSensitivity;
                if (m_sensitivity < m_minSensitivity)
                    m_sensitivity = m_minSensitivity;
            }
        }

        if (pad.IsRightStickPressed() && pad.IsLeftStickPressed())
        {
            Reset();
        }
        else if (pad.IsRightStickPressed())
        {
            m_cameraRotation = m_homeRotation;
            m_viewDirty = true;
        }
        else if (pad.IsLeftStickPressed())
        {
            m_cameraPosition = m_homePosition;
            m_viewDirty = true;
        }
    }

    void Update(float elapsedTime, Mouse& mouse, Keyboard& kb)
    {
        using namespace DirectX::SimpleMath;

        float handed = (m_lhcoords) ? 1.f : -1.f;

        Matrix im = XMMatrixInverse(nullptr, GetView());

        auto mstate = mouse.GetState();
        auto kbstate = kb.GetState();

        // Keyboard controls
        {
            // Arrow keys & WASD control translation of camera focus
            Vector3 move = Vector3::Zero;

            float scale = m_sensitivity;
            if (kbstate.LeftShift || kbstate.RightShift)
                scale *= 0.5f;

            if (m_flags & c_Flags_XY)
            {
                if (kbstate.Up || kbstate.W)
                    move.y += scale;

                if (kbstate.Down || kbstate.S)
                    move.y -= scale;

                if (kbstate.PageUp)
                    move.z += scale * handed;

                if (kbstate.PageDown)
                    move.z -= scale * handed;
            }
            else
            {
                if (kbstate.PageUp)
                    move.y += scale;

                if (kbstate.PageDown)
                    move.y -= scale;

                if (kbstate.Up || kbstate.W)
                    move.z += scale * handed;

                if (kbstate.Down || kbstate.S)
                    move.z -= scale * handed;
            }

            if (kbstate.Right || kbstate.D)
                move.x += scale;

            if (kbstate.Left || kbstate.A)
                move.x -= scale;

            if (move.x != 0 || move.y != 0 || move.z != 0)
            {
                move = Vector3::TransformNormal(move, im);

                m_cameraPosition += move * elapsedTime;

                Vector3 minBound = m_bounds.Center - m_bounds.Extents;
                Vector3 maxBound = m_bounds.Center + m_bounds.Extents;
                m_cameraPosition = XMVectorMax(minBound, XMVectorMin(maxBound, m_cameraPosition));

                m_viewDirty = true;
            }
        }

        if (kbstate.Home)
        {
            Reset();
        }

        // Mouse controls
        if (mstate.positionMode == Mouse::MODE_RELATIVE)
        {
            if (mstate.rightButton)
            {
                // Translate camera
                Vector3 delta;
                if (kbstate.LeftShift || kbstate.RightShift)
                {
                    delta = Vector3(0.f, 0.f, -float(mstate.y) * handed) * m_sensitivity * 4.f * elapsedTime;
                }
                else
                {
                    delta = Vector3(-float(mstate.x), float(mstate.y), 0.f) * m_sensitivity * 4.f * elapsedTime;
                }

                delta = Vector3::TransformNormal(delta, im);

                m_cameraPosition += delta * elapsedTime * m_sensitivity;
                m_viewDirty = true;
            }
            else if (mstate.leftButton && !(m_flags & c_FlagsDisableRotation))
            {
                // Rotate camera
                float invert = (m_flags & c_FlagsInvertY) ? -1.f : 1.f;

                Vector3 spin(float(mstate.x), float(mstate.y) * invert, 0.f);
                spin *= elapsedTime * m_rotRate / 4.f;

                if (spin.x != 0 || spin.y != 0)
                {
                    m_cameraRotation = XMQuaternionMultiply(m_cameraRotation, XMQuaternionRotationAxis(im.Right(), spin.y * handed));
                    m_cameraRotation = XMQuaternionMultiply(m_cameraRotation, XMQuaternionRotationAxis(im.Up(), spin.x * handed));
                    m_cameraRotation = XMQuaternionNormalize(m_cameraRotation);
                    m_viewDirty = true;
                }
            }
            else
            {
                mouse.SetMode(Mouse::MODE_ABSOLUTE);
            }
        }
        else if (mstate.rightButton)
        {
            mouse.SetMode(Mouse::MODE_RELATIVE);
        }
        else if (mstate.leftButton && !(m_flags & c_FlagsDisableRotation))
        {
            mouse.SetMode(Mouse::MODE_RELATIVE);
        }
    }

    void Reset()
    {
        m_cameraPosition = m_homePosition;
        m_cameraRotation = m_homeRotation;
        m_sensitivity = m_defaultSensitivity;
        m_viewDirty = m_projDirty = true;
    }

    mutable XMMATRIX        m_view;
    mutable XMMATRIX        m_projection;

    XMVECTOR                m_cameraPosition;
    XMVECTOR                m_homePosition;

    XMVECTOR                m_cameraRotation;
    XMVECTOR                m_homeRotation;

    float                   m_nearDistance;
    float                   m_farDistance;
    float                   m_fov;
    float                   m_sensitivity;
    float                   m_defaultSensitivity;
    float                   m_minSensitivity;
    float                   m_maxSensitivity;
    float                   m_stepSensitivity;
    float                   m_rotRate;
    unsigned int            m_flags;

    bool                    m_lhcoords;
    mutable bool            m_viewDirty;
    mutable bool            m_projDirty;

    int                     m_width;
    int                     m_height;

    DirectX::BoundingBox    m_bounds;

    XMMATRIX GetView() const
    {
        m_viewDirty = false;

        XMVECTOR dir = XMVector3Rotate((m_lhcoords) ? g_XMIdentityR2 : g_XMNegIdentityR2, m_cameraRotation);
        XMVECTOR up = XMVector3Rotate(g_XMIdentityR1, m_cameraRotation);

        if (m_lhcoords)
        {
            m_view = XMMatrixLookToLH(m_cameraPosition, dir, up);
        }
        else
        {
            m_view = XMMatrixLookToRH(m_cameraPosition, dir, up);
        }

        return m_view;
    }

    XMMATRIX GetProjection() const
    {
        m_projDirty = false;

        float aspectRatio = (m_height > 0.f) ? (float(m_width) / float(m_height)) : 1.f;

        if (m_lhcoords)
        {
            m_projection = XMMatrixPerspectiveFovLH(m_fov, aspectRatio, m_nearDistance, m_farDistance);
        }
        else
        {
            m_projection = XMMatrixPerspectiveFovRH(m_fov, aspectRatio, m_nearDistance, m_farDistance);
        }

        return m_projection;
    }
};


// Public constructor.
FlyCamera::FlyCamera()
{
    auto ptr = reinterpret_cast<Impl*>(_aligned_malloc(sizeof(Impl), 16));
    pImpl.reset(new (ptr) Impl);
    pImpl->Reset();
}


// Move constructor.
FlyCamera::FlyCamera(FlyCamera&& moveFrom)
    : pImpl(std::move(moveFrom.pImpl))
{
}


// Move assignment.
FlyCamera& FlyCamera::operator= (FlyCamera&& moveFrom)
{
    pImpl = std::move(moveFrom.pImpl);
    return *this;
}


// Public destructor.
FlyCamera::~FlyCamera()
{
}


// Public methods.
void FlyCamera::Update(float elapsedTime, const GamePad::State& pad)
{
    pImpl->Update(elapsedTime, pad);
}

void FlyCamera::Update(float elapsedTime, Mouse& mouse, Keyboard& kb)
{
    pImpl->Update(elapsedTime, mouse, kb);
}

void FlyCamera::Reset()
{
    pImpl->Reset();
}

void FlyCamera::SetWindow(int width, int height)
{
    pImpl->m_projDirty = true;
    pImpl->m_width = width;
    pImpl->m_height = height;
}

void FlyCamera::SetProjectionParameters(float fov, float nearDistance, float farDistance, bool lhcoords)
{
    pImpl->m_projDirty = true;
    pImpl->m_fov = fov;
    pImpl->m_nearDistance = nearDistance;
    pImpl->m_farDistance = farDistance;
    pImpl->m_lhcoords = lhcoords;
}

void FlyCamera::SetFlags(unsigned int flags)
{
    pImpl->m_flags = flags;
}

void FlyCamera::SetSensitivity(float defaultSensitivity, float minSensitivity, float maxSensitivity, float stepSensitivity)
{
    pImpl->m_sensitivity = pImpl->m_defaultSensitivity = defaultSensitivity;
    pImpl->m_minSensitivity = minSensitivity;
    pImpl->m_maxSensitivity = maxSensitivity;
    pImpl->m_stepSensitivity = stepSensitivity;
}

void FlyCamera::SetRotationRate(float rotRate)
{
    pImpl->m_rotRate = rotRate;
}

void FlyCamera::SetBoundingBox(const DirectX::BoundingBox& box)
{
    pImpl->m_bounds = box;
}

void XM_CALLCONV FlyCamera::SetPosition(FXMVECTOR position)
{
    pImpl->m_viewDirty = true;
    pImpl->m_cameraPosition = pImpl->m_homePosition = position;
}

void XM_CALLCONV FlyCamera::SetRotation(FXMVECTOR rotation)
{
    XMVECTOR nr = XMQuaternionNormalize(rotation);

    pImpl->m_viewDirty = true;
    pImpl->m_cameraRotation = pImpl->m_homeRotation = nr;
}

void XM_CALLCONV FlyCamera::SetLookAt(FXMVECTOR position, FXMVECTOR target)
{
    pImpl->m_viewDirty = true;
    pImpl->m_cameraPosition = pImpl->m_homePosition = position;

    XMMATRIX m;
    if (pImpl->m_lhcoords)
    {
        m = XMMatrixLookAtLH(position, target, g_XMIdentityR1);
    }
    else
    {
        m = XMMatrixLookAtRH(position, target, g_XMIdentityR1);
    }

    pImpl->m_cameraRotation = pImpl->m_homeRotation = XMQuaternionInverse(XMQuaternionRotationMatrix(m));
}


// Public accessors.
XMMATRIX FlyCamera::GetView() const
{
    if (pImpl->m_viewDirty)
    {
        return pImpl->GetView();
    }
    else
    {
        return pImpl->m_view;
    }
}

XMMATRIX FlyCamera::GetProjection() const
{
    if (pImpl->m_projDirty)
    {
        return pImpl->GetProjection();
    }
    else
    {
        return pImpl->m_projection;
    }
}

XMVECTOR FlyCamera::GetPosition() const
{
    return pImpl->m_cameraPosition;
}

XMVECTOR FlyCamera::GetRotation() const
{
    return pImpl->m_cameraRotation;
}

unsigned int FlyCamera::GetFlags() const
{
    return pImpl->m_flags;
}