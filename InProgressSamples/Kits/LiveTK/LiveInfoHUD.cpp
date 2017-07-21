//--------------------------------------------------------------------------------------
// File: LiveInfoHUD.cpp
//
// A Heads Up Display (HUD) for Xbox Live samples
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------
#include "pch.h"
#include "LiveInfoHUD.h"

#include "ATGColors.h"
#include "WICTextureLoader.h"

namespace ATG
{
    LiveInfoHUD::LiveInfoHUD(const wchar_t *sampleTitle) :
        m_sampleTitle(sampleTitle),
        m_gamerTag(L"No User Signed in")
    {

    }

    void LiveInfoHUD::Initialize(std::shared_ptr<xbox::services::xbox_live_context> context)
    {
        auto config = xbox::services::xbox_live_app_config::get_app_config_singleton();

        m_serviceConfigId = config->scid();

        wchar_t buffer[16] = {};
        swprintf_s(buffer,L"0x%08X",config->title_id());
        m_titleId = buffer;

        m_sandboxId = config->sandbox();

        if(context != nullptr)
            SetUser(context);
    }

    void LiveInfoHUD::ReleaseDevice()
    {
        m_batch.reset();
        m_smallFont.reset();
        m_boldFont.reset();
        m_titleFont.reset();
        m_gamerPic.Reset();
        m_context.Reset();
    }

    void LiveInfoHUD::RestoreDevice(_In_ ID3D11DeviceContext *context)
    {
        m_context = context;
        m_batch = std::make_unique<DirectX::SpriteBatch>(context);

        Microsoft::WRL::ComPtr<ID3D11Device> device;
        context->GetDevice(device.GetAddressOf());

        m_smallFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"SegoeUI_18.spritefont");
        m_boldFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"SegoeUI_18_Bold.spritefont");
        m_titleFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"SegoeUI_36.spritefont");

        DirectX::CreateWICTextureFromFile(device.Get(), L".\\Assets\\GamerPic.png", nullptr, m_gamerPic.GetAddressOf());
    }

    void LiveInfoHUD::SetUser(std::shared_ptr<xbox::services::xbox_live_context> context)
    {
#if defined(_XBOX_ONE) && defined(_TITLE)
        if (context == nullptr || context->user() == nullptr || !context->user()->IsSignedIn)
#else
        if (context == nullptr || context->user() == nullptr || !context->user()->is_signed_in())
#endif
        {
            m_gamerTag = L"No User Signed in";

            Microsoft::WRL::ComPtr<ID3D11Device> device;
            m_context->GetDevice(device.GetAddressOf());

            DirectX::CreateWICTextureFromFile(device.Get(), L".\\Assets\\GamerPic.png", nullptr, m_gamerPic.GetAddressOf());
        }
        else
        {
#if defined(_XBOX_ONE) && defined(_TITLE)
            m_gamerTag = context->user()->DisplayInfo->Gamertag->Data();
#else
            m_gamerTag = context->user()->gamertag();

            // sandbox is not available in UWP apps until a user has signed in
            auto config = xbox::services::xbox_live_app_config::get_app_config_singleton();
            m_sandboxId = config->sandbox();
#endif

            GetGamerPic(context);
        }
    }

    void LiveInfoHUD::Render()
    {
#if defined(_XBOX_ONE) && defined(_TITLE)
        static const DirectX::XMMATRIX scale = DirectX::XMMatrixIdentity();
#else
        static const DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(720.f / 1080.f, 720.f / 1080.f, 1.f);
#endif
        m_batch->Begin(DirectX::SpriteSortMode_Deferred,nullptr,nullptr,nullptr,nullptr,nullptr,scale);

        m_titleFont->DrawString(m_batch.get(), m_sampleTitle.c_str(), DirectX::XMFLOAT2(78.f,44.f), ATG::Colors::OffWhite, 0.0f);

        m_boldFont->DrawString(m_batch.get(), L"Sandbox Id:", DirectX::XMFLOAT2(270.f, 1019.f), ATG::Colors::OffWhite, 0.0f);
        m_boldFont->DrawString(m_batch.get(), L"Title Id:", DirectX::XMFLOAT2(590.f, 1019.f), ATG::Colors::OffWhite, 0.0f);
        m_boldFont->DrawString(m_batch.get(), L"Service Config Id:", DirectX::XMFLOAT2(950.f, 1019.f), ATG::Colors::OffWhite, 0.0f);

        m_smallFont->DrawString(m_batch.get(), m_sandboxId.c_str(), DirectX::XMFLOAT2(410.f, 1019.f), ATG::Colors::OffWhite, 0.0f);
        m_smallFont->DrawString(m_batch.get(), m_titleId.c_str(), DirectX::XMFLOAT2(680.f, 1019.f), ATG::Colors::OffWhite, 0.0f);
        m_smallFont->DrawString(m_batch.get(), m_serviceConfigId.c_str(), DirectX::XMFLOAT2(1155.f, 1019.f), ATG::Colors::OffWhite, 0.0f);

        m_smallFont->DrawString(m_batch.get(), m_serviceConfigId.c_str(), DirectX::XMFLOAT2(1155.f, 1019.f), ATG::Colors::OffWhite, 0.0f);

        auto pos = DirectX::XMFLOAT2(1770.f, 70.f);
        pos.x -= DirectX::XMVectorGetX(m_smallFont->MeasureString(m_gamerTag.c_str()));

        m_smallFont->DrawString(m_batch.get(), m_gamerTag.c_str(), pos, ATG::Colors::OffWhite, 0.0f);

        static const RECT gamerPicRect = { 1780, 55, 1780 + 64, 55 + 64 };

        m_batch->Draw(m_gamerPic.Get(), gamerPicRect);

        m_batch->End();
    }

    void LiveInfoHUD::GetGamerPic(std::shared_ptr<xbox::services::xbox_live_context> context)
    {
        auto user = context->user();
        if (user)
        {
#if defined(_XBOX_ONE) && defined(_TITLE)
            auto buffer = ref new Windows::Storage::Streams::Buffer(1024 * 100);

            concurrency::create_task(
                user->DisplayInfo->GetGamerPictureAsync(
                    Windows::Xbox::System::UserPictureSize::Small,
                    buffer
                )
            ).then([this, user, buffer](concurrency::task<Windows::Xbox::System::GetPictureResult^> t) mutable
            {
                try
                {
                    auto result = t.get();
        
                    if (result->RequiredBufferSize > 0)
                    {
                        buffer = ref new Windows::Storage::Streams::Buffer(result->RequiredBufferSize);
        
                        return concurrency::create_task(
                            user->DisplayInfo->GetGamerPictureAsync(
                                Windows::Xbox::System::UserPictureSize::Small,
                                buffer
                            )
                        );
                    }
                    else
                    {
                        return concurrency::create_task([this, result]()
                        {
                            return result;
                        });
                    }
                }
                catch (...)
                {
                    // Ignore and keep default
                }
        
                return concurrency::create_task([this]() -> Windows::Xbox::System::GetPictureResult^
                {
                    return nullptr;
                });
            }).then([this, buffer](concurrency::task<Windows::Xbox::System::GetPictureResult^> t)
            {
                try
                {
                    auto result = t.get();
        
                    if (SUCCEEDED(result->Result.Value))
                    {
                        auto reader = Windows::Storage::Streams::DataReader::FromBuffer(buffer);
        
                        std::vector<uint8_t> data(reader->UnconsumedBufferLength);
        
                        if (!data.empty())
                        {
                            reader->ReadBytes(
                                Platform::ArrayReference<uint8_t>(
                                    &data[0],
                                    static_cast<unsigned int>(data.size())
                                    )
                            );
        
                            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex;
        
                            Microsoft::WRL::ComPtr<ID3D11Device> device;
                            m_context->GetDevice(device.GetAddressOf());
        
                            auto hr = DirectX::CreateWICTextureFromMemory(
                                device.Get(),
                                reinterpret_cast<uint8_t*>(data.data()),
                                data.size(),
                                nullptr,
                                tex.GetAddressOf()
                            );
        
                            if (SUCCEEDED(hr))
                            {
                                m_gamerPic.Swap(tex); // take ownership of resource
                            }
                        }
                    }
                }
                catch (...)
                {
                    // Ignore and keep default
                }
            });
#else
            // Have to use the profile service to get the gamer pic for UWP
            context->profile_service().get_user_profile(context->user()->xbox_user_id())
                .then([this, context](xbox::services::xbox_live_result<xbox::services::social::xbox_user_profile> result)
                      {
                          if(result.err())
                              return;

                          auto &payload = result.payload();

                          web::uri uri = payload.game_display_picture_resize_uri();

                          try
                          {
                              web::uri_builder pathQueryFragment;
                              pathQueryFragment.append_path(uri.path());
                              auto split = web::uri::split_query(uri.query());
                              pathQueryFragment.append_query(L"url",split[L"url"]);
                              pathQueryFragment.append_query(L"format", split[L"format"]);
                              auto liveHttpCall = xbox::services::create_xbox_live_http_call(context->settings(), L"GET", uri.scheme() + L"://" + uri.host(), pathQueryFragment.to_uri());
                              liveHttpCall->get_response(xbox::services::http_call_response_body_type::vector_body)
                                  .then([this](Concurrency::task<std::shared_ptr<xbox::services::http_call_response>> responseTask)
                              {
                                  try
                                  {
                                      auto response = responseTask.get();
                                      if (response->err_code())
                                          return;

                                      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex;

                                      Microsoft::WRL::ComPtr<ID3D11Device> device;
                                      m_context->GetDevice(device.GetAddressOf());

                                      auto &image = response->response_body_vector();

                                      auto hr = DirectX::CreateWICTextureFromMemory(
                                          device.Get(), 
                                          image.data(), 
                                          image.size(), 
                                          nullptr, 
                                          tex.GetAddressOf()
                                      );

                                      if (SUCCEEDED(hr))
                                      {
                                          m_gamerPic.Swap(tex); // take ownership of resource
                                      }
                                  }
                                  catch (...)
                                  {
                                      OutputDebugString(L"Getting the http_call_response when getting a gamerpic threw an exception\n");
                                  }
                              });

                          }
                          catch (...)
                          {
                              OutputDebugString(L"Creating an HTTP call to get a gamerpic or getting its response threw an exception\n");
                          }
                      });
#endif
        }
    }
}