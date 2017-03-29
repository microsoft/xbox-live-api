// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SampleGUI.h"
#include "SessionListView.h"

void SessionView::Show()
{
    SetVisibility(true);
}

void SessionView::Hide()
{
    SetVisibility(false);
}

void SessionView::SetVisibility(bool visible)
{
    m_sessionNameLabel->SetVisible(visible);
    m_gameModeLabel->SetVisible(visible);
    m_mapLabel->SetVisible(visible);
    for (auto &row : m_rows)
    {
        row.m_xuid->SetVisible(visible);
        row.m_health->SetVisible(visible);
        row.m_skill->SetVisible(visible);

        if (!visible)
        {
            // Only set this when you want to Hide()
            row.m_gamerpic->SetVisible(visible);
            row.m_hostIcon->SetVisible(visible);
        }
    }
}

void SessionView::Clear()
{
    m_sessionNameLabel->SetText(L"");
    m_gameModeLabel->SetText(L"");
    m_mapLabel->SetText(L"");
    for (auto &row : m_rows)
    {
        row.m_health->SetText(L"");
        row.m_skill->SetText(L"");
        row.m_xuid->SetText(L"");
        row.m_gamerpic->SetVisible(false);
        row.m_hostIcon->SetVisible(false);
    }
}

void SessionView::SetControls(ATG::IPanel *parent, int rowStart)
{
    m_sessionNameLabel = dynamic_cast<ATG::TextLabel*>(parent->Find(rowStart));
    m_gameModeLabel = dynamic_cast<ATG::TextLabel*>(parent->Find(rowStart + 1));
    m_mapLabel = dynamic_cast<ATG::TextLabel*>(parent->Find(rowStart + 2));

    rowStart += 10;
    for (auto &row : m_rows)
    {
        row.m_gamerpic = dynamic_cast<ATG::Image*>(parent->Find(rowStart++));
        row.m_xuid = dynamic_cast<ATG::TextLabel*>(parent->Find(rowStart++));
        row.m_hostIcon = dynamic_cast<ATG::Image*>(parent->Find(rowStart++));
        row.m_skill = dynamic_cast<ATG::TextLabel*>(parent->Find(rowStart++));
        row.m_health = dynamic_cast<ATG::TextLabel*>(parent->Find(rowStart++));
    }
}