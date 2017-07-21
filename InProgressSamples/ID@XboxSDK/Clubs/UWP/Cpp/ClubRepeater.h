// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "ItemRepeater.h"

class ClubListItem
{
public:
    ClubListItem(xbox::services::clubs::club* pClub) : m_pClub(pClub) { }

    std::wstring GetLabel();
        
private:
    xbox::services::clubs::club* m_pClub;
};

class ClubRepeater : public ItemRepeater<std::shared_ptr<ClubListItem>>
{
public:
    ClubRepeater(
        std::shared_ptr<ATG::UIManager> mgr,
        POINT origin,
        SIZE itemBounds,
        unsigned idBase
    ) :
        ItemRepeater(mgr, origin, itemBounds, idBase),
        _readonly(false)
    {
    }

    void SetReadOnly(bool readonly) { _readonly = readonly; }
    void ShadeItem(unsigned index, const ATG::UIConfig& config);

protected:
    virtual void CreateItem(unsigned index, std::shared_ptr<ClubListItem> item, RECT& bounds) override;
    virtual void UpdateItem(unsigned index, std::shared_ptr<ClubListItem> item) override;

private:
    bool _readonly;
};
