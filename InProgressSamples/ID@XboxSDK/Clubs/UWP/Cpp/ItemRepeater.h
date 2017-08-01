// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "SampleGUI.h"

template<class T> class ItemRepeater
{
public:
    ItemRepeater(
        std::shared_ptr<ATG::UIManager> mgr,
        POINT origin,
        SIZE itemBounds,
        unsigned idBase
    ) :
        _mgr(mgr),
        _origin(origin),
        _itemBounds(itemBounds),
        _idBase(idBase),
        _panelId(0)
    {
    }

    void GenerateList(unsigned int id, std::vector<T> items, unsigned int spacing);
    void UpdateList(std::vector<T> items);

    void SetSelectedCallback(std::function<void(unsigned index)> callback)
    {
        _callback = callback;
    }

protected:
    virtual void CreateItem(unsigned index, T item, RECT& bounds) = 0;
    virtual void UpdateItem(unsigned index, T item) = 0;

    inline unsigned int GetItemId(unsigned int item) { return _idBase + (item * 100); }

    std::shared_ptr<ATG::UIManager> _mgr;
    POINT _origin;
    SIZE _itemBounds;
    unsigned _idBase;
    unsigned _panelId;
    std::function<void(unsigned index)> _callback;
};

template<class T>
inline void ItemRepeater<T>::GenerateList(unsigned int id, std::vector<T> items, unsigned int spacing)
{
    unsigned x = _origin.x;
    unsigned y = _origin.y;
    unsigned c = 0;
    long offset = 1;

    _panelId = id;

    for (auto item : items)
    {
        RECT r, o;

        o.left = x;
        o.top = y;
        o.right = x + _itemBounds.cx;
        o.bottom = y + _itemBounds.cy;

        r = o;

        o.left -= offset;
        o.top -= offset;
        o.right += offset;
        o.bottom += offset;

        auto panel = _mgr->FindPanel<ATG::Overlay>(id);
        auto button = new ATG::Button(
            GetItemId(c),
            L"",
            o
            );

        panel->Add(button);

        button->SetCallback([this, c](ATG::IPanel*, ATG::IControl*)
        {
            if (_callback)
            {
                _callback(c);
            }
        });

        CreateItem(c, item, r);
        UpdateItem(c, item);

        c++;
        y += _itemBounds.cy + spacing + (offset * 2);
    }
}

template<class T>
inline void ItemRepeater<T>::UpdateList(std::vector<T> items)
{
    unsigned c = 0;

    for (auto item : items)
    {
        UpdateItem(c++, item);
    }
}
