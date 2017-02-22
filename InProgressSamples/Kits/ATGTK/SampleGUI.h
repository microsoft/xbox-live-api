// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// A simple set of UI widgets for use in ATG samples
//
#pragma once

#include "GamePad.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <wchar.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifndef _CPPRTTI 
#error ATG Sample GUI requires RTTI
#endif

namespace ATG
{
    class IPanel;

    //----------------------------------------------------------------------------------
    // A control is an individual UI element
    class IControl
    {
    public:
        virtual ~IControl() {}

        // Methods
        virtual void Render() = 0;

        virtual bool Contains(long x, long y) const
        {
            return (m_screenRect.left <= x) && (x < m_screenRect.right) && (m_screenRect.top <= y) && (y < m_screenRect.bottom);
        }

        virtual void ComputeLayout(const RECT& parent);
        virtual void ComputeLayout(const RECT& bounds, float dx, float dy);

        virtual bool CanFocus() const { return false; }
        virtual bool DefaultFocus() const { return false; }
        virtual void OnFocus(bool in) { m_focus = in; }

        virtual bool OnSelected(IPanel* panel) { panel; return false; }

        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) { elapsedTime; pad;  return false; }
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) { elapsedTime; mstate; kbstate; return false; }

        // Properties
        using callback_t = std::function<void(_In_ IPanel*, _In_ IControl*)>;

        void SetCallback(_In_opt_ callback_t callback)
        {
            m_callBack = callback;
        }

        unsigned GetHotKey() const { return m_hotKey; }
        void SetHotKey(unsigned hotkey) { m_hotKey = hotkey; }

        void SetId(unsigned id) { m_id = id; }
        unsigned GetId() const { return m_id; }

        void SetUser(void* user) { m_user = user; }
        void* GetUser() const { return m_user; }

        void SetParent(IPanel* panel) { m_parent = panel; }

        void SetVisible(bool visible = true);
        bool IsVisible() const { return m_visible; }

        const RECT* GetRectangle() const { return &m_screenRect; }

    protected:
        IControl(const RECT& rect, unsigned id) :
            m_visible(true),
            m_focus(false),
            m_layoutRect(rect),
            m_screenRect(rect),
            m_hotKey(0),
            m_id(id),
            m_user(nullptr),
            m_parent(nullptr)
        {
        }

        bool        m_visible;
        bool        m_focus;
        RECT        m_layoutRect;
        RECT        m_screenRect;
        callback_t  m_callBack;
        unsigned    m_hotKey;
        unsigned    m_id;
        void*       m_user;
        IPanel*     m_parent;
    };

    // Static text label
    class TextLabel : public IControl
    {
    public:
        TextLabel(unsigned id, _In_z_ const wchar_t* text, const RECT& rect, unsigned style = 0);

        // Properties
        void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_color, color); }

        static const unsigned c_StyleAlignLeft = 0;
        static const unsigned c_StyleAlignCenter = 0x1;
        static const unsigned c_StyleAlignRight = 0x2;
        static const unsigned c_StyleAlignTop = 0x0;
        static const unsigned c_StyleAlignMiddle = 0x4;
        static const unsigned c_StyleAlignBottom = 0x8;
        static const unsigned c_StyleTransparent = 0x10;
        static const unsigned c_StyleWordWrap = 0x20;

        static const unsigned c_StyleFontSmall = 0x10000;
        static const unsigned c_StyleFontMid = 0;
        static const unsigned c_StyleFontLarge = 0x20000;
        static const unsigned c_StyleFontBold = 0x40000;
        static const unsigned c_StyleFontItalic = 0x80000;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetText(const wchar_t* text);
        const wchar_t* GetText() const { return m_text.c_str(); }

        // IControl
        virtual void Render() override;
        virtual void ComputeLayout(const RECT& parent) override;
        virtual void ComputeLayout(const RECT& bounds, float dx, float dy) override;
        virtual bool Contains(long, long) const override { return false; }

    private:
        unsigned            m_style;
        DirectX::XMFLOAT4   m_color;
        std::wstring        m_text;
        std::wstring        m_wordWrap;
    };

    // Static image
    class Image : public IControl
    {
    public:
        Image(unsigned id, unsigned imageId, const RECT& rect);

        void SetImageId(unsigned imageId) { m_imageId = imageId; }
        unsigned GetImageId() const { return m_imageId; }

        // IControl
        virtual void Render() override;
        virtual bool Contains(long, long) const override { return false; }

    private:
        unsigned m_imageId;
    };

    // Static text label that supports the controller font
    class Legend : public IControl
    {
    public:
        Legend(unsigned id, _In_z_ const wchar_t* text, const RECT& rect, unsigned style = 0);

        // Properties
        void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_color, color); }

        static const unsigned c_StyleAlignLeft = 0;
        static const unsigned c_StyleAlignCenter = 0x1;
        static const unsigned c_StyleAlignRight = 0x2;
        static const unsigned c_StyleAlignTop = 0x0;
        static const unsigned c_StyleAlignMiddle = 0x4;
        static const unsigned c_StyleAlignBottom = 0x8;
        static const unsigned c_StyleTransparent = 0x10;

        static const unsigned c_StyleFontSmall = 0x10000;
        static const unsigned c_StyleFontMid = 0;
        static const unsigned c_StyleFontLarge = 0x20000;
        static const unsigned c_StyleFontBold = 0x40000;
        static const unsigned c_StyleFontItalic = 0x80000;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetText(const wchar_t* text) { m_text = text; }
        const wchar_t* GetText() const { return m_text.c_str(); }

        // IControl
        virtual void Render() override;
        virtual bool Contains(long, long) const override { return false; }

    private:
        unsigned            m_style;
        DirectX::XMFLOAT4   m_color;
        std::wstring        m_text;
    };

    // Pressable button
    class Button : public IControl
    {
    public:
        Button(unsigned id, _In_z_ const wchar_t* text, const RECT& rect);

        // Properties
        void SetEnabled(bool enabled = true) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        static const unsigned c_StyleExit = 0x1;
        static const unsigned c_StyleDefault = 0x2;
        static const unsigned c_StyleTransparent = 0x4;

        static const unsigned c_StyleFontSmall = 0x10000;
        static const unsigned c_StyleFontMid = 0;
        static const unsigned c_StyleFontLarge = 0x20000;
        static const unsigned c_StyleFontBold = 0x40000;
        static const unsigned c_StyleFontItalic = 0x80000;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetText(const wchar_t* text) { m_text = text; }
        const wchar_t* GetText() const { return m_text.c_str(); }

        // IControl
        virtual void Render() override;
        virtual bool CanFocus() const override { return m_enabled; }
        virtual bool DefaultFocus() const { return (m_style & c_StyleDefault) != 0; }
        virtual bool OnSelected(IPanel* panel) override;

    private:
        bool            m_enabled;
        unsigned        m_style;
        std::wstring    m_text;
    };

    // Pressable image
    class ImageButton : public IControl
    {
    public:
        ImageButton(unsigned id, unsigned imageId, const RECT& rect);

        // Properties
        void SetEnabled(bool enabled = true) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        static const unsigned c_StyleExit = 0x1;
        static const unsigned c_StyleDefault = 0x2;
        static const unsigned c_StyleBackground = 0x4;
        static const unsigned c_StyleTransparent = 0x8;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetImageId(unsigned imageId) { m_imageId = imageId; }
        unsigned GetImageId() const { return m_imageId; }

        // IControl
        virtual void Render() override;
        virtual bool CanFocus() const override { return m_enabled; }
        virtual bool DefaultFocus() const { return (m_style & c_StyleDefault) != 0; }
        virtual bool OnSelected(IPanel* panel) override;

    private:
        bool        m_enabled;
        unsigned    m_style;
        unsigned    m_imageId;
    };

    // Two-state check box
    class CheckBox : public IControl
    {
    public:
        CheckBox(unsigned id, _In_z_ const wchar_t* text, const RECT& rect, bool checked = false);

        // Properties
        void SetEnabled(bool enabled = true) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        void SetChecked(bool checked = true) { m_checked = checked; }
        bool IsChecked() const { return m_checked; }

        static const unsigned c_StyleTransparent = 0x1;

        static const unsigned c_StyleFontSmall = 0x10000;
        static const unsigned c_StyleFontMid = 0;
        static const unsigned c_StyleFontLarge = 0x20000;
        static const unsigned c_StyleFontBold = 0x40000;
        static const unsigned c_StyleFontItalic = 0x80000;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetText(const wchar_t* text) { m_text = text; }
        const wchar_t* GetText() const { return m_text.c_str(); }

        // IControl
        virtual void Render() override;
        virtual bool CanFocus() const override { return m_enabled; }
        virtual bool OnSelected(IPanel* panel) override;

    private:
        bool            m_enabled;
        bool            m_checked;
        unsigned        m_style;
        std::wstring    m_text;
    };

    // Slider
    class Slider : public IControl
    {
    public:
        Slider(unsigned id, const RECT& rect, int value = 50, int minValue = 0, int maxValue = 100);

        // Properties
        void SetEnabled(bool enabled = true) { m_enabled = enabled; if (!enabled) m_dragging = false; }
        bool IsEnabled() const { return m_enabled; }

        static const unsigned c_StyleTransparent = 0x1;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetValue(int value);
        int GetValue() const { return m_value; }

        void SetRange(int minValue, int maxValue);
        void GetRange(int& minValue, int& maxValue) const { minValue = m_minValue; maxValue = m_maxValue; }

        // IControl
        virtual void Render() override;
        virtual bool CanFocus() const override { return m_enabled; }
        virtual void OnFocus(bool in) override;
        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) override;
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) override;

    private:
        bool        m_enabled;
        bool        m_dragging;
        unsigned    m_style;
        int         m_value;
        int         m_minValue;
        int         m_maxValue;
        RECT        m_thumbRect;
    };

    // Progress bar that goes from 0.0 to 1.0
    class ProgressBar : public IControl
    {
    public:
        ProgressBar(unsigned id, const RECT& rect, bool visible = false, float start = 0.f);
        ~ProgressBar();

        // Properties
        void SetProgress(float progress) { m_progress = std::min(std::max(progress, 0.f), 1.f); }
        float GetProgress() const { return m_progress; }

        // IControl
        virtual void Render() override;

    private:
        float m_progress;
    };

    // List box
    class ListBox : public IControl
    {
    public:
        ListBox(unsigned id, const RECT& rect, unsigned style = 0, int itemHeight = 0);
        ~ListBox();

        // Items
        struct Item
        {
            std::wstring    text;
            void*           user;
            bool            selected;

            Item() : user(nullptr), selected(false) {}

            // TODO - add optional image
        };

        void AddItem(_In_z_ const wchar_t* text, _In_opt_ void *user = nullptr);

        void InsertItem(int index, _In_z_ const wchar_t* text, _In_opt_ void *user = nullptr);

        void RemoveItem(int index);

        void RemoveAllItems();

        int GetSelectedItem() const;
        std::vector<int> GetSelectedItems() const;

        void ClearSelection();

        void SelectItem(int index);

        const Item* GetItem(int index) const { return &m_items[index]; }
        Item* GetItem(int index) { return &m_items[index]; }

        // Properties
        void SetEnabled(bool enabled = true) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        static const unsigned c_StyleMultiSelection = 0x1;
        static const unsigned c_StyleTransparent = 0x2;

        static const unsigned c_StyleFontSmall = 0x10000;
        static const unsigned c_StyleFontMid = 0;
        static const unsigned c_StyleFontLarge = 0x20000;
        static const unsigned c_StyleFontBold = 0x40000;
        static const unsigned c_StyleFontItalic = 0x80000;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        // IControl
        virtual void Render() override;
        virtual bool CanFocus() const override { return m_enabled; }
        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) override;
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) override;

    private:
        bool                m_enabled;
        int                 m_itemHeight;
        unsigned            m_style;
        int                 m_topItem;
        int                 m_focusItem;
        RECT                m_itemRect;
        int                 m_lastHeight;
        std::vector<Item>   m_items;

        static const long c_ScrollWidth = 16;
        static const long c_BorderSize = 6;
        static const long c_MarginSize = 5;
    };

    // Text box
    class TextBox : public IControl
    {
    public:
        TextBox(unsigned id, _In_z_ const wchar_t* text, const RECT& rect, unsigned style = 0);
        ~TextBox();

        // Properties
        void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_color, color); }

        static const unsigned c_StyleTransparent = 0x1;

        static const unsigned c_StyleFontSmall = 0x10000;
        static const unsigned c_StyleFontMid = 0;
        static const unsigned c_StyleFontLarge = 0x20000;
        static const unsigned c_StyleFontBold = 0x40000;
        static const unsigned c_StyleFontItalic = 0x80000;

        void SetStyle(unsigned style) { m_style = style; }
        unsigned GetStyle() const { return m_style; }

        void SetText(const wchar_t* text);
        const wchar_t* GetText() const { return m_text.c_str(); }

        // IControl
        virtual void Render() override;
        virtual void ComputeLayout(const RECT& parent) override;
        virtual void ComputeLayout(const RECT& bounds, float dx, float dy) override;
        virtual bool CanFocus() const override { return true; }
        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) override;
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) override;

    private:
        unsigned            m_style;
        int                 m_topLine;
        RECT                m_itemRect;
        int                 m_lastHeight;
        DirectX::XMFLOAT4   m_color;
        std::wstring        m_text;
        std::wstring        m_wordWrap;
        std::vector<size_t> m_wordWrapLines;

        static const long c_ScrollWidth = 16;
        static const long c_BorderSize = 6;
        static const long c_MarginSize = 5;
    };

    //----------------------------------------------------------------------------------
    // A panel is a container for UI controls
    class IPanel
    {
    public:
        virtual ~IPanel() {}

        // Methods
        virtual void Show() = 0;

        virtual void Render() = 0;

        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) = 0;
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) = 0;
        virtual void Update(float elapsedTime) { elapsedTime; };

        virtual void Close() = 0;
        virtual void Cancel() {}

        virtual void Add(_In_ IControl* ctrl) = 0;
        virtual IControl* Find(unsigned id) = 0;

        virtual void SetFocus(_In_ IControl* ctrl) { ctrl; }

        virtual void OnWindowSize(const RECT& layout) = 0;

        // Properties
        using callback_t = std::function<void(_In_ IPanel*, unsigned)>;

        void SetCallback(_In_opt_ callback_t callback)
        {
            m_callBack = callback;
        }

        void SetUser(void* user) { m_user = user; }
        void* GetUser() const { return m_user; }

        bool IsVisible() const { return m_visible; }

    protected:
        IPanel(const RECT& rect) :
            m_visible(false),
            m_layoutRect(rect),
            m_screenRect(rect),
            m_user(nullptr)
        {
        }

        bool        m_visible;
        RECT        m_layoutRect;
        RECT        m_screenRect;
        callback_t  m_callBack;
        void*       m_user;
    };

    class Popup : public IPanel
    {
    public:
        Popup(const RECT& rect);
        ~Popup();

        // IPanel
        virtual void Show() override;
        virtual void Render() override;
        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) override;
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) override;
        virtual void Close() override;
        virtual void Cancel() override;
        virtual void Add(_In_ IControl* ctrl) override;
        virtual IControl* Find(unsigned id) override;
        virtual void SetFocus(_In_ IControl* ctrl) override;
        virtual void OnWindowSize(const RECT& layout) override;

    private:
        bool                    m_select;
        bool                    m_cancel;
        IControl*               m_focusControl;
        std::vector<IControl*>  m_controls;
    };

    class HUD : public IPanel
    {
    public:
        HUD(const RECT& rect);
        ~HUD();

        // IPanel
        virtual void Show() override;
        virtual void Render() override;
        virtual bool Update(float, const DirectX::GamePad::State&) override { return false; }
        virtual bool Update(float, const DirectX::Mouse::State&, const DirectX::Keyboard::State&) override { return false; }
        virtual void Close() override;
        virtual void Add(_In_ IControl* ctrl) override;
        virtual IControl* Find(unsigned id) override;
        virtual void OnWindowSize(const RECT& layout) override;

    private:
        std::vector<IControl*>  m_controls;
    };

    class Overlay : public IPanel
    {
    public:
        Overlay(const RECT& rect);
        ~Overlay();

        // IPanel
        virtual void Show() override;
        virtual void Render() override;
        virtual bool Update(float elapsedTime, const DirectX::GamePad::State& pad) override;
        virtual bool Update(float elapsedTime, const DirectX::Mouse::State& mstate, const DirectX::Keyboard::State& kbstate) override;
        virtual void Close() override;
        virtual void Cancel() override;
        virtual void Add(_In_ IControl* ctrl) override;
        virtual IControl* Find(unsigned id) override;
        virtual void SetFocus(_In_ IControl* ctrl) override;
        virtual void OnWindowSize(const RECT& layout) override;

    private:
        bool                    m_select;
        bool                    m_cancel;
        IControl*               m_focusControl;
        std::vector<IControl*>  m_controls;
    };

    //----------------------------------------------------------------------------------
    struct UIConfig
    {
        bool forceSRGB;

        wchar_t largeFontName[MAX_PATH];
        wchar_t largeItalicFontName[MAX_PATH];
        wchar_t largeBoldFontName[MAX_PATH];

        wchar_t midFontName[MAX_PATH];
        wchar_t midItalicFontName[MAX_PATH];
        wchar_t midBoldFontName[MAX_PATH];

        wchar_t smallFontName[MAX_PATH];
        wchar_t smallItalicFontName[MAX_PATH];
        wchar_t smallBoldFontName[MAX_PATH];

        wchar_t largeLegendName[MAX_PATH];
        wchar_t smallLegendName[MAX_PATH];

        DirectX::XMFLOAT4 colorNormal;
        DirectX::XMFLOAT4 colorDisabled;
        DirectX::XMFLOAT4 colorHighlight;
        DirectX::XMFLOAT4 colorSelected;
        DirectX::XMFLOAT4 colorFocus;
        DirectX::XMFLOAT4 colorBackground;
        DirectX::XMFLOAT4 colorTransparent;
        DirectX::XMFLOAT4 colorChecked;
        DirectX::XMFLOAT4 colorProgress;

        enum COLORS
        {
            GREEN,
            BLUE,
            ORANGE,
            DARK_GREY,
            LIGHT_GREY,
            OFF_WHITE,
            WHITE,
            MAX_COLORS,
        };

        DirectX::XMFLOAT4 colorDictionary[MAX_COLORS];

        UIConfig(bool linear = false) :
            forceSRGB(linear)
        {
            using DirectX::XMFLOAT4;

            wcscpy_s(largeFontName, L"SegoeUI_36.spritefont");
            wcscpy_s(largeItalicFontName, L"SegoeUI_36_Italic.spritefont");
            wcscpy_s(largeBoldFontName, L"SegoeUI_36_Bold.spritefont");

            wcscpy_s(midFontName, L"SegoeUI_22.spritefont");
            wcscpy_s(midItalicFontName, L"SegoeUI_22_Italic.spritefont");
            wcscpy_s(midBoldFontName, L"SegoeUI_22_Bold.spritefont");

            wcscpy_s(smallFontName, L"SegoeUI_18.spritefont");
            wcscpy_s(smallItalicFontName, L"SegoeUI_18_Italic.spritefont");
            wcscpy_s(smallBoldFontName, L"SegoeUI_18_Bold.spritefont");

            wcscpy_s(largeLegendName, L"XboxOneControllerLegend.spritefont");
            wcscpy_s(smallLegendName, L"XboxOneControllerLegendSmall.spritefont");

            if (linear)
            {
                colorNormal = XMFLOAT4(0.361306787f, 0.361306787f, 0.361306787f, 1.f);              // OffWhite
                colorDisabled = XMFLOAT4(0.194617808f, 0.194617808f, 0.194617808f, 1.f);            // LightGrey
                colorHighlight = XMFLOAT4(0.545724571f, 0.026241219f, 0.001517635f, 1.f);           // Orange
                colorSelected = XMFLOAT4(0.955973506f, 0.955973506f, 0.955973506f, 1.f);            // White
                colorFocus = XMFLOAT4(0.005181516f, 0.201556236f, 0.005181516f, 1.f);               // Green
                colorBackground = XMFLOAT4(0.f, 0.f, 0.f, 1.f);                                     // Black
                colorTransparent = XMFLOAT4(0.033105, 0.033105, 0.033105f, 0.5f);
                colorChecked = XMFLOAT4(0.001517635f, 0.114435382f, 0.610495627f, 1.f);             // Blue
                colorProgress = XMFLOAT4(1.f, 1.f, 0.f, 1.f);                                       // Yellow

                colorDictionary[GREEN] = XMFLOAT4(0.005181516f, 0.201556236f, 0.005181516f, 1.f);
                colorDictionary[BLUE] = XMFLOAT4(0.001517635f, 0.114435382f, 0.610495627f, 1.f);
                colorDictionary[ORANGE] = XMFLOAT4(0.545724571f, 0.026241219f, 0.001517635f, 1.f);
                colorDictionary[DARK_GREY] = XMFLOAT4(0.033104762f, 0.033104762f, 0.033104762f, 1.f);
                colorDictionary[LIGHT_GREY] = XMFLOAT4(0.194617808f, 0.194617808f, 0.194617808f, 1.f);
                colorDictionary[OFF_WHITE] = XMFLOAT4(0.361306787f, 0.361306787f, 0.361306787f, 1.f);
                colorDictionary[WHITE] = XMFLOAT4(0.955973506f, 0.955973506f, 0.955973506f, 1.f);
            }
            else
            {
                colorNormal = XMFLOAT4(0.635294139f, 0.635294139f, 0.635294139f, 1.f);              // OffWhite
                colorDisabled = XMFLOAT4(0.478431374f, 0.478431374f, 0.478431374f, 1.f);            // LightGrey
                colorHighlight = XMFLOAT4(0.764705896f, 0.176470593f, 0.019607844f, 1.f);           // Orange
                colorSelected = XMFLOAT4(0.980392158f, 0.980392158f, 0.980392158f, 1.f);            // White
                colorFocus = XMFLOAT4(0.062745102f, 0.486274511f, 0.062745102f, 1.f);               // Green
                colorBackground = XMFLOAT4(0.f, 0.f, 0.f, 1.f);                                     // Black
                colorTransparent = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f);
                colorChecked = XMFLOAT4(0.019607844f, 0.372549027f, 0.803921580f, 1.f);             // Blue
                colorProgress = XMFLOAT4(1.f, 1.f, 0.f, 1.f);                                       // Yellow

                colorDictionary[GREEN] = XMFLOAT4(0.062745102f, 0.486274511f, 0.062745102f, 1.f);
                colorDictionary[BLUE] = XMFLOAT4(0.019607844f, 0.372549027f, 0.803921580f, 1.f);
                colorDictionary[ORANGE] = XMFLOAT4(0.764705896f, 0.176470593f, 0.019607844f, 1.f);
                colorDictionary[DARK_GREY] = XMFLOAT4(0.200000003f, 0.200000003f, 0.200000003f, 1.f);
                colorDictionary[LIGHT_GREY] = XMFLOAT4(0.478431374f, 0.478431374f, 0.478431374f, 1.f);
                colorDictionary[OFF_WHITE] = XMFLOAT4(0.635294139f, 0.635294139f, 0.635294139f, 1.f);
                colorDictionary[WHITE] = XMFLOAT4(0.980392158f, 0.980392158f, 0.980392158f, 1.f);
            }
        }
    };

    class UIManager
    {
    public:
        UIManager(const UIConfig& config);
        UIManager(_In_ ID3D11DeviceContext* context, const UIConfig& config);

        UIManager(UIManager&& moveFrom);
        UIManager& operator= (UIManager&& moveFrom);

        UIManager(UIManager const&) = delete;
        UIManager& operator=(UIManager const&) = delete;

        virtual ~UIManager();

        // Load UI layout from disk
        void LoadLayout(const wchar_t* layoutFile, const wchar_t* imageDir = nullptr, unsigned offset = 0);

        // Add a panel (takes ownership)
        void Add(unsigned id, _In_ IPanel* panel);

        // Find a panel
        IPanel* Find(unsigned id) const;

        template<class T>
        T* FindPanel(unsigned id) const
        {
            auto panel = dynamic_cast<T*>(Find(id));
            if (panel)
            {
                return panel;
            }

            throw std::exception("Find (panel)");
        }

        // Find a control
        template<class T>
        T* FindControl(unsigned panelId, unsigned ctrlId) const
        {
            auto panel = Find(panelId);
            if (panel)
            {
                auto ctrl = dynamic_cast<T*>(panel->Find(ctrlId));
                if (ctrl)
                    return ctrl;
            }

            throw std::exception("Find (control)");
        }

        // Process user input for gamepad controls
        bool Update(float elapsedTime, const DirectX::GamePad::State& pad);

        // Process user input for keyboard & mouse controls
        bool Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& kb);

        // Render the visible UI panels
        void Render();

        // Set the screen viewport
        void SetWindow(const RECT& layout);

        // Texture registry for images (used by controls)
        static const unsigned c_LayoutImageIdStart = 0x10000;

        void RegisterImage(unsigned id, _In_ ID3D11ShaderResourceView* tex);
        void UnregisterImage(unsigned id);
        void UnregisterAllImages();

        // Direct3D device management
        void ReleaseDevice();
        void RestoreDevice(_In_ ID3D11DeviceContext* context);

        // Reset UI state (such as coming back from suspend)
        void Reset();

        // Release all objects
        void Clear();

        // Enumerators
        void Enumerate(std::function<void(unsigned id, IPanel*)> enumCallback);

        // Common callback adapters
        void CallbackYesNoCancel(_In_ IPanel* panel, std::function<void(bool, bool)> yesnocallback);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;

        friend class IControl;
        friend class TextLabel;
        friend class Image;
        friend class Legend;
        friend class Button;
        friend class ImageButton;
        friend class CheckBox;
        friend class Slider;
        friend class ProgressBar;
        friend class ListBox;
        friend class TextBox;

        friend class Popup;
        friend class HUD;
        friend class Overlay;
    };
}