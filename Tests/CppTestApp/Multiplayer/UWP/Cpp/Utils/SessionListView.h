// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

// Helper UI class for displaying Multiplayer Session View
class SessionViewRow
{
public:
    ATG::TextLabel *m_xuid;
    ATG::TextLabel *m_skill;
    ATG::TextLabel *m_health;
    ATG::Image *m_gamerpic;
    ATG::Image *m_hostIcon;
};

// Helper UI class for displaying Multiplayer Session View
class SessionView
{
public:
    SessionView() { m_rows.resize(4); }
    void Show();
    void Hide();
    void Clear();
    void SetVisibility(bool visible);
    void SetControls(ATG::IPanel *parent, int rowStart);

    ATG::TextLabel* m_sessionNameLabel;
    ATG::TextLabel* m_gameModeLabel;
    ATG::TextLabel* m_mapLabel;
    std::vector<SessionViewRow> m_rows;
};
