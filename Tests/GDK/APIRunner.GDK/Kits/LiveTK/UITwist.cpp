//--------------------------------------------------------------------------------------
// File: UITwist.cpp
//
// A wrapper to make a Twist Menu
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------
#include "pch.h"
#include "SampleGUI.h"
#include "UITwist.h"

UITwist::UITwist(ATG::UIManager *ui, unsigned parentPanel, unsigned twistStart, const std::vector<std::wstring> &options) :
    m_options(options), m_currentIndex(0)
{
    m_previous = ui->FindControl<ATG::TextLabel>(parentPanel, twistStart);
    m_current = ui->FindControl<ATG::TextLabel>(parentPanel, twistStart + 1);
    m_next = ui->FindControl<ATG::TextLabel>(parentPanel, twistStart + 2);

    Update();
}

int UITwist::MoveNext()
{
    m_currentIndex++;
    if (m_currentIndex == static_cast<int>(m_options.size()))
        m_currentIndex = 0;
    Update();
    return m_currentIndex;
}
int UITwist::MovePrevious()
{
    m_currentIndex--;
    if (m_currentIndex == -1)
        m_currentIndex = static_cast<int>(m_options.size()) - 1;
    Update();
    return m_currentIndex;
}
void UITwist::Update()
{
    int previous = m_currentIndex - 1;
    int next = m_currentIndex + 1;

    if (next >= static_cast<int>(m_options.size()))
    {
        next = 0;
    }
    if (previous < 0)
    {
        previous = static_cast<int>(m_options.size()) - 1;
    }

    m_previous->SetText(m_options[size_t(previous)].c_str());
    m_current->SetText(m_options[size_t(m_currentIndex)].c_str());
    m_next->SetText(m_options[size_t(next)].c_str());
}
