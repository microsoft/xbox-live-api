// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubRepeater.h"

using namespace xbox::services::presence;

void ClubRepeater::CreateItem(unsigned index, std::shared_ptr<ClubListItem> item, RECT & bounds)
{
    auto base = GetItemId(index);
    auto panel = _mgr->FindPanel<ATG::Overlay>(_panelId);

    // Label
    auto r = bounds;

    r.left += 24;
    r.right -= 24;
    r.top += 5;
    r.bottom = r.top + 40;

    panel->Add(new ATG::TextLabel(base + 1, L"", r, ATG::TextLabel::c_StyleFontSmall));   
}

void ClubRepeater::UpdateItem(unsigned index, std::shared_ptr<ClubListItem> item)
{
    auto base = GetItemId(index);

    _mgr->FindControl<ATG::TextLabel>(_panelId, base + 1)->SetText(item->GetLabel().c_str());
}

void ClubRepeater::ShadeItem(unsigned index, const ATG::UIConfig& config)
{
    for (unsigned i = 0; i < 15; ++i)
    {
        DirectX::XMVECTOR color;
        if (i == index)
        {
            color = DirectX::XMLoadFloat4(&config.colorHighlight);
        }
        else
        {
            color = DirectX::XMLoadFloat4(&config.colorNormal);
        }

        auto base = GetItemId(i);
        _mgr->FindControl<ATG::TextLabel>(_panelId, base + 1)->SetForegroundColor(color);
    }
}

std::wstring ClubListItem::GetLabel()
{
    if (m_pClub != nullptr)
    {
        stringstream_t ss;

        ss << m_pClub->profile().name().value() << ": ";
        ss << m_pClub->id();

        return ss.str();
    }

    return std::wstring();
}
