//--------------------------------------------------------------------------------------
// File: UITwist.h
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
#pragma once

namespace ATG
{
    class UIManager;
    class TextLabel;
}

class UITwist
{
public:
    UITwist(ATG::UIManager *ui, unsigned parentPanel, unsigned twistStart, const std::vector<std::wstring> &options);

    int MoveNext();
    int MovePrevious();
    int CurrentIndex() const { return m_currentIndex; }
private:
    void Update();

    std::vector<std::wstring> m_options;
    int             m_currentIndex;
    ATG::TextLabel *m_previous;
    ATG::TextLabel *m_current;
    ATG::TextLabel *m_next;
};